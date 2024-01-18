use std::{net::Ipv4Addr, env, str::FromStr};

use pubhub_rs::{request::Request, PubHubConnection};

fn main() {
    let mut args = env::args().skip(1);
    let ip = args.next().unwrap();
    let port: u16 = args.next().unwrap().parse().unwrap();
    let addr = Ipv4Addr::from_str(&ip).unwrap();
    
    let mut conn = PubHubConnection::new((addr, port)).unwrap();

    let _ = args
        .map(|name| Request::Subscribe(name)).for_each(|req| {
            let res = conn.execute(&req).unwrap();
            dbg!(res);
        });

    let (listener, responses) = conn.into_listener().unwrap();
    dbg!(responses);
    
    for msg in listener {
        println!("{:#?}\n", msg.unwrap());
    }
}
// https://pl.wikipedia.org/wiki/Hibneryt
