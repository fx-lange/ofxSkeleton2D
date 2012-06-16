#version 130
#extension GL_ARB_texture_rectangle : enable

uniform float width;
uniform float height;
uniform int maxPlacement;
uniform sampler2D tex0;
uniform sampler2DRect colorTex;

varying vec2 texCoord;  

/*
float LinearizeDepth(vec2 uv)
{
  float n = 1.0; // camera z near
  float f = 100.0; // camera z far
  float z = texture2D(tex0, uv).x;
  return (2.0 * n) / (f + n - z * (f - n));
}*/

float getDepthDisk(vec2 xy){
	float d = texture2D(tex0, xy/vec2(width,height)).x;
	return round(d/0.0005)*0.0005;
}

vec4 getColor(vec2 xy){
	return texture2DRect(colorTex,xy);
}


void main(){
		//CPs must lie inside (inside != black)
		if(getColor(texCoord).rgb == vec3(0.0)){
			gl_FragColor = vec4(0.0);
			return;
		}
		
		float d = getDepthDisk(texCoord);
		bool locMax = true;
		
		//ranking/placement testing (must be in the first "maxPlacement")
		int count = 1;
		for(int i=-1;i<2;++i){
			for(int j=-1;j<2;++j){
				if(count != 5){
					vec2 nCoord = clamp(texCoord + vec2(i,j),vec2(0,0),vec2(width,height));
					float nDepth = getDepthDisk(nCoord);
					if(nDepth > d){
						locMax = false;
						//break;
					}
				}
				++count;
			}
		}
		
		float c = 1.0;
		if(!locMax){
			c = 0.0;
		}
		
		gl_FragColor = vec4(c, c, c, 1.0);
}