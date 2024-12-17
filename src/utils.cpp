#include <iostream>
#include <fstream>
#include <nfd.h>
#include <vector>

nfdchar_t* chosefileload() {
    nfdchar_t* outPath = nullptr;
    nfdresult_t result = NFD_OpenDialog(nullptr, nullptr, &outPath);

    if (result == NFD_OKAY) {
        return outPath;
    } else if (result == NFD_CANCEL) {
        std::cout << "User pressed cancel." << std::endl;
        return nullptr;
    } else {
        std::cerr << "Error: " << NFD_GetError() << std::endl;
        return nullptr;
    }
}

bool isPrintable(char c, bool b) {
    return b ? std::isprint(static_cast<unsigned char>(c)) 
             : std::isalnum(static_cast<unsigned char>(c));
}

std::string extractStrings(const std::string& filepath, size_t minLength, bool usespecialandspace) {
    std::ifstream file(filepath, std::ios::binary);
    if (!file) {
        std::cerr << "Error: Could not open file: " << filepath << std::endl;
        return "Error opening file";
    }

    std::string result;
    std::string currentString;
    char c;

    while (file.get(c)) {
        if (isPrintable(c, usespecialandspace)) {
            currentString += c;
        } else {
            if (currentString.length() >= minLength) {
                result += currentString + "\n";
            }
            currentString.clear();
        }
    }

    if (currentString.length() >= minLength) {
        result += currentString + "\n";
    }

    file.close();
    return result;
}