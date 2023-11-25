#[derive(serde::Deserialize, Debug)]
pub enum Response {
    Ok { content: String },
    Error { why: String },
}
