
#shader vertex
#version 330 core

layout(location = 0) in vec3 position;
layout(location = 2) in vec2 texCoord;

uniform mat4 u_Model;
uniform mat4 u_LightPV;

out vec2 v_TexCoord;

void main()
{
    gl_Position = u_LightPV * u_Model * vec4(position.xyz, 1);
    v_TexCoord = texCoord;
};


#shader fragment
#version 330 core

in vec2 v_TexCoord;
uniform sampler2D u_Texture;

void main()
{
    if(texture2D(u_Texture, v_TexCoord).a == 0)
        discard;
    gl_FragColor = vec4(gl_FragCoord.zzz, 1.0);
};