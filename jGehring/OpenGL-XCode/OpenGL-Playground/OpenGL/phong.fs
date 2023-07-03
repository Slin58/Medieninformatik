//
//  phong.fs
//  OpenGL
//
//  Created by Johannes Gehring on 19.06.23.
//  Copyright © 2023 Frank Deinzer. All rights reserved.
//

const char phongFragment[] = R"EOF(
#version 330
out vec4 outColor;
in vec3 viewDir;
in vec3 lightPosition;
in vec3 pos;
in vec3 lightCol;
smooth in vec3 normal;

void main(){
    vec3 objectColor = vec3(1.0, 0.0, 0.0);
    vec3 lightDir = lightPosition - pos;
    vec3 l = normalize(lightDir);
    vec3 n = normalize(normal);
    vec3 diff = max(dot(l, n), 0.0) * lightCol;
    vec3 reflectDir = reflect(-l, n);
    vec3 r = normalize(reflectDir);
    vec3 v = normalize(viewDir);
    float spec = pow(max(dot(v, r), 0.0), 64);
    vec3 specular = 0.5f * spec * lightCol;
    vec3 ambient = 0.1f *  lightCol;
    vec3 result = (ambient+diff)*objectColor+specular;

    outColor = vec4(result, 1.0f);
}
)EOF";
