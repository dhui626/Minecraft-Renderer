
#shader vertex
#version 330 core

layout(location = 0) in vec2 position;
layout(location = 1) in vec2 texCoord;

out vec2 v_TexCoord;

void main()
{
    gl_Position = vec4(position.xy, 0, 1);
    v_TexCoord = texCoord;
};


#shader fragment
#version 330 core

uniform sampler2D screenTexture;
uniform sampler2D depthTexture;

in vec2 v_TexCoord;

void main()
{
    vec3 color = texture2D(screenTexture, v_TexCoord).xyz;
    gl_FragColor = vec4(color, 1.0);
};