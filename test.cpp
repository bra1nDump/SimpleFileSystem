#define DEBUG_MODE true

#include <stdlib.h>
#include <time.h>

#include "CFileSystem/CFileSystem.cpp"

// Create files
void test_0(CFileSystem* test_file_system, int iterations) {
	for (int i = 0; i < iterations; ++i) {
		test_file_system->create_file();
	}
}

// Delete files
void test_1(CFileSystem* test_file_system, int iterations, int range) {
	for (int i = 0; i < iterations; ++i) {
		int id = rand() % range;
		test_file_system->delete_file(id);
	}
}

// Adding lines to files
void test_2(CFileSystem* test_file_system, int iterations, int range) {
	char sample_line_0[] = "College sucks!\n";
	char sample_line_1[] = "Chicken is a pimp\n";
	char sample_line_2[] = "KKK  forever!\n";
	char sample_line_3[] = "SWAGA bitch!\n";

	char* sample_line;

	for (int i = 0; i < iterations; ++i) {
		switch(rand() % 4) {
		case(0): { sample_line = sample_line_0; break; }
		case(1): { sample_line = sample_line_1; break; }
		case(2): { sample_line = sample_line_2; break; }
		case(3): { sample_line = sample_line_3; break; }
		}

		int file_id = rand() % range;
		
		test_file_system->add_line_to_file(file_id, sample_line);
	}
}

// Reading lines
void test_3(CFileSystem* test_file_system, int iterations, int range) {
	for (int i = 0; i < iterations; ++i) {
		int file_id = rand() % range;

		test_file_system->read_line(file_id);
	}
}

void combined_test_0() {
	printf("\n\n\n\n\n");
	printf("#################combined_test_0####################\n");


	printf("#######################end##########################\n");
}

void combined_test_1() {
	printf("\n\n\n\n\n");
	printf("#################combined_test_1####################\n");

	CFileSystem test_file_system(8, 150);

	test_0(&test_file_system, 3);
	test_2(&test_file_system, 5, 1);
	test_3(&test_file_system, 7, 1);

	test_file_system.dump();

	printf("#######################end##########################\n");
}

void combined_test_2() {
	printf("\n\n\n\n\n");
	printf("#################combined_test_2####################\n");

	CFileSystem test_file_system(1, 150);

	test_0(&test_file_system, 3);
	test_2(&test_file_system, 3, 1);
	test_3(&test_file_system, 7, 1);

	test_file_system.dump();

	printf("#######################end##########################\n");
}

int main() {
	time_t t;
	srand((unsigned) time(&t));

	combined_test_1();
	combined_test_2();



	return 0;
}
