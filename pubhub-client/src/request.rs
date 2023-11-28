use serde_json::{Map, Value};
use strum::Display;

#[derive(Debug, strum::IntoStaticStr, Display)]
pub enum Request {
    Error,
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

        use Request::*;
        let new_values: Vec<(&str, String)> = match self {
            Subscribe(name) | Unsubscribe(name) | CreateChannel(name) | DeleteChannel(name) => {
                vec![("target", name)]
            }
            Publish { channel, content } => vec![("channel", channel), ("content", content)],
            Ask => vec![],
	    Error => vec![],
        };

        for (k, v) in new_values {
            m.insert(k.to_string(), Value::String(v));
        }

        Value::Object(m)
    }
}
