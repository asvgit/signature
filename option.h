#include <boost/program_options.hpp>

namespace option {

namespace po = boost::program_options;

using string = std::string;

struct Options {
	unsigned workers_number;
	unsigned block_size_mb;
	string input_file;
	string output_file;
	po::options_description desc;
	po::variables_map var_map;

	Options(int ac, char* av[]);
};
using OptionsPtr = std::shared_ptr<Options>;

OptionsPtr GetOptions(int ac, char* av[]);

} // end of namespace app_params
