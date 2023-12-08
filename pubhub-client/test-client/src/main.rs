use pubhub_rs::{request::Request, PubHubConnection};
use std::net::Ipv4Addr;

type Result<T> = std::result::Result<T, anyhow::Error>;

fn main() -> Result<()> {
    let addr = (Ipv4Addr::LOCALHOST, 8080);
    let mut conn = PubHubConnection::new(addr)?;

    let requests = &[
        Request::CreateChannel("testchannel1".into()),
        Request::CreateChannel("testchannel2".into()),
        Request::CreateChannel("testchannel3".into()),
        Request::Subscribe("testchannel1".into()),
        Request::Subscribe("testchannel2".into()),
        Request::Publish {
            channel: "testchannel1".into(),
            content: "Hello to channel1!".into(),
        },
        Request::Publish {
            channel: "testchannel2".into(),
            content: "Hello to channel2!".into(),
        },
        Request::Subscribe("testchannel1".into()),
        Request::Subscribe("testchannel5".into()),
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
    
    let (_, mut listener) = conn.into_inner();

    println!("listening...");
    loop {
        let msg = listener.next_message().unwrap();
        eprintln!("{msg}");
    }

    //Ok(())

    // loop {
        
    // }
}
