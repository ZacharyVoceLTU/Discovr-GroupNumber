// Discovr-GroupNumber.cpp : Defines the entry point for the application.
//

#include <iostream>
#include <filesystem>
#include <vector>
#include <memory>
#include "extractors/NmapExtractor.h"
#include "extractors/DllExtractor.h"

void createNmapFolder(const std::filesystem::path& tempNmapFolder);
void deleteTempFolders(const std::filesystem::path& tempNmapFolder);
void menu(const std::filesystem::path& tempNmapFolder);
void displayVersion(std::filesystem::path& nmapPath);
void testScan(std::filesystem::path& nmapPath);

enum class Choice {
	Version = 1,
	TestScan,
	Quit
}; 

int main() {
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

	#if defined(_DEBUG)
		#if defined(__clang__)
			std::cout << "Compiled with Clang!\n";
		#elif defined(_MSC_VER)
			std::cout << "Compiled with MSVC!\n";
		#else
			std::cout << "Other compiler\n";
		#endif
	#endif

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
	
	int input{ };
	Choice userChoice{ };
	while (userChoice != Choice::Quit) {
		std::cout << "1. Print nmap version\n"
					 "2. Test Scan\n"
					 "3. Quit\n"
					 "What do you want to do: ";

		if (!(std::cin >> input)) {
			std::cin.clear();
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			std::cout << "\nOnly enter a valid int!!\n\n";
			continue;
		}

		std::cout << '\n';

		userChoice = static_cast<Choice>(input);

		switch (userChoice) {
			case Choice::Version:
				displayVersion(nmapPath);
				break;
			case Choice::Quit:
				break;
			case Choice::TestScan:
				testScan(nmapPath);
				break;
			default:
				std::cout << "Unknown option!\n\n";
		}
	}
}

// Currently just a test to see if nmap works
void displayVersion(std::filesystem::path& nmapPath) {
	std::string command{ "\"" + nmapPath.string() + "\" -version"};
	std::system(command.c_str());
	std::cout << '\n';
}

void testScan(std::filesystem::path& nmapPath) {
	std::string command{ "\"" + nmapPath.string() + "\" -sV 127.0.0.1 -p 80"};
	std::system(command.c_str());
	std::cout << '\n';
}
