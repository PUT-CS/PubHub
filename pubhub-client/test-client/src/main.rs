use pubhub_rs::{request::Request, PubHubConnection};
use std::time::Duration;

type Result<T> = std::result::Result<T, anyhow::Error>;

fn main() -> Result<()> {
    let addr = ("127.0.0.1", 8080);
    let mut conn = PubHubConnection::new(addr)?;

    let requests = &[
        Request::CreateChannel("testchannel".into()),
        Request::Publish {
            channel: "testchannel".into(),
            content: "Hello!".into(),
        },
        //Request::Ask,
        Request::Subscribe("testchannel".into()),
        Request::DeleteChannel("testchannel".into()),
        //Request::Ask,

        // nonexistent channel
        Request::Publish {
            channel: "null".into(),
            content: "Hello!".into(),
        },
        Request::Subscribe("null".into()),
        Request::DeleteChannel("null".into()),
    ];

    let responses = requests.iter().map(|r| conn.execute(&r));

    for (req, res) in requests.iter().zip(responses) {
        println!("{:<70} -> {res:->30?}", req.to_json().to_string());
    }

    std::thread::sleep(Duration::from_secs(1));

    Ok(())
}
