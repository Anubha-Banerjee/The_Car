#version 130


in vec3 interpolateNormal;
in vec3 interpolateLightVector;
in vec2 textureCoord;

uniform sampler2D colorMap2D;
uniform vec3 diffLight;
uniform vec3 specLight;
uniform vec3 ambLight;
uniform vec3 material;
uniform float shininess;
uniform float noShine;

out vec4 finalestColor;

void main()
{
	float diff=max(0,dot(normalize(interpolateNormal),normalize(interpolateLightVector)));
	
	vec3 finalColor=diffLight*material*diff;
	
	vec3 ambContri=ambLight*material;
	
	finalColor=finalColor+ambContri;
	
	
	vec3 ref=normalize(reflect(-normalize(interpolateLightVector),normalize(interpolateNormal)));
	float spec=max(0,dot(ref,normalize(interpolateNormal)));
	float shine=pow(spec,shininess);
	if(noShine==1)
	{
		shine=0;
	}
	
	vec4 specContri=vec4(shine,shine,shine,0);
	
	finalestColor.rgb=finalColor.rgb;
	finalestColor.a=1;
		
	finalestColor=finalestColor*texture2D(colorMap2D,textureCoord);
	

	finalestColor=finalestColor+specContri;
	
	
}