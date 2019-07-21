#include <boost/filesystem/path.hpp>
#include <boost/crc.hpp>

namespace operation {

using string = std::string;
using CharVec = std::vector<char>;

class Mapper {
public:
	void operator () (const CharVec &item);
	void operator () (std::ofstream &stream) const;
	void operator () (std::exception_ptr except) { m_exception = except; }
	operator std::exception_ptr  () { return m_exception; }
	operator bool () { return m_exception == nullptr; }
private:
	std::vector<uint> m_hash;
	std::exception_ptr m_exception = nullptr;
};
using MapperVec = std::vector<Mapper>;

MapperVec GetMappers(
		  const string &input_file
		, const unsigned block_size_mb
		, const unsigned workers_number);

void PrintMappers(const MapperVec &mappers, const string &output_file);

} //end of operation namespace
