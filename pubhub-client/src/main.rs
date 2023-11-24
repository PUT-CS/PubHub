use std::io::{stdin, Read, Write};
use std::net::TcpStream;

#[derive(Debug, serde::Serialize, serde::Deserialize)]
struct MyMessage {
    message: String,
}

fn main() {
    let server_address = "127.0.0.1";
    let server_port = 8080;

    let mut stream = TcpStream::connect((server_address, server_port))
        .expect("Failed to connect to the server. Is it running?");

    loop {
        let input = stdin().lines().next().unwrap().expect("Read line");

        let json_data = MyMessage { message: input };
        let json_string = serde_json::to_string(&json_data).expect("Failed to serialize JSON");

        let integer_bytes: u32 = json_string.len().try_into().map(socket::htonl).unwrap();
        
        let data = [
            integer_bytes.to_be_bytes().as_slice(),
            json_string.as_bytes(),
        ]
        .concat();

        eprintln!("Sending: {json_string:?}");
        
        stream
            .write_all(&data)
            .expect("Failed to send data to the server");
    }
}
