// chip8.cpp : Defines the entry point for the application.
//

#include "chip8.h"
#include "system.h"
#include "SDL.h"
#include "iostream"

using namespace std;

int main(int argc, char* args[])
{
	if (argc < 2) {
		std::cout << "Please provide ROM filepath." << std::endl;
		return -1;
	}

	const char* filepath = args[1];
	bool modern = false;
	if (argc == 3) {
		if (args[2] == "modern") {
			modern = true;
		}
	}

	System system = System(modern);
	system.read_rom_to_memory(filepath);

	system.run();
	return 0;
}
