#pragma once

#include "Extractor.h"
#include <string>

class DllExtractor : public Extractor {
public:
	DllExtractor(const std::string& name, unsigned char* data, unsigned int length)
		: fileName(name), fileData(data), fileLength(length) {
	}

	void extract(const std::filesystem::path& outputDir) override;

private:
	std::string fileName;
	unsigned char* fileData;
	unsigned int fileLength;
};