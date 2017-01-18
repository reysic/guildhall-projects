#include <vector>
#include <string>

bool SaveBufferToBinaryFile(const std::vector<unsigned char>& buffer, const std::string& filepath);
bool LoadBufferFromBinaryFile(std::vector<unsigned char>& out_buffer, const std::string& filepath);
bool LoadBufferFromBinaryFile(std::string& out_buffer, const std::string& filepath);