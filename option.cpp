#include <iostream>
#include <thread>

#include "option.h"

namespace {

template<typename T>
T GetArg(int ac, char *av[], const int ind, T def) {
	if (ac < ind + 1)
		return def;
	std::stringstream ss(av[ind]);
	T arg;
	ss >> arg;
	return arg;
}

} // end of private namespace

namespace option {

Options::Options(int ac, char* av[])
	: workers_number(std::thread::hardware_concurrency()) 
	, desc("General options")
{
	desc.add_options()
		("help,h", "Show help")
		("input-file,i"
		 , po::value<string>(&input_file)
		 , "Target file for procedure")
		("output-file,o"
		 , po::value<string>(&output_file)
		 , "Output information file")
		("block-size,b"
		 , po::value<unsigned>(&block_size_mb)->default_value(1)
		 , "Procedure block size");
	po::parsed_options parsed = po::command_line_parser(ac, av)
		.options(desc)
		.allow_unregistered()
		.run();
	po::store(parsed, var_map);
	po::notify(var_map);
}

OptionsPtr GetOptions(int ac, char* av[]) {
	auto opt = std::make_shared<Options>(ac, av);
	if (opt->var_map.count("help")) {
		std::cout << opt->desc << std::endl;
		return nullptr;
	}

	if (opt->input_file.empty()
			&& opt->output_file.empty()
			&& !opt->var_map.count("block-size"))
	{
		opt->input_file = GetArg<string>(ac, av, 1, "");
		opt->output_file = GetArg<string>(ac, av, 2, "");
		opt->block_size_mb = GetArg<unsigned>(ac, av, 3, 1);
	} else if (opt->input_file.empty() || opt->output_file.empty()) {
		std::cerr << "Too few arguments!" << std::endl;
		throw;
	}
	if (opt->input_file.empty() && opt->output_file.empty()) {
		std::cerr << "Too few arguments!" << std::endl;
		throw;
	}
	return opt;
}

} // end of option namespace
