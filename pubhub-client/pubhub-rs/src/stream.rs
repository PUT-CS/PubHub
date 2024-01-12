use std::{net::TcpStream, mem::size_of, io::{Read, Write}};

pub trait PubHubStream {
    fn next_pubhub_message(&mut self) -> std::result::Result<Vec<u8>, std::io::Error>;
    fn send_pubhub_request(&mut self, request: &[u8]) -> anyhow::Result<()>;
}

impl PubHubStream for TcpStream {
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
    fn send_pubhub_request(&mut self, request: &[u8]) -> anyhow::Result<()> {
        let size: u32 = request.len().try_into()?;

        // Convert to network byte order (big endian)
        let all: &[u8] = &[size.to_be_bytes().as_slice(), request].concat();

        self.write_all(all)?;

        Ok(())
    }
}
