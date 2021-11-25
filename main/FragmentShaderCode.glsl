#version 430



#define num 3

in vec3 vertexFrag;
in vec2 uvFrag;
in vec3 normalFrag;
in mat3 TBN;

// mapping parameter
uniform sampler2D mapping;
uniform sampler2D mapping_N;
uniform bool normalMapping;

// light control
uniform float abientControl;
uniform float diffuseControl;
uniform float specularControl;

// light parameter
uniform vec3 viewPort;
uniform vec3 lightSource[num];
uniform vec3 lightColor[num];

// attenuation coeffient
float constantCoeffient = 0.3f;
float linearCoeffient = 0.001f;
float quadraticCoeffient = 0.001f;

out vec4 colorFrag;

void main()
{
    vec3 ambient = vec3(0.0f);
    vec3 diffuse = vec3(0.0f);
    vec3 specular = vec3(0.0f);
    float brightness = 0;
    float shininess = 0;

    // material color
    vec3 ambientcolor = texture(mapping, uvFrag).rgb;
    vec3 diffusecolor = texture(mapping, uvFrag).rgb;
    vec3 specularcolor = vec3(1.0f, 1.0f, 1.0f);

    vec3 normaltangent, lighttangent, eyetangent;
    for (int i = 0; i < num; i++) {
        // normal mapping
        if (normalMapping) {
            normaltangent = texture(mapping_N, uvFrag).rgb * 2.0 - 1.0;
            lighttangent = TBN * lightSource[i];
            eyetangent = TBN * viewPort;
        }
        else {
            normaltangent = normalFrag;
            lighttangent = lightSource[i];
            eyetangent = viewPort;
        }

        // attenuation
        float distance = length(lighttangent - vertexFrag);
        float attenuation = constantCoeffient + linearCoeffient * distance + quadraticCoeffient * distance * distance;

        // diffuse
        vec3 normalvector = normalize(normaltangent);
        vec3 lightvector = normalize(lighttangent - vertexFrag);
        brightness = clamp(dot(normalvector, lightvector), 0, 1) / attenuation;

        // specular
        vec3 reflectvector = reflect(-lightvector, normalvector);
        vec3 eyevector = normalize(eyetangent - vertexFrag);
        shininess = clamp(dot(reflectvector, eyevector), 0, 1);

        // object color
        ambient += ambientcolor * lightColor[i];
        diffuse += diffusecolor * lightColor[i] * brightness;
        specular += specularcolor * lightColor[i] * pow(shininess, 10);
    }

    vec3 color = ambient * abientControl + diffuse * diffuseControl + specular * specularControl;
    colorFrag = vec4(color, 1.0);
    //colorFrag=vec4(0.5,0.5,0.5,1.0);
}
