use crate::error::ConnectionError;

#[derive(serde::Deserialize, Debug)]
pub enum Response {
    Ok { content: String },
    Err { why: String },
}

impl TryFrom<serde_json::Value> for Response {
    type Error = anyhow::Error;
    fn try_from(value: serde_json::Value) -> Result<Self, Self::Error> {
        let kind = value
            .get("responseKind")
            .map(|s| s.to_string())
            .ok_or_else(|| ConnectionError::new("Server sent a response of unspecified kind"))?;

        match kind.as_str() {
            "Ok" => Ok(Response::Ok { content: "".into() }),
            "Error" => match value.get("why") {
                Some(reason) => Ok(Response::Err {
                    why: reason.to_string(),
                }),
                None => Err(ConnectionError::new(
                    "Server sent an error response without giving the reason",
                )
                .into()),
            },
            _ => Err(ConnectionError::new("Server sent a response with an invalid kind").into()),
        }
    }
}
