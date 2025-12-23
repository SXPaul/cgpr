#version 410 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

out VS_OUT {
    vec3 FragPos;       // Fragment position in world space
    vec3 Normal;        // Normal in world space
    vec2 TexCoords;
    mat3 TBN;           // Tangent-Bitangent-Normal matrix for normal mapping
} vs_out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    // Transform position to world space
    vec4 worldPos = model * vec4(aPos, 1.0);
    vs_out.FragPos = worldPos.xyz;
    
    // Transform normal to world space (use normal matrix for non-uniform scaling)
    mat3 normalMatrix = transpose(inverse(mat3(model)));
    vs_out.Normal = normalize(normalMatrix * aNormal);
    
    // Pass through texture coordinates
    vs_out.TexCoords = aTexCoords;
    
    // Compute TBN matrix for normal mapping
    vec3 T = normalize(normalMatrix * aTangent);
    vec3 B = normalize(normalMatrix * aBitangent);
    vec3 N = vs_out.Normal;
    vs_out.TBN = mat3(T, B, N);
    
    gl_Position = projection * view * worldPos;
}
