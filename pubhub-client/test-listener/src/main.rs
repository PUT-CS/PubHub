use std::net::Ipv4Addr;

use pubhub_rs::{request::Request, PubHubConnection};

fn main() {
    let addr = (Ipv4Addr::LOCALHOST, 8080);
    let mut conn = PubHubConnection::new(addr).unwrap();

    let _ = std::env::args()
        .skip(1)
        .map(|name| Request::Subscribe(name)).for_each(|req| {
            let res = conn.execute(&req).unwrap();
            dbg!(res);
        });

    loop {
        let message = conn.next_message().unwrap();
        println!("{message:#?}\n");
    }
}
