// chip8.cpp : Defines the entry point for the application.
//

#include "chip8.h"
#include "system.h"
#include "SDL.h"
#include "iostream"

using namespace std;

int main(int argc, char* args[])
{
	System system = System();

	system.read_rom_to_memory("D:/Code/chip8/chip8/roms/ibm_logo.ch8");

	system.run();
	return 0;
}
