#ifndef WINDOWS_SOCKET_CLIENT_HPP
	#define WINDOWS_SOCKET_CLIENT_HPP

	#include "socket.hpp"

	namespace winLib {
		class ClientSocket : public Socket {
		public:
			PCSTR host_name;

			ClientSocket(PCSTR host_name, PCSTR port = DEFAULT_PORT);

			int create_socket() override;
			int connect_socket();
			int send_data(char* data, int length) override;
			int receive_data(char* buffer, int length = DEFAULT_BUFLEN) override;
			bool stop() override;
			bool disconnect() override;
		};
	}
#endif