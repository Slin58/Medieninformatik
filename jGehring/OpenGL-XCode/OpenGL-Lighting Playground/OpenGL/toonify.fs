//
//  toonify.fs
//  OpenGL
//
//  Created by Johannes Gehring on 19.06.23.
//  Copyright © 2023 Frank Deinzer. All rights reserved.
//

const char toonifyFragment[] = R"EOF(
#version 330
in vec3 normal;
in vec3 lightDir;
out vec4 outColor;
vec4 toonify(in float intensity) {
vec4 color;
if (intensity > 0.98) color = vec4(0.8,0.8,0.8,1.0);
else if (intensity > 0.5) color = vec4(0.4,0.4,0.8,1.0);
else if (intensity > 0.25) color = vec4(0.2,0.2,0.4,1.0);
else color = vec4(0.1,0.1,0.1,1.0);
return color;
}
void main() {
vec3 n = normalize(normal);
vec3 l = normalize(lightDir);
float intensity = max(dot(l, n),0.0);
outColor = toonify(intensity);
}
)EOF";
