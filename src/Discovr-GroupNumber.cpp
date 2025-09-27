// Discovr-GroupNumber.cpp : Defines the entry point for the application.
//

#include <fstream>
#include <iostream>
#include <filesystem>
#include <memory>
#include <limits>
#include <stdexcept>
#include "extractors/Extractor.h"

#if defined(__linux__)
	#include "../scripts/Linux/bannergrab.h"
#elif defined(_WIN64)
	#include "../scripts/Windows/bannergrab.h"
#endif

void createNmapFolder(const std::filesystem::path& tempNmapFolder);
void deleteFolderContents(const std::filesystem::path& path);
void menu(const std::filesystem::path& nmapPath, const std::filesystem::path& scriptPath);
void displayVersion(const std::filesystem::path& nmapPath);
std::string getTargetFromUser();
void installp0f();
void uninstallp0f();
void writeScripts(const std::filesystem::path& scriptsPath);

void scan(const std::filesystem::path& nmapPath, const std::filesystem::path& scriptPath, const std::string& scanType);

enum class Choice {
	// TODO_FIX: Setting Quit to 0 exits the menu straight away
	Version = 1,
	FullScan,
	FastScan,
	StealthScan,
	FullScanBetter,
	Quit
}; 

int main() {
	// TODO: Use RAII to handle deleting tempNmapFolder on unhandled exception
	const std::filesystem::path tempNmapFolder{ "tempNmap" };
	const std::filesystem::path scriptsPath{ "scripts"};

	createNmapFolder(tempNmapFolder);
	
	std::unique_ptr<Extractor> extractor = std::make_unique<Extractor>();

	extractor->extract(tempNmapFolder);

	// Defining nmap path and setting changemode on Linux
	std::filesystem::path nmapPath{ };
	#if defined(_WIN64)
		nmapPath = tempNmapFolder / "nmap.exe";
	#elif defined(__linux__)
		nmapPath = tempNmapFolder / "nmap";
		std::string command{"chmod +x " + nmapPath.string()};
		std::system(command.c_str());
	#endif

	writeScripts(scriptsPath);

	// NOT RECOMMENDED OR FINALISED for production, I understand not professional or recommended but for prototype sake
	installp0f();

	menu(nmapPath, scriptsPath);

	deleteFolderContents(tempNmapFolder);
	deleteFolderContents(scriptsPath);

	// NOT RECOMMENDED OR FINALISED for production, I understand not professional or recommended but for prototype sake
	uninstallp0f();

	return 0;
}

void createNmapFolder(const std::filesystem::path& tempNmapFolder) {
	try {
		if (std::filesystem::create_directory(tempNmapFolder)) {
			std::cout << "Temp Nmap folder created successfully: " << tempNmapFolder << '\n';
		} else {
			std::cout << "Temp Nmap folder already exists" << '\n';
		}
	} catch (const std::filesystem::filesystem_error& e) {
		std::cerr << "Error creating folder: " << e.what() << '\n';
	}
}

void deleteFolderContents(const std::filesystem::path& path) {
	try {
		std::uintmax_t removedCount = std::filesystem::remove_all(path);
		std::cout << "Deleted " << removedCount << " files/folders in: " << path.string() << '\n';
	} catch (const std::filesystem::filesystem_error& e) {
		std::cerr << "Error deleting folder: " << e.what() << '\n';
	}
}

void menu(const std::filesystem::path& nmapPath, const std::filesystem::path& scriptPath) {
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
				scan(nmapPath, scriptPath, "full");
				break;
			case Choice::FastScan:
				scan(nmapPath, scriptPath, "fast");
				break;
			case Choice::StealthScan:
				scan(nmapPath, scriptPath, "stealth");
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

void scan(const std::filesystem::path& nmapPath, const std::filesystem::path& scriptPath, const std::string& scanType) {
	std::string nmp{"/home/kali/Documents/Discovr-GroupNumber/out/build/developer-tools/tempNmap/nmap"};
	std::string command{ "./" + scriptPath.string() + "/bannerScript.sh " + scanType + " " + scriptPath.string() + "/targets.txt " + nmp};
	std::cout << command << '\n';
	std::system(command.c_str());
}

std::string getTargetFromUser() {
	std::cout << "What ip do you want to scan >> ";
	std::string target{ };
	std::cin >> target;

	// SECURITY: Validate/Sanitise input before returning
	return target;
}

void writeScripts(const std::filesystem::path& scriptsPath) {
	try {
		std::filesystem::create_directory(scriptsPath);
	} catch (const std::filesystem::filesystem_error& e) {
		std::cerr << "Error creating directory " << "scripts" << " : " << e.what() << '\n';
	}

	std::string scriptFileName(scriptsPath / "bannerScript.sh");
	std::string targetFileName(scriptsPath / "targets.txt");

	std::ofstream scriptFile(scriptFileName);
	std::ofstream targetFile(targetFileName);

	if (!scriptFile) {
		throw std::runtime_error("Failed to open file: " + scriptFileName + " for writing: scripts/");
	}
	if (!targetFile) {
		throw std::runtime_error("Failed to open file: " + targetFileName + " for writing: scripts/");
	}

	scriptFile.write(bannerScript.data(), bannerScript.size());
	std::string localIP{"127.0.0.1\n"};
	targetFile.write(localIP.data(), localIP.size());

	std::string command{ "chmod +x " + scriptFileName };
	std::system(command.c_str());
}

// NOT RECOMMENDED OR FINALISED for production, I understand not professional or recommended but for prototype sake
void installp0f() {
	#if defined(__linux__)
		std::system("sudo apt install p0f");
	#elif defined(_WIN64)
		std::cout << "p0f not supported on Windows yet\n";
	#endif
}

// NOT RECOMMENDED OR FINALISED for production, I understand not professional or recommended but for prototype sake
void uninstallp0f() {
	#if defined(__linux__)
		std::system("sudo apt purge p0f");
	#elif defined(_WIN64)
		std::cout << "p0f not supported on Windows yet\n";
	#endif
}
