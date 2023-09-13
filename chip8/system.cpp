#include<system.h>
#include<iostream>
#include<fstream>
#include<string>
#include<stdexcept>

System::System() : delay_timer(0), sound_timer(0), pc(0), i_register(NULL), rom_loaded(false) {
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

uint8_t System::read_from_memory(uint16_t address) {
	if (address >> 12)throw std::invalid_argument("Address needs to be 12-bit");
	return *(memory + address);
}

bool System::file_exists(const char* file_path) {
	return std::ifstream(file_path).good();
}

void System::extract_nibbles(uint16_t instruction, uint8_t(&nibbles)[4]) {
	nibbles[0] = (instruction & 0xF000) >> 12;
	nibbles[1] = (instruction & 0x0F00) >> 8;
	nibbles[2] = (instruction & 0x00F0) >> 4;
	nibbles[3] = (instruction & 0x000F) >> 0;
}

void System::x0NNN(uint16_t instruction, bool &done) {
	if (instruction == 0x00E0) {
		display->clear_screen();
	}else if(instruction == 0x00EE) {
		// subroutine return
		pc = stack.top();
		stack.pop();
	}
}

void System::x1NNN(uint16_t instruction, bool& done) {
	pc = instruction & 0x0FFF;
}

void System::x2NNN(uint16_t instruction, bool& done) {
	stack.push(pc);
	pc = instruction & 0x0FFF;
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
	rom_loaded = true;
}


void System::run() {
	pc = 0x200;
	if (!rom_loaded)throw std::logic_error("Load ROM before running.");
	bool done = false;
	while (!done) {
		if (pc > 0x0FFF) {
			done = true;
			continue;
		}
		uint16_t full_instruction = ((uint16_t)read_from_memory(pc) << 8) + read_from_memory(pc + 1);
		// increment PC by 2
		pc += 2;


		uint8_t nibbles[4];
		extract_nibbles(full_instruction, nibbles);

		switch (nibbles[0]) {
		case 0:
			x0NNN(full_instruction, done);
			break;
		case 1:
			x1NNN(full_instruction, done);
			break;
		case 2:
			x2NNN(full_instruction, done);
		}

	}
}