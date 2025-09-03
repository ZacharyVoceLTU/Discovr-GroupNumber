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

int main()
{
	std::filesystem::path tempNmapFolder{ "tempNmap" };

	createNmapFolder(tempNmapFolder);
	
	// Create extractors
	std::vector<std::unique_ptr<Extractor>> extractors{ };
	extractors.push_back(std::make_unique<NmapExtractor>());
	extractors.push_back(std::make_unique<DllExtractor>());

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

		std::cout << "What do you want to do: ";
		std::cin >> choice;
		std::cout << '\n';

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
	std::cout << '\n';
}
