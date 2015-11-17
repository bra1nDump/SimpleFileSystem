#include <iostream>

#include "CVirtualDisk.cpp"

int main() {
	CVirtualDisk test_disk = CVirtualDisk(32, 256);

	std::string str = "Damir is a lazy bitch))";
	char* block = new char[32];
	std::strcpy(block, str.c_str());
	
	test_disk.write_block(block, 4);

	char* new_block = new char[32];
	test_disk.read_block(new_block, 4);

	printf("%s\n", new_block);

	delete [] block;
	delete [] new_block;

	return 0;
}
