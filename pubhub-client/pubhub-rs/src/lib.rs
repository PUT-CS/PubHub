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
    request_stream: TcpStream,
    broadcast_listener: TcpStream,
}

impl PubHubConnection {
    pub fn new(addr: (Ipv4Addr, u16)) -> std::result::Result<Self, std::io::Error> {
        let request_stream = TcpStream::connect(&addr)?;
        
        // Listen on the same port as the server + 1
        let listener_addr = (Ipv4Addr::LOCALHOST, addr.1 + 1);
        eprintln!("Listening on {listener_addr:?}");
        
        // wait for the server to reciprocate the connection
        let (broadcast_listener, _) = TcpListener::bind(listener_addr)?.accept()?;

        eprintln!("Accepted broadcast connection");

        Ok(Self {
            request_stream,
            broadcast_listener,
        })
    }

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

        self.request_stream.write_all(all)?;

        Ok(())
    }

    /// Block while waiting for the next incoming response
    fn await_response(&mut self) -> Result<Response> {
        let mut size_buffer = [0; size_of::<u32>()];

        self.request_stream.read_exact(&mut size_buffer)?;

        // From network order
        let size = u32::from_be_bytes(size_buffer);

        let mut msg_buffer: Vec<u8> = Vec::with_capacity(size as usize);
        self.request_stream.read_exact(msg_buffer.as_mut_slice())?;

        let msg = String::from_utf8(msg_buffer)?;
        let json: Value = serde_json::from_str(&msg)?;

        Response::try_from(json)
    }

    /// Receive a message from the broadcast listener
    fn next_broadcast(&mut self) {
        
    }
}

impl Drop for PubHubConnection {
    fn drop(&mut self) {
        let _ = self.request_stream.shutdown(std::net::Shutdown::Both);
    }
}
