#include "renderopengl.h"
#include <QImage>
#include<QGLWidget>
#include "reader.h"
#include "math.h"
#include <glm/ext.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <QMouseEvent>

RenderOpengl::RenderOpengl(QWidget* parent)
    :QOpenGLWidget(parent)
{
    cam.at = QVector3D(0.0f,0.f,0.f);
    //cam.eye =  QVector3D(0.0f,0.f,3);
    cam.eye =  QVector3D(0.0f,-1.5,2.5);
    cam.up = QVector3D(0.f,1.f,0.f);
    cam.zNear = 0.1f;
    cam.zFar  = 100.f;
    cam.fovy  = 45.f;
    cam.width = 1002;
    cam.height = 701;
    this->setFocus();

    createGrid();
    _mode = 0;
}

RenderOpengl::~RenderOpengl()
{
    delete _program;
}


void RenderOpengl::initializeGL()
{
    initializeOpenGLFunctions();

    //glEnable (GL_BLEND);
    //glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    makeCurrent();
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glClearColor(0,0,0,1);
    glPointSize(10);
    glLineWidth(1);
    _program = new QOpenGLShaderProgram();

    //Adicionando shaders ao programa

    _program->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/vertexshader.glsl");
    //_program->addShaderFromSourceFile(QOpenGLShader::Geometry, ":/shaders/geometryshader.glsl");
    _program->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/fragmentshader.glsl");

    //Linka shaders que foram adicionados ao programa
    _program->link();

    if (!_program->isLinked())
    {
        std::cout<<"Problemas ao linkar shaders"<<std::endl;
    }

    _program->bind();

    createVAO();

}
void RenderOpengl::paintGL()
{

    //Dando bind no programa e no vao
    _program->bind();
    _vao.bind();

    //Definindo matriz view e projection
     _view.setToIdentity();
     _view.lookAt(cam.eye, cam.at, cam.up);
     _proj.setToIdentity();
     _proj.perspective(cam.fovy, (float)cam.width/cam.height, cam.zNear, cam.zFar);

    //Definindo matrizes para passar para os shaders

    QMatrix4x4 m = QMatrix4x4(_model[0][0],_model[0][1],_model[0][2],_model[0][3],
            _model[1][0],_model[1][1],_model[1][2],_model[1][3],
            _model[2][0],_model[2][1],_model[2][2],_model[2][3],
            _model[3][0],_model[3][1],_model[3][2],_model[3][3]);

    //    QMatrix4x4 rot;
    //    rot.rotate(-50,QVector3D(1,0,0));

    const QMatrix4x4& v = _view;
    const QMatrix4x4& p = _proj;

    QMatrix4x4 mv = v*m;
    QMatrix4x4 mvp = p*mv;

    //Variáveis de material e luz
    if(_mode == 0)
    {
        _program->setUniformValue("material.ambient", QVector3D(0.2f,0.2f,0.2f));
        _program->setUniformValue("material.diffuse", QVector3D(0.8f,0.8f,0.8f));
        _program->setUniformValue("material.specular", QVector3D(1.0f,1.0f,1.0f));
        _program->setUniformValue("material.shininess", _shininess);
    }
    else if (_mode == 1)
    {
        _program->setUniformValue("material.ambient", QVector3D(0.9f,0.9f,0.9f));
        _program->setUniformValue("material.diffuse", QVector3D(0.9f,0.9f,0.9f));
        _program->setUniformValue("material.specular", QVector3D(0.0f,0.0f,0.0f));
        _program->setUniformValue("material.shininess", 50.0f);
    }

    //Passando as variáveis uniformes para os shaders
    _program->setUniformValue("mv", mv);
    _program->setUniformValue("mvp", mvp);
    _program->setUniformValue("normalMatrix", mv.inverted().transposed());

    //    QMatrix4x4 rot;
    //    rot.rotate(-50,QVector3D(1,0,0));
    QVector3D posLight(0,2,3);
    QVector3D posLight2(0,3,3);
//    QMatrix4x4 rot;
//    rot.rotate(-50,QVector3D(1,0,0));
//    QVector3D posLight(0,0,3);
//    posLight = rot * posLight;
//    posLight.setY(10);
   // posLight = rot*posLight;
    //Posição da luz e outras variáveis
    _program->setUniformValue("light", mv*posLight/*v * cam.eye /*v*QVector3D(10,10,2)*/);
    _program->setUniformValue("light2", v*posLight2/*v * cam.eye /*v*QVector3D(10,10,2)*/);
    _program->setUniformValue("mode", _mode);
    _program->setUniformValue("numOctaves", _octaves);
    _program->setUniformValue("timez", _timez);
    _program->setUniformValue("timey", _timey);
    _program->setUniformValue("delta", _delta);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    //Desenhando os triângulos que formam o cubo
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(_indexGrid.size()), GL_UNSIGNED_INT, nullptr);

    update();


    //Para reiniciar time...checar

//    _timez = _incZ;
//    if((int)_incZ > _x)
//    {
//        _timez = 2*_x - _incZ;
//    }
//    _incZ += 0.01;
//    if(_incZ > 2 * _x)
//    {
//        _incZ = 0;
//    }

//    _timey = _incY;
//    if((int)_incY > _x)
//    {
//        _timey = 2*_x - _incY;
//    }
//    _incY += 0.01;
//    if(_incY> 2 * _x)
//    {
//        _incY = 0;
//    }
    _timez+=0.001;
    _timey+=0.005;

}

void RenderOpengl::createVAO()
{
    //Criando e configurando vao
    _vao.create();
    _vao.bind();

    //Criando buffer de pontos dos vértices
    glGenBuffers(1, &_pointsTestBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, _pointsTestBuffer);
    glBufferData(GL_ARRAY_BUFFER, _pointsTest.size()*sizeof(QVector2D), _pointsTest.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(3);

    glGenBuffers(1, &_meshBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _meshBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indexGrid.size()*sizeof(int), _indexGrid.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _meshBuffer);

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


void RenderOpengl::createGrid()
{
    //Criando Grid
    _gridWidth = 0;
    _gridHeight = 0;
    _delta = 0.01;
    _pointsTest.clear();

    for(double y = -1; y <= 1; y += _delta)
    {
        for(double x = -1; x <= 1; x += _delta)
        {
            QVector2D point(x,y);
            _pointsTest.push_back(point);
        }
        _gridHeight++;
    }
    _gridWidth = _gridHeight;

    makeTriangleMesh(_gridWidth,_gridHeight);
    printf("width: %d, height: %d",_gridWidth, _gridHeight);
}

unsigned int GetSubgridIndex(int i, int j, int width)
{
    return (unsigned int) i*width + j;
}

void RenderOpengl::makeTriangleMesh(int width, int height)
{
    int numCols = width;
    int numRows = height;
    _indexGrid.clear();
    for(unsigned int i = 0; i < numRows - 1 ; i++)
    {
        for(unsigned int j = 0; j < numCols - 1; j++)
        {
            _indexGrid.push_back(GetSubgridIndex(i,j,numCols));
            _indexGrid.push_back(GetSubgridIndex(i + 1,j,numCols));
            _indexGrid.push_back(GetSubgridIndex(i + 1,j + 1,numCols));

            _indexGrid.push_back(GetSubgridIndex(i + 1,j + 1,numCols));
            _indexGrid.push_back(GetSubgridIndex(i,j + 1,numCols));
            _indexGrid.push_back(GetSubgridIndex(i,j,numCols));
        }
    }

}

void RenderOpengl::setMode(int mode)
{
    _mode = mode;
}

void RenderOpengl::setOctaves(int octave)
{
    _octaves = octave;
}

void RenderOpengl::setShininess(int shi)
{
    _shininess = shi;
}

void RenderOpengl::changeAnimationStatus()
{
    _animation = !_animation;
}



//Relativo ao arcball
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
    if(event->key() == Qt::Key_A)
    {
        cam.eye.setZ(cam.eye.z() - 20);
    }
    else if (event->key() == Qt::Key_Z)
    {
         cam.eye.setZ(cam.eye.z() + 20);
    }
    else if (event->key() == Qt::Key_S)
    {
        cam.eye.setX(cam.eye.x() + 20);
    }
    else if(event->key() == Qt::Key_X)
    {
        cam.eye.setX(cam.eye.x() - 20);
    }
    else if(event->key() == Qt::Key_Y)
    {
        cam.eye.setY(cam.eye.y() + 20);
    }
    else if (event->key() == Qt::Key_H)
    {
        cam.eye.setY(cam.eye.y() - 20);
    }
    else if (event->key() == Qt::Key_F)
    {
        cam.eye = QVector3D(0.f,0.f,300.f);
    }
    update();
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
           cam.eye=cam.eye/0.8;
      }
      update();
}
