#version 330

in vec3 vNormal_vs;
in vec3 color;
out vec3 fColor;

void main()
{
   fColor = color;
}