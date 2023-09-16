#pragma once

#include<stack>
#include<stdexcept>
#include "display.h"

class System {

	// 4Kb direct access
	uint8_t* memory;
	// current instruction offset in memory
	uint16_t pc;
	// 16 bit I register
	uint16_t i_register;
	// stack for 16 bit addresses
	std::stack<uint16_t> stack;
	// 8-bit delay timer
	uint8_t delay_timer;
	// 8-bit sound timer
	uint8_t sound_timer;
	// general-purpose registers
	uint8_t registers[16];
	// Hold key press infos
	bool key_info[16];

	Display* display;

	bool rom_loaded;
	bool modern_chip8;

	// check if file exists
	bool file_exists(const char* file_path);
	// extract 4 nibbles from 16-bit instruction
	void extract_nibbles(uint16_t instruction, uint8_t (& nibbles)[4]);

	// 0NNN instructions
	void x0NNN(uint16_t instruction, bool &done);
	// 1NNN instructions
	void x1NNN(uint16_t instruction, bool& done);
	// 2NNN instructions
	void x2NNN(uint16_t instruction, bool& done);
	// 3XNN instructions
	void x3NNN(uint16_t instruction, bool& done);
	// 4XNN instructions
	void x4NNN(uint16_t instruction, bool& done);
	// 5XNN instructions
	void x5NNN(uint16_t instruction, bool& done);
	// 6XNN instructions
	void x6NNN(uint16_t instruction, bool& done);
	// 7XNN instructions
	void x7NNN(uint16_t instruction, bool& done);
	// 8XNN instructions
	void x8NNN(uint16_t instruction, bool& done);
	// 9XNN instructions
	void x9NNN(uint16_t instruction, bool& done);
	// ANNN instructions
	void xANNN(uint16_t instruction, bool& done);
	// BNNN instructions
	void xBNNN(uint16_t instruction, bool& done);
	// CNNN instructions
	void xCNNN(uint16_t instruction, bool& done);
	// DNNN instructions
	void xDNNN(uint16_t instruction, bool& done);
	// ENNN instructions
	void xENNN(uint16_t instruction, bool& done);

public:
	// Default constructor
	System();

	// Writes a given byte to memory location
	void write_to_memory(uint16_t address, uint8_t value);

	// Read a byte from given memory location
	uint8_t read_from_memory(uint16_t address);

	// Read rom
	void read_rom_to_memory(const char* path_to_rom, uint16_t offset=0x200);

	// Run ROM
	void run();
};