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
