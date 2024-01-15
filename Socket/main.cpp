#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "client.hpp"
#include "server.hpp"

// TODO: Dual-Stack Sockets
// https://learn.microsoft.com/en-gb/windows/win32/winsock/dual-stack-sockets

int main() {
	winLib::ClientSocket client("www.google.com");

	std::cout << "\nconnectinging...\n";
	if (const int r = client.connect_socket())
		return r;
	std::cout << "\ndisconnecting...\n";
	if (const int r = client.disconnect())
		return r;

	return 0;
}
