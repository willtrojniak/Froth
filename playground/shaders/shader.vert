#version 450

layout(binding = 0) uniform UniformBufferObject {
    mat4 view;
    mat4 proj;
    vec4 cameraPos;
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec3 inNorm;
layout(location = 3) in vec2 inTexCoord;

layout(push_constant) uniform constants {
    mat4 model;
} pcs;

layout(location = 0) out vec4 worldPos;
layout(location = 1) out vec3 fragColor;
layout(location = 2) out vec3 fragNorm;
layout(location = 3) out vec2 fragTexCoord;

void main() {
    worldPos = pcs.model * vec4(inPosition, 1.0);
    gl_Position = ubo.proj * ubo.view * worldPos;

    mat3 normalMatrix = transpose(inverse(mat3(pcs.model)));
    fragNorm = normalize(normalMatrix * inNorm);

    fragColor = inColor;
    fragTexCoord = inTexCoord;
}
