#version 450
#extension GL_ARB_separate_shader_objects : enable

out vec4 FragColor;

struct Light {
    vec3 direction;

    float ambient;
    vec3 color;
};

uniform Light light;
uniform vec3 viewPos;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;
in vec4 FragPosLightSpace;

uniform sampler2D texture_diffuse1;
uniform sampler2D shadowMap;

float ShadowCalculation(vec4 fragPosLightSpace)
{
	// perform perspective divide
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	// transform to [0,1] range
	projCoords = projCoords * 0.5 + 0.5;
	// get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
	float closestDepth = texture(shadowMap, projCoords.xy).r;
	// get depth of current fragment from light's perspective
	float currentDepth = projCoords.z;
	// calculate bias (based on depth map resolution and slope)
	vec3 normal = normalize(Normal);
	vec3 lightDir = normalize(-light.direction);
	float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
	// check whether current frag pos is in shadow
	// float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
	// PCF
	float shadow = 0.0;
	vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
	for(int x = -1; x <= 1; ++x)
	{
		for(int y = -1; y <= 1; ++y)
		{
			float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
			shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;
		}
	}
	shadow /= 9.0;

	// keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
	if(projCoords.z > 1.0)
	shadow = 0.0;

	return shadow;
}

void main()
{
	vec3 color = texture(texture_diffuse1, TexCoord).rgb;
	vec3 normal = normalize(Normal);
	vec3 lightColor = light.color;

	// ambient
	vec3 ambient = light.ambient * color;

	// diffuse
	vec3 lightDir = normalize(-light.direction);
	float diff = max(dot(lightDir, normal), 0.0);
	vec3 diffuse = diff * lightColor;

	// specular
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = 0.0;
	vec3 halfwayDir = normalize(lightDir + viewDir);
	spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
	vec3 specular = spec * lightColor;

	// calculate shadow
	float shadow = ShadowCalculation(FragPosLightSpace);
	//vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;
	vec3 lighting = (ambient + (1.0) * (diffuse + specular)) * color;

	// Final
	FragColor = vec4(lighting, 1.0);

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


	/*float specularStrength = 0.8;
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
	FragColor = texture(blockMapTexture, TexCoord) * vec4(result, 1.0);*/
} 