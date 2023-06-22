//
//  basic.fs
//  OpenGL
//
//  Created by Johannes Gehring on 22.06.23.
//  Copyright © 2023 Frank Deinzer. All rights reserved.
//

const char basicFragment[] = R"EOF(
#version 330
out vec4 outColor;

void main(){
    outColor = vec4(0.0, 1.0, 0.0, 1.0);
}
)EOF";

//float otColor = NdotL*vec4( 1.0f, 1.0f, 1.0f, 1.0f );
//vec3 dx = dFdx( pos );
//vec3 dy = dFdy( pos );
//vec3 N = normalize(cross(dx, dy));
//N *= sign(N.z);
