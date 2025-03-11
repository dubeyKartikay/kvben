#include <MuserBinding.hpp>
#include <arpa/inet.h>
#include <cstdint>
#include <cstring>
#include <endian.h>
#include <fcntl.h>
#include <iostream>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>

MustoreBinding::MustoreBinding(const std::string &ip, int port)
    : ip(ip), port(port) {
  std::cout << "Connecting to Mustore at " << ip << ":" << port << std::endl;
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0) {
    perror("Socket creation error");
    std::exit(EXIT_FAILURE);
  }
  struct timeval tv;
  tv.tv_sec = 5; /* 30 Secs Timeout */
  setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (struct timeval *)&tv,
             sizeof(struct timeval));

  sockaddr_in serv_addr;
  std::memset(&serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(port);
  if (inet_pton(AF_INET, ip.c_str(), &serv_addr.sin_addr) <= 0) {
    std::cerr << "Invalid address/ Address not supported: " << ip << "\n";
    std::exit(EXIT_FAILURE);
  }
  if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
    perror("Connection Failed");
    std::exit(EXIT_FAILURE);
  }
  // int flags = fcntl(sockfd, F_GETFL, 0);
  // fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);
  std::cout << "Connected to Mustore at " << ip << ":" << port << std::endl;
  this->server_fd = sockfd;
}

MustoreBinding::~MustoreBinding() { close(this->server_fd); }

int64_t MustoreBinding::set(const std::string &key,
                            const std::vector<std::string> &fieldnames,
                            const std::vector<std::string> &fieldvalues) {
  std::string message = "SET_FIELDS " + key + " ";
  for (int i = 0; i < fieldnames.size(); i++) {
    message += fieldnames[i] + "=\"" + fieldvalues[i] + "\" ";
  }
  message += "\n";
  u_int64_t len = message.size();
  u_int64_t len_network_order = htobe64(len);
  char *buffer = (char *)malloc(len + sizeof(u_int64_t));
  std::memcpy(buffer, &len_network_order, sizeof(u_int64_t));
  std::memcpy(buffer + sizeof(u_int64_t), message.c_str(), len);
  std::lock_guard<std::mutex> lock(this->mutex);
  int64_t sentBytes = ::send(this->server_fd, buffer, len + sizeof(len), 0);
  std::free(buffer);
  // std::cout << "Sent " << sentBytes << " bytes" << std::endl;
  return sentBytes;
}

int64_t MustoreBinding::get(const std::string &key) {
  std::string message = "GETALL " + key + "\n";
  u_int64_t len = message.size();
  u_int64_t len_network_order = htobe64(len);
  char *buffer = (char *)malloc(len + sizeof(u_int64_t));
  std::memcpy(buffer, &len_network_order, sizeof(u_int64_t));
  std::memcpy(buffer + sizeof(u_int64_t), message.c_str(), len);
  std::lock_guard<std::mutex> lock(this->mutex);
  int64_t sentBytes = ::send(this->server_fd, buffer, len + sizeof(len), 0);
  std::free(buffer);
  // std::cout << "Sent " << sentBytes << " bytes" << std::endl;
  return sentBytes;
}

std::string MustoreBinding::recieve() {
  std::string result;
  char buffer[1024];
  while (true) {
    int valread = recv(this->server_fd, buffer, sizeof(buffer), 0);
    if (valread < 0) {
      perror("recv failed");
      return "";
    }
    if (valread == 0) {
      break;
    }
    result.append(buffer, valread);
    if (result.find('\n') != std::string::npos || valread < sizeof(buffer)) {
      break;
    }
  }
  return result;
}
