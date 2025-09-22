// Discovr-GroupNumber.cpp : Defines the entry point for the application.
//

#include <iostream>
#include <filesystem>
#include <memory>
#include <limits>
#include "extractors/Extractor.h"

void createNmapFolder(const std::filesystem::path& tempNmapFolder);
void deleteTempFolders(const std::filesystem::path& tempNmapFolder);
void menu(const std::filesystem::path& nmapPath);
void displayVersion(const std::filesystem::path& nmapPath);
void fullScan(const std::filesystem::path& nmapPath);
void fastScan(const std::filesystem::path& nmapPath);
void stealthScan(const std::filesystem::path& nmapPath);
std::string getTargetFromUser();

enum class Choice {
	Version = 1,
	FullScan,
	FastScan,
	StealthScan,
	Quit
}; 

int main() {
	// TODO: Use RAII to handle deleting tempNmapFolder on unhandled exception
	std::filesystem::path tempNmapFolder{ "tempNmap" };

	createNmapFolder(tempNmapFolder);
	
	std::unique_ptr<Extractor> extractor = std::make_unique<Extractor>();

	extractor->extract(tempNmapFolder);

	std::filesystem::path nmapPath{ };
	#if defined(_WIN64)
		nmapPath = tempNmapFolder / "nmap.exe";
	#elif defined(__linux__)
		nmapPath = tempNmapFolder / "nmap";
		std::string command{"chmod +x " + nmapPath.string()};
		std::system(command.c_str());
	#endif

	menu(nmapPath);

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

void menu(const std::filesystem::path& nmapPath) {
	int input{ -1 };
	Choice userChoice{ };
	while (userChoice != Choice::Quit) {
		std::cout << "1. Print nmap version\n"
					 "2. Full Scan\n"
					 "3. Fast Scan\n"
					 "4. Stealth Scan\n"
					 "0. Quit\n"
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
			case Choice::FullScan:
				fullScan(nmapPath);
				break;
			case Choice::FastScan:
				fastScan(nmapPath);
				break;
			case Choice::StealthScan:
				stealthScan(nmapPath);
				break;
			default:
				std::cout << "Unknown option!\n\n";
		}
	}
}

// Currently just a test to see if nmap works
void displayVersion(const std::filesystem::path& nmapPath) {
	std::string command{ "\"" + nmapPath.string() + "\" -version"};
	std::system(command.c_str());
	std::cout << '\n';
}

void fullScan(const std::filesystem::path& nmapPath) {
	std::string target = getTargetFromUser();

	std::cout << "Running full scan\n";
	std::cout << "=================";
	
	std::string prefix{""};
	#if defined(__linux__)
		prefix += "sudo ";
	#endif
	prefix += "\"";
	std::string command{ prefix + nmapPath.string() + "\" -sV -Pn -T3 -p 22,80,443,135,139,445,3389,5985,5986,25,53,389,636,1433,3306,5432,8080,8443,515,9100,161,16 " + target};
	std::system(command.c_str());
	std::cout << '\n';
}

void fastScan(const std::filesystem::path& nmapPath) {
	std::string target = getTargetFromUser();

	std::cout << "Running fast scan\n";
	std::cout << "=================";
	
	std::string prefix{""};
	#if defined(__linux__)
		prefix += "sudo ";
	#endif
	prefix += "\"";
	std::string command{ prefix + nmapPath.string() + "\" -sV -Pn -T4 -p 22,80,443,135,445,3389 " + target};
	std::system(command.c_str());
	std::cout << '\n';
}

void stealthScan(const std::filesystem::path& nmapPath) {
	std::string target = getTargetFromUser();

	std::cout << "Running stealth scan\n";
	std::cout << "====================";

	std::string prefix{""};
	// SECURITY: Ask for user consent to use sudo
	#if defined(__linux__) 
		prefix += "sudo ";
	#endif
	prefix += "\"";
	std::string command{ prefix + nmapPath.string() + "\" -sV -Pn -T2 -p 22,80,443,135,139,445,3389,5985,5986,25,53,389,636,1433,3306,5432,8080,8443 " + target};
	// TODO: Implement Boost.process instead of using std::system
	// SECURITY: Implement Error handling
	std::system(command.c_str());
	std::cout << '\n';
}

std::string getTargetFromUser() {
	std::cout << "What ip do you want to scan >> ";
	std::string target{ };
	std::cin >> target;

	// SECURITY: Validate/Sanitise input before returning
	return target;
}
