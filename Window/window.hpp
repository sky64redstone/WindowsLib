#pragma once
#ifndef WINDOWS_WINDOW_WINDOW_HPP
	#define WINDOWS_WINDOW_WINDOW_HPP

	#include "util.hpp"
	#include "input.hpp"
	#include "image.hpp"
	#include "vector2.hpp"

	namespace winLib {
		constexpr unsigned int UNLIMITED_FRAME_RATE = 1000;

		struct RenderState {
			unsigned int width, height;
			unsigned int* memory;

			BITMAPINFO info;
		};

		enum PolygonStructure {
			LINE,
			LIST,
			STRIP,
			FAN
		};

		enum AlphaMode {
			NONE,
			MASK,
			ALPHA
		};

		class Window {
		public:
			static LRESULT __stdcall window_callback(HWND window_handle, _In_ UINT message, _In_ WPARAM wparam, _In_ LPARAM lparam) noexcept;

			GDI_PLUS_IMAGE_LOADER gdi_plus_image_loader;

			RenderState state;
			AlphaMode mode;

			KeyBoard keyboard;
			Mouse	 mouse;

			HWND window_handle;

			HDC device_context_handle;

			void (*onInitalise)(Window* window);
			void (*onUpdate)(Window* window, float delta);
			void (*onTerminate)(Window* window);

			float blendFactor;
			float minDelta;

			bool running;

			Window() noexcept;

			void create(int width = CW_USEDEFAULT, int height = CW_USEDEFAULT, DWORD window_style = WS_OVERLAPPEDWINDOW, HINSTANCE instance = nullptr, NATIV_CURSOR cursor = NATIV_CURSOR::ARROW) noexcept;
			void start(int showCMD = SW_SHOW, unsigned int maxFrameRate = 60) noexcept;

			LRESULT window_event(UINT message, WPARAM wparam, LPARAM lparam) noexcept;

			// window parameter methods

			void setTitle(const std::string title);
			std::string getTitle();
			int getWidth() const noexcept;
			int getHeight() const noexcept;
			
			// render methods

			void setAlphaMode(AlphaMode mode) noexcept;
			void setBlendFactor(float factor) noexcept;

			void clearScreen(unsigned int color_code = 0xFF000000) const noexcept;
			bool draw(unsigned int x, unsigned int y, unsigned int color_code = 0xFFFFFFFF) const noexcept;
			void drawLine(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, unsigned int color_code = 0xFFFFFFFF, unsigned int pattern = 0xFFFFFFFF) const;
			void drawRect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int color_code = 0xFFFFFFFF, unsigned int pattern = 0xFFFFFFFF) const;
			void fillRect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int color_code = 0xFFFFFFFF) const;
			void drawTriangle(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, unsigned int x3, unsigned int y3, unsigned int color_code = 0xFFFFFFFF, unsigned int pattern = 0xFFFFFFFF) const;
			void fillTriangle(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, unsigned int x3, unsigned int y3, unsigned int color_code = 0xFFFFFFFF);
			void fillTexturedTriangle(const std::span<vector2u>& screen_pos, const std::span<vector2f>& texture_pos, const std::span<unsigned int>& colors, Image* texture);
			void fillPolygon(const std::span<vector2u>& points, unsigned int color_code = 0xFFFFFFFF, unsigned char structure = PolygonStructure::LIST);
			void fillTexturedPolygon(const std::span<vector2u>& screen_pos, const std::span<vector2f>& texture_pos, const std::span<unsigned int>& colors, Image* texture, unsigned char structure = PolygonStructure::LIST);
			void drawCircle(int x, int y, int radius, unsigned int color_code = 0xFFFFFFFF, unsigned char mask = 0xFF) const noexcept;
			void fillCircle(int x, int y, int radius, unsigned int color_code = 0xFFFFFFFF) const;
			void drawImage(unsigned int x, unsigned int y, Image* image, unsigned int scale = 1, unsigned char flip = Image::Flip::NONE) const noexcept;
			void drawImageF(unsigned int x, unsigned int y, Image* image, float scale = 1.f, unsigned char flip = Image::Flip::NONE) const noexcept;
			void drawSubImage(unsigned int x, unsigned int y, unsigned int src_x, unsigned int src_y, unsigned int w, unsigned h, Image* image, unsigned int scale = 1, unsigned char flip = Image::Flip::NONE) const noexcept;
			//void drawString(unsigned int x, unsigned int y, const char* text, const Font* font, unsigned int color_code = 0xFFFFFFFF, unsigned int scale = 1, unsigned char flip = Font::Flip::NONE);

			unsigned int colorLerp(unsigned int color1, unsigned int color2, float t) noexcept;
			unsigned long getFrameRateOfMonitor() noexcept;

			// cursor

			void setCursor(const HCURSOR cursor) noexcept;
			void showCursor(bool visible = true) noexcept;

			// input

			bool isKeyDown(const unsigned short key_code) const noexcept;
			bool isKeyUp  (const unsigned short key_code) const noexcept;
			bool isMouseDown(const unsigned short mouse_code) const noexcept;
			bool isMouseUp  (const unsigned short mouse_code) const noexcept;
			unsigned short getMouseX() const noexcept;
			unsigned short getMouseY() const noexcept;
			vector2u getMousePos()     const noexcept;
			short getMouseWheelDelta() const noexcept;
		};
	}

	#ifdef WINDOWS_WINDOW_INCLUDE_CPP
		#include <image.cpp>
		#include <window.cpp>
		#include <util.cpp>
		#include <input.cpp>
	#endif

#endif