use clap::arg;
use clap::Command;
use connection::{PubHubConnection, Request};
use handlers::ask_request;
use handlers::create_requests;
use log::info;
use logs::info;
use std::time::Duration;

use crate::handlers::delete_requests;
use crate::handlers::listen_requests;
use crate::handlers::publish_requests;

mod connection;
mod error;
mod handlers;
mod logs;
mod request;
mod response;

type Result<T> = std::result::Result<T, anyhow::Error>;

fn cli() -> Command {
    Command::new("phc")
        .about("PubHub Client")
        .version("1.0")
        .subcommand_required(true)
        .arg_required_else_help(true)
        .author("Michał Miłek & Sebastian Nowak")
        .subcommand(
            Command::new("listen")
                .short_flag('l')
                .long_flag("listen")
                .about("Listen to messages from one or more channels")
                .arg_required_else_help(true)
                .arg(arg!(<CHANNEL> ... "Channels to listen to")),
        )
        .subcommand(
            Command::new("publish")
                .short_flag('p')
                .long_flag("publish")
                .about("Publish a message on a channel")
                .arg_required_else_help(true)
                .arg(arg!(<CHANNEL> ... "Target channel[s]"))
                .arg(arg!(<MESSAGE> "Your message (string)").last(true)),
        )
        .subcommand(
            Command::new("create")
                .short_flag('c')
                .long_flag("create")
                .about("Create a channel in your PubHub instance")
                .arg_required_else_help(true)
                .arg(arg!(<NAME> ... "Name[s] of the new channel[s]")),
        )
        .subcommand(
            Command::new("delete")
                .short_flag('d')
                .long_flag("delete")
                .about("Delete a channel in your PubHub instance")
                .arg_required_else_help(true)
                .arg(arg!(<NAME> ... "Name[s] of the deleted channel[s]")),
        )
        .subcommand(
            Command::new("ask")
                .short_flag('a')
                .long_flag("ask")
                .about("Ask the server for a list of available channels"),
        )
}

fn main() -> Result<()> {
    let matches = cli().get_matches();

    let requests = match matches.subcommand() {
        Some(("listen", listen_matches)) => {
            let channels: Vec<_> = listen_matches
                .get_many::<String>("CHANNEL")
                .expect("required")
                .into_iter()
                .collect();
	    
	    listen_requests(&channels)
        }
        Some(("publish", publish_matches)) => {
            let channels: Vec<_> = publish_matches
                .get_many::<String>("CHANNEL")
                .expect("required")
                .into_iter()
                .collect();
            let message: String = publish_matches
                .get_one::<String>("MESSAGE")
                .expect("required")
                .to_string();
	    
	    publish_requests(&channels, message)
        }
        Some(("create", create_matches)) => {
            let channels: Vec<_> = create_matches
                .get_many::<String>("NAME")
                .expect("required")
                .into_iter()
                .collect();
	    
	    create_requests(&channels)
        }
        Some(("delete", create_matches)) => {
            let channels: Vec<_> = create_matches
                .get_many::<String>("NAME")
                .expect("required")
                .into_iter()
                .collect();

	    delete_requests(&channels)
        }
        Some(("ask", _)) => {
	    vec![ask_request()]
        }
        _ => unreachable!(),
    };

    let address = "127.0.0.1";
    let port = 8080;

    let mut conn = PubHubConnection::new((address, port))
        .expect("Failed to connect to the server. Is it running?");

    // let requests = vec![
    //     Request::CreateChannel("datetime".into()),
    //     Request::Subscribe("nproc".into()),
    //     Request::Publish {
    //         channel: "nproc".into(),
    //         content: "hello!".into(),
    //     },
    //     Request::Unsubscribe("nproc".into()),
    //     Request::Ask,
    // ];

    for request in requests {
        let res = conn.execute(request)?;
        info(format!("Response: {res:?}").as_str());
    }

    loop {
        std::thread::sleep(Duration::from_millis(100));
    }
}
