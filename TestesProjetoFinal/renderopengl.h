#pragma once

#include <iostream>
#include <QOpenGLFunctions>
#include<QOpenGLWidget>
#include<QOpenGLShaderProgram>
#include<QOpenGLBuffer>
#include<QOpenGLVertexArrayObject>
#include <QOpenGLExtraFunctions>

#include <glm/glm.hpp>
#include "meshtypes.h"

class RenderOpengl:
        public QOpenGLWidget
        , protected QOpenGLFunctions
{
public:

    RenderOpengl(QWidget *parent);
    ~RenderOpengl();

    //Funções de OpenGl
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

    /**
     * @brief setMode - Define qual vai ser a malha de acordo com o ENUM MeshTypes
     * @param type - Enum que define qual malha será exibida
     */
    void setMode(MeshTypes type);

    /**
     * @brief setOthers - Define se a interface está em um modo (phong com sujeira) ou outro (PBR e bumps)
     * @param isOthers - 0 ou 1 se é outros
     */
    void setOthers(int isOthers);

    /**
     * @brief setDirty - Define se malha ficará suja ou não
     * @param isDirty - 0 não sujo ou 1 sujo
     */
    void setDirty(int isDirty);

    /**
     * @brief setMarble - Define se o mármore vai aparecer
     * @param isMarble - 0 ou 1
     */
    void setMarble(int isMarble);

    /**
     * @brief setBumpType - Define qual será o tipo de bump que aparecerá na interface
     * @param bumpType - O tipo de bump
     */
    void setBumpType(int bumpType);


    void setOption(Options option);
    void setNumberImperfections(int n);
    void setSizeImperfections(int n);

    /**
     * @brief setDirtyType - Define se a sujeira será pesada(Soma de oitavas) ou leve (Turbulência)
     * @param dirtyType - 0 ou 1
     */
    void setDirtyType(int dirtyType);

private:
    //Classe do qt que permite os shaders serem linkados e usados
    QOpenGLShaderProgram* _program{nullptr}; // Programa OpenGl


   struct Camera {
      QVector3D eye;      /* posicao do olho ou centro de projecao conica */
      QVector3D at;       /* ponto para onde a camera aponta              */
      QVector3D up;       /* orientacao da direcao "para cima" da camera  */
      float fovy;         /* angulo de abertura da camera                 */
      float zNear,zFar;   /* distancia do plano proximo e distante        */
      float width,height; /* largura e altura da janela em pixels         */
   };

   //Matrizes de espaços
   Camera cam; //Instância da câmera
   glm::mat4x4 _model; //Matriz model
   QMatrix4x4 _view; //Matriz view
   QMatrix4x4 _proj; //Matriz projection

   //Vetores de coordenadas e indices
   std::vector<QVector3D> _points; //Vetor de pontos
   std::vector<QVector3D> _normals; //Vetor de normais
   std::vector<QVector3D> _tangents; //Vetor de tangentes
   std::vector<QVector2D> _texCoords; //Vetor de coordenadas de textura
   std::vector<unsigned int> _indexPoints; //vetor com os indices que formam os triângulos que serão renderizados
   std::vector<unsigned int> _indexNormals; //vetor com os indices de normais
   std::vector<unsigned int> _indexTex; //vetor com os indices de coordenadas textura

   //Pontos de min e max
   QVector3D _maxMeshPoint; //Maior x e y da malha
   QVector3D _minMeshPoint; //Menor x e y da malha

   //Arcball
   QVector3D _oldPoint; // Pontos antigo para fazer cálculos
   bool mousepress; // Variável que indica se mouse está pressionado ou não
   QVector3D p0,p1; //pontos para fazer rotação
   double radius; //Sphere Radius

   //Buffers
   unsigned int _pointsBuffer = static_cast<unsigned int>(-1); //Buffer de Pontos
   unsigned int _normalsBuffer = static_cast<unsigned int>(-1); //Buffer de Normais
   unsigned int _texCoordsBuffer = static_cast<unsigned int>(-1); //Buffer de coordenadas de textura
   unsigned int _tangentsBuffer = static_cast<unsigned int>(-1); //Buffer de tangentes
   unsigned int _meshBuffer = static_cast<unsigned int>(-1); //Buffer de Indices

   //Buffers para imagens de textura
   unsigned int _textureID;
   unsigned int _textureAlbedo; // Buffer de textura do Albedo
   unsigned int _textureMetallic; // Buffer de textura do Metálico
   unsigned int _textureAo; // Buffer de textura de "Ambient Oclusion"
   unsigned int _textureRoughness; // Buffer de textura de rugosidade

   QOpenGLVertexArrayObject _vao; //Vertex Array Object

   //Teste normal map
   unsigned int _normalMap;

   //Variáveis que ligam interface ao programa
   int _isOthers{0};
   int _isDirty{1};
   int _bumpType{4};
   int _numberImperfections{0};
   int _sizeImperfections{0};
   int _isMarble{0};
   int _dirtyType{0};
   Options _option;


private:
    void createVAO(); //Cria VAO

    //Eventos e funções necessários para Arcball
    virtual void mousePressEvent(QMouseEvent *event) override; //Evento de pressionar o mouse
    virtual void mouseReleaseEvent(QMouseEvent *event) override; //Evento de soltar o mouse
    virtual void mouseMoveEvent(QMouseEvent *event) override; //Evento de mover o mouse
    virtual void wheelEvent(QWheelEvent *event) override; //Evento de mover a roda do mouse
    virtual void keyPressEvent(QKeyEvent* event) override; //Evento de pressionar tecla
    QVector3D Points_Sphere(QVector3D pointT);



    //Funções que organizam malha
    void setFile(std::vector<std::string> fileNames); //Define qual será a malha mostrada e lê arquivo
    void quadToTriangleMesh(std::vector<unsigned int> &indexPointsQuad, std::vector<unsigned int> &indexPointsTriangle, std::vector<unsigned int> &indexNormalsTriangles, std::vector<unsigned int> &indexTexTriangles, std::vector<unsigned int> &indexNormalsQuad, std::vector<unsigned int> &indexTexQuad); //Transforma malha de quads em malha de triângulos
    void organizingData(); // Duplica vértices que precisam ser duplicados (Que possuem índices diferentes do de pontos)
    void getMinMaxMesh(); //Descobre coordenadas de min e max da malha
    void computeTangents(); // Calcula tangentes das malhas
    void createPBRTextures(const std::vector<std::string> imagePath); // Cria, define e seta texturas usadas no PBR

    //Teste Normal Map
    void printThings();

};

