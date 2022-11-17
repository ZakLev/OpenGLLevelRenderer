#pragma once
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include "h2bParser.h"
#include "Model.h"
#include <commdlg.h>
//#include <GLFW/glfw3.h>
#define GATEWARE_ENABLE_MATH  // Math Library
class FileIO
{
    H2B::Parser parser;
public:
    unsigned int amount = 0;
//void ReadGameLevel(std::vector<GW::MATH::GMATRIXF>& worldPositions, std::vector<std::string>& Names, std::vector<H2B::Parser>& parsers, char* fileName = "../../Assets/GameLevel.txt")
void ReadGameLevel(std::vector<Model>& models,const char* fileName = "../../../Assets/GameLevel.txt")
{
    std::ifstream file;
 //  fileName =  "C:\\Users\\levin\\OneDrive\\Documents\\GitHub\\OpenGLLevelRenderer\\Assets\\GameLevel.txt"; //Error Finding Text File -- Temp Fix
    file.open(fileName);// -- fixed
    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line))
        {
            
            std::istringstream iss(line);
           if (line.compare("MESH") == 0)
           {
           std::cout << line.c_str() << " #" << amount << std::endl;
               amount++;
               std::getline(file, line);
               std::istringstream iss(line);
               std::cout << line.c_str() <<  std::endl;
               int checkDecimal = line.length() - 4;
               if (line.compare(checkDecimal, 1, ".") == 0)
               {
                   line.erase(checkDecimal);
               }
               std::string namePath = "../../../Assets/Models/" + line + ".h2b";
               
               Model newModel;
               newModel.Name = line;
               newModel.NamePath = namePath;
               
               parser.Clear();
               parser.Parse(namePath.c_str());
               //newModel->parser = parser;
               newModel.parser = parser;
               
               GW::MATH::GMATRIXF mat = GW::MATH::GIdentityMatrixF;
               
               int k = 0;
          
               for (int i = 0; i < 4; i++)
               {
                   std::getline(file, line);
                   std::cout << line.c_str() << std::endl;
                   if(i == 0)
                   line.erase(0, 11);
                   filter(line);
                   std::istringstream iss(line);
              
                   for (int j = 0; j < 4; j++)
                   {
                       iss >> mat.data[k];
                       k++;
                   }
                  
               }
             
               newModel.worldPosition = mat;
            
               models.push_back(newModel);
             
           }
        }
    }
    file.close();
}
void filter(std::string &line)
{
    std::string temp = "";
    for (int i = 0; i < line.size(); ++i) {
        if ((line[i] >= '0' && line[i] <= '9') || (line[i] == '.') || (line[i] == ' ') || (line[i] == '-')) {
            temp = temp + line[i];
        }
    }
    line = temp;
}
std::string OpenFile(const char* filter, GW::SYSTEM::GWindow win)
{
    OPENFILENAMEA ofn;
    CHAR fileSize[180] = { 0 };
    ZeroMemory(&ofn, sizeof(OPENFILENAME));
    ofn.lStructSize = sizeof(OPENFILENAME);
    GW::SYSTEM::UNIVERSAL_WINDOW_HANDLE w;
    win.GetWindowHandle(w);
    HWND* window = (HWND*)&w;
    ofn.hwndOwner = *window;
    ofn.lpstrFile = fileSize;
    ofn.nMaxFile = sizeof(fileSize);
    ofn.lpstrFilter = filter;
    ofn.nFilterIndex = 1;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
    if (GetOpenFileNameA(&ofn) == true)
    {
        return ofn.lpstrFile;
    }
    return "";
}
};