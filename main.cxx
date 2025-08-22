#include <string>
#include <thread>
#include <iostream>
#include <cstdio>
#include <errno.h>

#if defined(__linux__) || defined(__APPLE__)
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#else
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#endif

using namespace std;


// Print error message on failure.
inline void tryError(int ret, const char *code, const char *file, int line) {
  char buf[256];
  if (ret >= 0) return;
  strerror_s(buf, sizeof(buf), errno);
  fprintf(stderr, "ERROR: %s:%d: %s failed: %s\n", file, line, code, buf);
  exit(EXIT_FAILURE);
}

// Test if function call succeeded.
#define TRY(exp) tryError((exp), #exp, __FILE__, __LINE__)


// Define socket datatypes.
#if defined(__linux__) || defined(__APPLE__)
typedef SOCKET int;
typedef sockaddr_in SOCKADDR_IN;
#endif




// Startup sockets API.
inline void startupSockets() {
#if !defined(__linux__) && !defined(__APPLE__)
  WSADATA wsaData;
  TRY( WSAStartup(MAKEWORD(2, 2), &wsaData) );
#endif
}


// Cleanup sockets API.
inline void cleanupSockets() {
#if defined(__linux__) || defined(__APPLE__)
  return;
#else
  TRY( WSACleanup() );
#endif
}


// Close a socket.
inline void closeSocket(SOCKET sock) {
#if defined(__linux__) || defined(__APPLE__)
  TRY( close(sock) );
#else
  TRY( closesocket(sock) );
#endif
}


// Use a socket in unbuffered mode (disable Nagle's algorithm).
inline void setSocketUnbuffered(SOCKET sock) {
  int flag = 1;
  TRY( setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (char*) &flag, sizeof(flag)) );
}


// Launch the server.
inline int runServer(int port) {
  char buf[1024];
  SOCKET server = socket(AF_INET, SOCK_STREAM, 0);  // 0 = default protocol
  SOCKADDR_IN addr;
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = INADDR_ANY;
  addr.sin_port = htons(port);
  TRY( bind(server, (SOCKADDR*) &addr, sizeof(addr)) );
  TRY( listen(server, 5) );  // 5 = backlog (max queued connections)
  printf("Server listening on port %d\n", port);
  while (1) {
    SOCKADDR_IN caddr;
    socklen_t CADDR = sizeof(caddr);
    SOCKET client = accept(server, (SOCKADDR*) &caddr, &CADDR);
    thread th([=]() {
      char buf[1024];
      inet_ntop(AF_INET, &caddr.sin_addr, buf, sizeof(buf));
      printf("Client connected: %s:%d\n", buf, ntohs(caddr.sin_port));
      int r  = recv(client, buf, sizeof(buf), 0);
      buf[r] = '\0';
      printf("Received message: %s (%d bytes)\n", buf, r);
      int s  = send(client, buf, r, 0);
      printf("Sent echo: %s (%d bytes)\n", buf, s);
      closeSocket(client);
    });
    th.detach();
  }
  closeSocket(server);
  return 0;
}


// Launch the client.
inline int runClient(const char *address, int port) {
  char buf[1024];
  SOCKET client = socket(AF_INET, SOCK_STREAM, 0);
  SOCKADDR_IN addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  inet_pton(AF_INET, address, &addr.sin_addr);
  TRY( connect(client, (SOCKADDR*) &addr, sizeof(addr)) );
  const char* msg = "Hello, World!";
  int s = send(client, msg, strlen(msg), 0);
  printf("Sent message: %s (%d bytes)\n", msg, s);
  int r = recv(client, buf, sizeof(buf), 0);
  buf[r] = '\0';
  printf("Received echo: %s (%d bytes)\n", buf, r);
  closeSocket(client);
  return 0;
}


// Main function.
int main(int argc, char **argv) {
  startupSockets();
  const char *mode    = argc>1? argv[1] : "client";
  const char *address = argc>2? argv[2] : "127.0.0.1";
  int port = argc>3? atoi(argv[3]) : 5000;
  if (strcmp(mode, "server") == 0) runServer(port);
  else runClient(address, port);
  cleanupSockets();
  return 0;
}
