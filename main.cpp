#include <iostream>

int main(int ac, char* av[]) {
	try {
		std::cout << "Hello, world!" << std::endl;
	} catch(const std::exception &e) {
		std::cerr << e.what() << std::endl;
	} catch(...) {
		std::cerr << "Something is wrong!" << std::endl;
	}
	return 0;
}
