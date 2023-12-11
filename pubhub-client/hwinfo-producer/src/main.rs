use std::net::Ipv4Addr;

use pubhub_rs::{PubHubConnection, request::Request};
use serde_json::json;
use sysinfo::{ComponentExt, NetworkExt, System, SystemExt};
use::std::time::Duration;

fn main() {
    let mut connection = PubHubConnection::new((Ipv4Addr::LOCALHOST, 8080)).unwrap();
    let create_request = Request::CreateChannel("hwinfo".to_string());
    let _ = connection.execute(&create_request).expect("Unable to create channel");
    
    loop {
	let message = get_system_info();
	std::thread::sleep(Duration::from_secs(1));
	let publish_request = Request::Publish { channel: "hwinfo".to_string(), content: message };
	let _ = connection.execute(&publish_request).expect("Unable to publish message");
    }
}

fn get_system_info() -> serde_json::Value {
    let mut sys = System::new_all();

    // First we update all information of our `System` struct.
    sys.refresh_all();

    let mut networks = String::new();
    // Network interfaces name, data received and data transmitted:
    for (interface_name, data) in sys.networks() {
	networks += &(interface_name.to_owned() + " " + &data.received().to_string() + " " + &data.transmitted().to_string());
    }
    
    let mut components = String::new();
    // Components temperature:
    for component in sys.components() {
        components += &(component.label().to_owned() + " " + &component.temperature().to_string() + " ");
    }

    let mut system = String::new();
    // RAM and swap information:
    system += &("total memory ".to_owned()
        + &sys.total_memory().to_string()
        + " used memory  "
        + &sys.used_memory().to_string()
        + " total swap "
        + &sys.total_swap().to_string()
        + " used swap "
        + &sys.used_swap().to_string());
    json!({
	"networks": networks.trim(),
	"components": components.trim(),
	"system": system.trim()
    })
}
