
// Client side C/C++ program to demonstrate Socket
// programming
#include <arpa/inet.h>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <string>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>
using namespace std;
#define PORT 8080
void recieve(int client_fd) {
  while (true) {
    char buffer[1024] = {0};
    int valread = read(client_fd, buffer, 1024);
    printf("%s\n", buffer);
    if (valread == -1) {
      cout << "Error recieving message" << endl;
      exit(0);
    }
    if (valread == 0) {
      cout << "Server disconnected" << endl;
      exit(0);
    }
  }
}

void send_message(int client_fd) {
  while (true) {
    string message;
    cout << "Enter message: ";
    getline(cin, message);
    int res = send(client_fd, message.c_str(), message.length(), 0);
    if (res == -1) {
      cout << "Error sending message" << endl;
      exit(0);
    }
    if (res == 0) {
      cout << "Server disconnected" << endl;
      exit(0);
    }
  }
}

int main(int argc, char const *argv[]) {
  int client_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (client_fd == -1) {
    printf("Error creating socket");
    return -1;
  }
  if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    printf("\n Socket creation error \n");
    return -1;
  }

  struct sockaddr_in serv_addr;
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(PORT);

  // Convert IPv4 and IPv6 addresses from text to binary
  // form
  if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
    printf("\nInvalid address/ Address not supported \n");
    return -1;
  }
  int status =
      connect(client_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
  if (status < 0) {
    printf("\nConnection Failed \n");
    return -1;
  }

  thread s(send_message, client_fd);
  thread r(recieve, client_fd);
  s.detach();
  r.detach();
  while (true) {
    signal(SIGINT, [](int) { exit(0); });
  }

  return 0;
}
