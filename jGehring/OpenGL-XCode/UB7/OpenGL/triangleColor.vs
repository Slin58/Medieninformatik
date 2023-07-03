//
//  phong.vs
//  OpenGL
//
//  Created by Johannes Gehring on 19.06.23.
//  Copyright © 2023 Frank Deinzer. All rights reserved.
//

const char triangleColorVertex[] = R"EOF(
#version 330
layout (location = 0) in vec4 position;
layout (location = 1) in vec4 color;

uniform mat4 transform;

smooth out vec4 outColor;

void main() {
    gl_Position = transform*position;
    outColor = color;
}
)EOF";
