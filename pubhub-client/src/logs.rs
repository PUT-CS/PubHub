use colored::Colorize;

pub fn info(msg: &str) {
    eprintln!("{}", format!("[INFO] {}", msg).green())
}
pub fn warn(msg: &str) {
    eprintln!("{}", format!("[WARN] {}", msg).yellow())
}
pub fn error(msg: &str) {
    eprintln!("{}", format!("[ERROR] {}", msg).red())
}
