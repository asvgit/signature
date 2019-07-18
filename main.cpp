#include <iostream>
#include <boost/program_options.hpp>

namespace po = boost::program_options;

using string = std::string;
using size_t = std::size_t;
using StringVec = std::vector<string>;

struct ProgramOptions {
	po::options_description desc;
	po::variables_map var_map;
	string input_file;
	string output_file;
	int block_size_mb;

	ProgramOptions(int ac, char* av[]) : desc("General options") {
		desc.add_options()
			("help,h", "Show help")
			("input-file,i", po::value<string>(&input_file), "Target file for procedure")
			("output-file,o", po::value<string>(&output_file), "Output information file")
			("block-size,b", po::value<int>(&block_size_mb)->default_value(1), "Procedure block size");

		po::parsed_options parsed = po::command_line_parser(ac, av).options(desc).allow_unregistered().run();
		po::store(parsed, var_map);
		po::notify(var_map);
	}
};

int main(int ac, char* av[]) {
	try {
		auto opt = std::make_shared<ProgramOptions>(ac, av);
		if (opt->var_map.count("help")) {
			std::cout << opt->desc << std::endl;
			return 0;
		}
	} catch(const std::exception &e) {
		std::cerr << e.what() << std::endl;
	} catch(...) {
		std::cerr << "Something is wrong!" << std::endl;
	}
	return 0;
}
