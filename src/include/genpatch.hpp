#pragma once
#include <string>

#define TMPFILEGEN "tmp/tmppatchoutc.bin"

class PatchGenerator
{
public:
    PatchGenerator();
    void Render();

private:
    std::string originalFilePath;
    std::string modifiedFilePath;
    std::string gmapFilePath;

    std::string statusMessage;

    void chooseFile(std::string &filePath);
    void saveFile(std::string &filePath);

    void handleGeneratePatch();
    void genpatch(const std::string &ori, const std::string &mod, const std::string &out);
    void patchcompress(const std::string &in, const std::string &out);

    unsigned long sizediffaddr(std::ifstream &fori, std::ifstream &fmod);
    void setdiffaddr(std::ifstream &fori, std::ifstream &fmod, std::ofstream &out);
    void setdiffraw(std::ifstream &fori, std::ifstream &fmod, std::ofstream &out);
};