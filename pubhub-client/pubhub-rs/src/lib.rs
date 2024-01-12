use std::{
    collections::HashSet,
    error::Error,
    io::Write,
    net::{Ipv4Addr, TcpStream},
};

use request::Request;
use response::Response;
use serde_json::Value;

pub mod error;
pub mod request;
pub mod response;
mod stream;

use stream::PubHubStream;

type Result<T> = std::result::Result<T, anyhow::Error>;

trait PubHubClient {
    fn socket(&mut self) -> &mut TcpStream;

    fn execute(&mut self, request: &Request) -> Result<Response> {
        self.send_request(request)?;
        let res = self.await_response()?;

        Ok(res)
    }

    /// Issue a request to the PubHub Server
    fn send_request(&mut self, msg: &Request) -> Result<()> {
        let msg_bytes = msg.to_json().to_string();
        let size: u32 = msg_bytes.len().try_into()?;

        // Convert to network byte order (big endian)
        let all: &[u8] = &[size.to_be_bytes().as_slice(), msg_bytes.as_bytes()].concat();

        self.socket().write_all(all)?;

        Ok(())
    }

    /// Block while waiting for the next incoming response
    fn await_response(&mut self) -> Result<Response> {
        let message_bytes = self.socket().next_pubhub_message()?;
        let msg = String::from_utf8(message_bytes)?;
        let json: Value = serde_json::from_str(&msg)?;

        Response::try_from(json)
    }

    fn next_message(&mut self) -> Result<serde_json::Value> {
        let message_bytes = self.socket().next_pubhub_message()?;
        let msg = String::from_utf8(message_bytes)?;
        let json = serde_json::from_str(&msg)?;

        Ok(json)
    }
}

pub struct PubHubConnection {
    socket: TcpStream,
    subscriptions: Vec<String>,
}
pub struct PubHubListener {
    socket: TcpStream,
}

impl PubHubConnection {
    pub fn new(addr: (Ipv4Addr, u16)) -> std::result::Result<Self, std::io::Error> {
        Ok(Self {
            socket: TcpStream::connect(&addr)?,
            subscriptions: vec![],
        })
    }

    pub fn execute(&mut self, request: &Request) -> Result<Response> {
        match request {
            Request::Subscribe(name) => {
                self.subscriptions.push(name.to_owned());
                Ok(Response::Ok { content: None })
            }
            _ => {
                let str = request.to_json().to_string();
                self.socket.send_pubhub_request(str.as_bytes())?;
                self.await_response()
            }
        }
    }

    pub fn into_listener(self) -> std::result::Result<(PubHubListener, Vec<Response>), Box<dyn Error>> {
        let mut listener = PubHubListener {
            socket: self.socket,
        };
        let mut responses = vec![];
        for channel in self.subscriptions.iter() {
            let response = listener.subscribe(&channel)?;
            responses.push(response);
        }
        Ok((listener, responses))
    }
}

impl PubHubClient for PubHubConnection {
    fn socket(&mut self) -> &mut TcpStream {
        &mut self.socket
    }
}
impl PubHubClient for PubHubListener {
    fn socket(&mut self) -> &mut TcpStream {
        &mut self.socket
    }
}

impl PubHubListener {
    pub fn subscribe(&mut self, channel_name: &String) -> Result<Response> {
        let request = Request::Subscribe(channel_name.to_owned());
        let str = request.to_json().to_string();
        self.socket.send_pubhub_request(str.as_bytes())?;
        self.await_response()
    }
}

impl Iterator for PubHubListener {
    type Item = Result<serde_json::Value>;
    fn next(&mut self) -> Option<Self::Item> {
        Some(self.next_message())
    }
}
