
# Client-Server Chat Application with FAQ Chatbot

This project is a comprehensive Client-Server Chat Application built with a focus on Network Programming. The application supports multiple clients connected to a central server, facilitating peer-to-peer chat, managing chat history, and integrating a chatbot that can answer Frequently Asked Questions (FAQs) using both predefined answers and a GPT-2 language model.

## Table of Contents
- [Features](#features)
- [Requirements](#requirements)
- [Installation](#installation)
- [Usage](#usage)
  - [Starting the Server](#starting-the-server)
  - [Connecting Clients](#connecting-clients)
  - [Available Commands](#available-commands)
- [Chatbot](#chatbot)
  - [FAQ Chatbot](#faq-chatbot)
  - [FAQ Chatbot with GPT-2](#faq-chatbot-with-gpt-2)
- [Error Handling](#error-handling)
- [License](#license)

## Features

- **Peer-to-Peer Chat:** Supports up to 10 concurrent client connections with unique identifiers for each client.
- **Chatbot Integration:** Two chatbot modes:
  - FAQ-based chatbot with predefined answers.
  - GPT-2 based chatbot for more dynamic interactions.
- **Chat History Management:** Clients can retrieve and delete chat history.
- **Command-based Interface:** Allows clients to perform various tasks such as chatting, checking active users, and toggling the chatbot feature.

## Requirements

- **Operating System:** Linux (Ubuntu 22.04 or similar)
- **Dependencies:** 
  - `uuid-dev` library: Install it using the following command:
    ```bash
    sudo apt-get install uuid-dev
    ```

## Installation

Clone the repository and navigate to the project directory:

```bash
git clone <repository-url>
cd <project-directory>
```

## Usage

### Starting the Server

To compile and run the server:

```bash
gcc -o server server.c -luuid
./server
```

### Connecting Clients

Ensure that the server is running before starting the client.

To compile and run the client:

```bash
gcc -o client client.c
./client
```

### Available Commands

- **Chat Commands:** 
  - `/list`: List active users.
  - `/history`: Retrieve chat history.
  - `/deletehistory`: Delete chat history.
  - `/togglebot`: Toggle the chatbot feature.

## Chatbot

### FAQ Chatbot

A simple chatbot with predefined answers for Frequently Asked Questions.

### FAQ Chatbot with GPT-2

A more advanced chatbot that uses the GPT-2 language model to generate responses.

### Important Notes

- **IP Address and Port:** The IP address and port number are not required to run the client, contrary to what may be stated elsewhere.
- **Chatbot Design:** When the chatbot is active, the client can still receive messages from other clients. This is a design choice. If you prefer not to receive other messages when the chatbot is on, uncomment lines 149-151 in `server.c`.

## Error Handling

- The application has basic error handling for network issues, connection timeouts, and command errors.

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for more information.
