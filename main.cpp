#include <iostream>
#include <fstream>
#include <thread>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/program_options.hpp>
#include <boost/crc.hpp>

namespace po = boost::program_options;
namespace fs = boost::filesystem;

using string = std::string;
using PosVector = std::vector<unsigned>;
using StringVec = std::vector<string>;

struct ProgramOptions {
	unsigned workers_number;
	unsigned block_size_mb;
	string input_file;
	string output_file;
	po::options_description desc;
	po::variables_map var_map;

	ProgramOptions(int ac, char* av[])
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
};

PosVector SplitFile(const ProgramOptions &opt) {
	fs::path path(opt.input_file);
	if (!fs::exists(path)) {
		std::cerr << "Failed to find: " << opt.input_file << std::endl;
		throw;
	}

	const unsigned file_size = fs::file_size(path);
	const auto block_size = opt.block_size_mb * 1024 * 1024;
	const uint block_number = fs::file_size(path) % block_size
		? fs::file_size(path) / block_size + 1
		: fs::file_size(path) / block_size;
	const unsigned block_per_work = block_number / opt.workers_number;
	const unsigned addon_blocks = block_number % opt.workers_number;

	PosVector pos(opt.workers_number + 1);
	pos[0] = 0;
	for (unsigned i(1); i < opt.workers_number; ++i) {
		const auto current_block_number = i - 1 < addon_blocks
					? block_per_work + 1
					: block_per_work;
		const auto new_pos = i * block_size * current_block_number;
		pos[i] = new_pos < file_size ? new_pos : file_size; 
	}
	pos.back() = file_size;

	std::sort(pos.begin(), pos.end());
	auto del_it = std::unique(pos.begin(), pos.end());
	pos.erase(del_it, pos.end());
	return pos;
}

class Mapper {
public:
	void operator () (const string &item) {
		m_val.push_back(item);
		boost::crc_32_type result;
		result.process_bytes(item.data(), item.size());
		m_hash.push_back(result.checksum());
	}

	void operator () (std::ofstream &stream) {
		for (const auto &v : m_val)
			std::cout << v.size() << " ";
		std::cout << std::endl;
		for (const auto h : m_hash)
			stream << h << " ";
	}

private:
	StringVec m_val;
	std::vector<uint> m_hash;
};

struct ReadingParams {
	string src;
	unsigned start;
	unsigned end;
	unsigned block_size;
};

void Read(const ReadingParams &params, Mapper &m) {
	std::ifstream stream(params.src);
	stream.seekg(params.start);
	for (unsigned i(params.start); i < params.end; i += params.block_size) {
		const unsigned buf_size = i + params.block_size > params.end
			? params.end - i
			: params.block_size;
		string buf;
		buf.resize(buf_size);
		stream.read(&buf[0], buf_size);
		m(buf_size < params.block_size
				? buf + string(params.block_size - buf_size, '\0')
				: buf);
	}
}

int main(int ac, char* av[]) {
	try {
		auto opt = std::make_shared<ProgramOptions>(ac, av);
		if (opt->var_map.count("help")) {
			std::cout << opt->desc << std::endl;
			return 0;
		}

		auto points = SplitFile(*opt);

		for (auto p : points)
			std::cout << p << std::endl;

		std::vector<std::thread> readers;
		std::vector<Mapper> mappers(points.size() - 1);
		for (size_t i(0); i < points.size() - 1; ++i) {
			ReadingParams params{
				  opt->input_file
				, points[i]
				, points[i + 1]
				, opt->block_size_mb * 1024 * 1024
			};
			readers.emplace_back(Read
					, std::move(params)
					, std::ref(mappers[i])
					);
		}
		for (auto &r : readers)
			r.join();

		std::ofstream ostream(opt->output_file);
		for (auto &m : mappers)
			m(ostream);
	} catch(const std::exception &e) {
		std::cerr << e.what() << std::endl;
	} catch(...) {
		std::cerr << "Something is wrong!" << std::endl;
	}
	return 0;
}
