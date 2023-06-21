//
//  phong.vs
//  OpenGL
//
//  Created by Johannes Gehring on 19.06.23.
//  Copyright © 2023 Frank Deinzer. All rights reserved.
//

const char phongVertex[] = R"EOF(
#version 330
layout (location = 0) in vec4 position;
layout (location = 1) in vec3 normalIn;
uniform vec3 lightPos;
uniform mat3 normalTransform;
smooth out vec3 normal;
out vec3 viewDir;
out vec3 pos;
out vec3 lightPosition;
layout(std140) uniform TBlock {
mat4 transform;
mat4 look;
mat4 proj;
};

void main() {
    gl_Position = proj * look * transform * position;
    vec3 vertexPos = vec3(gl_Position);
    vec3 viewPos = vec3(0.0f, 0.0f, 0.0f);
    viewDir = normalize(viewPos - vertexPos);
    normal = nt*normalIn;
    pos = vertexPos;
    lightPosition = lightPos;
}
)EOF";
