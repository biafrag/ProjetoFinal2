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
uniform vec3 light; //Posição da luz no espaço tangente
in vec3 fragNormal;
in vec3 fragPos;
in vec2 UV;
out vec4 finalColor; // Cor final do objeto
uniform sampler2D sampler; //Textura difusa


void main()
{
    //vec3 colorNoise;
    vec4 ambient = material.ambient * texture(sampler,UV);
    vec3 N = normalize(fragNormal);
    vec3 V = normalize(-fragPos);
    vec3 L = normalize(light - fragPos);

    vec4 diffuse = vec4(0.0,0.0,0.0,1);
    vec4 specular = vec4(0.0,0.0,0.0,1);
    float iDif = max(0,dot(L,N));

    diffuse = iDif * material.diffuse * texture(sampler,UV);

    vec3 r = normalize(reflect(-L, N));
    float iSpec = pow(max(dot(V,r),0.0), material.shininess);
    specular = iSpec * material.specular;

    finalColor = diffuse + ambient + specular;

}
