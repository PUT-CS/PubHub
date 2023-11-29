use clap::ArgMatches;
use cli::cli;
use cli::handle_cli_input;
use logs::info;
use pubhub::connection::PubHubConnection;
use std::time::Duration;

mod cli;
mod handlers;
mod logs;
mod pubhub;

type Result<T> = std::result::Result<T, anyhow::Error>;

fn main() -> Result<()> {
    let matches = cli().get_matches();
    let requests = handle_cli_input(&matches);

    let address = "127.0.0.1";
    let port = 8080;

    let mut conn = PubHubConnection::new((address, port))
        .expect("Failed to connect to the server. Is it running?");

    for request in requests {
        let res = conn.execute(request)?;
        info(format!("Response: {res:?}").as_str());
    }

    loop {
        std::thread::sleep(Duration::from_millis(100));
    }
}
