use std::{net::Ipv4Addr, str::FromStr};

use ::std::time::Duration;
use pubhub_rs::{request::Request, PubHubConnection};
use serde_json::{json, Value};
use sysinfo::{ComponentExt, NetworkExt, System, SystemExt};

fn main() {
    let mut args = std::env::args().skip(1);
    let ip = args.next().unwrap();
    let port: u16 = args.next().unwrap().parse().unwrap();
    let addr = Ipv4Addr::from_str(&ip).unwrap();
    
    let mut connection = PubHubConnection::new((addr, port)).unwrap();
    let create_request = Request::CreateChannel("hwinfo".to_string());
    let _ = connection
        .execute(&create_request)
        .expect("Unable to create channel");

    loop {
        let message = get_system_info();
        std::thread::sleep(Duration::from_secs(1));
        let publish_request = Request::Publish {
            channel: "hwinfo".to_string(),
            content: message,
        };
        println!("Publishing: {publish_request:#?}");
        let _ = connection
            .execute(&publish_request)
            .expect("Unable to publish message");
    }
}

fn get_system_info() -> serde_json::Value {
    let mut sys = System::new_all();

    // First we update all information of our `System` struct.
    sys.refresh_all();

    let mut network_map = serde_json::Map::new();
    // Network interfaces name, data received and data transmitted:
    for (interface_name, data) in sys.networks() {
        network_map.insert(
            interface_name.to_string(),
            json!({
                "received" : data.received().to_string(),
                "transmitted" : data.transmitted().to_string()
            }),
        );
    }
    let networks = Value::from(network_map);

    let mut component_map = serde_json::Map::new();
    for component in sys.components() {
        component_map.insert(
            component.label().to_string(),
            serde_json::Value::String(component.temperature().to_string()),
        );
    }
    let components = Value::from(component_map);

    // RAM and swap information:
    let mem_info = json!({
        "totalMemory" : sys.total_memory().to_string(),
        "usedMemory" : sys.used_memory().to_string(),
        "totalSwap" : sys.total_swap().to_string(),
        "usedSwap" : sys.used_swap().to_string(),
    });

    json!({
        "networks": networks,
        "components": components,
        "memory": mem_info
    })
}
