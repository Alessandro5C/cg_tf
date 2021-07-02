#version 330 core 

in vec3 pos;
in vec3 normal;
in vec2 texCoords;

out vec4 color;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 viewPos;

uniform sampler2D texture_diffuse1;

void main() {	
	// ambient
	float strength = 0.5;
	vec3 ambient = strength * lightColor;

	// diffuse
	vec3 norm = normalize(normal);
	vec3 lightDir = normalize(lightPos - pos);
	float diff = max(dot(norm, lightDir), 0.0f);
	vec3 diffuse = diff * lightColor;

	// specular 
	float specularStrength = 0.5;
	vec3 viewDir = normalize(viewPos - pos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
	vec3 specular = specularStrength * spec * lightColor;

	vec3 result = (ambient + diffuse + specular) * vec3(1.0f);
	color = texture(texture_diffuse1, texCoords) * vec4(result, 1.0f);
}