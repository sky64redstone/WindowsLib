#include "socket.hpp"

namespace winLib {
	Socket::Socket(PCSTR port) {
		this->port = port;
		this->sock = INVALID_SOCKET;

		if (const int error = initialize_winsock())
			throw std::system_error(error, std::system_category());

		if (const int error = create_socket())
			throw std::system_error(error, std::system_category());
	}

	Socket::~Socket() {
		WSACleanup();
	}

	int Socket::initialize_winsock() {
		if (const int result = WSAStartup(WINSOCK_VERSION, &wsa_data)) {
			std::cout << "\nERROR: WSAStartup failed: " << result << std::endl;
			return result;
		}
		return 0;
	}
}