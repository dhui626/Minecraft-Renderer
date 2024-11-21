
#shader vertex
#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Proj;
uniform float time;

out VS_OUT {
    vec2 v_TexCoord;
    vec3 v_Normal;
} vs_out;

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

    vs_out.v_TexCoord = v_TexCoord;
    vs_out.v_Normal = v_Normal;
};


#shader geometry
#version 330 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in VS_OUT {
    vec2 v_TexCoord;
    vec3 v_Normal;
} gs_in[];

out vec2 v_TexCoord;
out vec3 v_Normal;
out vec3 v_FragPos;

void main() {
    for (int i = 0; i < 3; ++i) {
        v_TexCoord = gl_in[i].v_TexCoord;
        gl_Position = gl_in[i].gl_Position;
        EmitVertex();
    }
    EndPrimitive();
}


#shader fragment
#version 330 core
// Phong Related
in vec2 v_TexCoord;
in vec3 v_Normal;
in vec3 v_FragPos;

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

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

    vec3 color = vec3(texture2D(u_Texture, v_TexCoord).r / 3, texture2D(u_Texture, v_TexCoord).g / 2, texture2D(u_Texture, v_TexCoord).b);

    // Blinn-Phong Light Model
    // ambient term
    vec3 ambient = 0.05 * color;
    // diffuse term
    vec3 lightDir = normalize(u_LightPos - v_FragPos);
    vec3 normal = normalize(v_Normal);
    float diff = max(dot(lightDir, normal), 0.0);
    float light_atten_coff = u_LightIntensity ; // Sun light
    vec3 diffuse = u_Kd * light_atten_coff * diff * color;
    // specular term
    vec3 viewDir = normalize(u_CameraPos - v_FragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
    vec3 specular = u_Ks * light_atten_coff * spec * vec3(1.0, 1.0, 1.0);  

    FragColor = vec4(ambient + diffuse + specular, 0.9);

    BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
    // Check whether fragment output is higher than threshold, if so output as brightness color
    float brightness = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
    if(brightness > 1.0)
        BrightColor = vec4(FragColor.rgb, 1.0);
};