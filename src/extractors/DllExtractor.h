#pragma once

#include <string>
#include "Extractor.h"

class DllExtractor : public Extractor {
public:
	void extractDll(const std::filesystem::path& outputDir, const std::string& name, const unsigned char* data, const unsigned int length);

	void extract(const std::filesystem::path& outputDir) override;
};