#version 330 core

layout (location = 3) in vec4 v; //Posição dos vértices do quadrilátero
//Matrizes
uniform mat4 mvp; //Matriz model view projection
uniform mat4 mv; // Matriz model view
uniform mat4 normalMatrix; //Inversa transposta da MV
uniform float timez;
uniform float timey;
uniform int numOctaves;
uniform float delta;
uniform int mode;
out vec3 fragPos; //Coordenadas de textura do quad
out vec3 fragNormal;

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

float hash1(float n)
{
    return fract(sin(n) * 1e4);
}
float hash1(vec2 p)
{
    return fract(1e4 * sin(17.0 * p.x + p.y * 0.1) * (0.1 + abs(sin(p.y * 13.0 + p.x))));
}

float noise1(vec3 x) {
    const vec3 step = vec3(110, 241, 171);

    vec3 i = floor(x);
    vec3 f = fract(x);

    // For performance, compute the base input to a 1D hash from the integer part of the argument and the
    // incremental change to the 1D based on the 3D -> 1D wrapping
    float n = dot(i, step);

    vec3 u = f * f * (3.0 - 2.0 * f);
    return mix(mix(mix( hash1(n + dot(step, vec3(0, 0, 0))), hash1(n + dot(step, vec3(1, 0, 0))), u.x),
                   mix( hash1(n + dot(step, vec3(0, 1, 0))), hash1(n + dot(step, vec3(1, 1, 0))), u.x), u.y),
               mix(mix( hash1(n + dot(step, vec3(0, 0, 1))), hash1(n + dot(step, vec3(1, 0, 1))), u.x),
                   mix( hash1(n + dot(step, vec3(0, 1, 1))), hash1(n + dot(step, vec3(1, 1, 1))), u.x), u.y), u.z);
}

vec3 computeNormal(vec3 p, float scale, float proportion, float timez,float timey)
{
    //Calculando normais
    vec3 right = vec3(p.x + delta, p.y + timey       , 0);
    vec3 left  = vec3(p.x - delta, p.y + timey      , 0);
    vec3 up    = vec3(p.x        , p.y + timey + delta, 0);
    vec3 down  = vec3(p.x        , p.y + timey - delta, 0);
    for(int i = 0; i < numOctaves;i++)
    {
        right.z += noise(scale * vec3(right.xy, timez)) * proportion;
        left.z  += noise(scale * vec3(left.xy , timez)) * proportion;
        down.z  += noise(scale * vec3(down.xy , timez)) * proportion;
        up.z    += noise(scale * vec3(up.xy   , timez)) * proportion;

        scale *= 2;
        proportion/=2;
    }

    right.y -= timey;
    left.y  -= timey;
    down.y  -= timey;
    up.y    -= timey;

    vec3 v1 = left - right;
    vec3 v2 = up - down;
    return normalize(cross(v2, v1));
}

vec3 computeNormal2(vec3 p, float scale, float proportion, float timez,float timey)
{
    //Calculando normais
    vec3 right = vec3(p.x + delta, p.y + timey       , 0);
    vec3 left  = vec3(p.x - delta, p.y + timey      , 0);
    vec3 up    = vec3(p.x        , p.y + timey + delta, 0);
    vec3 down  = vec3(p.x        , p.y + timey - delta, 0);
    for(int i = 0; i < numOctaves;i++)
    {
        right.z += noise1(scale * vec3(right.xy, timez)) * proportion;
        left.z  += noise1(scale * vec3(left.xy , timez)) * proportion;
        down.z  += noise1(scale * vec3(down.xy , timez)) * proportion;
        up.z    += noise1(scale * vec3(up.xy   , timez)) * proportion;

        scale *= 2;
        proportion/=2;
    }

    right.y -= timey;
    left.y  -= timey;
    down.y  -= timey;
    up.y    -= timey;

    vec3 v1 = left - right;
    vec3 v2 = up - down;
    return normalize(cross(v2, v1));
}

void main()
{
    vec3 pos = v.xyz;

    pos.z = timez;
    pos.y += timey;

    float z = 0;
    float scale = 1;
    float persistency = 0.25;
    for(int i = 0; i < numOctaves;i++)
    {
        z += noise(scale * pos) * persistency;
        scale *= 2;
        persistency /= 2;
    }
    pos.z = z;
    pos.y -= timey;
    //Posição do vértice no espaço de projeção
    gl_Position = mvp * vec4( pos, 1 );

    fragPos =  (mv * vec4( pos, 1 ) ).xyz;

    vec3 normal = computeNormal(pos, scale, persistency,timez,timey);
    fragNormal = normalize(( normalMatrix * vec4( normal, 0 ) ).xyz);

    }
