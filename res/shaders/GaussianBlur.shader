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

out vec4 FragColor;
in vec2 v_TexCoord;

uniform sampler2D image;
uniform int horizontal;
uniform float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

void main()
{             
    vec2 tex_offset = 1.0 / textureSize(image, 0); // gets size of single texel
    vec3 result = texture(image, v_TexCoord).rgb * weight[0]; // current fragment's contribution
    if(bool(horizontal))
    {
        for(int i = 1; i < 5; ++i)
        {
            result += texture(image, v_TexCoord + vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
            result += texture(image, v_TexCoord - vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
        }
    }
    else
    {
        for(int i = 1; i < 5; ++i)
        {
            result += texture(image, v_TexCoord + vec2(0.0, tex_offset.y * i)).rgb * weight[i];
            result += texture(image, v_TexCoord - vec2(0.0, tex_offset.y * i)).rgb * weight[i];
        }
    }
    FragColor = vec4(result, 1.0);
}