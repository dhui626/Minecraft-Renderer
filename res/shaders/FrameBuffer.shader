
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

in vec2 v_TexCoord;

uniform sampler2D screenTexture;
uniform sampler2D depthTexture;
uniform int underwater;

const vec3 fogColor = vec3(0, 0, 0.25);
const float fogNear = 0.99;
const float fogFar = 1.0;

void main()
{
    vec3 color = texture2D(screenTexture, v_TexCoord).xyz;
    float depth = texture2D(depthTexture, v_TexCoord).r;
    depth = pow(depth, 1.0);
    vec3 finalColor = color;
    // Screen Space Fog
    if (bool(underwater))
	{
		float fogFactor = (fogFar - depth) / (fogFar - fogNear);
		fogFactor = clamp(fogFactor, 0.0, 1.0);

		finalColor = mix(fogColor, color, fogFactor);
	}

    gl_FragColor = vec4(vec3(finalColor), 1.0);
};