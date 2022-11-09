#pragma once
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include "h2bParser.h"
#define GATEWARE_ENABLE_MATH  // Math Library
class FileIO
{
    H2B::Parser parser;
public:
   
void ReadGameLevel(std::vector<GW::MATH::GMATRIXF>& worldPositions, std::vector<std::string>& Names, std::vector<H2B::Parser>& parsers, char* fileName = "../../Assets/GameLevel.txt")
{
    std::ifstream file;
 //  fileName =  "C:\\Users\\levin\\OneDrive\\Documents\\GitHub\\OpenGLLevelRenderer\\Assets\\GameLevel.txt"; //Error Finding Text File -- Temp Fix
    file.open(fileName);// -- fixed
    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line))
        {
            
            std::istringstream iss(line);
           std::cout << line.c_str() << std::endl;
           if (line.compare("MESH") == 0)
           {
               std::getline(file, line);
               std::istringstream iss(line);
               std::cout << line.c_str() << std::endl;
               int checkDecimal = line.length() - 4;
               if (line.compare(checkDecimal, checkDecimal, "."))
               {
                   line.erase(checkDecimal);
               }
               std::string namePath = "../../Assets/Models/" + line + ".h2b";
               Names.push_back(namePath);
               parser.Clear();
               parser.Parse(namePath.c_str());
               parsers.push_back(parser);
               //verts.push_back(parser.vertices);
              // parser.Clear();
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
               worldPositions.push_back(mat);
           }
        }
    }
    file.close();
}
void filter(std::string &line)
{
    std::string temp = "";
    for (int i = 0; i < line.size(); ++i) {
        if ((line[i] >= '0' && line[i] <= '9') || (line[i] == '.') || (line[i] == ' ')) {
            temp = temp + line[i];
        }
    }
    line = temp;
}
};