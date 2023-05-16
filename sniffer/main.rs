// use std::net::{TcpStream, ToSocketAddrs};

// fn main() {
//     let target = "127.0.0.1";
//     let ports = vec![80, 443, 8080];

//     for port in ports {
//         let addr = (target, port).to_socket_addrs().unwrap().next().unwrap();
//         match TcpStream::connect_timeout(&addr, std::time::Duration::from_secs(1)) {
//             Ok(_) => println!("{}:{} is open", target, port),
//             Err(_) => println!("{}:{} is closed", target, port),
//         }
//     }
// }

use std::net::{IpAddr, TcpStream};
use std::thread;

fn main() {
    let ip = IpAddr::from([127, 0, 0, 1]); // 目标 IP 地址
    let start_port = 1; // 起始端口号
    let end_port = 65535; // 终止端口号

    for port in start_port..=end_port {
        let target = ip.clone();
        thread::spawn(move || {
            if let Ok(_) = TcpStream::connect((target, port)) {
                println!("Port {} is open", port);
            }
        });
    }
}
