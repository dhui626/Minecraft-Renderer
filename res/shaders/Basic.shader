
#shader vertex
#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Proj;
uniform mat4 u_LightPV;

out vec2 v_TexCoord;
out vec3 v_Normal;
out vec3 v_FragPos;
out vec4 v_PositionFromLight;

void main()
{
    v_FragPos = (u_Model * vec4(position, 1.0)).xyz;
    v_Normal =  (u_Model * vec4(normal, 0.0)).xyz;
    gl_Position = u_Proj * u_View * u_Model * vec4(position.xyz, 1);
    v_TexCoord = texCoord;
    v_PositionFromLight = u_LightPV * u_Model * vec4(position, 1.0);
};


#shader fragment
#version 330 core
// Phong Related
in vec2 v_TexCoord;
in vec3 v_Normal;
in vec3 v_FragPos;
uniform sampler2D u_Texture;
uniform vec3 u_LightPos;
uniform float u_LightIntensity;
uniform vec3 u_CameraPos;
uniform float u_Kd;
uniform float u_Ks;

// Settings
uniform int show_Shadow;
uniform int show_PCF;
uniform int show_PCSS;

// Shadow Map Related
in vec4 v_PositionFromLight;
uniform sampler2D u_ShadowMap;

#define NUM_SAMPLES 50
#define BLOCKER_SEARCH_NUM_SAMPLES NUM_SAMPLES
#define PCF_NUM_SAMPLES NUM_SAMPLES
#define NUM_RINGS 10
#define PCF_FILTER_RADIUS 0.002
#define PCSS_FILTER_RADIUS 0.02
#define BIAS 0.002
#define LIGHT_WIDTH 2

#define EPS 1e-3
#define PI 3.141592653589793
#define PI2 6.283185307179586

highp float rand_1to1(highp float x) { 
  // -1 ~ 1
  return fract(sin(x)*10000.0);
}

highp float rand_2to1(vec2 uv) { 
  // 0 ~ 1
	const highp float a = 12.9898, b = 78.233, c = 43758.5453;
	highp float dt = dot( uv.xy, vec2( a,b ) ), sn = mod( dt, PI );
	return fract(sin(sn) * c);
}

vec2 poissonDisk[NUM_SAMPLES];
void poissonDiskSamples( const in vec2 randomSeed ) {

    float ANGLE_STEP = PI2 * float( NUM_RINGS ) / float( NUM_SAMPLES );
    float INV_NUM_SAMPLES = 1.0 / float( NUM_SAMPLES );

    float angle = rand_2to1( randomSeed ) * PI2;
    float radius = INV_NUM_SAMPLES;
    float radiusStep = radius;

    for( int i = 0; i < NUM_SAMPLES; i ++ ) {
        poissonDisk[i] = vec2( cos( angle ), sin( angle ) ) * pow( radius, 0.75 );
        radius += radiusStep;
        angle += ANGLE_STEP;
    }
}

float useShadowMap(sampler2D shadowMap, vec4 shadowCoord)
{
    float shadowMapDepth = texture2D(shadowMap, shadowCoord.xy).x;
    if(abs(shadowMapDepth - 0.0) < EPS) // outside shadow map
        return 1.0;
    float depth = shadowCoord.z;

    float visibility = depth - EPS > shadowMapDepth ? 0.0 : 1.0;
    return visibility;
}

float PCF(sampler2D shadowMap, vec4 coords)
{
    poissonDiskSamples(coords.xy);
    float depth = coords.z;
    float visibility = 0.0;

    for(int i = 0; i < PCF_NUM_SAMPLES; i++)
    {
        float sampleDepth = texture2D(shadowMap, coords.xy + poissonDisk[i] * PCF_FILTER_RADIUS).r;
        if(abs(sampleDepth - 0.0) < EPS) // outside shadow map
            sampleDepth = 1.0;
        if (depth <= sampleDepth + BIAS)
        {
            visibility += 1.0; // 不在阴影中
        }
    }

    visibility /= float(PCF_NUM_SAMPLES); //归一化
    return visibility;
}

float findBlocker( sampler2D shadowMap,  vec2 uv, float zReceiver ) {
    float avgDepth = 0.0;
    int count = 0;

    for(int i = 0; i < BLOCKER_SEARCH_NUM_SAMPLES; i++)
    {
        float sampleDepth = texture2D(shadowMap, uv + poissonDisk[i] * PCSS_FILTER_RADIUS).r;
        if(sampleDepth - BIAS < zReceiver + EPS)
        {
            avgDepth += sampleDepth;
            count++;
        }
    }
    if(count > 0)
        avgDepth /= float(count);
    return avgDepth;
}

float PCSS(sampler2D shadowMap, vec4 coords){
    poissonDiskSamples(coords.xy);

    // STEP 1: avgblocker depth
    float avgDepth = findBlocker(shadowMap, coords.xy, coords.z);

    // STEP 2: penumbra size
    float depth = coords.z;
    float penumbraSize = (depth - avgDepth) * float(LIGHT_WIDTH) / avgDepth;

    // STEP 3: filtering
    float visibility = 1.0;
    for(int i = 0; i < BLOCKER_SEARCH_NUM_SAMPLES; i++)
    {
        float sampleDepth = texture2D(shadowMap, coords.xy + poissonDisk[i] * PCSS_FILTER_RADIUS * penumbraSize).r;
        if(abs(sampleDepth - 0.0) < EPS) // outside shadow map
            sampleDepth = 1.0;
        if (depth <= sampleDepth + BIAS) 
        {
            visibility += 1.0; // 不在阴影中
        }
    }
    visibility /= float(BLOCKER_SEARCH_NUM_SAMPLES); //归一化
    return visibility;
}

void main()
{
    if(texture2D(u_Texture, v_TexCoord).a == 0) //Transparent part
        discard;

    vec3 color = texture2D(u_Texture, v_TexCoord).rgb;

    // Blinn-Phong Light Model
    // ambient term
    vec3 ambient = 0.15 * color;
    // diffuse term
    vec3 lightDir = normalize(u_LightPos - v_FragPos);
    vec3 normal = normalize(v_Normal);
    float diff = max(dot(lightDir, normal), 0.0);
    float light_atten_coff = u_LightIntensity ; // Sun light
    vec3 diffuse = u_Kd * light_atten_coff * diff  * color;
    // specular term
    vec3 viewDir = normalize(u_CameraPos - v_FragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
    vec3 specular = u_Ks * light_atten_coff * spec * vec3(1.0, 1.0, 1.0);  

    float visibility = 1.0;
    if(bool(show_Shadow))
    {
        // visibility
        vec3 shadowCoord = v_PositionFromLight.xyz / v_PositionFromLight.w;
        shadowCoord = (shadowCoord + 1.0) * 0.5; // NDC coord
        if(bool(show_PCF))
        {
            visibility = PCF(u_ShadowMap, vec4(shadowCoord, 1.0));
        }
        else if(bool(show_PCSS))
        {
            visibility = PCSS(u_ShadowMap, vec4(shadowCoord, 1.0));
        }
        else{
            visibility = useShadowMap(u_ShadowMap, vec4(shadowCoord, 1.0));
        }
    }

    gl_FragColor = vec4(ambient + visibility * (diffuse + specular), 1.0);
};