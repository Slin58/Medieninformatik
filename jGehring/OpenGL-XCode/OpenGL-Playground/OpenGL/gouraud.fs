//
//  gouraud.fs
//  OpenGL
//
//  Created by Johannes Gehring on 19.06.23.
//  Copyright © 2023 Frank Deinzer. All rights reserved.
//

const char gouraudFragment[] = R"EOF(
#version 330
out vec4 outColor;
smooth in vec4 inColor;

void main(){
    outColor = inColor;
}
)EOF";
