# RemoteLink

**RemoteLink** is a remote desktop control and screen-sharing application written entirely in **C++**, using **ImGui** for the user interface and **raw Linux sockets** for communication.

This project is a low-level, from-scratch implementation that showcases advanced C++ and cybersecurity skills by avoiding all high-level networking libraries. Everything, from TCP logic to data transmission, is handcrafted to emphasize full control over the network stack.

## ✨ Features

- 🖥️ **Remote Control** – Interact with a remote machine’s desktop in real-time.
- 📺 **Screen Streaming** – Transmit and view the target screen with minimal delay.
- 🔐 **Basic Security** – Simple encryption to protect communication.
- ⚙️ **Raw TCP-like Protocol** – Custom, self-built reliable protocol over raw sockets (no `bind`/`listen`/`accept` helpers).
- 🧠 **From Scratch Networking** – No external networking frameworks; all socket logic is manual and low-level.

## 🛠️ Tech Stack

- **C++** – Core implementation.
- **ImGui** – Lightweight, cross-platform GUI.
- **Linux Raw Sockets** – Direct control over networking at the OS level.

## 🎯 Purpose

RemoteLink is a demonstration of raw socket programming skills with a focus on:

- Manual TCP-like protocol handling.
- Cybersecurity principles.
- Full-stack understanding of how remote desktop tools function under the hood.

## 📄 License

This project is licensed under the [MIT License](LICENSE).
