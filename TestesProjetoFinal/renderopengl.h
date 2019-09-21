//Usar debugger MSVC2017 32 bits
#ifndef RENDER_H
#define RENDER_H

#include <iostream>
#include <QOpenGLFunctions>
#include<QOpenGLWidget>
#include<QOpenGLShaderProgram>
#include<QOpenGLBuffer>
#include<QOpenGLVertexArrayObject>
#include <QOpenGLExtraFunctions>

#include <glm/glm.hpp>
#include "meshtypes.h"
#include "materialtypes.h"

class RenderOpengl:
        public QOpenGLWidget
        , protected QOpenGLFunctions
{
public:
    RenderOpengl(QWidget *parent);
    ~RenderOpengl();
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

    void setMode(MeshTypes type);
    void setMaterial(MaterialTypes type);
    void setPBR(int isPBR);
    void setOption(Options option);

private:
    //Classe do qt que permite os shaders serem linkados e usados
    QOpenGLShaderProgram* _program{nullptr};


   struct Camera {
      QVector3D eye;      /* posicao do olho ou centro de projecao conica */
      QVector3D at;       /* ponto para onde a camera aponta              */
      QVector3D up;       /* orientacao da direcao "para cima" da camera  */
      float fovy;         /* angulo de abertura da camera                 */
      float zNear,zFar;   /* distancia do plano proximo e distante        */
      float width,height; /* largura e altura da janela em pixels         */
   };

   Camera cam;
   glm::mat4x4 _model;
   QMatrix4x4 _view;
   QMatrix4x4 _proj;

   QOpenGLVertexArrayObject _vao;


private:
    void createVAO(); //Cria VAO
    std::vector<QVector3D> _points; //Vetor de cada ponto do meu objeto que será renderizado
    std::vector<QVector3D> _normals; //Vetor de normal pra cada vértice do meu cubo
    std::vector<QVector2D> _texCoords; //Vetor de coordenadas de textura
    std::vector<int> _indexPoints; //vetor com os indices que formam os triângulos que serão renderizados
    std::vector<int> _indexNormals; //vetor com os indices que formam os triângulos que serão renderizados
    std::vector<int> _indexTex; //vetor com os indices que formam os triângulos que serão renderizados
    QVector3D _maxMeshPoint;
    QVector3D _minMeshPoint;
    MaterialTypes _materialType{MaterialTypes::COPPER};

    QVector3D _oldPoint;
    //Arcball
    bool mousepress;
    QVector3D p0,p1; //pontos para fazer rotação
    //Parte do arcball
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void wheelEvent(QWheelEvent *event);
    virtual void keyPressEvent(QKeyEvent* event);
    double radius; //Sphere Radius
    QVector3D Points_Sphere(QVector3D pointT);
    void setMaterialProperties();
    void createTexture(const std::string &imagePath); //Seta textura difusa
    void quadToTriangleMesh(std::vector<int> &indexPointsQuad, std::vector<int> &indexPointsTriangle, std::vector<int> &indexNormalsTriangles, std::vector<int> &indexTexTriangles, std::vector<int> &indexNormalsQuad, std::vector<int> &indexTexQuad); //Transforma malha de quads em malha de triângulos
    void organizingData();

    void getMinMaxMesh();
    void printThings();
    void createPBRTextures(const std::vector<std::string> imagePath);
    void setFile(std::vector<std::string> fileNames);
    //Buffers
    unsigned int _pointsBuffer = static_cast<unsigned int>(-1);
    unsigned int _normalsBuffer = static_cast<unsigned int>(-1);
    unsigned int _meshBuffer = static_cast<unsigned int>(-1);
    unsigned int _texCoordsBuffer = static_cast<unsigned int>(-1);
    unsigned int _textureID;

    unsigned int _textureAlbedo;
    unsigned int _textureMetallic;
    unsigned int _textureAo;
    unsigned int _textureRoughness;

    int _isPBR{0};
    Options _option;
};

#endif // RENDER_H
