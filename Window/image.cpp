#include "image.hpp"

namespace winLib {
	void GDI_PLUS_IMAGE_LOADER::gdi_plus_startup() noexcept {
		const Gdiplus::GdiplusStartupInput startupInput;
		GdiplusStartup(&token, &startupInput, nullptr);
	}

	void GDI_PLUS_IMAGE_LOADER::gdi_plus_shutdown() noexcept {
		Gdiplus::GdiplusShutdown(token);
	}

	Image::Image(unsigned int width, unsigned int height) {
		this->width = width;
		this->height = height;
		this->memory = new unsigned int[width * height];
	}

	Image::Image(std::string& image_file) {
		if (!load_image_file(image_file)) {
			// TODO handle error
		}
	}

	Image::~Image() {
		if (this->memory)
			delete this->memory;
	}

	unsigned int Image::getColor(unsigned int x, unsigned int y) noexcept {
		if (x < 0 || x >= width || y < 0 || y >= height || !memory)
			return 0x00000000;

		return *(memory + x + y * width);
	}

	bool Image::setColor(unsigned int x, unsigned int y, unsigned int color_code) noexcept {
		if (x < 0 || x >= width || y < 0 || y >= height || !memory)
			return false;

		*(memory + x + y * width) = color_code;

		return true;
	}

	Image* Image::getSubimage(unsigned int src_x, unsigned int src_y, unsigned int width, unsigned int height) {
		Image* subimage = new Image(width, height);

		for (unsigned int i = 0; i < width; i++) {
			for (unsigned int j = 0; j < height; j++) {
				subimage->setColor(i, j, this->getColor(src_x + i, src_y + j));
			}
		}

		return subimage;
	}

	bool Image::load_image_file(const std::string& image_file) {
		// clear out existing image
		if (this->memory)
			delete this->memory;

		// Check file exists
		if (!std::filesystem::exists(image_file)) {
			OutputDebugString(Txt("The input file doesn't exist!"));
			return false;
		}

		// Load image from file
		Gdiplus::Bitmap* bmp = Gdiplus::Bitmap::FromFile(convertS2W(image_file).c_str());

		if (bmp->GetLastStatus() != Gdiplus::Ok)
			return false;

		this->width = bmp->GetWidth();
		this->height = bmp->GetHeight();

		this->memory = new unsigned int[this->width * this->height];

		for (int y = 0; y < static_cast<long long>(height); y++)
			for (int x = 0; x < static_cast<long long>(width); x++) {
				Gdiplus::Color c;
				bmp->GetPixel(x, y, &c);
				this->setColor(static_cast<unsigned int>(x), static_cast<unsigned int>(y), static_cast<unsigned int>(c.GetValue()));
			}

		delete bmp;

		return true;
	}
}
