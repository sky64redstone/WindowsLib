#ifndef WINDOWS_SOCKET_SERVER_HPP
	#define WINDOWS_SOCKET_SERVER_HPP

	#include "socket.hpp"

	// TODO:
	// https://learn.microsoft.com/en-us/windows/win32/api/winsock2/nf-winsock2-select

	namespace winLib {
		class ServerSocket : public Socket {
		public:
			ServerSocket(LPCSTR port = DEFAULT_PORT);

			int create_socket() override;
			bool bind_socket();
			int listen_state();
			SOCKET accept_client();
			int send_data(char* data, int length) override;
			int recive_data(char* buffer, int length = DEFAULT_BUFLEN) override;
			bool stop() override;
			bool disconnect() override;
		};
	}
#endif