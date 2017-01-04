#version 330

in vec3 vNormal_vs;
out vec3 fColor;

void main()
{
   fColor = vNormal_vs;
}