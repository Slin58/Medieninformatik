//
//  basic.vs
//  OpenGL
//
//  Created by Johannes Gehring on 22.06.23.
//  Copyright © 2023 Frank Deinzer. All rights reserved.
//
const char normalVertex[] = R"EOF(
#version 330
layout (location = 0) in vec4 position;
layout (location = 1) in vec3 normalIn;
uniform mat3 normalTransform;
out vec3 normal;
layout(std140) uniform TBlock {
    mat4 transform;
    mat4 look;
    mat4 proj;
};

void main() {
    gl_Position = proj * look * transform * position;
    normal = normalTransform*normalIn;
}
)EOF";
