#version 330

//uniform mat4 projectionMatrix;
//uniform mat4 modelViewMatrix;

layout(location = 0) in vec3 position;
void main()
{
    gl_Position = vec4(position, 1.0);
    //gl_Position = projectionMatrix * modelViewMatrix * vec4(position, 1.0);
}
