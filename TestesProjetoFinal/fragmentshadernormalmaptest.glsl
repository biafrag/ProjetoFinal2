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
in vec3 tang;
in vec3 fragNormal;
in vec3 fragPos;

out vec3 finalColor; // Cor final do objeto

uniform sampler2D normalsampler; // Textura de mapa de normal

vec3 expand(vec3 v)
{
   return (v - 0.5) * 2;
}

void main()
{
    vec3 ambient = material.ambient.rgb;//Componente da luz ambiente
    vec3 specular = vec3(0.0,0.0,0.0);

    //Normal usada eh a de textura de mapa de normal
    vec3 N = normalize(expand(texture(normalsampler,fragUV).rgb));

    //Normalizando novamente a luz no espaço tangente
    vec3 L = normalize(light);

    //Calcula produto interno entre luz e normal no espaco tangente
    float iDif = dot(light,N);

    //Calcula componente difusa da luz
    vec3 diffuse = max(0,iDif) * material.diffuse.rgb;

    finalColor = (ambient + diffuse) /** texture(normalsampler,fragUV).rgb*/;

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

    //finalColor = tang;

}
