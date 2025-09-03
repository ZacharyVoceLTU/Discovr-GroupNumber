#pragma once

#include <filesystem>


class Extractor {
public:
	virtual ~Extractor() = default;
	virtual void extract(const std::filesystem::path& outputFolder) = 0;
};