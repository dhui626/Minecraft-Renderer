
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
uniform int foggy;

const vec3 waterColor = vec3(0, 0.2, 0.4);
const vec3 fogColor = vec3(1.0, 1.0, 1.0);
const float waterNear = 0.95;
const float fogNear = 0.996;
const float fogFar = 1.0;

uniform float brightness;
uniform float contrast;
uniform float gamma;

void main()
{
    vec3 color = texture2D(screenTexture, v_TexCoord).xyz;
    float depth = texture2D(depthTexture, v_TexCoord).r;
   
    // Brightness
    color += brightness;
    // Contrast
    color = ((color - 0.5) * contrast) + 0.5;
    // Gamma Correction
    color = pow(color, vec3(1.0 / gamma));

    vec3 finalColor = clamp(color, 0.0, 1.0);
    // Screen Space Fog
    if (bool(underwater))
	{
		float fogFactor = (fogFar - depth) / (fogFar - waterNear);
		fogFactor = clamp(fogFactor, 0.0, 1.0);
		finalColor = mix(waterColor, color, fogFactor);
	}
    else if (bool(foggy)){
        float fogFactor = (fogFar - depth) / (fogFar - fogNear);
		fogFactor = clamp(fogFactor, 0.0, 1.0);
		finalColor = mix(fogColor, color, fogFactor);
    }

    gl_FragColor = vec4(vec3(finalColor), 1.0);
};