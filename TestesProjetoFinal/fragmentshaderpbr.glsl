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
uniform int isPBR; //Posição da luz no espaço tangente
uniform int option; //Posição da luz no espaço tangente
uniform mat4 normalMatrix; //Inversa transposta da MV

in vec3 fragNormal;
in vec3 fragPos;
in vec2 UV;
in vec3 worldPos;
in vec3 worldNorm;
out vec4 finalColor; // Cor final do objeto

uniform sampler2D Albedo; //Textura difusa
uniform sampler2D Metallic; //Textura difusa
uniform sampler2D Ao; //Textura difusa
uniform sampler2D Roughness; //Textura difusa

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

float calculateNoise(vec3 pos)
{
    float z = 0;
    float scale = 1;
    float persistency = 0.25;
    int numOctaves = 4;
    for(int i = 0; i < numOctaves;i++)
    {
        z += noise(scale * pos) * persistency;
        scale *= 2;
        persistency /= 2;
    }
    return z;
}


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
    //Noise com 6 oitavas
    float f = (noise(4*worldPos)*0.5 + noise(8*worldPos)*0.25 + noise(16*worldPos)*0.125 + noise(32*worldPos)*0.0625 + noise(64*worldPos)*0.03125 + noise(128*worldPos)*0.015625);
    vec3 skyColor = vec3(1, 1, 1);
    vec3 cloudColor = vec3(0.19125, 0.0735, 0.0225);
    vec3 colorNoise = mix(skyColor,cloudColor,f);

    if(isPBR == 1)
    {
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
        else if (option == 2)
        {
            vec3 albedo = texture(Albedo,UV).rgb /*colorNoise*/;
            float metallic = texture(Metallic,UV).r;
            float roughness = /*texture(Roughness,UV).r*/ colorNoise.r;
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
        else if (option == 3)
        {
            //Calculando pontos usando derivadas
            vec3 left = worldPos - dFdx(worldPos);
            vec3 right = worldPos + dFdx(worldPos);
            vec3 up = worldPos + dFdy(worldPos);
            vec3 down = worldPos - dFdy(worldPos);
            vec3 upRight = worldPos + dFdx(worldPos) + dFdy(worldPos);
            vec3 upLeft = worldPos - dFdx(worldPos) + dFdy(worldPos);
            vec3 downRight = worldPos + dFdx(worldPos) - dFdy(worldPos);
            vec3 downLeft = worldPos - dFdx(worldPos) - dFdy(worldPos);

            //Pegando vetores da borda
            vec3 normal1 = cross(downRight - downLeft, upLeft - downLeft);
            vec3 normal2 = cross(upLeft - upRight, downRight - upRight);
            vec3 normal = normalize((normal1 + normal2)/2.0);

            //Pegando normal usando pontos da vertical e horizontal
//            vec3 left = worldPos - dFdx(worldPos);
//            vec3 right = worldPos + dFdx(worldPos);
//            vec3 up = worldPos + dFdy(worldPos);
//            vec3 down = worldPos - dFdy(worldPos);
//            vec3 upRight = worldPos + dFdx(worldPos) + dFdy(worldPos);
//            vec3 upLeft = worldPos - dFdx(worldPos) + dFdy(worldPos);
//            vec3 downRight = worldPos + dFdx(worldPos) - dFdy(worldPos);
//            vec3 downLeft = worldPos - dFdx(worldPos) - dFdy(worldPos);

//            vec3 normal = cross(upLeft - upRight, downRight - upRight);

            //Agora tentando com Sobel
//            mat3 mx = mat3(vec3(-1,-2,-1),vec3(0,0,0),vec3(1,2,1));
//            mat3 my = mat3(vec3(1,0,-1),vec3(2,0,-2),vec3(1,0,-1));
//            vec3 dx = mx * worldPos;
//            vec3 dy = my * worldPos;

//            vec3 left = worldPos - dx;
//            vec3 right = worldPos + dx;
//            vec3 up = worldPos + dy;
//            vec3 down = worldPos - dy;
//            vec3 upRight = worldPos + dx + dy;
//            vec3 upLeft = worldPos - dx + dy;
//            vec3 downRight = worldPos + dx - dy;
//            vec3 downLeft = worldPos - dx - dy;

//            vec3  normal1 = cross(downRight - downLeft, upLeft - downLeft);
//            vec3  normal2 = cross(upLeft - upRight, downRight - upRight);
//            vec3  normal = cross(upLeft - upRight, downRight - upRight);

//            left = vec3(left.x,left.y,calculateNoise(left));
//            right = vec3(right.x,right.y,calculateNoise(right));
//            up = vec3(up.x,up.y,calculateNoise(up));
//            down = vec3(down.x,down.y,calculateNoise(down));
//            upRight = vec3(upRight.x,upRight.y,calculateNoise(upRight));
//            upLeft = vec3(upLeft.x,upLeft.y,calculateNoise(upLeft));
//            downRight = vec3(downRight.x,downRight.y,calculateNoise(downRight));
//            downLeft = vec3(downLeft.x,downLeft.y,calculateNoise(downLeft));


//            vec3 normal1 = normalize(cross(downRight - downLeft, upLeft - downLeft));
//            vec3 normal2 = normalize(cross(upLeft - upRight, downRight - upRight));
//            vec3 normal = normalize((normal1 + normal2)/2.0);

            //Ideia 2 => Pegar normal já existente e já somar com outras com um peso mais alto
//            vec3 normal1 = normalize(cross(downRight - downLeft, upLeft - downLeft));
//            vec3 normal2 = normalize(cross(upLeft - upRight, downRight - upRight));
//            vec3 normal = normalize((abs(normal1) + abs(normal2) + 6*normalize(worldNorm))/8.0);



            vec4 ambient = material.ambient;
            vec3 N = normalize(( normalMatrix * vec4( normal, 0 ) ).xyz);
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

            finalColor = vec4(vec3(ambient) + vec3(diffuse) + vec3(specular),1);
        }
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

        finalColor = diffuse + ambient + specular;

        finalColor = vec4(vec3(ambient)*colorNoise + vec3(diffuse)*colorNoise + vec3(specular),1);
    }
}