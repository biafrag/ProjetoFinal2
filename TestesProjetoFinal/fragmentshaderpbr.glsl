#version 410 core
const float PI = 3.14159265359;

struct Material //Propriedades do material
{
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    float shininess;
};

const int NR_LIGHTS = 4;

struct Light {
    vec3 Position;
    vec3 Color;
};

uniform Light lights[NR_LIGHTS]; //Vetor de luzes na posição do olho

uniform Material material;

//Variaveis de entrada
uniform vec3 light; //Posição da luz no espaço tangente
in vec3 fragNormal;
in vec3 fragPos;
in vec2 UV;
out vec4 finalColor; // Cor final do objeto
uniform sampler2D sampler; //Textura difusa

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a      = roughness*roughness;
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float num   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float num   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return num / denom;
}
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
    float ggx1  = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}
void main()
{
    float metallic = 0.5f;
    float roughness = 0.5f;
    float ao = 1.0;
    vec3  albedo = vec3 (0.5f,0.0f,0.0f);
    vec3 N = normalize(fragNormal);
    vec3 V = normalize(-fragPos);
    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metallic);

      // reflectance equation
      vec3 Lo = vec3(0.0);
      vec3 specular;
      for(int i = 0; i < 4; i++)
      {
          // calculate per-light radiance
          vec3 L = normalize(lights[i].Position - fragPos);
          vec3 H = normalize(V + L);
          float distance   = length(lights[i].Position - fragPos);
          float attenuation = 1.0 / (distance * distance);
          vec3 radiance = vec3(1,1,1) * attenuation;

          // cook-torrance brdf
          float NDF = DistributionGGX(N, H, roughness);
          float G   = GeometrySmith(N, V, L, roughness);
          vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);

          vec3 kS = F;
          vec3 kD = vec3(1.0) - kS;
          kD *= 1.0 - metallic;

          vec3 numerator    = NDF * G * F;
          float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0);
          specular     = numerator / max(denominator, 0.001);

          // add to outgoing radiance Lo
          float NdotL = max(dot(N, L), 0.0);
          Lo += (kD * albedo / PI + specular) * radiance * NdotL;


      }
      vec3 ambient = vec3(0.05) * albedo * ao;
      vec3 color = ambient + Lo;
      color = color / (color + vec3(1.0));
      color = pow(color, vec3(1.0/2.2));;

      finalColor = vec4(color, 1.0);

}
