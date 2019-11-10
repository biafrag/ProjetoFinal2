#version 330 core

in vec2 UV; //Coordenadas de textura do quad
in vec3 fragPos;
in vec3 fragNormal;
in vec3 worldPos;
out vec4 finalColor; // Cor final do objeto
uniform int mode;
uniform vec3 light; // Posição da luz em coordenada do olho

struct Material //Propriedades do material
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

uniform Material material;
//
// Author @patriciogv - 2015 - patriciogonzalezvivo.com

#ifdef GL_ES
precision mediump float;
#endif

uniform vec2 u_resolution;
uniform vec2 u_mouse;
uniform float u_time;

float hash1(float n)
{
    return fract(sin(n) * 1e4);
}
float hash1(vec2 p)
{
    return fract(1e4 * sin(17.0 * p.x + p.y * 0.1) * (0.1 + abs(sin(p.y * 13.0 + p.x))));
}

// 2D Noise based on Morgan McGuire @morgan3d
// https://www.shadertoy.com/view/4dS3Wd
float noise1(vec2 x) {
    vec2 i = floor(x);
    vec2 f = fract(x);

        // Four corners in 2D of a tile
    float a = hash1(i);
    float b = hash1(i + vec2(1.0, 0.0));
    float c = hash1(i + vec2(0.0, 1.0));
    float d = hash1(i + vec2(1.0, 1.0));

    // Simple 2D lerp using smoothstep envelope between the values.
        // return vec3(mix(mix(a, b, smoothstep(0.0, 1.0, f.x)),
        //			mix(c, d, smoothstep(0.0, 1.0, f.x)),
        //			smoothstep(0.0, 1.0, f.y)));

        // Same code, with the clamps in smoothstep and common subexpressions
        // optimized away.
    vec2 u = f * f * (3.0 - 2.0 * f);
        return mix(a, b, u.x) + (c - a) * u.y * (1.0 - u.x) + (d - b) * u.x * u.y;
}


vec2 hash( vec2 x )  // replace this by something better
{
    const vec2 k = vec2( 0.3183099, 0.3678794 );
    x = x*k + k.yx;
    return -1.0 + 2.0*fract( 16.0 * k*fract( x.x*x.y*(x.x+x.y)) );
}

float noise( in vec2 p )
{
    vec2 i = floor( p );
    vec2 f = fract( p );

        vec2 u = f*f*(3.0-2.0*f);

    return mix( mix( dot( hash( i + vec2(0.0,0.0) ), f - vec2(0.0,0.0) ),
                     dot( hash( i + vec2(1.0,0.0) ), f - vec2(1.0,0.0) ), u.x),
                mix( dot( hash( i + vec2(0.0,1.0) ), f - vec2(0.0,1.0) ),
                     dot( hash( i + vec2(1.0,1.0) ), f - vec2(1.0,1.0) ), u.x), u.y);
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
float noise3D(vec3 P)
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
    for(int i = 0; i < 4;i++)
    {
        z += abs(noise3D(frequency * pos) * persistency);
        frequency *= 2;
        persistency /= 2;
    }
    return z ;
}
float sumOctaves(vec3 pos)
{
    float z = 0;
    float frequency = 4;
    float persistency = 0.5;
    int numOctaves = 4;
    for(int i = 0; i < 20;i++)
    {
        z += noise3D(frequency * pos) * persistency;
        frequency *= 2;
        persistency /= 2;
    }
    return z + 0.5 ;
}

float calculateNoiseMarble(vec3 pos)
{
    float oct1 =  noise3D(2*pos)*0.5;
    float oct2 = noise3D(4*pos)*0.25;
    float oct3 = noise3D(8*pos)*0.25/2;
    float oct4 = noise3D(16*pos)*0.25/4;

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

void main()
{
    //Clouds

    if(mode == 0)
    {
        float f = 0.0;

        vec2 pos = UV;
        pos *= 8.0;
        mat2 m = mat2( 1.6,  1.2, -1.2,  1.6 );
        //f  = 0.5000*noise( pos );
        pos = m*pos;
        f += 0.2500*noise1( pos );
        pos = m*pos;
        f += 0.1250*noise1( pos );
        pos = m*pos;
        f += 0.0625*noise1( pos );


        f = 0.5 + 0.5*f;

        f *= smoothstep( 0.0, 0.005, abs(pos.x-0.6) );

        //f = noise1(100*UV);
        finalColor = vec4(vec3(f),1.0);
        vec3 skyColor = vec3(0,0,0.8);
        vec3 cloudColor = vec3(0.8,0.8,0.8);
        finalColor = vec4(mix(skyColor,cloudColor,f),1);

    }
    else if(mode == 1)
    {
        //Noise com 6 oitavas
        float f = (noise1(4*UV)*0.5 + noise1(8*UV)*0.25 + noise1(16*UV)*0.125 + noise1(32*UV)*0.0625 + noise1(64*UV)*0.03125 + noise1(128*UV)*0.015625);

        finalColor = vec4(vec3(f),1.0);

        vec3 skyColor = vec3(0,0,0.8);
        vec3 cloudColor = vec3(0.8,0.8,0.8);
        finalColor = vec4(mix(cloudColor,skyColor,f),1);


    }
    else
    {
        vec3 colorNoise;
        vec3 ambient = material.ambient;//texture(sampler, fragUV).rgb; // * light.ambient;
        vec3 diffuse = vec3(0.0,0.0,0.0);
        vec3 specular = vec3(0.0,0.0,0.0);

        vec3 N = normalize(fragNormal);
        vec3 L = normalize(light - fragPos);

        float iDif = dot(L,N);

        if( iDif > 0 )
        {
            diffuse = iDif * material.diffuse;//texture(sampler, fragUV).rgb; // * light.diffuse;

            vec3 V = normalize(-fragPos);
            vec3 H = normalize(L + V);

            float iSpec = pow(max(dot(N,H),0.0),material.shininess);
            //specular = iSpec * material.specular; // * light.specular;
        }

        if (mode == 2)
        {
            //Noise com 6 oitavas
            float f = sumOctaves(worldPos);
            vec3 skyColor = vec3(0,0,0.8);
            vec3 cloudColor = vec3(0.8,0.8,0.8);
            colorNoise = mix(skyColor,cloudColor,f);
        }
        else if (mode == 3)
        {
            float f = turbulence(worldPos);
            f = clamp(f*2 ,0,1);
            vec3 color1 = vec3(0.8,0.7,0);
            vec3 color2 = vec3(0.6,0.1,0);
            colorNoise = mix(color1,color2,f);
        }
        else if (mode == 4)
        {
            //Noise com 4 oitavas
            colorNoise = calculateMarbleColor(worldPos);
        }
        finalColor = vec4(ambient*colorNoise + diffuse*colorNoise + specular,1);
    }
}
