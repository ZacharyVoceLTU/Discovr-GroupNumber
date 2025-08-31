// Discovr-GroupNumber.cpp : Defines the entry point for the application.
//

#include <iostream>
#include <fstream>
#include <filesystem>

void createNmapFolder(const std::filesystem::path& tempNmapFolder);
void extractNmapBinary(const std::filesystem::path& outputPath);
void deleteTempFolders(const std::filesystem::path& tempNmapFolder);

extern unsigned char nmap_exe[];
extern unsigned int nmap_exe_len;

int main()
{
	std::filesystem::path tempNmapFolder{ "tempNmap" };

	createNmapFolder(tempNmapFolder);
	
	try {
		extractNmapBinary(tempNmapFolder.string());
	} catch (const std::exception& e) {
		std::cerr << e.what() << '\n';
	}

	deleteTempFolders(tempNmapFolder);

	return 0;
}

void createNmapFolder(const std::filesystem::path& tempNmapFolder) {
	try {
		if (std::filesystem::create_directory(tempNmapFolder)) {
			std::cout << "Temp Nmap fodler created successfully: " << tempNmapFolder << '\n';
		} else {
			std::cout << "Temp Nmap folder already exists" << '\n';
		}
	} catch (const std::filesystem::filesystem_error& e) {
		std::cerr << "Error creating folder: " << e.what() << '\n';
	}
}

void extractNmapBinary(const std::filesystem::path& outputPath) {
	std::ofstream outFile(outputPath / "nmap.exe", std::ios::binary);
	
	if (!outFile) {
		throw std::runtime_error("Failed to open file for writing: " + outputPath.string());
	}

	outFile.write(reinterpret_cast<const char*>(nmap_exe), nmap_exe_len);
}

void deleteTempFolders(const std::filesystem::path& tempNmapFolder) {
	try {
		std::uintmax_t removedCount = std::filesystem::remove_all(tempNmapFolder);
		std::cout << "Deleted " << removedCount << " files/folders.\n";
	} catch (const std::filesystem::filesystem_error& e) {
		std::cerr << "Error deleting folder: " << e.what() << '\n';
	}
}
