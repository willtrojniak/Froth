#version 450
#define MAX_LIGHTS 3

struct Light {
    vec4 pos;
    vec4 color;
    vec4 params;
};

layout(binding = 1) uniform UniformBufferObject {
    Light lights[MAX_LIGHTS];
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

vec3 pointLight(Light light, vec3 normal, vec3 fragPos) {
    vec3 lightDir = normalize(vec3(light.pos) - fragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    float dist = length(vec3(light.pos) - fragPos);

    float constant = light.params.x;
    float linear = light.params.y;
    float quadratic = light.params.z;

    float attenuation = 1 / (constant + linear * dist + (quadratic * dist * dist));
    vec3 diffuse = vec3(light.color) * diff;

    diffuse *= attenuation;

    return diffuse;
}

void main() {
    vec3 norm = normalize(fragNorm);
    vec3 result = vec3(0, 0, 0);
    result += lightAmbient;
    result += directionalLight(lightDir, lightCol, norm);
    for (int i = 0; i < MAX_LIGHTS; i++) {
        result += pointLight(lights_ubo.lights[i], norm, vec3(worldPos));
    }
    result *= vec3(fragColor) * vec3(texture(texSampler[pcs.texIndex], texCoord));
    outColor = vec4(result, 1.0);
}
