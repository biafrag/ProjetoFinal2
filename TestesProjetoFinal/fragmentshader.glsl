#version 410 core

struct Material //Propriedades do material
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

uniform Material material;

//Variaveis de entrada
uniform vec3 light; //Posição da luz no espaço tangente
in vec3 fragNormal;
in vec3 fragPos;
out vec3 finalColor; // Cor final do objeto


void main()
{
    //vec3 colorNoise;
    vec3 ambient = material.ambient * vec3(0.8,0.8,0.8);
    vec3 N = normalize(fragNormal)/*normalize(cross(dFdx(fragPos),dFdy(fragPos)))*/;
    vec3 V = normalize(-fragPos);
    vec3 L = normalize(light - fragPos);

    vec3 diffuse = vec3(0.0,0.0,0.0) * vec3(0.8,0.8,0.8);
    vec3 specular = vec3(0.0,0.0,0.0);
    float iDif = max(0,dot(L,N));

    diffuse = iDif * material.diffuse;

    vec3 r = normalize(reflect(-L, N));
    float iSpec = pow(max(dot(V,r),0.0), material.shininess);
    specular = iSpec * material.specular;

    finalColor = vec3(diffuse + ambient + specular);
   // finalColor = vec3(1,0,1);

}
