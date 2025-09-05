#include "NmapExtractor.h"
#include <fstream>
#include <stdexcept>

void NmapExtractor::extract(const std::filesystem::path& outputDir) {
	std::ofstream outFile(outputDir / "nmap.exe", std::ios::binary);

	if (!outFile) {
		throw std::runtime_error("Failed to open file for writing: " + outputDir.string());
	}

	outFile.write(reinterpret_cast<const char*>(nmap_exe), nmap_exe_len);
}