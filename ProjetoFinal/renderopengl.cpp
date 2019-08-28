#include "renderopengl.h"
#include <QImage>
//#include<QGLWidget>
#include "reader.h"
#include "readeroff.h"
#include "math.h"
#include <glm/ext.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <QMouseEvent>

RenderOpengl::RenderOpengl(QWidget* parent)
    :QOpenGLWidget(parent)
{
    cam.at = QVector3D(0.f,0.f,0.f);
    cam.eye =  QVector3D(0.f,0.f,200);
    cam.up = QVector3D(0.f,3.f,0.f);
    cam.zNear = 0.1;
    cam.zFar  = 1000.f;
    cam.fovy  = 60.f;
    cam.width = 1002;
    cam.height = 701;
    this->setFocus();
}

RenderOpengl::~RenderOpengl()
{
    delete _program;
    glDeleteBuffers(1, &_meshBuffer);
    glDeleteBuffers(1, &_normalsBuffer);
    glDeleteBuffers(1, &_pointsBuffer);
}

void RenderOpengl::setFile(std::vector<std::string> fileNames)
{
        _points.clear();
        _normals.clear();
        _indexPoints.clear();
        _indexNormals.clear();

        for(unsigned int i = 0; i < fileNames.size(); i++)
        {
            std::cout << fileNames[i] << std::endl;
            readFile(fileNames[i],_points,_normals,_indexPoints,_indexNormals);
        }
        getMinMaxMesh();
        cam.eye =  QVector3D(0.f,0.f,200);
        _model = glm::mat4x4(1.0);
        //paintGL();

}

void RenderOpengl::getMinMaxMesh()
{
    float maxX,maxY,maxZ;
    float minX,minY,minZ;
    maxX = maxY = maxZ = -2000000;
    minX = minY = minZ = 2000000;
    for(int i = 0; i < _points.size(); i++)
    {
        if(_points[i].x() > maxX)
        {
            maxX = _points[i].x();
        }
        else
        {
            if(_points[i].x() < minX)
            {
                minX = _points[i].x();
            }
        }
        if(_points[i].y() > maxY)
        {
            maxY = _points[i].y();
        }
        else
        {
            if(_points[i].y() < minY)
            {
                minY = _points[i].y();
            }
        }
        if(_points[i].z() > maxZ)
        {
            maxZ = _points[i].z();
        }
        else
        {
            if(_points[i].z() < minZ)
            {
                minZ = _points[i].z();
            }
        }
    }

    printf("MIN X = %f\n",minX);
    printf("MIN Y = %f\n ",minY);
    printf("MIN Z = %f\n\n ",minZ);

    printf("MAX X = %f\n",maxX);
    printf("MAX Y = %f\n ",maxY);
    printf("MAX Z = %f\n\n ",maxZ);

    printf("Medium X: %f\n", (minX + maxX)/2);
    printf("Medium Y: %f\n", (minY + maxY)/2);
    printf("Medium Z: %f\n", (minZ + maxZ)/2);
    _minMeshPoint.setX(minX);
    _minMeshPoint.setY(minY);
    _minMeshPoint.setZ(minZ);
    _maxMeshPoint.setX(maxX);
    _maxMeshPoint.setY(maxY);
    _maxMeshPoint.setZ(maxZ);

    QVector3D medium((_maxMeshPoint + _minMeshPoint)/2);
   // cam.at = QVector3D(medium.x(),medium.y(),0.f);
    //cam.eye =  QVector3D(medium.x(),medium.y(),4*_maxMeshPoint.z());

    for(int i = 0; i < _points.size(); i++)
    {
        _points[i] = _points[i] - medium;
    }
}

void RenderOpengl::initializeGL()
{
    initializeOpenGLFunctions();

    makeCurrent();
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glClearColor(0,0,0,1);

    _program = new QOpenGLShaderProgram();
    //Adicionando shaders ao programa

    _program->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/vertexshader.glsl");
    _program->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/fragmentshader.glsl");

    //Linka shaders que foram adicionados ao programa
    _program->link();

    if (!_program->isLinked())
    {
        std::cout<<"Problemas ao linkar shaders"<<std::endl;
    }

    //setFile({"../../ModeloObj/modelosObj/U-2300-EQU.obj","../ModeloObj/modelosObj/U-2300-EST.obj"});

    setMode(MeshTypes::PLATAFORMA);
    _program->bind();

    //createVAO();
    printThings();


}




void RenderOpengl::paintGL()
{
    //Dando bind no programa e no vao
    _program->bind();
    _vao.bind();
   // createVAO();
    glViewport(0, 0, cam.width, cam.height);
    //Definindo matriz view e projection
     _view.setToIdentity();
     _view.lookAt(cam.eye, cam.at, cam.up);
     _proj.setToIdentity();
     _proj.perspective(cam.fovy, (float)cam.width/cam.height, cam.zNear, cam.zFar);
     //_model.setToIdentity();

    //Definindo matrizes para passar para os shaders

    QMatrix4x4 m = QMatrix4x4(_model[0][0],_model[0][1],_model[0][2],_model[0][3],
            _model[1][0],_model[1][1],_model[1][2],_model[1][3],
            _model[2][0],_model[2][1],_model[2][2],_model[2][3],
            _model[3][0],_model[3][1],_model[3][2],_model[3][3]);
    //QMatrix4x4 m = _model;
    QMatrix4x4 v = _view;
    QMatrix4x4 p = _proj;

    QMatrix4x4 mv = v*m;
    QMatrix4x4 mvp =p*mv;

    //Passando as variáveis uniformes para os shaders
    //model-view : Passa para espaço do olho
    _program->setUniformValue("mv", mv);
    //model-view : Passa para espaço de projeção
    _program->setUniformValue("mvp", mvp);
    //inversa transposta da model-view
    _program->setUniformValue("normalMatrix", mv.inverted().transposed());
    //Variáveis de material e luz
    _program->setUniformValue("light", v * cam.eye);

    //Bola
    _program->setUniformValue("material.ambient", QVector3D(0.2f,0.2f,0.2f));
    _program->setUniformValue("material.diffuse", QVector3D(0.8f,0.8f,0.8f));
    _program->setUniformValue("material.specular", QVector3D(1.0f,1.0f,1.0f));
    _program->setUniformValue("material.shininess", 100.0f);

    //Desenhando os triângulos que formam o cubo
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(_indexPoints.size()), GL_UNSIGNED_INT, nullptr);
    glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
    update();
}

void RenderOpengl::setMode(MeshTypes type)
{
    _program->bind();
    _points.clear();
    _normals.clear();
    _indexNormals.clear();
    _indexPoints.clear();

    if(type == MeshTypes::PLATAFORMA)
    {
        setFile({"../../ModeloObj/modelosObj/U-2300-EQU.obj","../../ModeloObj/modelosObj/U-2300-EST.obj"});
    }
    else if (type == MeshTypes::EQU1)
    {
        setFile({"../../ModeloObj/modelosObj/U-1231-EQU-FEED.obj"});
    }
    else if (type == MeshTypes::EQU2)
    {
        setFile({"../../ModeloObj/modelosObj/U-2300-EQU.obj"});
    }
    else if (type == MeshTypes::EQU3)
    {
         setFile({"../../ModeloObj/modelosObj/U-4530-EQU.obj"});
    }
    else if (type == MeshTypes::EQU4)
    {
         setFile({"../../ModeloObj/modelosObj/U-4710-EQU.obj"});
    }
    else if (type == MeshTypes::EST1)
    {
        setFile({"../../ModeloObj/modelosObj/U-2300-EST.obj"});
    }
    else if (type == MeshTypes::EST2)
    {
        setFile({"../../ModeloObj/modelosObj/U-4710-EST.obj"});
    }
    else if (type == MeshTypes::TUB)
    {
        setFile({"../../ModeloObj/modelosObj/U-4530-TUB.obj"});
    }
    else if (type == MeshTypes::TUBDET)
    {
        setFile({"../../ModeloObj/modelosObj/U-4710-TUB-DETALHAMENTO.obj"});
    }
    else if (type == MeshTypes::CIV)
    {
        setFile({"../../ModeloObj/modelosObj/U-5122-CIV.obj"});
    }

    createVAO();

}



void RenderOpengl::createVAO()
{
    makeCurrent();
    if(!_vao.isCreated())
    {
        //Criando e configurando vao
        _vao.create();
    }
    _vao.bind();

    //Criando buffer de pontos dos vértices
    glGenBuffers(1, &_pointsBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, _pointsBuffer);
    glBufferData(GL_ARRAY_BUFFER, _points.size()*sizeof(QVector3D), &_points[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(0);



    //Criando buffer de normais
    glGenBuffers(1, &_normalsBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, _normalsBuffer);
    glBufferData(GL_ARRAY_BUFFER, _normals.size()*sizeof(QVector3D), &_normals[0], GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(1);

    //Criando buffers de indexPoints
    glGenBuffers(1, &_meshBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _meshBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indexPoints.size()*sizeof(int), _indexPoints.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _meshBuffer);

}

void RenderOpengl::printThings()
{
//    printf("Points: \n");
//    for(int i = 0; i< _points.size(); i ++)
//    {
//        printf( "%f %f %f\n",_points[i].x(),_points[i].y(),_points[i].z());
//    }


//    printf("Normals: \n");
//    for(int i = 0; i< _normals.size(); i ++)
//    {
//        printf( "%f %f %f\n",_normals[i].x(),_normals[i].y(),_normals[i].z());
//    }


//    printf("Indices: \n");
//    for(int i = 0; i< _indexPoints.size(); i ++)
//    {
//        printf( "%d \n",_indexPoints[i]);
//    }

}
void RenderOpengl::resizeGL(int w, int h)
{
    //Atualizar a viewport
    glViewport(0,0,w,h);

    //Atualizar a câmera
    cam.width = w;
    cam.height = h;
    glm::vec3 zero(0);
    radius=((glm::min(h,w)/2.0)-1);
}

QVector3D RenderOpengl::Points_Sphere(QVector3D pointT)
{
    QVector3D pointf;
    double r,s;
    pointf.setX((pointT.x()-(cam.width/2))/radius);
    pointf.setY((pointT.y()-(cam.height/2))/radius);
    r=pointf.x()*pointf.x()+pointf.y()*pointf.y();

    if(r>1.0)
    {
        s=1.0/sqrt(r);
        pointf.setX(s*pointf.x());
        pointf.setY(s*pointf.y());
        pointf.setZ(0);
    }
    else
    {
        pointf.setZ(sqrt(1.0-r));
    }
    return pointf;

}

void RenderOpengl::keyPressEvent(QKeyEvent* event)
{

}

void RenderOpengl::mousePressEvent(QMouseEvent *event)
{
    if(mousepress==false && event->button() == Qt::LeftButton)
    {
        mousepress=true;
       QVector3D point( event->x(), height()-event->y(), 0 ); // Pegando o ponto que está na tela
       point.setZ(0.f);
       p0=Points_Sphere(point);


    }
    //Fit
    if(event->button() == Qt::MiddleButton)
    {
          cam.eye = QVector3D(0.f,20.f,20.f);
          _model= glm::mat4x4(1.f);
    }

    if(event->button() == Qt::RightButton)
    {
       QVector3D point( event->x(), height()-event->y(), 0 ); // Pegando o ponto que está na tela
       _oldPoint = point;
    }

    update();
}


void RenderOpengl::mouseReleaseEvent(QMouseEvent *event)
{

    mousepress=false;
    update();

}


void RenderOpengl::mouseMoveEvent(QMouseEvent *event)
{
    if(mousepress==true)
    {

            QVector3D point( event->x(), height()-event->y(), 0 ); // Pegando o ponto que está na tela
            point.setZ(0.f);
            p1=Points_Sphere(point);
            glm::vec3 pt0(p0.x(),p0.y(),p0.z());
            glm::vec3 pt1(p1.x(),p1.y(),p1.z());
            glm::dquat Q0(0,pt0);
            glm::dquat Q1(0,pt1);
            glm::dquat Qrot=Q1*glm::conjugate(Q0);
            glm::mat4x4 Matrot;
            Matrot=glm::mat4_cast(Qrot);
           _model=Matrot*_model;
           p0=p1;
    }
    update();

}


void RenderOpengl::wheelEvent(QWheelEvent *event)
{
    //Aqui o zoom
      if(event->delta() > 0) //Quer dizer que estou rolando para cima-> zoom in
      {
         cam.eye=cam.eye*0.8;
      }
      else if(event->delta() < 0) //Quer dizer que estou rolando para baixo-> zoom out
      {
           cam.eye=cam.eye/0.9;
      }
      update();
}

