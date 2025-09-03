#include "DllExtractor.h"
#include <fstream>
#include <stdexcept>

void DllExtractor::extract(const std::filesystem::path& outputDir) {
	std::ofstream outFile(outputDir / fileName, std::ios::binary);

	if (!outFile) {
		throw std::runtime_error("Failed to open file " + fileName + " for writing: " + outputDir.string());
	}

	outFile.write(reinterpret_cast<const char*>(fileData), fileLength);
}