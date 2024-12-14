#include <iostream>

#include <nfd.h>

nfdchar_t* chosefile() {
    nfdchar_t *outPath = nullptr;
    nfdresult_t result = NFD_OpenDialog(nullptr, nullptr, &outPath);

    if (result == NFD_OKAY) {
        //puts("Success!");
        //puts(outPath);
        return outPath;
    } else if (result == NFD_CANCEL) {
        //puts("User pressed cancel.");
        return nullptr;
    } else {
        std::cout << "Error: " <<  NFD_GetError() << std::endl;
    }
    return nullptr;
}
