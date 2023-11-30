use clap::{arg, ArgMatches, Command};

use crate::{handlers::requests_from_targets, pubhub::request::Request};

pub fn handle_cli_input(matches: &ArgMatches) -> Vec<Request> {
    match matches.subcommand() {
        Some(("listen", listen_matches)) => {
            let channels = get_many_of::<String>(listen_matches, "CHANNEL");
            requests_from_targets(&channels, |name| Request::Subscribe(name.to_owned()))
        }
        Some(("publish", publish_matches)) => {
            let channels = get_many_of::<String>(publish_matches, "CHANNEL");
            let message = get_one_of::<String>(publish_matches, "MESSAGE").to_string();
            requests_from_targets(&channels, |name| Request::Publish {
                channel: name.to_owned(),
                content: message.clone(),
            })
        }
        Some(("create", create_matches)) => {
            let channels = get_many_of::<String>(create_matches, "CHANNEL");
            requests_from_targets(&channels, |name| Request::CreateChannel(name.to_owned()))
        }
        Some(("delete", delete_matches)) => {
            let channels = get_many_of::<String>(delete_matches, "NAME");
            requests_from_targets(&channels, |name| Request::DeleteChannel(name.to_owned()))
        }
        Some(("ask", _)) => {
            vec![Request::Ask]
        }
        _ => unreachable!(),
    }
}

pub fn cli() -> Command {
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

fn get_many_of<'a, T>(matches: &'a ArgMatches, id: &str) -> Vec<T>
where
    T: Send + Sync + Clone + 'static,
{
    matches
        .get_many::<T>(id)
        .expect("required")
        .map(|x| x.to_owned())
        .into_iter()
        .collect()
}
fn get_one_of<'a, T>(matches: &'a ArgMatches, id: &str) -> &'a T
where
    T: Send + Sync + Clone + 'static,
{
    matches.get_one::<T>(id).expect("required")
}
