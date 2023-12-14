use serde_json::Value;

use crate::error::ConnectionError;

#[derive(Debug, Clone)]
pub enum Response {
    Ok { content: String },
    Err { why: String },
}

impl TryFrom<serde_json::Value> for Response {
    type Error = anyhow::Error;
    fn try_from(value: serde_json::Value) -> Result<Self, Self::Error> {
        let kind = match value.get("status") {
            Some(Value::String(s)) => s,
            _ => {
                return Err(ConnectionError::new(
                    "Invalid server response, kind missing or malformated",
                )
                .into());
            }
        };

        match kind.as_str() {
            "Ok" => Ok(Response::Ok { content: "".into() }),
            "Error" => match value.get("info") {
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

#[cfg(test)]
mod test {
    use serde_json::json;

    use crate::response::Response;

    #[test]
    fn response_from_invalid_kind_json() {
        let j = json!({
            "status" : "asdsa",
        });
        assert!(Response::try_from(j).is_err())
    }

    #[test]
    fn response_from_valid_ok_json() {
        let j = json!({
            "status" : "Ok",
        });
        assert!(Response::try_from(j).is_ok())
    }

    #[test]
    fn response_from_invalid_err_json_no_reason() {
        let j = json!({
            "status" : "Error",
        });
        assert!(Response::try_from(j).is_err())
    }

    #[test]
    fn response_from_valid_err_json() {
        let j = json!({
            "status" : "Error",
            "info" : "bad code"
        });
        assert!(Response::try_from(j).is_ok())
    }
}
