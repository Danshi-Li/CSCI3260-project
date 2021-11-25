
#version 430


in layout(location = 0) vec3 position;
in layout(location = 1) vec2 vertexUV;
in layout(location = 2) vec3 normal;
in layout(location = 3) vec3 tangent;
in layout(location = 4) vec3 bitangent;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

out vec3 vertexFrag;
out vec2 uvFrag;
out vec3 normalFrag;
out mat3 TBN;

void main()
{
    vec4 newPosition = model * vec4(position, 1);
    vec4 newNormal = model * vec4(normal, 0);
    gl_Position =  projection * view * newPosition;

    mat3 tempMatrix = mat3(model * view);
    vec3 tangentCamera = tempMatrix * normalize(tangent);
    vec3 bitangentCamera = tempMatrix * normalize(bitangent);
    vec3 normalCamera = tempMatrix * normalize(normal);
    vertexFrag = newPosition.xyz;
    uvFrag = vertexUV;
    normalFrag = newNormal.xyz;
    TBN = transpose(mat3(tangentCamera, bitangentCamera, normalCamera));
}
