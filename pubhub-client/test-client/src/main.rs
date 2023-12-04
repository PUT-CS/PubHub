use pubhub_rs::{request::Request, PubHubConnection};
use std::{time::Duration, net::Ipv4Addr};

type Result<T> = std::result::Result<T, anyhow::Error>;

fn main() -> Result<()> {
    let addr = (Ipv4Addr::LOCALHOST, 8080);
    let mut conn = PubHubConnection::new(addr)?;

    let requests = &[
        Request::CreateChannel("testchannel1".into()),
        Request::CreateChannel("testchannel2".into()),
        Request::CreateChannel("testchannel3".into()),
        Request::CreateChannel("testchannel4".into()),
        Request::CreateChannel("testchannel4".into()),
        Request::Publish {
            channel: "testchannel1".into(),
            content: "Hello!".into(),
        },
        //Request::Ask,
        Request::Subscribe("testchannel1".into()),
        Request::Unsubscribe("testchannel1".into()),
        Request::DeleteChannel("testchannel1".into()),
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
