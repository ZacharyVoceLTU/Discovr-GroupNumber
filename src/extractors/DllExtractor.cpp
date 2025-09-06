#include "DllExtractor.h"
#include "../NmapEmbedded/dll_list.h"
#include <fstream>
#include <stdexcept>

void DllExtractor::extractDll(const std::filesystem::path& outputDir, const std::string& name, const unsigned char* data, const unsigned int length) {
	std::ofstream outFile(outputDir / name, std::ios::binary);

	if (!outFile) {
		throw std::runtime_error("Failed to open file " + name + " for writing: " + outputDir.string());
	}

	outFile.write(reinterpret_cast<const char*>(data), length);
}

void DllExtractor::extract(const std::filesystem::path& outputDir) {
	for (const auto& dll : dlls) {
		extractDll(outputDir, dll.name, dll.data, dll.length);
	}
}