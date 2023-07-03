//
//  basic.vs
//  OpenGL
//
//  Created by Johannes Gehring on 22.06.23.
//  Copyright © 2023 Frank Deinzer. All rights reserved.
//
const char basicVertex[] = R"EOF(
#version 330
layout (location = 0) in vec4 position;
layout(std140) uniform TBlock {
    mat4 transform;
    mat4 look;
    mat4 proj;
};

void main() {
    gl_Position = proj * look * transform * position;
}
)EOF";
