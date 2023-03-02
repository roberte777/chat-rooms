#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>
#include <vector>
#define BUFFER_SIZE 1024
std::mutex mutex;
std::vector<int> client_sockets;
int server_socket;

void send_message(const std::string &message, int client_socket) {
  std::string message_with_name =
      "Client " + std::to_string(client_socket) + ": " + message;
  mutex.lock();
  for (int i : client_sockets) {
    if (i != client_socket) {
      send(i, message_with_name.c_str(), message_with_name.length(), 0);
    }
  }
  mutex.unlock();
}

void handle_client(int client_socket) {
  std::printf("Client connected\n");
  // Handle data transfer with client
  char buffer[BUFFER_SIZE];
  while (true) {
    // reset buffer
    memset(buffer, 0, sizeof(buffer));
    int bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
    if (bytes_received < 0) {
      std::printf("Error receiving data\n");
      break;
    } else if (bytes_received == 0) {
      std::printf("Client disconnected\n");
      break;
    } else {
      std::string message(buffer, 0, bytes_received);
      send_message(message, client_socket);
    }
  }
  close(client_socket);
  mutex.lock();
  client_sockets.erase(
      std::remove(client_sockets.begin(), client_sockets.end(), client_socket),
      client_sockets.end());
  mutex.unlock();
}

int main(int argc, char *argv[]) {
  // first param is domain. This is technically supposed to be AF_LOCAL for
  // local communication, but whatever
  // second param is type. This is the type of socket. SOCK_STREAM is a
  // TCP socket, SOCK_DGRAM is a UDP socket
  // third param is protocol. 0 is IP
  server_socket = socket(AF_INET, SOCK_STREAM, 0);
  if (server_socket == -1) {
    std::cerr << "Error creating socket" << std::endl;
    return -1;
  }

  // Bind socket to IP address and port
  sockaddr_in server_address;
  server_address.sin_family = AF_INET;
  server_address.sin_port = htons(8080); // Choose any available port
  server_address.sin_addr.s_addr =
      INADDR_ANY; // Bind to any available interface, makes use localhost

  if (bind(server_socket, (sockaddr *)&server_address,
           sizeof(server_address)) == -1) {
    std::cerr << "Error: could not bind socket to IP address and port"
              << std::endl;
    return -1;
  }
  // Listen for incoming connections
  if (listen(server_socket, 5) == -1) { // Backlog of 5 pending connections
    std::cerr << "Error: could not listen for incoming connections"
              << std::endl;
    return -1;
  }

  // Accept incoming connections and handle data transfer
  signal(SIGINT, [](int) {
    for (int i : client_sockets) {
      close(i);
    }
    close(server_socket);
    std::printf("Server closed\n");
    exit(0);
  });
  while (true) {
    sockaddr_in client_address;
    socklen_t client_address_size = sizeof(client_address);

    int client_socket = accept(server_socket, (sockaddr *)&client_address,
                               &client_address_size);
    if (client_socket == -1) {
      std::cerr << "Error: could not accept incoming connection" << std::endl;
      continue;
    }
    mutex.lock();
    client_sockets.push_back(client_socket);
    mutex.unlock();
    std::thread t(handle_client, client_socket);
    t.detach();
  }
  close(server_socket);
  return 0;
}
