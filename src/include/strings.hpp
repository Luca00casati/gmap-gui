#pragma once

#include <string>
#include <imgui.h>
#include <nfd.h>

struct Strings_Data
{
    nfdchar_t *Strings_File = nullptr;
    std::string Strings_Text = "";
    bool Strings_Loaded = false;
    int Strings_MinLength = 10;
    bool Strings_SpacialAndSpace = false;
};

void Strings_Loop(Strings_Data &SD);
