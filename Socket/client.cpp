#include "client.hpp"

namespace winLib {
	ClientSocket::ClientSocket(PCSTR host_name, PCSTR port) : Socket(port) {
		this->host_name = host_name;
	}

	int ClientSocket::create_socket() {
		ZeroMemory(&hints, sizeof(hints));
		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = IPPROTO_TCP;

		// Resolve the server address and port
		if (const int r = getaddrinfo(host_name, port, &hints, &result)) {
			std::cout << "\nERROR: getaddrinfo failed: " << r << std::endl;
			return r;
		}

		return 0;
	}

	int ClientSocket::connect_socket() {

		if (result == nullptr)
			std::cout << "(connect)result is nullptr";

		for (ptr = result; ptr != nullptr; ptr = ptr->ai_next) {

			sock = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);

			if (sock == INVALID_SOCKET) {
				std::cout << "\nERROR: socket failed: " << WSAGetLastError() << std::endl;
				freeaddrinfo(result);
				return WSAGetLastError();
			}

			if (connect(sock, ptr->ai_addr, (int)ptr->ai_addrlen) == SOCKET_ERROR) {
				closesocket(sock);
				sock = INVALID_SOCKET;
				continue;
			}

			break;
		}

		freeaddrinfo(result);

		if (sock == INVALID_SOCKET) {
			std::cout << "\nERROR: unable to connect to server!\n";
			const int err = WSAGetLastError();
			// return -1 if the result variable was a nullptr -> err == 0
			return err == 0 ? -1 : err;
		}

		return 0;
	}

	int ClientSocket::send_data(char* data, int length) {
		int bytes_sent;

		if ((bytes_sent = send(sock, data, length, 0)) == SOCKET_ERROR) {
			std::cout << "\nERROR: send failed: " << WSAGetLastError() << std::endl;
			return SOCKET_ERROR;
		}

		return bytes_sent;
	}

	int ClientSocket::receive_data(char* buffer, int length) {
		int bytes_recv;

		if ((bytes_recv = recv(sock, buffer, length, 0)) == SOCKET_ERROR) {
			std::cout << "\nERROR: recive failed: " << WSAGetLastError() << std::endl;
			return SOCKET_ERROR;
		}

		return bytes_recv;
	}

	bool ClientSocket::stop() {
		if (shutdown(sock, SD_SEND) == SOCKET_ERROR) {
			std::cout << "\nERROR: shutdown failed: " << WSAGetLastError() << std::endl;
			closesocket(sock);
			return false;
		}
		return true;
	}

	bool ClientSocket::disconnect() {
		if (closesocket(sock) == SOCKET_ERROR)
			return WSAGetLastError();
		return 0;
	}
}