#include "include/strings.hpp"
#include "include/common.hpp"

void Strings_Loop(Strings_Data &SD){
     ImGui::SetNextWindowSize(ImVec2(250, 400), ImGuiCond_FirstUseEver);
        ImGui::Begin("Strings");
        {            
            if (ImGui::InputInt("MinLength", &SD.Strings_MinLength)){
                SD.Strings_Loaded = false;
                if (SD.Strings_MinLength < 0) {
                    SD.Strings_MinLength = 0;
                }
                if (SD.Strings_MinLength > 100) {
                    SD.Strings_MinLength = 100;
                }
            }
            if (ImGui::Checkbox("get special and space char?", &SD.Strings_SpacialAndSpace)){
                SD.Strings_Loaded = false;
            }
            if (ImGui::Button("open file"))
            {   
                if (SD.Strings_File)
                {
                    free(SD.Strings_File);
                    SD.Strings_File = nullptr;
                    SD.Strings_Loaded = false;
                }
                SD.Strings_File = chosefileload();
            }

            if (SD.Strings_File == nullptr)
            {
                ImGui::Text("No file chosen");
            }
            else
            {
                ImGui::Text("Chosen file: %s", SD.Strings_File);
                ImGui::Separator();
                if (!SD.Strings_Loaded){
                    SD.Strings_Text = extractStrings(SD.Strings_File, SD.Strings_MinLength, SD.Strings_SpacialAndSpace);
                }
                ImGui::TextUnformatted(SD.Strings_Text.c_str());
                SD.Strings_Loaded = true;

            }
        }
        ImGui::End();
}