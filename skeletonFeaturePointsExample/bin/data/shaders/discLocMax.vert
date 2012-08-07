#version 130  

uniform float width;
uniform float height;

varying vec2 texCoord;
   
  
void main()  
{  
    //texCoord = vec2(gl_MultiTexCoord0)/vec2(width,height);
    
    texCoord = vec2(gl_MultiTexCoord0);
    gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * gl_Vertex;  

}  