#ifndef WINDOWS_WINDOW_IMAGE_HPP
	#define WINDOWS_WINDOW_IMAGE_HPP

	#include "util.hpp"

	// I'm sorry... A library but:
	// TODO rewrite all the image loader code!
	#ifndef min
		#define min(a, b) ((a < b) ? a : b)
	#endif
	#ifndef max
		#define max(a, b) ((a > b) ? a : b)
	#endif
	#include <objidl.h>
	#include <gdiplus.h>

	#if defined(__MINGW32__)
		#include <gdiplus/gdiplusinit.h>
	#else
		#include <gdiplusinit.h>
	#endif

	#include <shlwapi.h>

	#if !defined(__MINGW32__)
		#pragma comment(lib, "gdiplus.lib")
		#pragma comment(lib, "Shlwapi.lib")
	#endif

	#undef min
	#undef max

	namespace winLib {
		//I'm sorry
		class GDI_PLUS_IMAGE_LOADER {
		public:
			// I'm sorry
			ULONG_PTR token;

			// I'm sorry
			void gdi_plus_startup() noexcept;
			// I'm sorry
			void gdi_plus_shutdown() noexcept;
		};

		class Image {
		public:
			enum Flip {
				NONE,
				VERT,
				HORIZ
			};

			unsigned int width, height;
			unsigned int* memory;

			Image(unsigned int width, unsigned int height);
			Image(std::string& image_file);
			Image(const Image& image) = delete;
			~Image();

			Image& operator= (const Image&) = delete;

			unsigned int getColor(unsigned int x, unsigned int y) noexcept;
			bool setColor(unsigned int x, unsigned int y, unsigned int color_code = 0xFFFFFFFF) noexcept;

			Image* getSubimage(unsigned int src_x, unsigned int src_y, unsigned int width, unsigned int height);

			bool load_image_file(const std::string& image_file);
		};
	}

#endif