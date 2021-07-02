#version 330 core 

struct Material{
	float specular;
	float shininess;
};

struct PointLight{
	vec3 position;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float constant;
	float linear;
	float quadratic;
};

in vec3 pos;
in vec3 normal;
in vec2 texCoords;

out vec4 color;

uniform vec3 viewPos;
uniform PointLight pointLight;
uniform Material material;
uniform sampler2D texture_diffuse1;

vec3 CalcPointLight(PointLight light, vec3 normal1, vec3 fragPos, vec3 viewDir);

void main() {	
	// properties
	vec3 norm = normalize(normal);
	vec3 viewDir = normalize(viewPos - pos);
	
	// phase 2: point light
	vec3 result = CalcPointLight(pointLight, norm, pos, viewDir);
		
	color = vec4(result, 1.0f);
}

vec3 CalcPointLight(PointLight light, vec3 normal1, vec3 fragPos, vec3 viewDir){

	vec3 lightDir = normalize(light.position - fragPos);

	// diffuse shading
	float diff = max(dot(normal1, lightDir), 0.0);
	diff = 0.5;

	// specular shading
	vec3 reflectDir = reflect(-lightDir, normal1);
	float spec = pow(max(dot(viewDir, reflectDir), 0.2), material.shininess);
	
	// attenuation
	float distance = length(light.position - fragPos);
	float attenuation =  1.0f / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

	// combine the results
	vec3 ambient = light.ambient * vec3(texture(texture_diffuse1, texCoords));
	vec3 diffuse = light.diffuse * diff * vec3(texture(texture_diffuse1, texCoords));
	vec3 specular = light.specular * spec * material.specular;

	return (ambient + diffuse + specular);

	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

}