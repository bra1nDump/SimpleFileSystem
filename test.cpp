#define DEBUG_MODE true

#include <stdlib.h>
#include <time.h>

#include "CFileSystem/CFileSystem.cpp"

// Adding lines to files
void test_2(CFileSystem* test_file_system) {
	char buf[] = "Chicken pimp\n";

	for (int i = 0; i < 10; ++i) {
		int success = test_file_system->add_line_to_file(i, buf);

		std::cout << "Adding line: " << buf
			<< "To file: " << i << std::endl;
		std::cout << "success: " << success << std::endl;
	}

	test_file_system->dump();
}

// Create and delete files testing
void test_1(CFileSystem* test_file_system) {
	
	for (int i = 0; i < 10; ++i) {
		int id = test_file_system->create_file();

		std::cout << "Creating file with id: " << id << std::endl;
	}

	for (int i = 0; i < 15; ++i) {
		int id = rand() % 10;
		bool deleted = test_file_system->delete_file(id);

		if (deleted) {
			std::cout << "File with id: " << id
				<< " deleted successfully!" << std::endl;
		} else {
			std::cout << "File with id: " << id
				<< " not found!" << std::endl;
		}
	}

	test_file_system->dump();
}

int main() {
	/* Intializes random number generator */
	time_t t;
   	srand((unsigned) time(&t));

   	CFileSystem test_file_system(8, 40);

	test_1(&test_file_system);
	test_2(&test_file_system);

	return 0;
}
