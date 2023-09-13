#include<system.h>
#include<iostream>
#include<fstream>
#include<string>
#include<stdexcept>

System::System() : delay_timer(0), sound_timer(0), pc(NULL), i_register(NULL) {
	std::cout << "Initializing System... " << std::endl;
	memory = (uint8_t*)malloc(4096);
	i_register = malloc(2);

	display = new Display();

	for (int i = 0; i < 16; i++) {
		registers[i] = 0;
	}

	uint8_t font_memory[80] = {
		0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
		0x20, 0x60, 0x20, 0x20, 0x70, // 1
		0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
		0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
		0x90, 0x90, 0xF0, 0x10, 0x10, // 4
		0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
		0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
		0xF0, 0x10, 0x20, 0x40, 0x40, // 7
		0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
		0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
		0xF0, 0x90, 0xF0, 0x90, 0x90, // A
		0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
		0xF0, 0x80, 0x80, 0x80, 0xF0, // C
		0xE0, 0x90, 0x90, 0x90, 0xE0, // D
		0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
		0xF0, 0x80, 0xF0, 0x80, 0x80  // F
	};

	for (int address = 0x0050; address <= 0x009F; address++) {
		write_to_memory(address, font_memory[address - 0x0050]);
	}
}

void System::write_to_memory(uint16_t address, uint8_t value) {
	if (address >> 12)throw std::invalid_argument("Address needs to be 12-bit");
	*(memory + address) = value;
}

bool System::file_exists(const char* file_path) {
	return std::ifstream(file_path).good();
}

void System::read_rom_to_memory(const char* path_to_rom, uint16_t offset) {
	if (!file_exists(path_to_rom))throw std::invalid_argument("ROM " + std::string(path_to_rom) + " does not exist!");
	std::ifstream rom_file(path_to_rom, std::ios::in | std::ios::binary);
	char data[1];
	int position = offset;
	while (rom_file.read(data, 1)) {
		write_to_memory(position, (uint8_t) data[0]);
		position++;
	}
	std::cout << "Wrote " << position << " bytes." << std::endl;
	rom_file.close();
}