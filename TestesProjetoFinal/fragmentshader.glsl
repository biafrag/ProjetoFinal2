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
uniform sampler2D Albedo; //Textura difusa

float mod289(float x){return x - floor(x * (1.0 / 289.0)) * 289.0;}
vec4 mod289(vec4 x){return x - floor(x * (1.0 / 289.0)) * 289.0;}
vec4 perm(vec4 x){return mod289(((x * 34.0) + 1.0) * x);}
float noise(vec3 p){
    vec3 a = floor(p);
    vec3 d = p - a;
    d = d * d * (3.0 - 2.0 * d);

    vec4 b = a.xxyy + vec4(0.0, 1.0, 0.0, 1.0);
    vec4 k1 = perm(b.xyxy);
    vec4 k2 = perm(k1.xyxy + b.zzww);

    vec4 c = k2 + a.zzzz;
    vec4 k3 = perm(c);
    vec4 k4 = perm(c + 1.0);

    vec4 o1 = fract(k3 * (1.0 / 41.0));
    vec4 o2 = fract(k4 * (1.0 / 41.0));

    vec4 o3 = o2 * d.z + o1 * (1.0 - d.z);
    vec2 o4 = o3.yw * d.x + o3.xz * (1.0 - d.x);

    return o4.y * d.y + o4.x * (1.0 - d.y);
}



void main()
{
    //vec3 colorNoise;
    vec4 ambient = material.ambient;
    vec3 N = normalize(fragNormal);
    vec3 V = normalize(-fragPos);
    vec3 L = normalize(light - fragPos);

    vec4 diffuse = vec4(0.0,0.0,0.0,1);
    vec4 specular = vec4(0.0,0.0,0.0,1);
    float iDif = max(0,dot(L,N));

    diffuse = iDif * material.diffuse;

    vec3 r = normalize(reflect(-L, N));
    float iSpec = pow(max(dot(V,r),0.0), material.shininess);
    specular = iSpec * material.specular;

    finalColor = diffuse + ambient + specular;

}
