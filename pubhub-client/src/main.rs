use colored::Colorize;
use connection::{PubHubConnection, Request};
use log::{trace, debug, info, warn, error};
mod connection;

type Result<T> = std::result::Result<T, anyhow::Error>;

fn info(msg: &str) {
    eprintln!("{}", format!("[INFO] {}", msg).green())
}
fn warn(msg: &str) {
    eprintln!("{}", format!("[WARN] {}", msg).yellow())
}
fn error(msg: &str) {
    eprintln!("{}", format!("[ERROR] {}", msg).red())
}

fn main() -> Result<()> {
    let address = "127.0.0.1";
    let port = 8080;

    let mut conn = PubHubConnection::new((address, port))
        .expect("Failed to connect to the server. Is it running?");

    let reqs = vec![
        Request::CreateChannel("datetime".into()),
        Request::Subscribe("nproc".into()),
        Request::Publish {
            channel: "nproc".into(),
            content: "hello!".into(),
        },
        Request::Unsubscribe("nproc".into()),
        Request::Ask,
    ];

    for request in reqs {
        conn.execute(request).unwrap();
    }
    
    loop {}

    Ok(())
}
