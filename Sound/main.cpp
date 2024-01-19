#include <windows.h>
#include <mmsystem.h>
#include <iostream>

#pragma comment(lib, "winmm.lib")

int main() {

	std::cout << "starting...\n";

	// sample-9s.wav is just a sample file
	// NULL because SND_RESOURCE is not specified
	// SND_FILENAME specifies that the first param is a file name
	// SND_ASYNC specifies that the function only starts the sound and returns instant
	PlaySound(TEXT("sample-9s.wav"), NULL, SND_FILENAME | SND_ASYNC);

	std::cout << "waiting 5s...\n";

	Sleep(5000);

	std::cout << "ending...\n";

	// with the file name = NULL stops the async sound started prev.
	PlaySound(NULL, NULL, NULL);

	std::cout << "waiting 2s...\n";

	Sleep(2000);

	std::cout << "ending programm...\n";

	return 0;
}