#version 410 core
layout( location = 0 ) in vec3 vertexPos; //Posição do vértice
layout( location = 1 ) in vec3 vertexNormal; //Normal do vértice
layout( location = 2 ) in vec2 vertexTexCoord; //Normal do vértice
//Matrizes
uniform mat4 mvp; //Matriz model view projection
uniform mat4 mv; // Matriz model view
uniform mat4 normalMatrix; //Inversa transposta da MV

//Variáveis out
out vec3 fragNormal; //Normal no espaço do olho
out vec3 fragPos; //Posição no espaço do olho
out vec2 UV;
out vec3 worldPos;
out vec3 worldNorm;
void main()
{
    //Posição do vértice no espaço de projeção
    gl_Position = mvp * vec4( vertexPos, 1 );

    //Posição do vértice no espaço de olho
    fragPos =  (mv * vec4( vertexPos, 1 ) ).xyz;

    //Posição da normal no espaço do olho
    fragNormal = normalize(( normalMatrix * vec4( vertexNormal, 0 ) ).xyz);
    UV = vertexTexCoord;
    worldPos = vertexPos;
    worldNorm = vertexNormal;
}
