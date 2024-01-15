#include "window.hpp"

winLib::Image img(1, 1);

static void init(winLib::Window* wnd) {
	// choose a path for a image
	char mypicturespath[MAX_PATH];
	HRESULT result = SHGetFolderPathA(NULL, CSIDL_MYPICTURES, NULL, SHGFP_TYPE_CURRENT, mypicturespath);
	std::string filePath = std::string(mypicturespath) + "\\Windows Wallpaper\\1893692.jpg";

	img.load_image_file(filePath);

	wnd->drawImage(0, 0, &img);
}

static void update(winLib::Window* wnd, float delta) {

	wnd->setTitle(std::format("{:#x}", img.getColor(wnd->getMouseX(), wnd->getMouseY())));

	if (wnd->isMouseDown(winLib::MOUSE_LEFT)) {
		wnd->fillCircle(wnd->getMouseX(), wnd->getMouseY(), 10);
	}
}

int __stdcall WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd) {
	winLib::Window window;

	window.create(CW_USEDEFAULT, CW_USEDEFAULT, WS_OVERLAPPEDWINDOW, hInstance);

	window.onInitalise = init;
	window.onUpdate = update;

	window.clearScreen();

	window.start();

	return 0;
}

int main() {
	// Just call the windows Main method, if the subsystem is set to console

	OutputDebugString(Txt("\n\nWARNING: Please use the windows subsystem for better control!\n\n\n"));
	WinMain(NULL, NULL, NULL, NULL);

	return 0;
}