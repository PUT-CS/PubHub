use serde_json::{Map, Value};
use strum::Display;

#[derive(Debug, strum::IntoStaticStr, Display, Clone)]
pub enum Request {
    Subscribe(String),
    Unsubscribe(String),
    CreateChannel(String),
    DeleteChannel(String),
    Publish { channel: String, content: String },
    Ask,
}

impl Request {
    pub fn to_json(self) -> serde_json::Value {
        let mut m = Map::new();

        m.insert("kind".into(), Value::String(self.to_string()));

        use Request as R;
        let new_values: Vec<(&str, String)> = match self {
            R::Subscribe(name)
            | R::Unsubscribe(name)
            | R::CreateChannel(name)
            | R::DeleteChannel(name) => {
                vec![("target", name)]
            }
            R::Publish { channel, content } => vec![("channel", channel), ("content", content)],
            R::Ask => vec![],
        };

        for (k, v) in new_values {
            m.insert(k.to_string(), Value::String(v));
        }


        Value::Object(m)
    }
}

#[cfg(test)]
mod test {
    use serde_json::json;
    use crate::request::Request;

    #[test]
    fn subscribe_request_to_json() {
        let r = Request::Subscribe("test".into());
        let j = json!({
            "kind" : "Subscribe",
            "target" : "test"
        });
        assert_eq!(r.to_json(), j);
    }
    #[test]
    fn unsubscribe_request_to_string() {
        let r = Request::Unsubscribe("test".into());
        let j = json!({
            "kind" : "Unsubscribe",
            "target" : "test"
        });
        assert_eq!(r.to_json(), j);
    }

    #[test]
    fn create_request_to_string() {
        let r = Request::CreateChannel("test".into());
        let j = json!({
            "kind" : "CreateChannel",
            "target" : "test"
        });
        assert_eq!(r.to_json(), j);
    }

    #[test]
    fn delete_request_to_string() {
        let r = Request::DeleteChannel("test".into());
        let j = json!({
            "kind" : "DeleteChannel",
            "target" : "test"
        });
        assert_eq!(r.to_json(), j);
    }

    #[test]
    fn publish_request_to_string() {
        let r = Request::Publish {
            channel: "test".into(),
            content: "message".into(),
        };
        let j = json!({
            "kind" : "Publish",
            "channel" : "test",
            "content" : "message"
        });
        assert_eq!(r.to_json(), j);
    }

    #[test]
    fn ask_request_to_string() {
        let r = Request::Ask;
        let s = json!({
                "kind" : "Ask"
        });
        assert_eq!(r.to_json(), s);
    }
}
