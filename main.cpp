#include <iostream>

#include "option.h"
#include "operation.h"

int main(int ac, char* av[]) {
	try {
		option::OptionsPtr opt = option::GetOptions(ac, av);
		if (opt == nullptr)
			return 0;
		operation::MapperVec mappers = operation::GetMappers(
				  opt->input_file
				, opt->block_size_mb
				, opt->workers_number);
		operation::PrintMappers(mappers, opt->output_file);
	} catch(const std::exception &e) {
		std::cerr << e.what() << std::endl;
	} catch(...) {
		std::cerr << "Something is wrong!" << std::endl;
	}
	return 0;
}
