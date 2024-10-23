
#shader vertex
#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoord;
layout(location = 2) in vec3 color;

out vec2 v_TexCoord;
out vec3 v_Color;

uniform vec4 u_Color;
uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Proj;

void main()
{
    gl_Position = u_Proj * u_View * u_Model * vec4(position.xyz, 1);
    v_TexCoord = texCoord;
    v_Color = color;
};


#shader fragment
#version 330 core

layout(location = 0) out vec4 color;

in vec2 v_TexCoord;
in vec3 v_Color;

uniform vec4 u_Color;
uniform sampler2D u_Texture;
uniform sampler2D u_Texture2;

void main()
{
    vec4 texColor = mix(texture(u_Texture, v_TexCoord), texture(u_Texture2, v_TexCoord),
        (u_Color.x + u_Color.y)/2 );
    color = vec4(texColor.xyz, texColor.w);
    //color = vec4((v_Color + u_Color.xyz)/2, (u_Color.x + u_Color.y)/3 + 0.33);
};