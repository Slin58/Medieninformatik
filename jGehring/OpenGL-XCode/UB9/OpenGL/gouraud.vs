const char gouraudVertex[] = R"EOF(
#version 330
layout (location = 0) in vec4 position;
layout (location = 1) in vec3 normalIn;
uniform vec3 lightPos;
uniform mat3 normalTransform;
uniform vec3 lightColor;

smooth out vec4 inColor;
layout(std140) uniform TBlock {
    mat4 transform;
    mat4 look;
    mat4 proj;
};

void main() {
    vec3 objectColor = vec3(1.0, 0.0, 0.0);
    gl_Position = proj * look * transform * position;
    vec3 vertexPos = vec3(gl_Position);
    vec3 lightDir = lightPos - vertexPos;
    vec3 viewPos = vec3(0.0f, 0.0f, 0.0f);
    vec3 viewDir = normalize(viewPos - vertexPos);
    vec3 normal = normalTransform*normalIn;
    vec3 l = normalize(lightDir);
    vec3 n = normalize(normal);
    vec3 reflectDir = reflect(-l, n);
    vec3 r = normalize(reflectDir);
    float diff = max(dot(l, n), 0.0);
    float spec = pow(max(dot(viewDir, r), 0.0), 32);
    vec3 specular = 0.5f * spec * lightColor;
    vec3 ambient = 0.1f *  lightColor;
    vec3 result = (ambient+diff)*objectColor+specular;
    inColor = vec4(result, 1.0f);
}
)EOF";
