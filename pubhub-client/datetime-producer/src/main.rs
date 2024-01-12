use pubhub_rs::{request::Request, PubHubConnection};
use serde_json::json;
use std::{
    net::Ipv4Addr,
    time::{Duration, SystemTime},
};

fn main() {
    let mut connection = PubHubConnection::new((Ipv4Addr::LOCALHOST, 8080)).unwrap();
    let create_request = Request::CreateChannel("datetime".to_string());
    let _ = connection.execute(&create_request).unwrap();
    
    loop {
        let message = get_current_time();
        std::thread::sleep(Duration::from_secs(1));
        let publish_request = Request::Publish {
            channel: "datetime".to_string(),
            content: message,
        };
        println!("Publishing: {publish_request:#?}");
        let _ = connection.execute(&publish_request).unwrap();
    }
}

fn get_current_time() -> serde_json::Value {
    let seconds = SystemTime::now()
        .duration_since(SystemTime::UNIX_EPOCH)
        .unwrap();
    json!({
        "time": seconds.as_secs(),
    })
}
