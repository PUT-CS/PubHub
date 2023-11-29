use crate::pubhub::request::Request;

pub fn listen_requests(channels: &Vec<&String>) -> Vec<Request> {
    channels
        .iter()
        .map(|name| Request::Subscribe(name.to_string()))
        .collect()
}

pub fn publish_requests(channels: &Vec<&String>, message: String) -> Vec<Request> {
    channels
        .iter()
        .map(|name| Request::Publish {
            channel: name.to_string(),
            content: message.to_string(),
        })
        .collect()
}

pub fn create_requests(names: &Vec<&String>) -> Vec<Request> {
    names
        .iter()
        .map(|name| Request::CreateChannel(name.to_string()))
        .collect()
}

pub fn delete_requests(names: &Vec<&String>) -> Vec<Request> {
    names
        .iter()
        .map(|name| Request::DeleteChannel(name.to_string()))
        .collect()
}

pub fn requests_from_targets(
    channels: &Vec<String>,
    f: impl Fn(&String) -> Request,
) -> Vec<Request> {
    channels.iter().map(f).collect()
}
