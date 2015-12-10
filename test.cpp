#define FILE_DEBUG
#define FILE_SYSTEM_DEBUG
#define VIRTUAL_DISK_DEBUG

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
	char sample_line_0[23] = "College sucks!\n\0   \0";
	char sample_line_1[23] = "Chicken is a pimp\n\0\0";
	char sample_line_2[23] = "KKK forever!\n\0     \0";
	char sample_line_3[23] = "SWAAAG\n\0           \0";

	char* sample_line = new char[30];

	for (int i = 0; i < iterations; ++i) {
		switch(rand() % 4) {
		case(0): { std::memcpy(sample_line, sample_line_0, 23 * sizeof(char)); break; }
		case(1): { std::memcpy(sample_line, sample_line_1, 23 * sizeof(char)); break; }
		case(2): { std::memcpy(sample_line, sample_line_2, 23 * sizeof(char)); break; }
		case(3): { std::memcpy(sample_line, sample_line_3, 23 * sizeof(char)); break; }
		}

		int file_id = rand() % range;
		
		test_file_system->add_line_to_file(file_id, sample_line);
	}

	delete [] sample_line;
}

// Reading lines
void test_3(CFileSystem* test_file_system, int iterations, int range) {
	for (int i = 0; i < iterations; ++i) {
		int file_id = rand() % range;

		test_file_system->read_line(file_id);
	}
}

// Random test for segfalts
void combined_test_0() {
	printf("\n\n\n\n\n");
	printf("=================combined_test_0====================\n");


	printf("=======================end==========================\n");
}

// General test
void combined_test_1() {
	printf("\n\n\n\n\n");
	printf("=================combined_test_1====================\n");

	CFileSystem test_file_system(16, 20);

	test_0(&test_file_system, 4);
	test_1(&test_file_system, 1, 4);
	test_2(&test_file_system, 10, 4);
	test_3(&test_file_system, 3, 1);

	test_file_system.dump();

	printf("=======================end==========================\n");
}

// Strange partition tests
void combined_test_2() {
	printf("\n\n\n\n\n");
	printf("=================combined_test_2====================\n");

	CFileSystem test_file_system(1, 300);

	test_0(&test_file_system, 1);
	test_2(&test_file_system, 3, 1);
	test_3(&test_file_system, 7, 1);

	//test_file_system.dump();

	printf("=======================end==========================\n");
}

void combined_test_3() {
	printf("\n\n\n\n\n");
	printf("=================combined_test_3====================\n");

	CFileSystem test_file_system(50, 200);

	test_0(&test_file_system, 5);
	test_2(&test_file_system, 50, 5);
	test_3(&test_file_system, 80, 5);

	test_file_system.dump();

	printf("=======================end==========================\n");
}

void combined_test_4() {
	printf("\n\n\n\n\n");
	printf("=================combined_test_4====================\n");

	CFileSystem test_file_system(16, 50);

	test_0(&test_file_system, 5);
	test_2(&test_file_system, 30, 5);
	test_1(&test_file_system, 20, 5);
	test_0(&test_file_system, 5);
	test_2(&test_file_system, 30, 10);
	test_3(&test_file_system, 80, 1);

	printf("=======================end==========================\n");
}


int main() {
	time_t t;
	srand((unsigned) time(&t));

	combined_test_1();
	//combined_test_2();
	//combined_test_3();
	//combined_test_4();

	return 0;
}
