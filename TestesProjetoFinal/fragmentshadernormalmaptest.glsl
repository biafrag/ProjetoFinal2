#version 410 core

struct Material //Propriedades do material
{
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    float shininess;
};

uniform Material material;

//Variaveis de entrada
//Variaveis de entrada
in vec2 fragUV; //Coordenada de textura
in vec3 light; //Posição da luz no espaço tangente
in vec3 tanViewer; // Posição do viewer no espaço tangente
in vec3 tangente;
in vec3 bitangente;
in vec3 fragNormal;
in vec3 fragPos;

out vec3 finalColor; // Cor final do objeto

uniform sampler2D normalsampler; // Textura de mapa de normal

vec3 expand(vec3 v)
{
   return (v - 0.5) * 2;
}

mat3 inverse(mat3 m) {
  float a00 = m[0][0], a01 = m[0][1], a02 = m[0][2];
  float a10 = m[1][0], a11 = m[1][1], a12 = m[1][2];
  float a20 = m[2][0], a21 = m[2][1], a22 = m[2][2];

  float b01 = a22 * a11 - a12 * a21;
  float b11 = -a22 * a10 + a12 * a20;
  float b21 = a21 * a10 - a11 * a20;

  float det = a00 * b01 + a01 * b11 + a02 * b21;

  return mat3(b01, (-a22 * a01 + a02 * a21), (a12 * a01 - a02 * a11),
              b11, (a22 * a00 - a02 * a20), (-a12 * a00 + a02 * a10),
              b21, (-a21 * a00 + a01 * a20), (a11 * a00 - a01 * a10)) / det;
}

void main()
{
    vec3 ambient = material.ambient.rgb;//Componente da luz ambiente
    vec3 specular = vec3(0.0,0.0,0.0);

    //Normal usada eh a de textura de mapa de normal
    vec3 N = normalize(expand(texture(normalsampler,fragUV).rgb));

    //Matriz de rotação tbn para transformar luz para o eapaço tangente
    mat3 rotation = inverse(transpose(mat3(tangente,bitangente,fragNormal)));

    vec3 G = N;
    N = /*rotation*N*/fragNormal;

    //Normalizando novamente a luz no espaço tangente
    vec3 L = normalize(light);

    //Calcula produto interno entre luz e normal no espaco tangente
    float iDif = dot(light,N);

    //Calcula componente difusa da luz
    vec3 diffuse = max(0,iDif) * material.diffuse.rgb;

    finalColor = (ambient + diffuse)*G;

    //Se certifica que a luz e a normal nao sao perpendiculares
    if( iDif > 0 )
    {
        //Viewer
        vec3 V = tanViewer;

        //HalfVector
        vec3 H = normalize(L + V);

        float iSpec = pow(max(dot(N,H),0.0),material.shininess);

        //Calcula componente especular
        specular = iSpec * material.specular.rgb;
    }

    finalColor += specular;

    //finalColor = vec3(1,0,1);

}

