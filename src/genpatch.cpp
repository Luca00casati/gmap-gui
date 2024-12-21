#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>
#include <lzma.h>
#include <imgui.h>
#include <nfd.h>
#include "include/genpatch.hpp"
#include <vector>

PatchGenerator::PatchGenerator()
    : originalFilePath(""), modifiedFilePath(""), gmapFilePath(""), statusMessage("Ready.") {}

void PatchGenerator::Render()
{
    ImGui::Begin("Patch Generator");

    if (ImGui::Button("Load Original File"))
    {
        chooseFile(originalFilePath);
    }
    ImGui::Text("Original File: %s", originalFilePath.empty() ? "None selected" : originalFilePath.c_str());

    if (ImGui::Button("Load Modified File"))
    {
        chooseFile(modifiedFilePath);
    }
    ImGui::Text("Modified File: %s", modifiedFilePath.empty() ? "None selected" : modifiedFilePath.c_str());

    if (ImGui::Button("Save GMAP File"))
    {
        saveFile(gmapFilePath);
    }
    ImGui::Text("GMAP File: %s", gmapFilePath.empty() ? "None selected" : gmapFilePath.c_str());

    if (ImGui::Button("Generate Patch"))
    {
        handleGeneratePatch();
    }

    ImGui::Text("Status: %s", statusMessage.c_str());

    ImGui::End();
}

std::string originalFilePath;
std::string modifiedFilePath;
std::string gmapFilePath;
std::string statusMessage;

void PatchGenerator::chooseFile(std::string &filePath)
{
    nfdchar_t *outPath = nullptr;
    nfdresult_t result = NFD_OpenDialog(nullptr, nullptr, &outPath);

    if (result == NFD_OKAY)
    {
        filePath = outPath;
        free(outPath);
        statusMessage = "File selected.";
    }
    else if (result == NFD_CANCEL)
    {
        statusMessage = "File selection cancelled.";
    }
    else
    {
        statusMessage = std::string("Error: ") + NFD_GetError();
    }
}

void PatchGenerator::saveFile(std::string &filePath)
{
    nfdchar_t *outPath = nullptr;
    nfdresult_t result = NFD_SaveDialog(nullptr, nullptr, &outPath);

    if (result == NFD_OKAY)
    {
        filePath = outPath;
        free(outPath);
        statusMessage = "Save path selected.";
    }
    else if (result == NFD_CANCEL)
    {
        statusMessage = "Save cancelled.";
    }
    else
    {
        statusMessage = std::string("Error: ") + NFD_GetError();
    }
}

void PatchGenerator::handleGeneratePatch()
{
    if (originalFilePath.empty() || modifiedFilePath.empty() || gmapFilePath.empty())
    {
        statusMessage = "Please select all required files.";
        return;
    }

    try
    {
        genpatch(originalFilePath, modifiedFilePath, TMPFILEGEN);
        patchcompress(TMPFILEGEN, gmapFilePath);
        statusMessage = "Patch generated and compressed successfully.";
    }
    catch (const std::exception &e)
    {
        statusMessage = std::string("Error: ") + e.what();
    }
}

void PatchGenerator::genpatch(const std::string &ori, const std::string &mod, const std::string &out)
{
    std::ifstream fori(ori, std::ios::binary);
    if (!fori.is_open())
    {
        throw std::runtime_error("Failed to read " + ori);
    }

    std::ifstream fmod(mod, std::ios::binary);
    if (!fmod.is_open())
    {
        throw std::runtime_error("Failed to read " + mod);
    }

    fori.seekg(0, std::ios::end);
    long orisz = fori.tellg();
    fori.seekg(0);

    fmod.seekg(0, std::ios::end);
    long modsz = fmod.tellg();
    fmod.seekg(0);

    if (modsz != orisz)
    {
        throw std::runtime_error("Invalid size: original and modified file sizes differ");
    }

    std::ofstream fout(out, std::ios::binary);
    if (!fout.is_open())
    {
        throw std::runtime_error("Failed to write " + out);
    }

    unsigned long addsize = sizediffaddr(fori, fmod);
    unsigned char sig[] = {0xFF, 0x98};
    fout.write(reinterpret_cast<const char *>(sig), sizeof(sig));
    fout << addsize << '\n';
    setdiffaddr(fori, fmod, fout);
    setdiffraw(fori, fmod, fout);
}

void PatchGenerator::patchcompress(const std::string &in, const std::string &out)
{
    std::ifstream fin(in, std::ios::binary);
    if (!fin.is_open())
    {
        throw std::runtime_error("Failed to read " + in);
    }

    fin.seekg(0, std::ios::end);
    long in_size = fin.tellg();
    fin.seekg(0);

    std::vector<unsigned char> in_buffer(in_size);
    fin.read(reinterpret_cast<char *>(in_buffer.data()), in_size);

    std::ofstream fout(out, std::ios::binary);
    if (!fout.is_open())
    {
        throw std::runtime_error("Failed to write " + out);
    }

    lzma_stream strm = LZMA_STREAM_INIT;
    if (lzma_easy_encoder(&strm, LZMA_PRESET_DEFAULT, LZMA_CHECK_CRC64) != LZMA_OK)
    {
        throw std::runtime_error("Failed to initialize LZMA encoder");
    }

    size_t out_buffer_size = in_size + in_size / 3 + 128;
    std::vector<unsigned char> out_buffer(out_buffer_size);

    strm.next_in = in_buffer.data();
    strm.avail_in = in_size;
    strm.next_out = out_buffer.data();
    strm.avail_out = out_buffer_size;

    lzma_ret ret = lzma_code(&strm, LZMA_FINISH);
    if (ret != LZMA_STREAM_END)
    {
        lzma_end(&strm);
        throw std::runtime_error("Failed to compress data");
    }

    fout.write(reinterpret_cast<char *>(out_buffer.data()), strm.total_out);
    lzma_end(&strm);
}

unsigned long PatchGenerator::sizediffaddr(std::ifstream &fori, std::ifstream &fmod)
{
    bool newAddress = true;
    unsigned long naddress = 0;
    char co, cm;
    while (fori.get(co))
    {
        fmod.get(cm);
        if (co != cm && newAddress)
        {
            naddress++;
            newAddress = false;
        }
        if (co == cm && !newAddress)
        {
            naddress++;
            newAddress = true;
        }
    }
    fori.clear();
    fori.seekg(0);
    fmod.clear();
    fmod.seekg(0);
    return naddress;
}

void PatchGenerator::setdiffaddr(std::ifstream &fori, std::ifstream &fmod, std::ofstream &out)
{
    bool newAddress = true;
    long findex = 0;
    char co, cm;
    while (fori.get(co))
    {
        fmod.get(cm);
        if (co != cm && newAddress)
        {
            out << findex << '\n';
            newAddress = false;
        }
        if (co == cm && !newAddress)
        {
            out << findex << '\n';
            newAddress = true;
        }
        findex++;
    }
    fori.clear();
    fori.seekg(0);
    fmod.clear();
    fmod.seekg(0);
}

void PatchGenerator::setdiffraw(std::ifstream &fori, std::ifstream &fmod, std::ofstream &out)
{
    char co, cm;
    while (fori.get(co))
    {
        fmod.get(cm);
        if (co != cm)
        {
            out.put(cm);
        }
    }
    fori.clear();
    fori.seekg(0);
    fmod.clear();
    fmod.seekg(0);
}