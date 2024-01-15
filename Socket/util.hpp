#ifndef WINDOWS_SOCKET_UTIL_HPP
	#define WINDOWS_SOCKET_UTIL_HPP

	constexpr auto WINDOWS_SOCKET_LIBRARY_VERSION = 0.1f;

	constexpr auto DEFAULT_PORT   = "27015";
	constexpr auto DEFAULT_BUFLEN = 512;

	// Ensure that the build environment links to the Winsock Library file 
	// Ws2_32.lib. Applications that use Winsock must be linked with the 
	// Ws2_32.lib library file. The #pragma comment indicates to the linker 
	// that the Ws2_32.lib file is needed.
	#pragma comment(lib, "Ws2_32.lib")

	// The Winsock2.h header file contains most of the Winsock functions, 
	// structures, and definitions.
	#include <winsock2.h>
	// The Ws2tcpip.h header file contains definitions introduced in the 
	// WinSock 2 Protocol-Specific Annex document for TCP/IP that includes 
	// newer functions and structures used to retrieve IP addresses.
	#include <ws2tcpip.h>
	#include <iostream>

	#define null_method { return 0; }

	#define cin_wait system("pause")

#endif