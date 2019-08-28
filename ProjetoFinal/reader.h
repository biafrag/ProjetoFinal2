#pragma once
#include<iostream>
#include<vector>
#include <fstream>
#include<string>
#include <QVector3D>
#include <sstream>
void readFile(std::string fileName, std::vector<QVector3D>& points, std::vector<QVector3D>& normals,std::vector<int>& indexPoints, std::vector<int>& indexNormals)
{
    int tam = points.size();
    std::cout<<"Tamanho: "<<tam<<std::endl;
    std::string line;
    std::string type = " ";
    std::ifstream myfile;
    myfile.open(fileName);

    if (!myfile.is_open())
    {
        std::cout << "Could not open " << std::endl;
        perror(fileName.c_str());
        return;
    }
    while (1)
    {
      if(myfile.eof())
      {
          break;
      }

      else if(type != "v" && type != "vn" && type != "f")
      {
          myfile >> type;
      }
      else if(type == "v")
      {
          QVector3D point;
          float number;
          myfile >> number;
          point.setX(number);
          myfile >> number;
          point.setY(number);
          myfile >> number;
          point.setZ(number);
          points.push_back(point);

          myfile >> type;
      }

      else if(type == "vn")
      {
          QVector3D normal;
          float number;
          myfile >> number;
          normal.setX(number);
          myfile >> number;
          normal.setY(number);
          myfile >> number;
          normal.setZ(number);
          normals.push_back(normal);
          myfile >> type;
      }
      else if(type == "f" )
      {
          std::string s;
          std::string aux;
          std::vector<int> v;
          v.resize(9);
          int indPoint,indNormal;
          char lixo;
          for(unsigned int i = 0; i < 3; i++)
          {
              myfile >> indPoint;
              myfile >>lixo;
              myfile >>lixo;
              myfile >> indNormal;
              indexPoints.push_back(tam + indPoint - 1);
              indexNormals.push_back(tam + indNormal - 1);
          }
          myfile >> type;
      }

    }
    myfile.close();
}
