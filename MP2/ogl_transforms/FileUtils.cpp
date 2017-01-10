#include "FileUtils.hpp"

bool SaveBufferToBinaryFile(const std::vector<unsigned char>& buffer, const std::string& filepath)
{
	FILE* file = nullptr;
	errno_t didFileOpen = fopen_s(&file, filepath.c_str(), "wb");
	if (didFileOpen != 0)
		return false;
	fwrite(&buffer[0], sizeof(unsigned char), buffer.size(), file);
	fclose(file);
	return true;
}

bool LoadBufferFromBinaryFile(std::vector<unsigned char>& out_buffer, const std::string& filepath)
{
	FILE* file = nullptr;
	errno_t didFileOpen = fopen_s(&file, filepath.c_str(), "rb");
	if (didFileOpen != 0)
		return false;
	fseek(file, 0, SEEK_END);
	size_t size = ftell(file);
	rewind(file);
	out_buffer.resize(size);
	fread(&out_buffer[0], sizeof(unsigned char), size, file);
	fclose(file);
	return true;
}
