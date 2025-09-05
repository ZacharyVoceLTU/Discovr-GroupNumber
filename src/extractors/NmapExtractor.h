#pragma once

#include "Extractor.h"

extern unsigned char nmap_exe[];
extern unsigned int nmap_exe_len;

class NmapExtractor : public Extractor {
public:
	void extract(const std::filesystem::path& outputDir) override;
};