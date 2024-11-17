
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

const vec3 waterColor = vec3(0, 0.2, 0.4);
const vec3 fogColor = vec3(1.0, 1.0, 1.0);
const float waterNear = 0.95;
const float fogNear = 0.996;
const float fogFar = 1.0;

uniform float brightness;
uniform float contrast;
uniform float gamma;
uniform float saturation;
uniform int foggy;
uniform int bloom;

const float offset = 1.0 / 720.0;  
const vec2 offsets[9] = vec2[](
    vec2(-offset,  offset),
    vec2( 0.0f,    offset),
    vec2( offset,  offset),
    vec2(-offset,  0.0f),  
    vec2( 0.0f,    0.0f),  
    vec2( offset,  0.0f),  
    vec2(-offset, -offset),
    vec2( 0.0f,   -offset),
    vec2( offset, -offset) 
);

vec3 rgb2hsv(vec3 c)
{
    vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
    vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
    vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));

    float d = q.x - min(q.w, q.y);
    float e = 1.0e-10;
    return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}

vec3 hsv2rgb(vec3 c)
{
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

void main()
{
    vec3 color = texture2D(screenTexture, v_TexCoord).xyz;
    float depth = texture2D(depthTexture, v_TexCoord).r;
   
    // Saturation
    vec3 hsv = rgb2hsv(color);
    hsv.y = clamp(hsv.y * saturation, 0.0, 1.0);
    color = hsv2rgb(hsv);

    // Brightness and Contrast
    color = (color - 0.5) * contrast + 0.5 + brightness;

    // Gamma Correction
    color = pow(color, vec3(1.0 / gamma));

    // Bloom
    if(bool(bloom))
    {
        float kernel[9] = float[](
            1.0 / 16, 2.0 / 16, 1.0 / 16,
            2.0 / 16, 4.0 / 16, 2.0 / 16,
            1.0 / 16, 2.0 / 16, 1.0 / 16  
        );

        vec3 sampleTex[9];
        for(int i = 0; i < 9; i++)
            sampleTex[i] = vec3(texture(screenTexture, v_TexCoord.st + offsets[i]));

        vec3 blurredColor = vec3(0.0);
        for(int i = 0; i < 9; i++)
            blurredColor += sampleTex[i] * kernel[i];
    
        color = color + (blurredColor - color) * 0.5;
    }
    color = clamp(color, 0.0, 1.0);
    vec3 finalColor = vec3(0.0);

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
    else{
        finalColor = color;
    }

    gl_FragColor = vec4(vec3(finalColor), 1.0);
};