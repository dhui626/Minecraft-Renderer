
#shader vertex
#version 330 core

layout(location = 0) in vec3 position;

uniform mat4 u_Model;
uniform mat4 u_LightPV;

void main()
{
    gl_Position = u_LightPV * u_Model * vec4(position.xyz, 1);
};


#shader fragment
#version 330 core

void main()
{
    gl_FragColor = vec4(gl_FragCoord.zzz, 1.0);
};