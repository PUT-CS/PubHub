use std::io::{Read, Write};
use std::net::TcpStream;

#[derive(Debug, serde::Serialize, serde::Deserialize)]
struct MyMessage {
    // Define your JSON message structure here
    message: String,
}

fn main() {
    // Server address and port
    let server_address = "127.0.0.1";
    let server_port = 8080;

    // Create a TcpStream to connect to the server
    match TcpStream::connect((server_address, server_port)) {
        Ok(mut stream) => {
            // Data to be sent
            let json_data = MyMessage {
                message: String::from("Hello, server! You are my lovely friend! Lorem ipsum dolor sit amet przyjacielu!"),
            };


            // Serialize JSON data to a string
            let json_string = serde_json::to_string(&json_data).expect("Failed to serialize JSON");

	    //Get JSON string length
            let integer_bytes: u32 = json_string.len().try_into().expect("Should be convertable to u32");
	    let integer_bytes = socket::htonl(integer_bytes);
            // Combine integer and JSON data into a single byte vector
            let mut combined_data = integer_bytes.to_be_bytes().to_vec();
            combined_data.extend_from_slice(json_string.as_bytes());

            // Send data to the server
            stream.write_all(&combined_data).expect("Failed to send data to the server");
	    
	    loop {
		
	    }
        }
        Err(e) => {
            eprintln!("Failed to connect to the server: {}", e);
        }
    }
}
