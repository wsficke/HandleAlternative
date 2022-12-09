#include <iostream>
#include <filesystem>
#include <cstdio>
#include <cstdlib>
#include <memory>
#include <vector>
#include <string>

#ifdef _WIN32
#include <Windows.h>
#endif

int main(int argc, char* argv[])
{
    // Check if the correct number of arguments were provided
    if (argc != 2)
    {
        std::cerr << "Error: missing file handle argument" << std::endl;
        return 1;
    }

    // Parse the file handle argument
    int file_handle = atoi(argv[1]);

    // Print the file handle// Get the process ID of the current process
    int pid = _getpid();

    // Create the path to the directory containing the file handles of the current process
    std::filesystem::path dir_path;
#ifdef _WIN32
    // On Windows, the file handles are stored in the "Handles" sub-directory of the process ID
    // in the \.\global?? directory
    std::string dir_name = "\\.\global??\\" + std::to_string(pid) + "\\Handles";
    dir_path = dir_name;
#else
    // On POSIX systems, the file handles are stored in the "fd" sub-directory of the process ID
    // in the /proc directory
    std::string dir_name = "/proc/" + std::to_string(pid) + "/fd";
    dir_path = dir_name;
#endif

    // Check if the directory exists
    if (!std::filesystem::exists(dir_path))
    {
        std::cerr << "Error: directory " << dir_path << " does not exist" << std::endl;
        return 0;
    }

    // Check if the directory is a directory
    if (!std::filesystem::is_directory(dir_path))
    {
        std::cerr << "Error: " << dir_path << " is not a directory" << std::endl;
        return 0;
    }

    // List the entries in the directory
    std::vector<std::string> entries;
    for (const auto& entry : std::filesystem::directory_iterator(dir_path))
    {
        // Convert the path to a string
        std::string entry_name = entry.path().string();

#ifdef _WIN32
        // On Windows, the filesystem library uses the wide-character API, so we need to convert
        // the path to a UTF-8 encoded string
        int size = WideCharToMultiByte(CP_UTF8, 0, (LPCWCH)entry_name.c_str(), -1, nullptr, 0, nullptr, nullptr);
        std::unique_ptr<char[]> utf8_name(new char[size]);
        WideCharToMultiByte(CP_UTF8, 0, (LPCWCH)entry_name.c_str(), -1, utf8_name.get(), size, nullptr, nullptr);
        entry_name = utf8_name.get();
#endif
        entries.push_back(entry_name);
    }
	
        // Iterate over the entries in the directory
        for (const auto& entry_name : entries)
        {
            // Convert the entry name (which is a string representation of a file descriptor number)
            // to an integer file descriptor number
            int fd = atoi(entry_name.c_str());

            // Skip the entries that are not file handles (e.g. ".", "..")
            if (fd <= 0) continue;

            // Check if the current entry corresponds to the given file handle
            if (fd == file_handle)
            {
                // Print the file handle
                std::cout << fd << std::endl;
            }
        }

    return 0;
}