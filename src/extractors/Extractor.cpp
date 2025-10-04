#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>

#include "Extractor.h"
#include "../NmapEmbedded/embedded_manifest.h"

void Extractor::extract(const std::filesystem::path& outputFolder) {
    std::cout << "Creating nmap files\n";
    for (auto& file : embedded_assets) {
        std::filesystem::path full_Path(file.name);
        std::filesystem::path parent_path = full_Path.parent_path();

        if (!parent_path.empty()) {
            try {
                std::filesystem::create_directories(outputFolder / parent_path);
            } catch (const std::filesystem::filesystem_error& e) {
                std::cerr << "Error creating directory " << parent_path << " : " << e.what() << '\n';
                continue;
            }
        }

        std::ofstream outFile(outputFolder / file.name, std::ios::binary);

        if (!outFile) {
            throw std::runtime_error("Failed to open file: " + (std::string&)file.name + "for writing: " + outputFolder.string());
        }

        outFile.write(reinterpret_cast<const char*>(file.data), file.length);
    }
    std::cout << "Finished creating nmap files\n";
}