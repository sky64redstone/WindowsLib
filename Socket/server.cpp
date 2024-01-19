#include "server.hpp"

namespace winLib {
	ServerSocket::ServerSocket(LPCSTR port) : Socket(port) {}

	int ServerSocket::create_socket() {
		ZeroMemory(&hints, sizeof(hints));
		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = IPPROTO_TCP;
		hints.ai_flags = AI_PASSIVE;

		// Resolve the local address and port to be used by the server
		if (const int r = getaddrinfo(NULL, port, &hints, &result)) {
			std::cout << "\nERROR: getaddrinfo failed: " << r << std::endl;
			return r;
		}

		sock = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

		if (sock == INVALID_SOCKET) {
			std::cout << "\nERROR: socket failed: " << WSAGetLastError() << std::endl;
			freeaddrinfo(result);
			return WSAGetLastError();
		}

		return 0;
	}

	bool ServerSocket::bind_socket() {
		if (bind(sock, result->ai_addr, (int)result->ai_addrlen) == SOCKET_ERROR) {
			std::cout << "\nERROR: bind failed: " << WSAGetLastError() << std::endl;
			freeaddrinfo(result);
			closesocket(sock);
			return false;
		}

		// Once the bind function is called, the address information 
		// returned by the getaddrinfo function is no longer needed. 
		// The freeaddrinfo function is called to free the memory 
		// allocated by the getaddrinfo function for this address
		// information.
		freeaddrinfo(result);

		return true;
	}

	int ServerSocket::listen_state() {
		if (listen(sock, SOMAXCONN) == SOCKET_ERROR) {
			std::cout << "\nERROR: listen failed: " << WSAGetLastError() << std::endl;
			closesocket(sock);
			shutdown(sock, SD_BOTH);
			return WSAGetLastError();
		}

		return 0;
	}

	SOCKET ServerSocket::accept_client() {
		SOCKET client = INVALID_SOCKET;

		if ((client = accept(sock, nullptr, nullptr) == INVALID_SOCKET)) {
			std::cout << "\nERROR: accept failed: " << WSAGetLastError() << std::endl;
			return INVALID_SOCKET;
		}

		return client;
	}

	int ServerSocket::send_data(char* data, int length) {
		int bytes_sent;

		if ((bytes_sent = send(sock, data, length, 0)) == SOCKET_ERROR) {
			std::cout << "\nERROR: send failed: " << WSAGetLastError() << std::endl;
			return SOCKET_ERROR;
		}

		return bytes_sent;
	}

	int ServerSocket::receive_data(char* buffer, int length) {
		int bytes_recv;

		if ((bytes_recv = recv(sock, buffer, length, 0)) == SOCKET_ERROR) {
			std::cout << "\nERROR: recive failed: " << WSAGetLastError() << std::endl;
			return SOCKET_ERROR;
		}

		return bytes_recv;
	}

	bool ServerSocket::stop() {
		if (shutdown(sock, SD_SEND) == SOCKET_ERROR) {
			std::cout << "\nERROR: shutdown failed: " << WSAGetLastError() << std::endl;
			closesocket(sock);
			return false;
		}

		return true;
	}

	bool ServerSocket::disconnect() {
		if (closesocket(sock) == SOCKET_ERROR)
			return WSAGetLastError();
		return 0;
	}
}