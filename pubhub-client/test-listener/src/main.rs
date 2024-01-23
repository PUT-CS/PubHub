use std::{net::Ipv4Addr, env, str::FromStr};

use pubhub_rs::{request::Request, PubHubConnection};

fn main() {
    let args: Vec<_> = env::args().skip(1).collect();
    let ip = args[0].to_owned();
    dbg!(&args[1]);
    let port: u16 = args[1].parse().unwrap();
    let addr = Ipv4Addr::from_str(&ip).unwrap();
    
    let mut conn = PubHubConnection::new((addr, port)).unwrap();

    let _ = args.iter().skip(2)
        .map(|name| Request::Subscribe(name.to_string())).for_each(|req| {
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
