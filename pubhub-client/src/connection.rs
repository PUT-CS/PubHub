use std::{
    io::{Read, Write},
    mem::size_of,
    net::{TcpStream, ToSocketAddrs},
};

use anyhow::Result;
use serde_json::Value;

pub use crate::{error::ConnectionError, request::Request, response::Response};

pub struct PubHubConnection {
    stream: TcpStream,
}

impl PubHubConnection {
    pub fn new(addr: impl ToSocketAddrs) -> Result<Self, std::io::Error> {
        match TcpStream::connect(addr) {
            Ok(stream) => Ok(Self { stream }),
            Err(e) => Err(e),
        }
    }

    pub fn execute(&mut self, request: Request) -> Result<Response> {
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

        let size = socket::htonl(size);

        let all = [size.to_be_bytes().as_slice(), msg_bytes].concat();

        self.stream.write_all(&all)?;

        Ok(())
    }

    /// Block while waiting for the next incoming response
    fn await_response(&mut self) -> Result<Response> {
        let mut size_buffer = [0; size_of::<u32>()];

        self.stream.read_exact(&mut size_buffer)?;

        // from network order
        let size = u32::from_be_bytes(size_buffer);

        let mut msg_buffer: Vec<u8> = Vec::with_capacity(size as usize);
        self.stream.read_exact(msg_buffer.as_mut_slice())?;

        let msg = String::from_utf8(msg_buffer)?;
        let json: Value = serde_json::from_str(&msg)?;

        let kind = json
            .get("responseKind")
            .map(|s| s.to_string())
            .ok_or_else(|| ConnectionError::new("Server sent a response of unspecified kind"))?;

        match kind.as_str() {
            "Ok" => Ok(Response::Ok { content: "".into() }),
            "Error" => match json.get("why") {
                Some(reason) => Ok(Response::Error {
                    why: reason.to_string(),
                }),
                None => Err(ConnectionError::new(
                    "Server sent an error response without giving the reason",
                )
                .into()),
            },
            _ => Err(ConnectionError::new("Server sent a response with an invalid kind").into()),
        }
    }
}

impl Drop for PubHubConnection {
    fn drop(&mut self) {
        let _ = self.stream.shutdown(std::net::Shutdown::Both);
    }
}
