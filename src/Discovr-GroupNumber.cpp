// Discovr-GroupNumber.cpp : Defines the entry point for the application.
//

#include <iostream>
#include <fstream>
#include <filesystem>

void createNmapFolder(const std::filesystem::path& tempNmapFolder);
void extractNmapBinary(const std::filesystem::path& outputPath);
void deleteTempFolders(const std::filesystem::path& tempNmapFolder);
void extractNmapDlls(const std::filesystem::path& outputPath, const std::string& fileName, unsigned char* data, unsigned int length);

extern unsigned char nmap_exe[];
extern unsigned int nmap_exe_len;

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
	
	try {
		extractNmapBinary(tempNmapFolder.string());

	} catch (const std::exception& e) {
		std::cerr << e.what() << '\n';
	}

	extractNmapDlls(tempNmapFolder.string(), "libcrypto-1_1.dll", libcrypto_1_1_dll, libcrypto_1_1_dll_len);
	extractNmapDlls(tempNmapFolder.string(), "libssh2.dll", libssh2_dll, libssh2_dll_len);
	extractNmapDlls(tempNmapFolder.string(), "libssl-1_1.dll", libssl_1_1_dll, libssl_1_1_dll_len);
	extractNmapDlls(tempNmapFolder.string(), "zlibwapi.dll", zlibwapi_dll, zlibwapi_dll_len);

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

// TODO: Make not hardcoded -> generate list of embedded DLLs in CMake, throw in header to loop over
void extractNmapDlls(const std::filesystem::path& outputPath, const std::string& fileName, unsigned char* data, unsigned int length) {
	std::ofstream outFile(outputPath / fileName, std::ios::binary);

	if (!outFile) {
		throw std::runtime_error("Failed to open file " + fileName + " for writing: " + outputPath.string());
	}

	outFile.write(reinterpret_cast<const char*>(data), length);
}

void deleteTempFolders(const std::filesystem::path& tempNmapFolder) {
	try {
		std::uintmax_t removedCount = std::filesystem::remove_all(tempNmapFolder);
		std::cout << "Deleted " << removedCount << " files/folders.\n";
	} catch (const std::filesystem::filesystem_error& e) {
		std::cerr << "Error deleting folder: " << e.what() << '\n';
	}
}
