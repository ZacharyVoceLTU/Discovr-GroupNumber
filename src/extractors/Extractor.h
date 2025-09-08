#pragma once

#include <filesystem>


class Extractor {
public:
	~Extractor() = default;
	void extract(const std::filesystem::path& outputFolder);
};