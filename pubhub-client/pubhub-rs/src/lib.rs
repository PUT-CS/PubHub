use std::{
    io::{Read, Write},
    mem::size_of,
    net::{Ipv4Addr, TcpListener, TcpStream},
};

use request::Request;
use response::Response;
use serde_json::Value;

pub mod error;
pub mod request;
pub mod response;

type Result<T> = std::result::Result<T, anyhow::Error>;

pub struct PubHubConnection {
    request_handler: RequestHandler,
    publish_receiver: PublishReceiver,
}

pub struct RequestHandler {
    stream: TcpStream,
}
pub struct PublishReceiver {
    stream: TcpStream,
}

trait PubHubReceiver {
    fn next_pubhub_message(&mut self) -> std::result::Result<Vec<u8>, std::io::Error>;
}

impl PubHubReceiver for TcpStream {
    fn next_pubhub_message(&mut self) -> std::result::Result<Vec<u8>, std::io::Error> {
        // Read message size
        let mut size_buffer = [0; size_of::<u32>()];
        self.read_exact(&mut size_buffer)?;

        // From network order
        let size = u32::from_be_bytes(size_buffer);

        // Read the actual message
        let mut msg_buffer = Vec::with_capacity(size as usize);
        self.read_exact(msg_buffer.as_mut_slice())?;

        Ok(msg_buffer)
    }
}

impl RequestHandler {
    pub fn execute(&mut self, request: &Request) -> Result<Response> {
        let json = request.to_json().to_string();

        self.send_request(json)?;
        //let res = self.await_response()?;

        Ok(Response::Ok { content: "".into() })
        //Ok(res)
    }

    /// Issue a request to the PubHub Server
    fn send_request(&mut self, msg: String) -> Result<()> {
        let msg_bytes = msg.as_bytes();
        let size: u32 = msg_bytes.len().try_into()?;

        // Convert to network byte order (big endian)
        let all: &[u8] = &[size.to_be_bytes().as_slice(), msg_bytes].concat();

        self.stream.write_all(all)?;

        Ok(())
    }

    /// Block while waiting for the next incoming response
    fn await_response(&mut self) -> Result<Response> {
        let message_bytes = self.stream.next_pubhub_message()?;
        let msg = String::from_utf8(message_bytes)?;
        let json: Value = serde_json::from_str(&msg)?;

        Response::try_from(json)
    }
}

impl PublishReceiver {
    /// Receive a message from the broadcast listener
    fn next_message(&mut self) -> Result<serde_json::Value> {
        let message_bytes = self.stream.next_pubhub_message()?;
        let msg = String::from_utf8(message_bytes)?;
        let json = serde_json::from_str(&msg)?;

        Ok(json)
    }
}

impl PubHubConnection {
    pub fn new(addr: (Ipv4Addr, u16)) -> std::result::Result<Self, std::io::Error> {
        let request_stream = TcpStream::connect(&addr)?;

        // Listen on the same port as the server + 1
        let listener_addr = (Ipv4Addr::LOCALHOST, addr.1 + 1);
        eprintln!("Listening on {listener_addr:?}");

        // wait for the server to reciprocate the connection
        let (publish_stream, _) = TcpListener::bind(listener_addr)?.accept()?;

        Ok(Self {
            request_handler: RequestHandler {
                stream: request_stream,
            },
            publish_receiver: PublishReceiver {
                stream: publish_stream,
            },
        })
    }

    pub fn execute(&mut self, request: &Request) -> Result<Response> {
        self.request_handler.execute(request)
    }

    pub fn next_message(&mut self) -> Result<serde_json::Value> {
        self.publish_receiver.next_message()
    }

    pub fn into_inner(self) -> (RequestHandler, PublishReceiver) {
        (self.request_handler, self.publish_receiver)
    }
}
