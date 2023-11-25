use std::time::Duration;
use connection::{PubHubConnection, Request};
use log::info;
use logs::info;

mod connection;
mod logs;
mod request;
mod response;
mod error;

type Result<T> = std::result::Result<T, anyhow::Error>;


fn main() -> Result<()> {
    let address = "127.0.0.1";
    let port = 8080;

    let mut conn = PubHubConnection::new((address, port))
        .expect("Failed to connect to the server. Is it running?");
    
    let requests = vec![
        Request::CreateChannel("datetime".into()),
        Request::Subscribe("nproc".into()),
        Request::Publish {
            channel: "nproc".into(),
            content: "hello!".into(),
        },
        Request::Unsubscribe("nproc".into()),
        Request::Ask,
    ];

    for request in requests {
        let res = conn.execute(request)?;
        info(format!("Response: {res:?}").as_str());
    }
    
    loop {
        std::thread::sleep(Duration::from_millis(100));
    }
}
