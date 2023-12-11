use std::net::Ipv4Addr;
use pubhub_rs::{PubHubConnection, request::Request};
use rand::Rng;
use serde_json::json;
use std::time::Duration;

fn main() {
    let mut connection = PubHubConnection::new((Ipv4Addr::LOCALHOST, 8080)).unwrap();
    let create_request = Request::CreateChannel("randint".to_string());
    let _ = connection.execute(&create_request).expect("Unable to create channel");
    
    loop {
	let message = get_random_numbers();
	std::thread::sleep(Duration::from_secs(1));
	let publish_request = Request::Publish { channel: "datetime".to_string(), content: message };
	let _ = connection.execute(&publish_request).expect("Unable to publish message");
    }
}

fn get_random_numbers() -> serde_json::Value {
    let mut rng = rand::thread_rng();

    json!({
	"rInt": rng.gen::<i32>(),
	"rFloat": rng.gen::<f64>()
    })
}
