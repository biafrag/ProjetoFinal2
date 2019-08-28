#version 330 core

in vec2 UV; //Coordenadas de textura do quad
in vec3 fragPos;
in vec3 fragNormal;
out vec4 finalColor; // Cor final do objeto
uniform vec3 light; // Posição da luz em coordenada do olho
uniform int mode;

struct Material //Propriedades do material
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

uniform Material material;

void main()
{
    if(mode == 0)
    {
        //vec3 colorNoise;
        vec3 ambient = material.ambient * vec3(0,119/255.0,187/255.0);
        vec3 N = normalize(fragNormal)/*normalize(cross(dFdx(fragPos),dFdy(fragPos)))*/;
        vec3 V = normalize(-fragPos);
        vec3 L = normalize(light - fragPos);

        vec3 diffuse = vec3(0.0,0.0,0.0);
        vec3 specular = vec3(0.0,0.0,0.0);
        float iDif = max(0,dot(L,N));

        diffuse = iDif * material.diffuse * vec3(0,119/255.0,187/255.0);

        vec3 r = normalize(reflect(-L, N));
        float iSpec = pow(max(dot(V,r),0.0), material.shininess);
        specular = iSpec * material.specular;

        finalColor = vec4(diffuse + ambient + specular,1);
    }
    else if(mode == 1)
    {
        //vec3 colorNoise;
        vec3 ambient = material.ambient * vec3(40/255.0,26/255.0,13/255.0);
        vec3 N = normalize(fragNormal)/*normalize(cross(dFdx(fragPos),dFdy(fragPos)))*/;
        vec3 V = normalize(-fragPos);
        vec3 L = normalize(light - fragPos);

        vec3 diffuse = vec3(0.0,0.0,0.0);
        vec3 specular = vec3(0.0,0.0,0.0);
        float iDif = max(0,dot(L,N));

        diffuse = iDif * material.diffuse * vec3(40/255.0,26/255.0,13/255.0);;

        vec3 r = normalize(reflect(-L, N));
        float iSpec = pow(max(dot(V,r),0.0), material.shininess);
        specular = iSpec * material.specular;

        finalColor = vec4(diffuse + ambient + specular,1);
        finalColor = vec4(1,0,0,1);
    }


}
