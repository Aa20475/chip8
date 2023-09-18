#include<system.h>
#include<iostream>
#include<fstream>
#include<string>
#include<stdexcept>
#include <stdlib.h>
#include <chrono>

std::string decToHexa(int n)
{
	// ans std::string to store hexadecimal number
	std::string ans = "";

	while (n != 0) {
		// remainder variable to store remainder
		int rem = 0;

		// ch variable to store each character
		char ch;
		// storing remainder in rem variable.
		rem = n % 16;

		// check if temp < 10
		if (rem < 10) {
			ch = rem + 48;
		}
		else {
			ch = rem + 55;
		}

		// updating the ans std::string with the character variable
		ans += ch;
		n = n / 16;
	}

	// reversing the ans std::string to get the final result
	int i = 0, j = ans.size() - 1;
	while (i <= j)
	{
		std::swap(ans[i], ans[j]);
		i++;
		j--;
	}
	return ans;
}

System::System(bool modern) : delay_timer(0), sound_timer(0), pc(0), i_register(0), rom_loaded(false), modern_chip8(modern), waiting_for_key(false), pressed_key(-1) {
	std::cout << "Initializing System... " << std::endl;
	memory = (uint8_t*)malloc(4096);

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

void System::x3NNN(uint16_t instruction, bool& done) {
	uint8_t nibbles[4];
	extract_nibbles(instruction, nibbles);
	if(registers[nibbles[1]] == (instruction & 0x00FFu)){
		pc += 2;
	}
}

void System::x4NNN(uint16_t instruction, bool& done) {
	uint8_t nibbles[4];
	extract_nibbles(instruction, nibbles);
	if (registers[nibbles[1]] != (instruction & 0x00FFu)) {
		pc += 2;
	}
}

void System::x5NNN(uint16_t instruction, bool& done) {
	uint8_t nibbles[4];
	extract_nibbles(instruction, nibbles);

	if (nibbles[3] == 0) {
		if (registers[nibbles[1]] == registers[nibbles[2]]) {
			pc += 2;
		}
	}
}

void System::x6NNN(uint16_t instruction, bool& done) {
	uint8_t nibbles[4];
	extract_nibbles(instruction, nibbles);
	registers[nibbles[1]] = instruction & 0x00FFu;
}

void System::x7NNN(uint16_t instruction, bool& done) {
	uint8_t nibbles[4];
	extract_nibbles(instruction, nibbles);
	uint8_t byte = instruction & 0x00FFu;
	registers[nibbles[1]] += byte;
}

void System::x8NNN(uint16_t instruction, bool& done) {
	uint8_t nibbles[4];
	extract_nibbles(instruction, nibbles);

	uint8_t* vx = &registers[nibbles[1]];
	uint8_t* vy = &registers[nibbles[2]];
	uint16_t value = *vx + *vy;
	switch (nibbles[3]) {
	case 0:
		*vx = *vy;
		break;
	case 1:
		*vx = *vx | *vy;
		break;
	case 2:
		*vx = *vx & *vy;
		break;
	case 3:
		*vx = *vx ^ *vy;
		break;
	case 4:
		registers[15] = value > 255;
		*vx = (uint8_t) value;
		break;
	case 5:
		registers[15] = *vx > *vy;
		*vx -= *vy;
		break;
	case 6:
		if (!modern_chip8) *vx = *vy;
		registers[15] = *vx & 1;
		*vx >>= 1;
		break;
	case 7:
		registers[15] = *vy > *vx;
		*vx = *vy - *vx;
		break;
	case 0xE:
		if (!modern_chip8) *vx = *vy;
		registers[15] = *vx & 0x70;
		*vx <<= 1;
		break;
	}
}

void System::x9NNN(uint16_t instruction, bool& done) {
	uint8_t nibbles[4];
	extract_nibbles(instruction, nibbles);

	if (nibbles[3] == 0) {
		if (registers[nibbles[1]] != registers[nibbles[2]]) {
			pc += 2;
		}
	}
}

void System::xANNN(uint16_t instruction, bool& done) {
	i_register = instruction & 0x0FFF;
}

void System::xBNNN(uint16_t instruction, bool& done) {
	if (!modern_chip8) pc = (instruction & 0x0FFF) + registers[0];
	else pc = (instruction & 0x0FFF) + registers[(instruction & 0x0F00) >> 8];
}

void System::xCNNN(uint16_t instruction, bool& done) {
	uint8_t nibbles[4];
	extract_nibbles(instruction, nibbles);
	
	registers[nibbles[1]] = rand();
	registers[nibbles[1]] &= instruction & 0x00FFu;
}

void System::xDNNN(uint16_t instruction, bool& done) {
	uint8_t nibbles[4];
	extract_nibbles(instruction, nibbles);

	uint8_t x_coordinate = registers[nibbles[1]] % 64, y_coordinate = registers[nibbles[2]] % 32;
	registers[15] = 0;
	uint16_t offset = i_register;
	for (int i = 0; i < nibbles[3]; i++) {
		uint8_t pixel_info = *(memory + offset + i);
		for (int j = 7; j >= 0; j--) {
			bool bit = pixel_info & 1;
			pixel_info >>= 1;
			if (x_coordinate + j >= 64 || y_coordinate + i >= 32)continue;
			registers[15] = display->set_pixel(x_coordinate + j, y_coordinate + i, bit);
		}
	}
}

void System::xENNN(uint16_t instruction, bool& done) {
	uint8_t nibbles[4];
	extract_nibbles(instruction, nibbles);

	if (nibbles[2] == 9 && nibbles[3] == 0xE) {
		if (key_info[registers[nibbles[1]]]) {
			pc += 2;
		}
	}else if(nibbles[2] == 0xA && nibbles[3] == 1) {
		if (!key_info[registers[nibbles[1]]]) {
			pc += 2;
		}
	}
}

void System::xFNNN(uint16_t instruction, bool& done) {
	uint8_t nibbles[4];
	extract_nibbles(instruction, nibbles);
	if (nibbles[2] == 0 && nibbles[3] == 7) {
		registers[nibbles[1]] = delay_timer;
	}else if (nibbles[2] == 0 && nibbles[3] == 0xA) {
		if (!waiting_for_key) {
			if (pressed_key == -1) {
				waiting_for_key = true;
				pc -= 2;
			}
			else {
				registers[nibbles[1]] = pressed_key;
				pressed_key = -1;
			}
		}
		else {
			pc -= 2;
		}
	}
	else if (nibbles[2] == 1 && nibbles[3] == 5) {
		delay_timer = registers[nibbles[1]];
	}
	else if (nibbles[2] == 1 && nibbles[3] == 8) {
		sound_timer = registers[nibbles[1]];
	}
	else if (nibbles[2] == 1 && nibbles[3] == 0xE) {
		i_register += registers[nibbles[1]];
		registers[15] = (i_register & 0xF000) != 0;
	}
	else if (nibbles[2] == 2 && nibbles[3] == 9) {
		i_register = 0x0050 + 5 * registers[nibbles[1]];
	}
	else if (nibbles[2] == 3 && nibbles[3] == 3) {
		int number = registers[nibbles[1]];
		write_to_memory(i_register + 2, number % 10);
		number /= 10;
		write_to_memory(i_register + 1, number % 10);
		number /= 10;
		write_to_memory(i_register, number % 10);
		number /= 10;
	}
	else if (nibbles[3] == 5) {
		for (int i = 0; i <= nibbles[1]; i++) {
			if (nibbles[2] == 5) {
				write_to_memory(modern_chip8 ? i_register + i : i_register++, registers[i]);
			}
			else if (nibbles[2] == 6) {
				registers[i] = read_from_memory(modern_chip8 ? i_register + i : i_register++);
			}
		}
	}
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

void System::handle_instruction(uint16_t full_instruction, bool& done) {
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
		break;
	case 3:
		x3NNN(full_instruction, done);
		break;
	case 4:
		x4NNN(full_instruction, done);
		break;
	case 5:
		x5NNN(full_instruction, done);
		break;
	case 6:
		x6NNN(full_instruction, done);
		break;
	case 7:
		x7NNN(full_instruction, done);
		break;
	case 8:
		x8NNN(full_instruction, done);
		break;
	case 9:
		x9NNN(full_instruction, done);
		break;
	case 0xA:
		xANNN(full_instruction, done);
		break;
	case 0xB:
		xBNNN(full_instruction, done);
		break;
	case 0xC:
		xCNNN(full_instruction, done);
		break;
	case 0xD:
		xDNNN(full_instruction, done);
		break;
	case 0xE:
		xENNN(full_instruction, done);
		break;
	case 0xF:
		xFNNN(full_instruction, done);
		break;
	default:
		std::cout << decToHexa(full_instruction) << " not implemented " << std::endl;
	}
}


void System::run() {
	pc = 0x200;
	if (!rom_loaded)throw std::logic_error("Load ROM before running.");
	bool done = false;
	
	const int fps = 60;
	const int time_per_frame = 1000 / fps;
	const int instructions_per_second = 700;
	const int instructions_per_frame = instructions_per_second / fps;

	while (!done) {
		if (pc > 0x0FFF) {
			done = true;
			continue;
		}
		int instruction_count = 0;

		auto start = std::chrono::high_resolution_clock::now();

		done = display->handle_events(key_info, waiting_for_key, pressed_key);
		while (instruction_count < instructions_per_frame) {
			uint16_t full_instruction = ((uint16_t)read_from_memory(pc) << 8) + read_from_memory(pc + 1);
			// increment PC by 2
			pc += 2;
			handle_instruction(full_instruction, done);
			display->draw();
			instruction_count++;
		}
		auto stop = std::chrono::high_resolution_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
		if (duration.count() < time_per_frame) {
			SDL_Delay(time_per_frame - duration.count());
			if (delay_timer > 0)delay_timer--;
			if (sound_timer > 0)sound_timer--;
		}
	}
}