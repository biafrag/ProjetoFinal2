//#include <iostream>
//#include <fstream>
//#include<QVector3D>
//void readOFF(std::string fileName, std::vector<QVector3D>& points, std::vector<QVector3D>& normals,std::vector<int>& indexPoints)
//{
//    std::ifstream myfile;
//    std::string s;
//    char OFF[100];
//    myfile.open(fileName);

//    if (!myfile.is_open())
//    {
//        std::cout << "Could not open " << std::endl;
//        perror(fileName.c_str());
//        return;
//    }

//    //Lendo OFF
//    myfile >> s;
//    std::cout <<s<< std::endl;
//    unsigned int numPoints;
//    unsigned int numFace;
//    unsigned int number;
//    //Numero de pontos
//    myfile >> numPoints;
//    printf("%d ",numPoints);
//    //Numero de faces
//    myfile >> numFace;
//    printf("%d ",numFace);
//    //Numero de arestas (Não necessário)
//    myfile >> number;

//    float x,y,z;
//    for(int i = 0; i < numPoints; i++)
//    {
//        myfile >> x;
//        myfile >> y;
//        myfile >> z;
//        QVector3D point(x,y,z);
//        points.push_back(point);
//    }

//    int numVert;
//    int f;
//    for(int i = 0; i < numFace; i++)
//    {
//        myfile >> numVert;
//        for(int j = 0; j < numVert ; j++)
//        {
//             myfile >> f;
//            indexPoints.push_back(f);
//        }
//    }
// myfile.close();
//}


