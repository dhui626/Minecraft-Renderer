
#shader vertex
#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Proj;

out vec2 v_TexCoord;
out vec3 v_Normal;
out vec3 v_FragPos;

void main()
{
    v_FragPos = (u_Model * vec4(position, 1.0)).xyz;
    v_Normal =  (u_Model * vec4(normal, 0.0)).xyz;
    gl_Position = u_Proj * u_View * u_Model * vec4(position.xyz, 1);
    v_TexCoord = texCoord;
};


#shader fragment
#version 330 core

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
    vec3 color = pow(texture2D(u_Texture, v_TexCoord).rgb, vec3(2.2));

    // Blinn-Phong Light Model
    // ambient term
    vec3 ambient = 0.05 * color;
    // diffuse term
    vec3 lightDir = normalize(u_LightPos - v_FragPos);
    vec3 normal = normalize(v_Normal);
    float diff = max(dot(lightDir, normal), 0.0);
    float light_atten_coff = u_LightIntensity / length(u_LightPos - v_FragPos); // Point light
    // float light_atten_coff = u_LightIntensity ; // Sun light
    vec3 diffuse =  diff * light_atten_coff * color;

    vec3 viewDir = normalize(u_CameraPos - v_FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 512.0);
    vec3 specular = u_Ks * light_atten_coff * spec * vec3(1.0, 1.0, 1.0);  

    gl_FragColor = vec4(pow(ambient + diffuse + specular, vec3(1.0/2.2)), 1.0);
};