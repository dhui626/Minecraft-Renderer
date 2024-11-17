
#shader vertex
#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Proj;
uniform float time;

out vec2 v_TexCoord;
out vec3 v_Normal;
out vec3 v_FragPos;

float animationTime = 5.0;
// wave parameter
const vec3 waveParams[] = vec3[](
    vec3(0.06f, 2.0f, 0.0f),
    vec3(0.03f, 3.0f, 1.57f),
    vec3(0.02f, 10.0f, 0.7f),
    vec3(0.01f, 50.0f, 8.8f)
);

void main()
{
    // waves
    float height = 0.0;
    float horizonal = 0.0;
    float maxAmplitude = 0.0;
    for (int i = 0; i < 4; i++) {
        float amplitude = waveParams[i].x;
        float wavelength = waveParams[i].y;
        float direction = waveParams[i].z;
        float k = 2.0 * 3.14159 / wavelength;
        float parse = k * (position.x * cos(direction) + position.z * sin(direction)) + time;
        height += amplitude * cos(parse);
        horizonal -= amplitude * sin(parse);
        maxAmplitude += amplitude;
    }
    vec4 wavedPosition = vec4(position.x + horizonal, position.y - maxAmplitude + height, position.z + horizonal, 1);
    gl_Position = u_Proj * u_View * u_Model * wavedPosition;
    v_FragPos = (u_Model * wavedPosition).xyz;

    v_Normal =  (u_Model * vec4(normal, 0.0)).xyz;

    // Texture Coords (4 water textures)
    v_TexCoord = texCoord;
    v_TexCoord.x += floor(mod(time / animationTime, 2)) * 1.0f / 64.0f;
	v_TexCoord.y += floor(mod(time / animationTime * 2, 2)) * 1.0f / 32.0f;
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

void main()
{
    if(texture2D(u_Texture, v_TexCoord).a == 0) //Transparent part
        discard;

    vec3 color = pow(vec3(texture2D(u_Texture, v_TexCoord).rg / 2, texture2D(u_Texture, v_TexCoord).b), vec3(2.2));

    // Blinn-Phong Light Model
    // ambient term
    vec3 ambient = 0.05 * color;
    // diffuse term
    vec3 lightDir = normalize(u_LightPos - v_FragPos);
    vec3 normal = normalize(v_Normal);
    float diff = max(dot(lightDir, normal), 0.0);
    // float light_atten_coff = u_LightIntensity / length(u_LightPos - v_FragPos); // Point light
    float light_atten_coff = u_LightIntensity ; // Sun light
    vec3 diffuse =  diff * light_atten_coff * color;
    // specular term
    vec3 viewDir = normalize(u_CameraPos - v_FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 512.0);
    vec3 specular = u_Ks * light_atten_coff * spec * vec3(1.0, 1.0, 1.0);  

    gl_FragColor = vec4(ambient + diffuse + specular, 0.8);
};