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
void startP0f();
void stopP0f();

enum class Choice {
	Version = 1,
	FullScan,
	FastScan,
	StealthScan,
	StartP0f,
	StopP0f,
	Quit = 0
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
	Choice userChoice{ static_cast<Choice>(-1)  };
	while (userChoice != Choice::Quit) {
		std::cout << "1. Print nmap version\n"
					 "2. Full Scan\n"
					 "3. Fast Scan\n"
					 "4. Stealth Scan\n"
					 "5. Start p0f passive scan\n"
					 "6. Stop p0f passive scan\n"
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
			case Choice::StartP0f:
				startP0f();
				break;
			case Choice::StopP0f:
				stopP0f();
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
	// SECURITY: Some nmap scans need sudo, ask for user consent to use sudo
	#if defined(__linux__)
		std::string command{ "./" + scriptPath.string() + "/bannerScript.sh " + scanType + " " + scriptPath.string() + "/targets.txt " + nmapPath.string()};
	#elif defined(_WIN64)
		std::string command{ "" };
	#endif
	std::cout << command << '\n';
	// TODO: Implement Boost.process instead of using std::system
	// SECURITY: Implement Error handling
	std::system(command.c_str());
}

void startP0f() {
	#if defined(__linux__)
		std::cout << "Starting p0f Scan...\n";
		// ">/dev/null" redirects the nohup notification to a file that discards everything written to it
		std::system("nohup sudo p0f -i any -p -o p0f_log.txt >/dev/null 2>&1 &");
	#elif defined(_WIN64)
		std::cout << "p0f not supported on Windows yet\n";
	#endif
}

void stopP0f() {
	#if defined(__linux__)
		std::cout << "Stopping p0f Scan...\n";
		std::system("sudo pkill p0f");
	#elif defined(_WIN64)
		std::cout << "p0f not supported on Windows yet\n";
	#endif
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

	std::string scriptFileName((scriptsPath / "bannerScript.sh").string());
	std::string targetFileName((scriptsPath / "targets.txt").string());

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

	#if defined(__linux__)
		std::string command{ "chmod +x " + scriptFileName };
		std::system(command.c_str());
	#endif
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
