#version 330 core


layout(location = 0) in vec3 position;
layout(location = 1) in vec2 vertexUV;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec3 tangent;
layout(location = 4) in vec3 bitangent;

out vec3 vertexFrag;
out vec2 uvFrag;
out vec3 normalFrag;
out mat3 TBN;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main()
{
    vec4 newPosition = model * vec4(position, 1.0);
    vec4 newNormal = model * vec4(normal, 0.0);
    gl_Position =  projection * view * newPosition;

    mat3 tempMatrix = mat3(model * view);
//    vec3 tangentCamera = tempMatrix * normalize(tangent);
//    vec3 bitangentCamera = tempMatrix * normalize(bitangent);
//    vec3 normalCamera = tempMatrix * normalize(normal);
    vertexFrag = newPosition.xyz;
    uvFrag = vertexUV;
    normalFrag = newNormal.xyz;
    
//    TBN = transpose(mat3(tangentCamera, bitangentCamera, normalCamera));
    
    
    TBN = tempMatrix;
}
