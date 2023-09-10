#pragma once

#include<stack>
#include<stdexcept>

class System {

	// 4Kb direct access
	uint8_t* memory;
	// Points to current instruction in memory
	void** pc;
	// 16 bit I register
	void* i_register;
	// stack for 16 bit addresses
	std::stack<void*> stack;
	// 8-bit delay timer
	uint8_t delay_timer;
	// 8-bit sound timer
	uint8_t sound_timer;
	// general-purpose registers
	uint8_t registers[16];

public:
	// Default constructor
	System();

	// Writes a given byte to memory location
	void write_to_memory(uint16_t address, uint8_t value);
};