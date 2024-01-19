#ifndef WINDOWS_SOCKET_SOCKET_HPP
	#define WINDOWS_SOCKET_SOCKET_HPP

	// https://learn.microsoft.com/de-de/windows/win32/winsock/getting-started-with-winsock

	#include "util.hpp"

	namespace winLib {
		class Socket {
		public:
			WSADATA   wsa_data;
			SOCKET    sock;
			addrinfo* result = nullptr;
			addrinfo* ptr = nullptr;
			addrinfo  hints{};
			PCSTR     port;

			Socket(PCSTR port = DEFAULT_PORT);
			~Socket();

			int initialize_winsock();

			virtual int create_socket() null_method;
			virtual int send_data(char* data, int length) null_method;
			virtual int receive_data(char* buffer, int length = DEFAULT_BUFLEN) null_method;
			virtual bool stop() null_method;
			virtual bool disconnect() null_method;
		};
	}
#endif