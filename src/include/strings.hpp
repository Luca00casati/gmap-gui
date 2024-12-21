#pragma once

#include <string>
#include <imgui.h>
#include <nfd.h>

class StringsManager
{
public:
    StringsManager();
    ~StringsManager();

    void render();

private:
    nfdchar_t *selectedFile;
    std::string extractedText;
    bool isLoaded;
    bool failLoad;
    int minStringLength;
    bool includeSpecialCharacters;
    bool showErrorPopup;
    std::string errorMessage;

    void renderMinStringLengthInput();
    void renderSpecialCharacterCheckbox();
    void renderFileSelectionButton();
    void processFileIfNeeded();
    void clampMinStringLength();
    void freeFile();
    nfdchar_t *chooseFile();
    bool isPrintable(char c, bool allowSpecial);
    std::string extractStringsFromFile(const std::string &filepath, size_t minLength, bool allowSpecial);
};