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
//#include <QOpenGLFramebufferObject>
//#include <QOpenGLFramebufferObjectFormat>
#include<QOpenGLExtraFunctions>
#include <QOpenGLFramebufferObject>


#include <glm/glm.hpp>

class RenderOpengl:
        public QOpenGLWidget
        , protected QOpenGLExtraFunctions
{
public:
    RenderOpengl(QWidget *parent);
    ~RenderOpengl();
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;
    void setFile(std::string fileName);
    void setMode(int mode);
    void setOctaves(int octave);
    void setShininess(int shi);
    void changeAnimationStatus();

private:

    //Classe do qt que permite os shaders serem linkados e usados
    QOpenGLShaderProgram* _program{nullptr};
    std::vector<QVector2D> _pointsTest;

    std::vector<int> _indexGrid;
    unsigned int _pointsBuffer = static_cast<unsigned int>(-1);
    unsigned int _normalsBuffer = static_cast<unsigned int>(-1);
    unsigned int _meshBuffer = static_cast<unsigned int>(-1);
    unsigned int _pointsTestBuffer;
    unsigned int _pointsFixedBuffer;
    unsigned int _pointsScreenBuffer = static_cast<unsigned int>(-1);
    unsigned int _textureID;

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
   float angle = 120;
   int _octaves{3};
   bool _animation{true};
   int _gridWidth{0};
   int _gridHeight{0};
   float _delta{0.01};

private:
    void createVAO(); //Cria VAO
    void createGrid();
    void setMode();
    void makeTriangleMesh(int width, int height);
    float _timey{0};
    float _timez{0};
    float _incZ{0};
    float _incY{0};
    float _x{2};
    float _shininess{2000};

    //Arcball
    bool mousepress;
    QVector3D p0,p1; //pontos para fazer rotação
    double radius; //Sphere Radius
    int _mode;

    //Parte do arcball
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void wheelEvent(QWheelEvent *event);
    virtual void keyPressEvent(QKeyEvent* event);
    QVector3D Points_Sphere(QVector3D pointT);
};

#endif // RENDER_H
