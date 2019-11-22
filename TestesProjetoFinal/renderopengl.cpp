#include "renderopengl.h"
#include <QImage>
#include<QGLWidget>
#include "reader.h"
#include "reader2.h"
#include "readeroff.h"
#include "math.h"
#include <glm/ext.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <QMouseEvent>

RenderOpengl::RenderOpengl(QWidget* parent)
    :QOpenGLWidget(parent)
{
    //Setando paramteros de camera
    cam.at = QVector3D(0.f,0.f,0.f);
    cam.eye =  QVector3D(0.f,0.f,6);
    cam.up = QVector3D(0.f,3.f,0.f);
    cam.zNear = 0.1;
    cam.zFar  = 500.f;
    cam.fovy  = 30.f;
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
    glDeleteBuffers(1, &_texCoordsBuffer);
    glDeleteBuffers(1, &_tangentsBuffer);
}

//Funções de OpenGl

void RenderOpengl::initializeGL()
{
    initializeOpenGLFunctions();

    makeCurrent();
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_FRAMEBUFFER_SRGB);
    glDepthFunc(GL_LESS);
    glClearColor(0,0,0,1);

    _program = new QOpenGLShaderProgram();
    //Adicionando shaders ao programa

    _program->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/vertexshader.glsl");
    _program->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/fragmentshaderperlinnoise.glsl");

    //Linka shaders que foram adicionados ao programa
    _program->link();

    if (!_program->isLinked())
    {
        std::cout<<"Problemas ao linkar shaders"<<std::endl;
    }

    setMode(MeshTypes::ESFERA);
    _program->bind();
    createPBRTextures({"../MalhasTeste/Texturas/albedo.png","../MalhasTeste/Texturas/metalness.png","../MalhasTeste/Texturas/roughness.png","../MalhasTeste/Texturas/ao.png"});
}


void RenderOpengl::paintGL()
{
    //Dando bind no programa e no vao
    _program->bind();
    _vao.bind();

    glViewport(0, 0, cam.width, cam.height);

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
    _program->setUniformValue("light",v * cam.eye); //Luz do Phong

    //Setando propriedades da luz usado no Phong
    _program->setUniformValue("material.ambient", QVector4D(0.19, 0.19, 0.19,1));
    _program->setUniformValue("material.diffuse", QVector4D(0.51, 0.51, 0.51,1));
    _program->setUniformValue("material.specular", QVector4D(0.51, 0.51, 0.51,1));
    _program->setUniformValue("material.shininess", 51.2f);

    //4 luzes do PBR
    _program->setUniformValue("lights[0].Position", v*QVector3D(1,1,2));
    _program->setUniformValue("lights[1].Position",  v*QVector3D(-1,1,2));
    _program->setUniformValue("lights[2].Position",  v*QVector3D(1,-1,2));
    _program->setUniformValue("lights[3].Position",  v*QVector3D(-1,-1,2));

    //Variáveis para ajustar à interface
    _program->setUniformValue("isOthers",  _isOthers);
    _program->setUniformValue("isDirty",  _isDirty);
    _program->setUniformValue("dirtyType",  _dirtyType);
    _program->setUniformValue("isMarble",  _isMarble);
    _program->setUniformValue("option", (int)_option);
    _program->setUniformValue("bumpType", _bumpType);
    _program->setUniformValue("sizeImperfections", _sizeImperfections);
    _program->setUniformValue("numberImperfections", _numberImperfections);

    //Ativar e linkar texturas do PBR
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _textureAlbedo);
    unsigned int albedoLocation = glGetUniformLocation(_program->programId(), "Albedo");
    glUniform1i(albedoLocation, 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, _textureMetallic);
    unsigned int metallicLocation = glGetUniformLocation(_program->programId(), "Metallic");
    glUniform1i(metallicLocation, 1);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, _textureRoughness);
    unsigned int roughnessLocation = glGetUniformLocation(_program->programId(), "Roughness");
    glUniform1i(roughnessLocation, 2);

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, _textureAo);
    unsigned int aoLocation = glGetUniformLocation(_program->programId(), "Ao");
    glUniform1i(aoLocation, 3);

    //Desenhando os triângulos que formam o cubo
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(_indexPoints.size()), GL_UNSIGNED_INT, nullptr);
    glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);

    update();
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


//Funções de malha

void RenderOpengl::organizingData()
{
    //Organiza dados, duplica vértices que tem que duplicar

    std::vector<QVector3D> points; //Vetor de cada ponto do meu objeto que será renderizado
    std::vector<QVector3D> normals; //Vetor de normal pra cada vértice do meu cubo
    std::vector<QVector2D> texCoords; //Vetor de coordenadas de textura
    std::vector<unsigned int> indexes; //Vetor de indices

    //Duplicando ou não pontos
    for(unsigned int i = 0; i<_indexPoints.size(); i++)
    {
        unsigned int idP = _indexPoints[i];
        unsigned int idN = _indexNormals[i];
        unsigned int idT = _indexTex[i];

        points.push_back(_points[idP]);
        normals.push_back(_normals[idN]);
        texCoords.push_back(_texCoords[idT]);
        indexes.push_back(i);
    }

    _points = points;
    _normals = normals;
    _texCoords = texCoords;
    _indexPoints = indexes;

}

void RenderOpengl::quadToTriangleMesh(std::vector<unsigned int>& indexPointsQuad, std::vector<unsigned int>& indexPointsTriangle, std::vector<unsigned int>& indexNormalsTriangle, std::vector<unsigned int>& indexTexTriangle, std::vector<unsigned int>& indexNormalsQuad,std::vector<unsigned int>& indexTexQuad)
{
    //Transforma malha de quads em malha de triângulos
    std::vector<unsigned int> triangleMesh;

    unsigned int numberofQuads = static_cast<unsigned int>(indexPointsQuad.size() / 4);

    //Every four elements it's a quadrilateral element
    for(unsigned int i = 0; i < numberofQuads; i++)
    {
        unsigned int v0 = indexPointsQuad[4 * i];
        unsigned int v1 = indexPointsQuad[4 * i + 1];
        unsigned int v2 = indexPointsQuad[4 * i + 2];
        unsigned int v3 = indexPointsQuad[4 * i + 3];

        unsigned int n0 = indexNormalsQuad[4 * i];
        unsigned int n1 = indexNormalsQuad[4 * i + 1];
        unsigned int n2 = indexNormalsQuad[4 * i + 2];
        unsigned int n3 = indexNormalsQuad[4 * i + 3];

        unsigned int t0 = indexTexQuad[4 * i];
        unsigned int t1 = indexTexQuad[4 * i + 1];
        unsigned int t2 = indexTexQuad[4 * i + 2];
        unsigned int t3 = indexTexQuad[4 * i + 3];

        //First triangle from quadrilateral element
        _indexPoints.push_back(v0);
        _indexPoints.push_back(v1);
        _indexPoints.push_back(v3);

        //Second triangle from quadrilateral element
        _indexPoints.push_back(v2);
        _indexPoints.push_back(v3);
        _indexPoints.push_back(v1);


        //First triangle from quadrilateral element
        _indexNormals.push_back(n0);
        _indexNormals.push_back(n1);
        _indexNormals.push_back(n3);

        //Second triangle from quadrilateral element
        _indexNormals.push_back(n2);
        _indexNormals.push_back(n3);
        _indexNormals.push_back(n1);

        //First triangle from quadrilateral element
        _indexTex.push_back(t0);
        _indexTex.push_back(t1);
        _indexTex.push_back(t3);

        //Second triangle from quadrilateral element
        _indexTex.push_back(t2);
        _indexTex.push_back(t3);
        _indexTex.push_back(t1);

    }

    for(unsigned int i = 0; i<indexPointsTriangle.size(); i++)
    {
        _indexPoints.push_back(indexPointsTriangle[i]);
        _indexNormals.push_back(indexNormalsTriangle[i]);
        _indexTex.push_back(indexTexTriangle[i]);
    }

}

void RenderOpengl::getMinMaxMesh()
{
    float maxX,maxY,maxZ;
    float minX,minY,minZ;
    maxX = maxY = maxZ = -2000000;
    minX = minY = minZ = 2000000;
    for(unsigned int i = 0; i < _points.size(); i++)
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

    _minMeshPoint.setX(minX);
    _minMeshPoint.setY(minY);
    _minMeshPoint.setZ(minZ);
    _maxMeshPoint.setX(maxX);
    _maxMeshPoint.setY(maxY);
    _maxMeshPoint.setZ(maxZ);

    QVector3D medium((_maxMeshPoint + _minMeshPoint)/2);

    cam.eye =  QVector3D(0.f,0.f,4*_maxMeshPoint.z());

    //Organiza pontos para ficarem centrados no meio
    for(unsigned int i = 0; i < _points.size(); i++)
    {
        _points[i] = _points[i] - medium;
    }
}

void RenderOpengl::computeTangents()
{
    //Abordagem em funcionamento
    std::vector<QVector3D> tanA;
    std::vector<QVector3D> tanB;
    tanA.resize(_points.size());
    tanB.resize(_points.size());
    for(int i = 0; i< tanA.size();i++)
    {
        tanA[i] = QVector3D(0,0,0);
        tanB[i] = QVector3D(0,0,0);
    }

    _tangents.resize(_points.size());
    for (size_t i = 0; i < _indexPoints.size()/3; i++)
    {
            size_t i0 = _indexPoints[3*i];
            size_t i1 = _indexPoints[3*i + 1];
            size_t i2 = _indexPoints[3*i + 2];

            QVector3D pos0 = _points[i0];
            QVector3D pos1 = _points[i1];
            QVector3D pos2 = _points[i2];

            QVector2D tex0 = _texCoords[i0];
            QVector2D tex1 = _texCoords[i1];
            QVector2D tex2 = _texCoords[i2];

            QVector3D edge1 = pos1 - pos0;
            QVector3D edge2 = pos2 - pos0;

            QVector2D uv1 = tex1 - tex0;
            QVector2D uv2 = tex2 - tex0;

            float r = 1.0f / (uv1.x() * uv2.y() - uv1.y() * uv2.x());

            //Calculando dois vetores perpendiculares à normal (tangente e bitangente)
            QVector3D tangent(
                ((edge1.x() * uv2.y()) - (edge2.x() * uv1.y())) * r,
                ((edge1.y() * uv2.y()) - (edge2.y() * uv1.y())) * r,
                ((edge1.z() * uv2.y()) - (edge2.z() * uv1.y())) * r
            );

            QVector3D bitangent(
                ((edge1.x() * uv2.x()) - (edge2.x() * uv1.x())) * r,
                ((edge1.y() * uv2.x()) - (edge2.y() * uv1.x())) * r,
                ((edge1.z() * uv2.x()) - (edge2.z() * uv1.x())) * r
            );

            tanA[i0] += tangent;
            tanA[i1] += tangent;
            tanA[i2] += tangent;

            tanB[i0] += bitangent;
            tanB[i1] += bitangent;
            tanB[i2] += bitangent;
        }

        // (2)
        for (size_t i = 0; i < _points.size(); i++)
        {
            QVector3D n = _normals[i];
            QVector3D t0 = tanA[i];
            QVector3D t1 = tanB[i];

            QVector3D t = t0 - (n * QVector3D::dotProduct(n, t0));
            t.normalize();
            _tangents[i] = QVector3D(t.x(), t.y(), t.z());
        }

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

    //Criando buffers de textura
    glGenBuffers(1, &_texCoordsBuffer);
    glBindBuffer(GL_ARRAY_BUFFER,_texCoordsBuffer);
    glBufferData(GL_ARRAY_BUFFER, _texCoords.size()*sizeof(QVector2D), &_texCoords[0], GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(2);

    //Criando buffer de tangentes
    glGenBuffers(1, &_tangentsBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, _tangentsBuffer);
    glBufferData(GL_ARRAY_BUFFER, _tangents.size()*sizeof(QVector3D), &_tangents[0], GL_STATIC_DRAW);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(3);

    //Criando buffers de indexPoints
    glGenBuffers(1, &_meshBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _meshBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indexPoints.size()*sizeof(int), _indexPoints.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _meshBuffer);

}

//Funções de comunicação com interface

void RenderOpengl::setFile(std::vector<std::string> fileNames)
{
        std::vector<unsigned int> indexPointsQuad;
        std::vector<unsigned int> indexPointsTriangle;
        std::vector<unsigned int> indexNormalsTriangle;
        std::vector<unsigned int> indexNormalsQuads;
        std::vector<unsigned int> indexTexTriangle;
        std::vector<unsigned int> indexTexQuads;
        _points.clear();
        _normals.clear();
        _texCoords.clear();
        _indexPoints.clear();
        _indexNormals.clear();
        _indexTex.clear();

        for(unsigned int i = 0; i < fileNames.size(); i++)
        {
            readFile2(fileNames[i],_points,_normals,_texCoords,indexPointsTriangle,indexPointsQuad,indexNormalsTriangle,indexTexTriangle,indexNormalsQuads,indexTexQuads);
        }
        quadToTriangleMesh(indexPointsQuad, indexPointsTriangle,indexNormalsTriangle,indexTexTriangle,indexNormalsQuads,indexTexQuads);
        organizingData();
        getMinMaxMesh();
        computeTangents();
        printThings();
        _model = glm::mat4x4(1.0);
}
void RenderOpengl::setMode(MeshTypes type)
{
    _program->bind();

    if(type == MeshTypes::ESFERA)
    {
        setFile({"../MalhasTeste/MalhasComTextura/golfball.obj"});
    }
    else if (type == MeshTypes::BULE)
    {
        setFile({"../MalhasTeste/MalhasComTextura/teapot.obj"});
    }
    else if (type == MeshTypes::TROFEU)
    {
        setFile({"../MalhasTeste/MalhasComTextura/trofeu.obj"});
    }
    else if (type == MeshTypes::VENTILADOR)
    {
         setFile({"../MalhasTeste/ventilator.obj"});
    }
    else if (type == MeshTypes::BRINCOS)
    {
         setFile({"../MalhasTeste/MalhasComTextura/brincos.obj"});
    }
    else if (type == MeshTypes::LATA)
    {
        setFile({{"../MalhasTeste/MalhasComTextura/lata.obj"}});
    }
    else if (type == MeshTypes::ROBO)
    {
        setFile({{"../MalhasTeste/MalhasComTextura/robot2.obj"}});
    }

    createVAO();

}
void RenderOpengl::setOthers(int isOthers)
{
    _isOthers = isOthers;
}

void RenderOpengl::setDirty(int isDirty)
{
    _isDirty = isDirty;
}

void RenderOpengl::setMarble(int isMarble)
{
    _isMarble = isMarble;
}

void RenderOpengl::setBumpType(int bumpType)
{
    _bumpType = bumpType;
}

void RenderOpengl::setOption(Options option)
{
    _option = option;
}

void RenderOpengl::setNumberImperfections(int n)
{
    _numberImperfections = n;
}

void RenderOpengl::setSizeImperfections(int n)
{
    _sizeImperfections = n;
}

void RenderOpengl::setDirtyType(int dirtyType)
{
    _dirtyType = dirtyType;
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

//    printf("Tangentes: \n");
//    for(unsigned int i = 0; i< _tangents.size(); i ++)
//    {
//        printf( "%f %f %f\n",_tangents[i].x(),_tangents[i].y(),_tangents[i].z());
//    }

//    printf("Coordenadas de Textura: \n");
//    for(unsigned int i = 0; i< _texCoords.size(); i ++)
//    {
//        printf( "%f %f \n",_texCoords[i].x(),_texCoords[i].y());
//    }


//    printf("Indices: \n");
//    for(int i = 0; i< _indexPoints.size(); i ++)
//    {
//        printf( "%d \n",_indexPoints[i]);
//    }

}

void RenderOpengl::createPBRTextures(const std::vector<std::string> imagePath)
{
    //TEXTURA ALBEDO
    //Gerando textura e recebendo ID dessa textura
    glGenTextures(1, &_textureAlbedo);

    //Linkar (bind) a textura criada
    glBindTexture(GL_TEXTURE_2D, _textureAlbedo);

    //Abrir arquivo de imagem com o Qt
    QImage texImage = QGLWidget::convertToGLFormat(QImage(imagePath[0].c_str()));
    //QImage texImage(imagePath.c_str());

    //Enviar a imagem para o OpenGL
    glTexImage2D(GL_TEXTURE_2D, 0,GL_RGBA,
                 texImage.width(), texImage.height(),
                 0, GL_RGBA, GL_UNSIGNED_BYTE, texImage.bits());

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    glGenerateMipmap(GL_TEXTURE_2D);

    //TEXTURA METALLIC

    //Gerando textura e recebendo ID dessa textura
    glGenTextures(1, &_textureMetallic);

    //Linkar (bind) a textura criada
    glBindTexture(GL_TEXTURE_2D, _textureMetallic);

    //Abrir arquivo de imagem com o Qt
    texImage = QGLWidget::convertToGLFormat(QImage(imagePath[1].c_str()));
    //QImage texImage(imagePath.c_str());

    //Enviar a imagem para o OpenGL
    glTexImage2D(GL_TEXTURE_2D, 0,GL_RGBA,
                 texImage.width(), texImage.height(),
                 0, GL_RGBA, GL_UNSIGNED_BYTE, texImage.bits());

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    glGenerateMipmap(GL_TEXTURE_2D);

    //TEXTURA ROUGHNESS

    //Gerando textura e recebendo ID dessa textura
    glGenTextures(1, &_textureRoughness);

    //Linkar (bind) a textura criada
    glBindTexture(GL_TEXTURE_2D, _textureRoughness);

    //Abrir arquivo de imagem com o Qt
    texImage = QGLWidget::convertToGLFormat(QImage(imagePath[2].c_str()));
    //QImage texImage(imagePath.c_str());

    //Enviar a imagem para o OpenGL
    glTexImage2D(GL_TEXTURE_2D, 0,GL_RGBA,
                 texImage.width(), texImage.height(),
                 0, GL_RGBA, GL_UNSIGNED_BYTE, texImage.bits());

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    glGenerateMipmap(GL_TEXTURE_2D);

    //TEXTURA AO

    //Gerando textura e recebendo ID dessa textura
    glGenTextures(1, &_textureAo);

    //Linkar (bind) a textura criada
    glBindTexture(GL_TEXTURE_2D, _textureAo);

    //Abrir arquivo de imagem com o Qt
    texImage = QGLWidget::convertToGLFormat(QImage(imagePath[3].c_str()));
    //QImage texImage(imagePath.c_str());

    //Enviar a imagem para o OpenGL
    glTexImage2D(GL_TEXTURE_2D, 0,GL_RGBA,
                 texImage.width(), texImage.height(),
                 0, GL_RGBA, GL_UNSIGNED_BYTE, texImage.bits());

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    glGenerateMipmap(GL_TEXTURE_2D);
}

//Arcball
void RenderOpengl::keyPressEvent(QKeyEvent*)
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


void RenderOpengl::mouseReleaseEvent(QMouseEvent *)
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
