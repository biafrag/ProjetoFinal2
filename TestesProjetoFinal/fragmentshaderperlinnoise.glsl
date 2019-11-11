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
uniform int isPBR; //Variavel indicando se estamos usando PBR
uniform int isDirty; //Variavel indicando se no Phong colocamos sujeira com noise
uniform int dirtyType;
uniform int isMarble;
uniform int option; //Variável indicando qual o tipo de PBR, (sem noise, com noise no albedo etc)
uniform mat4 normalMatrix; //Inversa transposta da MV
uniform int bumpType;
uniform int sizeImperfections;
uniform int numberImperfections;

in vec3 fragNormal;
in vec3 fragPos;
in vec2 UV;
in vec3 worldPos;
in vec3 worldNorm;
in vec3 projPos;

in vec3 tangPos;

in vec3 tangente;
in vec3 bitangente;
out vec4 finalColor; // Cor final do objeto

uniform sampler2D Albedo;
uniform sampler2D Metallic;
uniform sampler2D Ao;
uniform sampler2D Roughness;

//Função que calcula a inversa de uma matriz m
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

vec3 mod289(vec3 x)
{
  return x - floor(x * (1.0 / 289.0)) * 289.0;
}

vec4 mod289(vec4 x)
{
  return x - floor(x * (1.0 / 289.0)) * 289.0;
}

vec4 permute(vec4 x)
{
  return mod289(((x*34.0)+1.0)*x);
}

vec4 taylorInvSqrt(vec4 r)
{
  return 1.79284291400159 - 0.85373472095314 * r;
}

vec3 fade(vec3 t) {
  return t*t*t*(t*(t*6.0-15.0)+10.0);
}

// Classic Perlin noise
float noise(vec3 P)
{
  vec3 Pi0 = floor(P); // Integer part for indexing
  vec3 Pi1 = Pi0 + vec3(1.0); // Integer part + 1
  Pi0 = mod289(Pi0);
  Pi1 = mod289(Pi1);
  vec3 Pf0 = fract(P); // Fractional part for interpolation
  vec3 Pf1 = Pf0 - vec3(1.0); // Fractional part - 1.0
  vec4 ix = vec4(Pi0.x, Pi1.x, Pi0.x, Pi1.x);
  vec4 iy = vec4(Pi0.yy, Pi1.yy);
  vec4 iz0 = Pi0.zzzz;
  vec4 iz1 = Pi1.zzzz;

  vec4 ixy = permute(permute(ix) + iy);
  vec4 ixy0 = permute(ixy + iz0);
  vec4 ixy1 = permute(ixy + iz1);

  vec4 gx0 = ixy0 * (1.0 / 7.0);
  vec4 gy0 = fract(floor(gx0) * (1.0 / 7.0)) - 0.5;
  gx0 = fract(gx0);
  vec4 gz0 = vec4(0.5) - abs(gx0) - abs(gy0);
  vec4 sz0 = step(gz0, vec4(0.0));
  gx0 -= sz0 * (step(0.0, gx0) - 0.5);
  gy0 -= sz0 * (step(0.0, gy0) - 0.5);

  vec4 gx1 = ixy1 * (1.0 / 7.0);
  vec4 gy1 = fract(floor(gx1) * (1.0 / 7.0)) - 0.5;
  gx1 = fract(gx1);
  vec4 gz1 = vec4(0.5) - abs(gx1) - abs(gy1);
  vec4 sz1 = step(gz1, vec4(0.0));
  gx1 -= sz1 * (step(0.0, gx1) - 0.5);
  gy1 -= sz1 * (step(0.0, gy1) - 0.5);

  vec3 g000 = vec3(gx0.x,gy0.x,gz0.x);
  vec3 g100 = vec3(gx0.y,gy0.y,gz0.y);
  vec3 g010 = vec3(gx0.z,gy0.z,gz0.z);
  vec3 g110 = vec3(gx0.w,gy0.w,gz0.w);
  vec3 g001 = vec3(gx1.x,gy1.x,gz1.x);
  vec3 g101 = vec3(gx1.y,gy1.y,gz1.y);
  vec3 g011 = vec3(gx1.z,gy1.z,gz1.z);
  vec3 g111 = vec3(gx1.w,gy1.w,gz1.w);

  vec4 norm0 = taylorInvSqrt(vec4(dot(g000, g000), dot(g010, g010), dot(g100, g100), dot(g110, g110)));
  g000 *= norm0.x;
  g010 *= norm0.y;
  g100 *= norm0.z;
  g110 *= norm0.w;
  vec4 norm1 = taylorInvSqrt(vec4(dot(g001, g001), dot(g011, g011), dot(g101, g101), dot(g111, g111)));
  g001 *= norm1.x;
  g011 *= norm1.y;
  g101 *= norm1.z;
  g111 *= norm1.w;

  float n000 = dot(g000, Pf0);
  float n100 = dot(g100, vec3(Pf1.x, Pf0.yz));
  float n010 = dot(g010, vec3(Pf0.x, Pf1.y, Pf0.z));
  float n110 = dot(g110, vec3(Pf1.xy, Pf0.z));
  float n001 = dot(g001, vec3(Pf0.xy, Pf1.z));
  float n101 = dot(g101, vec3(Pf1.x, Pf0.y, Pf1.z));
  float n011 = dot(g011, vec3(Pf0.x, Pf1.yz));
  float n111 = dot(g111, Pf1);

  vec3 fade_xyz = fade(Pf0);
  vec4 n_z = mix(vec4(n000, n100, n010, n110), vec4(n001, n101, n011, n111), fade_xyz.z);
  vec2 n_yz = mix(n_z.xy, n_z.zw, fade_xyz.y);
  float n_xyz = mix(n_yz.x, n_yz.y, fade_xyz.x);
  return 2.2 * n_xyz;
}

float turbulence(vec3 pos)
{
    float z = 0;
    float frequency = 4;
    float persistency = 0.5;
    int numOctaves = 4;
    for(int i = 0; i < numOctaves;i++)
    {
        z += abs(noise(frequency * pos)) * persistency;
        frequency *= 2;
        persistency /= 2;
    }
    return z;
}

float sumOctaves(vec3 pos)
{
    float z = 0;
    float frequency = 4;
    float persistency = 0.5;
    int numOctaves = 4;
    for(int i = 0; i < 20;i++)
    {
        z += noise(frequency * pos) * persistency;
        frequency *= 2;
        persistency /= 2;
    }
    return z + 0.5 ;
}


float calculateNoise1(vec3 pos)
{
    float z = turbulence(pos);
    return z;
}



float calculateNoise2(vec3 pos)
{
    float z = 0;
    float oct1 =  noise(4*pos)*0.5;
    float oct2 = noise(8*pos)*0.25;
    float oct3 = noise(16*pos)*0.25/2;
    float oct4 = noise(32*pos)*0.25/4;

    z =  min(0.1,turbulence(pos));
//    z = sumOctaves(pos);
    return z;
}

float calculateNoise3(vec3 pos)
{
    float z = 0;
    float oct1 =  noise(4*pos)*0.5;
    float oct2 = noise(8*pos)*0.25;
    z = abs(oct1 - 0.3) * oct2/oct1;
    return z;
}

float calculateNoise4(vec3 pos)
{
    float z = 0;
    float oct1 =  noise(4*pos)*0.5;
    float oct2 = noise(8*pos)*0.25;
    float oct3 = noise(16*pos)*0.25/2;
    float oct4 = noise(32*pos)*0.25/4;
    float oct5 = noise(64*pos)*0.25/8;

    float c;
    if(sizeImperfections == 0)
    {
        c = 0.05;
    }
    else if (sizeImperfections == 1)
    {
        c = 0.1;
    }
    else if (sizeImperfections == 2)
    {
        c = 0.2;
    }
    else
    {
        c = 0.3;
    }

    float z1 = 6*min(c,oct1) + oct1/4;
    float z2 = 6*min(c,oct2) + oct2/4;
    float z3 = 6*min(c,oct3) + oct3/4;
    float z4 = 6*min(c,oct4) + oct4/4;
    float z5 = 2*min(0.1,oct1) + oct1/4 + 6*min(0.03,oct2) + oct2/4;

    z =  min(c,sumOctaves(pos));
    return z;
}
float calculateNoiseMarble(vec3 pos)
{
    float oct1 =  noise(2*pos)*0.5;
    float oct2 = noise(4*pos)*0.25;
    float oct3 = noise(8*pos)*0.25/2;
    float oct4 = noise(16*pos)*0.25/4;

    float intensity = abs(oct1) + abs(oct2) + abs(oct3) + abs(oct4);

    float sineval = sin(pos.y * 24.0 + intensity * 48.0) * 0.5 + 0.5;
    return sineval;

}
vec3 calculateMarbleColor(vec3 pos)
{
    float f = calculateNoiseMarble(pos);
    vec3 color = mix(vec3(0.4,0.4,0.4), vec3(0.8,0.8,0.8), f);
    return color;
}

float calculateNoiseTeste(vec3 pos)
{
    float z = 0;
    float oct1 =  noise(4*pos)*0.5;
    float oct2 = noise(8*pos)*0.25;
    float oct3 = noise(16*pos)*0.25/2;
    float oct4 = noise(32*pos)*0.25/4;
    float oct5 = noise(64*pos)*0.25/8;

    float c;
    if(sizeImperfections == 0)
    {
        c = 0.05;
    }
    else if (sizeImperfections == 1)
    {
        c = 0.1;
    }
    else if (sizeImperfections == 2)
    {
        c = 0.2;
    }
    else
    {
        c = 0.3;
    }

    float z1 = 6*min(c,oct1) + oct1/4;
    float z2 = 6*min(c,oct2) + oct2/4;
    float z3 = 6*min(c,oct3) + oct3/4;
    float z4 = 6*min(c,oct4) + oct4/4;
    float z5 = 2*min(0.1,oct1) + oct1/4 + 6*min(0.03,oct2) + oct2/4;

    z =  min(c,turbulence(pos));
    return z;
}

//Funções relativas ao PBR
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

//Função para calcular o vetor normal (Do BumpMap)
vec3 calculateNormal(int type)
{
    //Pegando normal usando pontos da vertical e horizontal
    vec3 left = projPos - dFdx(projPos);
    vec3 right = projPos + dFdx(projPos);
    vec3 up = projPos + dFdy(projPos);
    vec3 down = projPos - dFdy(projPos);

    if(type == 0)
    {
        left.z = calculateNoise1(worldPos - dFdx(worldPos));
        right.z = calculateNoise1(worldPos + dFdx(worldPos));
        up.z = calculateNoise1(worldPos + dFdy(worldPos));
        down.z = calculateNoise1(worldPos - dFdy(worldPos));
    }
    else if(type == 1)
    {
        left.z = calculateNoise2(worldPos - dFdx(worldPos));
        right.z = calculateNoise2(worldPos + dFdx(worldPos));
        up.z = calculateNoise2(worldPos + dFdy(worldPos));
        down.z = calculateNoise2(worldPos - dFdy(worldPos));
     }
    else if(type == 2)
    {
        left.z = calculateNoise3(worldPos - dFdx(worldPos));
        right.z = calculateNoise3(worldPos + dFdx(worldPos));
        up.z = calculateNoise3(worldPos + dFdy(worldPos));
        down.z = calculateNoise3(worldPos - dFdy(worldPos));
     }
    else if (type == 6)
    {
        left.z = calculateNoiseTeste(worldPos - dFdx(worldPos));
        right.z = calculateNoiseTeste(worldPos + dFdx(worldPos));
        up.z = calculateNoiseTeste(worldPos + dFdy(worldPos));
        down.z = calculateNoiseTeste(worldPos - dFdy(worldPos));
    }
    else if (type == 7)
    {
        left.z = calculateNoiseMarble(worldPos - dFdx(worldPos));
        right.z = calculateNoiseMarble(worldPos + dFdx(worldPos));
        up.z = calculateNoiseMarble(worldPos + dFdy(worldPos));
        down.z = calculateNoiseMarble(worldPos - dFdy(worldPos));
    }
    else
    {
        left.z = calculateNoise4(worldPos - dFdx(worldPos));
        right.z = calculateNoise4(worldPos + dFdx(worldPos));
        up.z = calculateNoise4(worldPos + dFdy(worldPos));
        down.z = calculateNoise4(worldPos - dFdy(worldPos));
     }

    vec3 v1 = normalize(right - left);
    vec3 v2 = normalize(up - down);
    vec3 normal = normalize(cross(v1, v2));

    return normal;
}

void main()
{
    float f;
    //Noise com 6 oitavas
    if(dirtyType == 0)
    {
        f = turbulence(worldPos);
    }
    else
    {
        f = sumOctaves(worldPos);
    }
    vec3 skyColor = vec3(1, 1, 1);
    vec3 cloudColor = vec3(0.19125, 0.0735, 0.0225);
    vec3 colorNoise = mix(skyColor,cloudColor,f);

    if(isPBR == 1)
    {
        //PBR normal sem nada
        if(option == 0)
        {
            vec3 albedo = texture(Albedo,UV).rgb;
            float metallic = texture(Metallic,UV).r;
            float roughness = texture(Roughness,UV).r;
            float ao = texture(Ao,UV).r;

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
        //PBR com "sujeira" e aspecto de ouro
        else if (option == 1)
        {
            skyColor = vec3(0.7038, 0.27048, 0.0828);
            cloudColor = vec3(0.19125, 0.0735, 0.0225);
            colorNoise = mix(skyColor,cloudColor,f);

            vec3 albedo = texture(Albedo,UV).rgb;
            float metallic = texture(Metallic,UV).r;
            float roughness = texture(Roughness,UV).r;
            float ao = texture(Ao,UV).r;

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
                  Lo += (kD * albedo / PI + specular) * radiance * NdotL * colorNoise;


              }
              vec3 ambient = vec3(0.05) * albedo * ao * colorNoise;
              vec3 color = ambient + Lo;
              color = color / (color + vec3(1.0));
              color = pow(color, vec3(1.0/2.2));;

              finalColor = vec4(color, 1.0);
        }
        //PBR com noise em Roughness dando um aspecto de metal mais gasto
        else if (option == 2)
        {
            vec3 albedo = texture(Albedo,UV).rgb;
            float metallic = texture(Metallic,UV).r;
            float roughness = colorNoise.r;
            float ao = texture(Ao,UV).r;

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
        //Phong com Bump
        else if (option == 3)
        {
            mat3 TBN = transpose(mat3(tangente,bitangente,fragNormal));
            mat3 inverseTBN = inverse(TBN);
            vec3 tangPos = TBN * worldPos;

            //Sem Bump Nenhum
            if(bumpType == 4)
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

                finalColor = vec4((vec3(ambient) + vec3(diffuse)) * vec3(0.5,0.8,1) + vec3(specular),1);
            }
            else
            {
                //Com Bump
                vec3 normal = calculateNormal(bumpType);

                vec4 ambient = material.ambient;
                vec3 N = inverseTBN * normal;
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

                finalColor = vec4((vec3(ambient) + vec3(diffuse)) * vec3(0.5,0.8,1)+ vec3(specular),1);

                //Bump com cor
                if(bumpType == 5)
                {
                    //Noise com 6 oitavas
                    f = calculateNoise4(worldPos);
                    skyColor = vec3(1,1,1);
                    cloudColor = vec3(0, 0, 0);
                    colorNoise = mix(skyColor,cloudColor,6*f);
                    if(colorNoise.x > 0.6 && colorNoise.y > 0.6 && colorNoise.z > 0.6)
                    {
                        colorNoise = vec3(0.6, 0.5, 0.5);
                    }
                    else
                    {
                        colorNoise = vec3(0.5,0.8,1);
                    }
                    //colorNoise = vec3(0.6, 0.5, 0.5);
                    finalColor = vec4(vec3(ambient)*colorNoise + vec3(diffuse)*colorNoise + vec3(specular),1);

                }
            }

        }
        //PBR noise em Albedo dando um aspecto de metal mais gasto
        else if (option == 4)
        {
            vec3 albedo = colorNoise;
            float metallic = texture(Metallic,UV).r;
            float roughness = texture(Roughness,UV).r;
            float ao = texture(Ao,UV).r;

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
        //PBR com Bump
        else if (option == 5)
        {
            mat3 TBN = transpose(mat3(tangente,bitangente,fragNormal));
            mat3 inverseTBN = inverse(TBN);
            vec3 tangPos = TBN * worldPos;
            //Sem Bump Nenhum
            if(bumpType == 4)
            {
                vec3 albedo = texture(Albedo,UV).rgb;
                float metallic = texture(Metallic,UV).r;
                float roughness = colorNoise.r;
                float ao = texture(Ao,UV).r;

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
            else
            {
                vec3 normal = calculateNormal(bumpType);

                vec3 albedo = texture(Albedo,UV).rgb;
                float metallic = texture(Metallic,UV).r;
                float roughness = colorNoise.r;

                f = calculateNoise4(worldPos);
                skyColor = vec3(1,1,1);
                cloudColor = vec3(0, 0, 1);
                //f = clamp(f * 4 ,0,1);
                colorNoise = mix(skyColor,cloudColor,4*f);
                //colorNoise = vec3(f,f,f);
                float ao = texture(Ao,UV).r;

                vec3 N = inverseTBN * normal;
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

                  //Bump com cor
                  if(bumpType == 5)
                  {
                      //Noise com 6 oitavas
                      f = calculateNoise4(worldPos);
                      skyColor = vec3(1,1,1);
                      cloudColor = vec3(0, 0, 1);
                      colorNoise = mix(skyColor,cloudColor,4*f);
                      if(colorNoise.x > 0.6 && colorNoise.y > 0.6 && colorNoise.z > 0.6)
                      {
                          colorNoise = vec3(0.5, 0.2, 0.1);
                          colorNoise = vec3(0.9,0.7,0.7);
                          color = ambient + Lo;
                          color = color / (color + vec3(1.0));
                          color = pow(color, vec3(1.0/2.2));;
                          finalColor = vec4(color*colorNoise, 1.0);
                      }

                  }

            }
        }
    }
    else
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

        finalColor = vec4(vec3(ambient) + vec3(diffuse) + vec3(specular),1);

        if(isDirty == 1)
        {
            finalColor = vec4(vec3(ambient)*colorNoise + vec3(diffuse)*colorNoise + vec3(specular),1);
        }
        else if (isMarble == 1)
        {
            colorNoise = calculateMarbleColor(worldPos);
            finalColor = vec4(vec3(ambient)*colorNoise + vec3(diffuse)*colorNoise + vec3(specular),1);

        }
    }
}
