#ifndef WINDOWS_WINDOW_UTIL_HPP
	#define WINDOWS_WINDOW_UTIL_HPP

	constexpr auto WINDOWS_WINDOW_LIBRARY_VERSION = 0.1f;

	#include <windows.h>
	#include <iostream>
	#include <span>
	#include <filesystem>
	#include <string>
	#include <cmath>

	#if defined(UNICODE) || defined(_UNICODE)
		#define Txt(s) L##s
	#else
		#define Txt(s) s
	#endif

	#if defined(DEBUG) || defined(_DEBUG) || defined(FORCE_DEBUG)
		#define DEBUGNUMBER(str, size, ...) {			\
			char message[size];							\
			sprintf_s(message, str, __VA_ARGS__);		\
			OutputDebugStringA(message);				\
		}												\

		#define DEBUGSTRING(str, size, ...) {			\
			wchar_t buffer[size];						\
			swprintf(buffer, size, str, __VA_ARGS__);	\
			OutputDebugString(buffer);					\
		}												\

	#else
		#define DEBUGNUMBER(str, size, ...)
		#define DEBUGSTRING(str, size, ...)
	#endif

	#define nameof(a) #a

	#define getAlphaColorValue(color_code) ((color_code & 0xFF000000) >> 24)
	#define getRedColorValue(color_code)   ((color_code & 0x00FF0000) >> 16)
	#define getGreenColorValue(color_code) ((color_code & 0x0000FF00) >> 8)
	#define getBlueColorValue(color_code)   (color_code & 0x000000FF)

	// parameters in unsigned char [0;255]
	#define rgbColorCode(red, green, blue) (static_cast<unsigned int>(static_cast<unsigned char>(red) | (static_cast<unsigned char>(green) << 8) | (static_cast<unsigned char>(blue) << 16)))
	// parameters in unsigned char [0;255]
	#define rgbaColorCode(red, green, blue, alpha) (static_cast<unsigned int>(static_cast<unsigned char>(red) | (static_cast<unsigned char>(green) << 8) | (static_cast<unsigned char>(blue) << 16) | (static_cast<unsigned char>(alpha) << 24)))
	// parameters in float [0;1]
	#define rgbColorCodeF(red, green, blue) rgbColorCode(red * 255, green * 255, blue * 255)
	// parameters in float [0;1]
	#define rgbaColorCodeF(red, green, blue, alpha) rgbaColorCode(red * 255, green * 255, blue * 255, alpha * 255)

	#undef min
	#undef max

	namespace winLib {
		std::wstring convertS2W(std::string s);
		std::string  convertW2S(std::wstring s);
	}
#endif