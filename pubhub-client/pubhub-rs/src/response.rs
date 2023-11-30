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
        let kind = match value.get("kind") {
            Some(Value::String(s)) => s,
            _ => {
                return Err(ConnectionError::new(
                    "Invalid server response, kind missing or malformated",
                )
                           .into());
            }
        };

        match kind.as_str() {
            "Ok" => {
                dbg!("HERE");
                Ok(Response::Ok { content: "".into() })
            }
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

#[cfg(test)]
mod test {
    use serde_json::json;

    use crate::response::Response;

    #[test]
    fn response_from_invalid_kind_json() {
        let j = json!({
            "kind" : "asdsa",
        });
        assert!(Response::try_from(j).is_err())
    }

    #[test]
    fn response_from_valid_ok_json() {
        let j = json!({
            "kind" : "Ok",
        });
        assert!(Response::try_from(j).is_ok())
    }

    #[test]
    fn response_from_invalid_err_json_no_reason() {
        let j = json!({
            "kind" : "Error",
        });
        assert!(Response::try_from(j).is_err())
    }

    #[test]
    fn response_from_valid_err_json() {
        let j = json!({
            "kind" : "Error",
            "why" : "bad code"
        });
        assert!(Response::try_from(j).is_ok())
    }
}
