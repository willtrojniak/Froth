#version 450
#define MAX_LIGHTS 10

layout(binding = 1) uniform UniformBufferObject {
    // vec3 pos[MAX_LIGHTS];
    // vec3 col[MAX_LIGHTS];
    // float intensity[MAX_LIGHTS];
    vec4 pos;
    vec4 col;
    float constant;
    float linear;
    float quadratic;
} lights_ubo;

layout(binding = 2) uniform sampler2D texSampler[2];

layout(location = 0) in vec4 worldPos;
layout(location = 1) in vec3 fragColor;
layout(location = 2) in vec3 fragNorm;
layout(location = 3) in vec2 texCoord;

layout(location = 0) out vec4 outColor;

layout(push_constant) uniform constants {
    mat4 _;
    uint texIndex;
} pcs;

const vec3 lightAmbient = vec3(0.05, 0.05, 0.05);
const vec3 lightDir = vec3(0.4, 0.8, 1.0);
const vec3 lightCol = vec3(0.6, 0.6, 0.6);

vec3 directionalLight(vec3 lightDir, vec3 lightCol, vec3 normal) {
    float diff = max(dot(normal, normalize(lightDir)), 0.001);
    return diff * lightCol;
}

vec3 pointLight(vec3 lightPos, vec3 lightCol, float constant, float linear, float quadratic, vec3 normal, vec3 fragPos) {
    vec3 lightDir = normalize(lightPos - fragPos);
    float diff = max(dot(normal, lightDir), 0.0);

    float dist = length(lightPos - fragPos);
    float attenuation = 1 / (constant + linear * dist + (quadratic * dist * dist));
    vec3 diffuse = lightCol * diff;
    diffuse *= attenuation;

    return diffuse;
}

void main() {
    vec3 norm = normalize(fragNorm);
    vec3 result = vec3(0, 0, 0);
    result += lightAmbient;
    result += directionalLight(lightDir, lightCol, norm);
    result += pointLight(vec3(lights_ubo.pos), vec3(lights_ubo.col), lights_ubo.constant, lights_ubo.linear, lights_ubo.quadratic, norm, vec3(worldPos));
    result *= vec3(fragColor) * vec3(texture(texSampler[pcs.texIndex], texCoord));
    outColor = vec4(result, 1.0);
}
