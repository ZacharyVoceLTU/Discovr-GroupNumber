// Discovr-GroupNumber.cpp : Defines the entry point for the application.
//

#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>
#include <memory>
#include "extractors/NmapExtractor.h"
#include "extractors/DllExtractor.h"

void createNmapFolder(const std::filesystem::path& tempNmapFolder);
void deleteTempFolders(const std::filesystem::path& tempNmapFolder);
void menu(const std::filesystem::path& tempNmapFolder);
void displayVersion(std::filesystem::path& nmapPath);

// Remove when dynamic reimplementation of extractNmapDlls
extern unsigned char libcrypto_1_1_dll[];
extern unsigned int libcrypto_1_1_dll_len; 
extern unsigned char libssh2_dll[];
extern unsigned int libssh2_dll_len;
extern unsigned char libssl_1_1_dll[];
extern unsigned int libssl_1_1_dll_len;
extern unsigned char zlibwapi_dll[];
extern unsigned int zlibwapi_dll_len;

int main()
{
	std::filesystem::path tempNmapFolder{ "tempNmap" };

	createNmapFolder(tempNmapFolder);
	
	// Create extractors
	std::vector <std::unique_ptr<Extractor>> extractors;
	extractors.push_back(std::make_unique<NmapExtractor>());
	// TODO: Make not hardcoded -> generate list of embedded DLLs in CMake, chuck in header to loop over
	extractors.push_back(std::make_unique <DllExtractor>("libcrypto-1_1.dll", libcrypto_1_1_dll, libcrypto_1_1_dll_len));
	extractors.push_back(std::make_unique <DllExtractor>("libssh2.dll", libssh2_dll, libssh2_dll_len));
	extractors.push_back(std::make_unique <DllExtractor>("libssl-1_1.dll", libssl_1_1_dll, libssl_1_1_dll_len));
	extractors.push_back(std::make_unique <DllExtractor>("zlibwapi.dll", zlibwapi_dll, zlibwapi_dll_len));

	for (auto& e : extractors) {
		try {
			e->extract(tempNmapFolder);
		} catch (const std::exception& e) {
			std::cerr << e.what() << '\n';
		}
	}

	menu(tempNmapFolder);

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

void deleteTempFolders(const std::filesystem::path& tempNmapFolder) {
	try {
		std::uintmax_t removedCount = std::filesystem::remove_all(tempNmapFolder);
		std::cout << "Deleted " << removedCount << " files/folders.\n";
	} catch (const std::filesystem::filesystem_error& e) {
		std::cerr << "Error deleting folder: " << e.what() << '\n';
	}
}

void menu(const std::filesystem::path& tempNmapFolder) {
	std::filesystem::path nmapPath = tempNmapFolder / "nmap.exe";
	
	// TODO: Implement enum class
	int choice{};
	while (choice != 2) {
		std::cout << "1. Print nmap version\n"
					 "2. Quit\n";

		std::cout << "What do you want to do: " << '\n';
		std::cin >> choice;

		switch (choice) {
			case 1:
				displayVersion(nmapPath);
				break;
			case 2:
				continue;
		}
	}
}

// Currently just a test to see if nmap works
void displayVersion(std::filesystem::path& nmapPath) {
	std::string command{ "\"" + nmapPath.string() + "\" -version"};
	std::system(command.c_str());
}
