#include <stdlib.h>
#include <time.h>

#define FILE_DEBUG
#define FILE_SYSTEM_DEBUG

#include "CFileSystem/CFileSystem.cpp"

int to_int(const char* base_);
void read_file_str_to_buffer(char* dest);

int main() {

	char buf[1024];

	int command_number;
	int result;

	CFileSystem test_system;

	do {
		scanf("%s", buf);
		command_number = to_int(buf);

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

				result = test_system.get_last_line(file_key);
				break;
			}
			case (9): {


				break;
			}
			case (10): {


				break;
			}
			case (11): {


				break;
			}
			case (12): {


				break;
			}
			case (13): {


				break;
			}
			case (14): {


				break;
			}
			case (15): {


				break;
			}
			case (16): {


				break;
			}
		}

		printf("Command_number: %d\n", command_number);
		printf("Result:         %d\n", result);

	} while (*buf != '\0');

	return 0;
}

// heplp functions
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

void read_file_str_to_buffer(char* dest) {
	scanf("%s", dest);
	int len = std::strlen(dest);
	dest[len] = '\n';
	dest[len + 1] = '\0';
}
