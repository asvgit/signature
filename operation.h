#include <boost/filesystem/path.hpp>
#include <boost/crc.hpp>

namespace operation {

using string = std::string;

class Mapper {
public:
	void operator () (const string &item);
	void operator () (std::ofstream &stream) const;
private:
	std::vector<uint> m_hash;
};
using MapperVec = std::vector<Mapper>;

MapperVec GetMappers(
		  const string &input_file
		, const unsigned block_size_mb
		, const unsigned workers_number);

void PrintMappers(const MapperVec &mappers, const string &output_file);

} //end of operation namespace
