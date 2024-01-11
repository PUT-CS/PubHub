use pubhub_rs::{request::Request, PubHubConnection};
use rand::Rng;
use serde_json::json;
use std::net::Ipv4Addr;

type Result<T> = std::result::Result<T, anyhow::Error>;

fn main() -> Result<()> {
    //const SIZE: usize = 1_000_000;
    const SIZE: usize = 1_000;
    let huge_data: Vec<u8> = vec![rand::thread_rng().gen(); SIZE];
    assert_eq!(huge_data.len(), SIZE);

    let requests = &[
        Request::CreateChannel("testchannel1".into()),
        Request::CreateChannel("testchannel2".into()),
        Request::CreateChannel("testchannel3".into()),
        Request::Subscribe("testchannel1".into()),
        Request::Subscribe("testchannel2".into()),
        Request::Publish {
            channel: "testchannel1".into(),
            content: json!({
                "a" : 1,
                "b" : "bee",
                //"c" : [127,0,0,1],
                //"huuuge": huge_data
            }),
        },
        Request::Publish {
            channel: "testchannel2".into(),
            content: "Hello to channel2!".into(),
        },
        Request::Subscribe("testchannel1".into()),
        Request::Subscribe("testchannel5".into()),
        Request::Unsubscribe("testchannel1".into()),
        Request::DeleteChannel("testchannel1".into()),
        Request::Ask,
        // // nonexistent channel
        Request::Publish {
            channel: "null".into(),
            content: "Hello".into(),
        },
        Request::Subscribe("null".into()),
        Request::DeleteChannel("null".into()),
    ];

    let addr = (Ipv4Addr::LOCALHOST, 8080);
    let mut conn = PubHubConnection::new(addr)?;

    let responses = requests.iter().map(|r| conn.execute(&r));

    for (req, res) in requests.iter().zip(responses) {
        println!("{:<70} -> {res:->30?}", req.to_json().to_string());
    }

    let tid = std::thread::spawn(move || {
        println!("\nListening...\n");
        loop {
            let msg = conn.next_message().unwrap();
            eprintln!("Received a message: {}", msg.to_string());
        }
    });
    tid.join().unwrap();
    Ok(())
}
