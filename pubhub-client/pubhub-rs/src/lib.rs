use std::{
    io::{Read, Write},
    mem::size_of,
    net::{Ipv4Addr, TcpStream},
};

use request::Request;
use response::Response;
use serde_json::Value;

pub mod error;
pub mod request;
pub mod response;

type Result<T> = std::result::Result<T, anyhow::Error>;

pub struct PubHubConnection {
    socket: TcpStream,
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
        let mut msg_buffer = vec![0; size as usize];
        self.read_exact(&mut msg_buffer)?;

        Ok(msg_buffer)
    }
}

impl PubHubConnection {
    pub fn execute(&mut self, request: &Request) -> Result<Response> {
        let json = request.to_json().to_string();

        self.send_request(json)?;
        let res = self.await_response()?;

        Ok(res)
    }

    /// Issue a request to the PubHub Server
    fn send_request(&mut self, msg: String) -> Result<()> {
        let msg_bytes = msg.as_bytes();
        let size: u32 = msg_bytes.len().try_into()?;

        // Convert to network byte order (big endian)
        let all: &[u8] = &[size.to_be_bytes().as_slice(), msg_bytes].concat();

        self.socket.write_all(all)?;

        Ok(())
    }

    /// Block while waiting for the next incoming response
    fn await_response(&mut self) -> Result<Response> {
        let message_bytes = self.socket.next_pubhub_message()?;
        let msg = String::from_utf8(message_bytes)?;
        let json: Value = serde_json::from_str(&msg)?;

        Response::try_from(json)
    }

    pub fn new(addr: (Ipv4Addr, u16)) -> std::result::Result<Self, std::io::Error> {
        let socket = TcpStream::connect(&addr)?;

        // Listen on the same port as the server + 1
        let listener_addr = (Ipv4Addr::LOCALHOST, addr.1 + 1);
        eprintln!("Listening on {listener_addr:?}");

        Ok(Self {
            socket
        })
    }

    pub fn next_message(&mut self) -> Result<serde_json::Value> {
        let message_bytes = self.socket.next_pubhub_message()?;
        let msg = String::from_utf8(message_bytes)?;
        let json = serde_json::from_str(&msg)?;

        Ok(json)
    }
}
