#version 330 core
#define num 3

in vec3 vertexFrag;
in vec2 uvFrag;
in vec3 normalFrag;
in mat3 TBN;

out vec4 colorFrag;

// mapping parameter
uniform sampler2D mapping;
uniform sampler2D mapping_N;
uniform bool normalMapping;

// light control
uniform float abientControl;
uniform float diffuseControl;
uniform float specularControl;

// light parameter
//uniform vec3 viewPort;
//uniform vec3 lightSource[num];
//uniform vec3 lightColor[num];

// attenuation coeffient
float constantCoeffient = 0.3f;
float linearCoeffient = 0.001f;
float quadraticCoeffient = 0.001f;



void main()
{
	vec3 ambient = vec3(0.0f);
	vec3 diffuse = vec3(0.0f);
	vec3 specular = vec3(0.0f);
	float brightness = 0;
	float shininess = 0;

	// material color
	vec3 ambientColor = texture(mapping, uvFrag).rgb;
	vec3 diffuseColor = texture(mapping, uvFrag).rgb;
	vec3 specularColor = vec3(1.0f, 1.0f, 1.0f);

	vec3 normalTangent, lightTangent, eyeTangent;
	for (int i = 0; i < num; i++) {
		// normal mapping
		if (normalMapping) {
			normalTangent = texture(mapping_N, uvFrag).rgb * 2.0 - 1.0;
			lightTangent = TBN * lightSource[i];
			eyeTangent = TBN * viewPort;
		}
		else {
			normalTangent = normalFrag;
			lightTangent = lightSource[i];
			eyeTangent = viewPort;
		}

		// attenuation
		float distance = length(lightTangent - vertexFrag);
		float attenuation = constantCoeffient + linearCoeffient * distance + quadraticCoeffient * distance * distance;

		// diffuse
		vec3 normalVector = normalize(normalTangent);
		vec3 lightVector = normalize(lightTangent - vertexFrag);
		brightness = clamp(dot(normalVector, lightVector), 0, 1) / attenuation;

		// specular
		vec3 reflectVector = reflect(-lightVector, normalVector);
		vec3 eyeVector = normalize(eyeTangent - vertexFrag);
		shininess = clamp(dot(reflectVector, eyeVector), 0, 1);

		// object color
		ambient += ambientColor * lightColor[i];
		diffuse += diffuseColor * lightColor[i] * brightness;
		specular += specularColor * lightColor[i] * pow(shininess, 10);
	}

	vec3 color = ambient * abientControl + diffuse * diffuseControl + specular * specularControl;
	colorFrag = vec4(color, 1.0);
}