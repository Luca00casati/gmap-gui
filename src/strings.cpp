#include "strings.hpp"
#include <fstream>
//#include <iostream>
#include <algorithm>

StringsManager::StringsManager()
    : selectedFile(nullptr), extractedText(""), isLoaded(false), failLoad(false),
      minStringLength(10), includeSpecialCharacters(false), showErrorPopup(false) {}

StringsManager::~StringsManager() {
    freeFile();
}

void StringsManager::render() {
    ImGui::SetNextWindowSize(ImVec2(250, 400), ImGuiCond_FirstUseEver);
    if (ImGui::Begin("Strings")) {
        renderMinStringLengthInput();
        renderSpecialCharacterCheckbox();
        renderFileSelectionButton();

        if (selectedFile == nullptr) {
            ImGui::Text("No file chosen");
        } else {
            ImGui::Text("Chosen file: %s", selectedFile);
            ImGui::Separator();
            processFileIfNeeded();
            ImGui::TextUnformatted(extractedText.c_str());
        }

        if (showErrorPopup) {
            ImGui::OpenPopup("Error");
            if (ImGui::BeginPopupModal("Error", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
                ImGui::TextUnformatted(errorMessage.c_str());
                if (ImGui::Button("OK")) {
                    showErrorPopup = false;
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();
            }
        }
    }
    ImGui::End();
}

void StringsManager::renderMinStringLengthInput() {
    if (ImGui::InputInt("Min Length", &minStringLength)) {
        clampMinStringLength();
        isLoaded = false;
    }
}

void StringsManager::renderSpecialCharacterCheckbox() {
    if (ImGui::Checkbox("Include special and space chars?", &includeSpecialCharacters)) {
        isLoaded = false;
    }
}

void StringsManager::renderFileSelectionButton() {
    if (ImGui::Button("Open File")) {
        freeFile();
        selectedFile = chooseFile();
        isLoaded = false;
    }
}

void StringsManager::processFileIfNeeded() {
    if (!isLoaded && selectedFile != nullptr) {
        extractedText = extractStringsFromFile(selectedFile, minStringLength, includeSpecialCharacters);
        isLoaded = true;
    }
}

void StringsManager::clampMinStringLength() {
    minStringLength = std::clamp(minStringLength, 0, 100);
}

void StringsManager::freeFile() {
    if (selectedFile) {
        free(selectedFile);
        selectedFile = nullptr;
    }
}

nfdchar_t* StringsManager::chooseFile() {
    nfdchar_t* outPath = nullptr;
    nfdresult_t result = NFD_OpenDialog(nullptr, nullptr, &outPath);

    switch (result) {
    case NFD_OKAY:
        return outPath;
    case NFD_CANCEL:
        //std::cerr << "File selection cancelled.\n";
        break;
    default:
        errorMessage = NFD_GetError();
        showErrorPopup = true;
        //std::cerr << "File dialog error: " << errorMessage << "\n";
        break;
    }
    return nullptr;
}

bool StringsManager::isPrintable(char c, bool allowSpecial) {
    return allowSpecial ? std::isprint(static_cast<unsigned char>(c))
                        : std::isalnum(static_cast<unsigned char>(c));
}

std::string StringsManager::extractStringsFromFile(const std::string& filepath, size_t minLength, bool allowSpecial) {
    std::ifstream file(filepath, std::ios::binary);
    if (!file.is_open()) {
        errorMessage = "Error: Could not open file: " + filepath;
        showErrorPopup = true;
        return "";
    }

    std::string result;
    std::string currentString;
    char c;

    while (file.get(c)) {
        if (isPrintable(c, allowSpecial)) {
            currentString += c;
        } else {
            if (currentString.length() >= minLength) {
                result += currentString + '\n';
            }
            currentString.clear();
        }
    }

    if (currentString.length() >= minLength) {
        result += currentString + '\n';
    }

    return result;
}
