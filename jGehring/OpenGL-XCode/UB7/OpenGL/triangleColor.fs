//
//  gouraud.fs
//  OpenGL
//
//  Created by Johannes Gehring on 19.06.23.
//  Copyright © 2023 Frank Deinzer. All rights reserved.
//

const char triangleColorFragment[] = R"EOF(
#version 330
out vec4 myColor;
smooth in vec4 outColor;

void main(){
    myColor = outColor;
}
)EOF";
