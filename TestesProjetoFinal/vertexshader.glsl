#version 410 core
layout( location = 0 ) in vec3 vertexPos; //Posição do vértice
layout( location = 1 ) in vec3 vertexNormal; //Normal do vértice
layout( location = 2 ) in vec2 vertexTexCoord; //Normal do vértice
layout( location = 3 ) in vec3 tangent; //Tangente do vértice

//Matrizes
uniform mat4 mvp; //Matriz model view projection
uniform mat4 mv; // Matriz model view
uniform mat4 normalMatrix; //Inversa transposta da MV

//Variáveis out
out vec3 fragNormal; //Normal no espaço do olho
out vec3 fragPos; //Posição no espaço do olho
out vec2 UV; //Coordenada de textura
out vec3 worldPos; // Posição no espaço do mundo
out vec3 worldNorm; // Normal no espaço do mundo
out vec3 tangPos; // Posição no espaço tangente
out vec3 tangente; // Tangente no espaço do mundo
out vec3 bitangente; // Bitangente no espaço do mundo
out vec3 projPos; // Posição no espaço de projeção

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
    worldNorm = normalize(vertexNormal);
    projPos = vec3 (gl_Position);

    //Posição da tangente no espaço do olho
    vec3 tangentVertexEye = normalize(( normalMatrix * vec4( tangent, 0 ) ).xyz);

    //Bitangente no espaço do olho
    vec3 bitangentVertexEye= normalize(cross(fragNormal,tangentVertexEye));

    //Matriz de rotação tbn para transformar luz para o eapaço tangente
    mat3 rotation = transpose(mat3(tangentVertexEye,bitangentVertexEye,fragNormal));

    tangente = normalize(tangent);
    bitangente = normalize(cross(fragNormal,tangentVertexEye));


}
