#include <iostream>
#include <stdlib.h>
#include <string>
#include <sstream>

//#define FILE_DEBUG
//#define FILE_SYSTEM_DEBUG

#include "CFileSystem/CFileSystem.cpp"

int   get_command_number(const char* base_);
int   to_int(const char* base_);
char* to_ptr(const char* base_);

void read_file_str_to_buffer(char* dest);

int main() {

	char buf[1024];

	int command_number;
	int result;

	CFileSystem test_system;

	do {
		scanf("%s", buf);
		command_number = get_command_number(buf);

		switch(command_number) {
			case (1): {
				scanf("%s", buf);
				int block_size = to_int(buf);

				result = test_system.set_block_size(block_size);
				break;
			}
			case (2): {
				scanf("%s", buf);
				int disk_size = to_int(buf);

				result = test_system.start_with_disk_size(disk_size);
				break;
			}
			case (3): {
				result = test_system.creatable();
				break;
			}
			case (4): {
				result = test_system.create_file();
				break;
			}
			case (5): {
				scanf("%s", buf);
				int file_key = to_int(buf);

				result = test_system.is_empty(file_key);
				break;
			}
			case (6): {
				scanf("%s", buf);
				int file_key = to_int(buf);
				read_file_str_to_buffer(buf);

				result = test_system.can_add_line(file_key, buf);
				break;
			}
			case (7): {
				scanf("%s", buf);
				int file_key = to_int(buf);
				read_file_str_to_buffer(buf);

				result = test_system.push_back_line(file_key, buf);
				break;
			}
			case (8): {
				scanf("%s", buf);
				int file_key = to_int(buf);
				scanf("%s", buf);
				char* ptr = to_ptr(buf);

				result = test_system.get_last_line(file_key);
				break;
			}
			case (9): {
				scanf("%s", buf);
				int file_key = to_int(buf);

				result = test_system.at_begin(file_key);
				break;
			}
			case (10): {
				scanf("%s", buf);
				int file_key = to_int(buf);

				result = test_system.at_end(file_key);
				break;
			}
			case (11): {
				scanf("%s", buf);
				int file_key = to_int(buf);

				result = test_system.set_cursor_to_begin(file_key);
				break;
			}
			case (12): {
				scanf("%s", buf);
				int file_key = to_int(buf);

				result = test_system.set_cursor_to_end(file_key);
				break;
			}
			case (13): {
				scanf("%s", buf);
				int file_key = to_int(buf);
				scanf("%s", buf);
				char* ptr = to_ptr(buf);

				result = test_system.read_forward(file_key);
				break;
			}
			case (14): {
				scanf("%s", buf);
				int file_key = to_int(buf);
				scanf("%s", buf);
				char* ptr = to_ptr(buf);

				result = test_system.read_backward(file_key);
				break;

				break;
			}
			case (15): {
				scanf("%s", buf);
				int file_key = to_int(buf);

				result = test_system.delete_file(file_key);
				break;
			}
			case (16): {
				result = test_system.end_work();
				goto exit_main_loop;
			}
			default: {
				std::cout << "Unknown command" << std::endl;
				goto exit_main_loop;
			}
		}

		std::cout.flush();

		//std::cout << "Command_number: " << command_number << std::endl;
		std::cout << result << std::endl;

	} while (*buf != '\0');
	exit_main_loop:

	return 0;
}

// heplp functions
int get_command_number(const char* base_) {
	if (std::strcmp(base_, "init") == 0) {
		return 1;
	} else if (std::strcmp(base_, "start") == 0) {
		return 2;
	} else if (std::strcmp(base_, "creatable") == 0) {
		return 3;
	} else if (std::strcmp(base_, "create") == 0) {
		return 4;
	} else if (std::strcmp(base_, "empty") == 0) {
		return 5;
	} else if (std::strcmp(base_, "addable") == 0) {
		return 6;
	} else if (std::strcmp(base_, "add") == 0) {
		return 7;
	} else if (std::strcmp(base_, "get") == 0) {
		return 8;
	} else if (std::strcmp(base_, "atbegin") == 0) {
		return 9;
	} else if (std::strcmp(base_, "atend") == 0) {
		return 10;
	} else if (std::strcmp(base_, "tobegin") == 0) {
		return 11;
	} else if (std::strcmp(base_, "toend") == 0) {
		return 12;
	} else if (std::strcmp(base_, "forward") == 0) {
		return 13;
	} else if (std::strcmp(base_, "backward") == 0) {
		return 14;
	} else if (std::strcmp(base_, "del") == 0) {
		return 15;
	} else if (std::strcmp(base_, "end") == 0) {
		return 16;
	}

	return -1;
}

int to_int(const char* base_) {

	int number = 0;
	int it = 0;
	while (*base_ != '\0') {
		number *= 10;
		number += base_[0] - '0';

		base_++;
	}

	return number;
}

char* to_ptr(const char* base_) {

	unsigned long ptr = 0;
	while (*base_ != '\0') {
		ptr *= 10;
		ptr += base_[0] - '0';

		base_++;
	}

	return ((char*) ptr);
}

void read_file_str_to_buffer(char* dest) {

	std::string input_str;
	std::getline(std::cin, input_str);

	int len = input_str.length();
	std::memcpy(dest, input_str.c_str(), len);
	dest[len] = '\n';
	dest[len + 1] = '\0';
}
