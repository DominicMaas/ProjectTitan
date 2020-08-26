#version 330 core
out vec4 FragColor;

struct Light {
    vec3 direction;
    float ambient;
    vec3 color;
};

uniform Light light;
uniform vec3 viewPos;

in vec3 Normal; 
in vec3 FragPos;
in vec2 TexCoord;

uniform sampler2D blockMapTexture;

void main()
{
	// ambient
	/*float ambientStrength = 0.1;
	vec3 ambient = ambientStrength * light.color;

	// diffuse
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(light.direction - FragPos);
	float diff = max(dot(norm, light.direction), 0.0);
	vec3 diffuse = diff * light.color;

	vec3 result = (ambient + diffuse);
	FragColor = texture(blockMapTexture, TexCoord) * vec4(result, 1.0);*/


	float specularStrength = 0.2;
    vec3 ambient = light.ambient * light.color;

	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(-light.direction);  

	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * light.color;

	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);  

	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
	vec3 specular = specularStrength * spec * light.color;  

	vec3 result = (ambient + diffuse + specular);
	FragColor = texture(blockMapTexture, TexCoord) * vec4(result, 1.0);
} 