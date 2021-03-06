#include <iostream>
#include <fstream>
#include <thread>
#include <boost/filesystem/operations.hpp>

#include "operation.h"

namespace {

namespace fs = boost::filesystem;

using string = std::string;
using PosVector = std::vector<unsigned>;
using CharVec = operation::CharVec;

PosVector SplitFile(
		  const string &input_file
		, const unsigned block_size_mb
		, const unsigned workers_number)
{
	fs::path path(input_file);
	if (!fs::exists(path)) {
		std::cerr << "Failed to find: " << input_file << std::endl;
		throw;
	}

	const unsigned file_size = fs::file_size(path);
	const unsigned block_size = block_size_mb * 1024 * 1024;
	const uint block_number = fs::file_size(path) % block_size
		? fs::file_size(path) / block_size + 1
		: fs::file_size(path) / block_size;
	const unsigned block_per_work = block_number / workers_number;
	const unsigned addon_blocks = block_number % workers_number;

	PosVector pos(workers_number + 1);
	pos[0] = 0;
	for (unsigned i(1); i < workers_number; ++i) {
		const unsigned current_block_number = i - 1 < addon_blocks
					? block_per_work + 1
					: block_per_work;
		const unsigned new_pos = i * block_size * current_block_number;
		pos[i] = new_pos < file_size ? new_pos : file_size; 
	}
	pos.back() = file_size;

	std::sort(pos.begin(), pos.end());
	pos.erase(std::unique(pos.begin(), pos.end()), pos.end());
	return pos;
}

struct ReadingParams {
	string src;
	unsigned start;
	unsigned end;
	unsigned block_size;
};

void Read(const ReadingParams &params, operation::Mapper &m) {
	try {
		const size_t read_by = 1024;
		std::ifstream stream(params.src, std::ios_base::binary);
		stream.seekg(params.start);
		for (unsigned i(params.start); i < params.end; i += params.block_size) {
			const unsigned current_block_size =
				  i + params.block_size > params.end
				? params.end - i
				: params.block_size;
			const unsigned block_end = i + current_block_size;
			unsigned read_pos = i;
			CharVec block_content;
			do {
				const unsigned buf_size = read_pos + read_by < block_end
					? read_by
					: block_end - read_pos;
				CharVec buf(buf_size);
				stream.read(buf.data(), buf_size);
				block_content.insert(block_content.end(), buf.begin(), buf.end());
				read_pos += buf_size;
			} while (read_pos < block_end);
			// CharVec zero_vec(params.block_size - current_block_size, 0);
			// block_content.insert(block_content.end(), zero_vec.begin(), zero_vec.end());
			if (unsigned addon_size = params.block_size - current_block_size) {
				block_content.resize(block_content.size() + addon_size);
				std::generate(
						  block_content.end() - addon_size
						, block_content.end()
						, [](){ return 0;});
			}
			m(block_content);
		}
	} catch (...) {
		m(std::current_exception());
	}
}

} // end of private namespace

namespace operation {

void Mapper::operator () (const CharVec &item) {
	boost::crc_32_type result;
	result.process_bytes(item.data(), item.size());
	m_hash.push_back(result.checksum());
}

void Mapper::operator () (std::ofstream &stream) const {
	for (const uint hash : m_hash)
		stream << hash;
}

MapperVec GetMappers(
		  const string &input_file
		, const unsigned block_size_mb
		, const unsigned workers_number)
{
	PosVector points = SplitFile(input_file, block_size_mb, workers_number);
	std::vector<std::thread> readers;
	MapperVec mappers(points.size() - 1);
	for (size_t i(0); i < points.size() - 1; ++i) {
		ReadingParams params{
			  input_file
			, points[i]
			, points[i + 1]
			, block_size_mb * 1024 * 1024
		};
		readers.emplace_back(Read
				, std::move(params)
				, std::ref(mappers[i])
				);
	}
	for (std::thread &reader : readers)
		reader.join();
	for (Mapper &mapper : mappers)
		if (!mapper)
			std::rethrow_exception(mapper);
	return mappers;
}

void PrintMappers(const MapperVec &mappers, const string &output_file) {
	std::ofstream ostream(output_file);
	for (const Mapper &mapper : mappers)
		mapper(ostream);
}

} //end of operation namespace
