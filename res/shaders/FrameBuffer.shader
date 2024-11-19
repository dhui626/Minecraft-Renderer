
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
uniform sampler2D brightTexture;
uniform sampler2D depthTexture;
uniform int underwater;

const vec3 waterColor = vec3(0.1, 0.3, 0.6);
const vec3 fogColor = vec3(1.0, 1.0, 1.0);
const float waterNear = 0.95;
const float fogNear = 0.998;
const float fogFar = 1.0;

uniform float brightness;
uniform float contrast;
uniform float gamma;
uniform float saturation;
uniform int foggy;
uniform int bloom;
uniform int reinhard;
uniform int ce;
uniform int filmic;
uniform int aces;
uniform float exposure;

const float weight[5] = float[] (0.227027, 0.0972973, 0.0608108, 0.027027, 0.008108);

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

vec3 ACESToneMapping(vec3 color, float adapted_lum)
{
	const float A = 2.51f;
	const float B = 0.03f;
	const float C = 2.43f;
	const float D = 0.59f;
	const float E = 0.14f;

	color *= adapted_lum;
	return (color * (A * color + B)) / (color * (C * color + D) + E);
}

vec3 F(vec3 x)
{
	const float A = 0.22f;
	const float B = 0.30f;
	const float C = 0.10f;
	const float D = 0.20f;
	const float E = 0.01f;
	const float F = 0.30f;
 
	return ((x * (A * x + C * B) + D * E) / (x * (A * x + B) + D * F)) - E / F;
}

vec3 Uncharted2ToneMapping(vec3 color, float adapted_lum)
{
	const float WHITE = 11.2f;
	return F(1.6f * adapted_lum * color) / F(vec3(WHITE));
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

    // Bloom
    if(bool(bloom))
    {
        vec2 tex_offset = 1.0 / textureSize(brightTexture, 0); // gets size of single texel
        vec3 blurredColor = texture(brightTexture, v_TexCoord).rgb * weight[0]; // current fragment's contribution
        // Gaussian Blur 5¡Á5
        for(int i = 1; i < 5; ++i) //horizontal
        {
            blurredColor += texture(brightTexture, v_TexCoord + vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
            blurredColor += texture(brightTexture, v_TexCoord - vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
        }
        for(int i = 1; i < 5; ++i) //vertical
        {
            blurredColor += texture(brightTexture, v_TexCoord + vec2(0.0, tex_offset.y * i)).rgb * weight[i];
            blurredColor += texture(brightTexture, v_TexCoord - vec2(0.0, tex_offset.y * i)).rgb * weight[i];
        }

        color = color + blurredColor;
    }

    // Screen Space Fog
    vec3 finalColor = vec3(0.0);
    if (bool(underwater))
	{
		float fogFactor = clamp((depth - waterNear) / (fogFar - waterNear),0.0, 1.0);
		fogFactor = pow(fogFactor, 3.0);
		finalColor = mix(color, waterColor, fogFactor);
	}
    else if (bool(foggy)){
        float fogFactor = clamp((depth - fogNear) / (fogFar - fogNear), 0.0, 1.0);
		fogFactor = pow(fogFactor, 2.0);
		finalColor = mix(color, fogColor, fogFactor);
    }
    else{
        finalColor = color;
    }

    // Tone Mapping
    vec3 mapped = finalColor;
    if(bool(reinhard))
        mapped = mapped * exposure / (1.0 + mapped * exposure);
    if(bool(ce))
        mapped = vec3(1.0) - exp(-finalColor * exposure);
    if(bool(filmic))
        mapped = Uncharted2ToneMapping(finalColor, exposure);
    if(bool(aces))
        mapped = ACESToneMapping(finalColor, exposure);

    // Gamma Correction
    finalColor = pow(mapped, vec3(1.0 / gamma));

    gl_FragColor = vec4(vec3(finalColor), 1.0);
};