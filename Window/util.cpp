#include "util.hpp"

namespace winLib {
	std::wstring convertS2W(std::string s) {
		#ifdef __MINGW32__
			wchar_t* buffer = new wchar_t[s.length() + 1];
			mbstowcs(buffer, s.c_str(), s.length());
			buffer[s.length()] = L'\0';
		#else
			const int count = MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, NULL, 0);
			wchar_t* buffer = new wchar_t[count];
			MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, buffer, count);
		#endif
		std::wstring w(buffer);
		delete[] buffer;
		return w;
	}

	std::string  convertW2S(std::wstring s) {
		#ifdef UNICODE
			#ifdef __MINGW32__
				unsigned long long len = s.length();
				char* buffer = new char[len + 1];
				wcstombs(buffer, s.c_str(), len);
				buffer[len] = '\0';
			#else
				const int count = WideCharToMultiByte(CP_UTF8, 0, s.c_str(), -1, nullptr, 0, nullptr, nullptr);
				char* buffer = new char[count];
				WideCharToMultiByte(CP_UTF8, 0, s.c_str(), -1, buffer, count, nullptr, nullptr);
			#endif				
			return std::string(buffer);
			delete[] buffer;
		#else
			return std::string(s.c_str());
		#endif
	}
}