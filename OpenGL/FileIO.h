#pragma once
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#define GATEWARE_ENABLE_MATH  // Math Library
class FileIO
{

public:
   
void ReadGameLevel(std::vector<GW::MATH::GMATRIXF>& worldPositions, std::vector<std::string>& Names, char* fileName = "../Assets/GameLevel.txt")
{
    std::ifstream file;
    file.open("C:\\Users\\Aweso\\Documents\\GitHub\\Assignment_2_OpenGL\\Assets\\GameLevel.txt");//Error Finding Text File -- Temp Fix
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
               Names.push_back(line + ".h2b");
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