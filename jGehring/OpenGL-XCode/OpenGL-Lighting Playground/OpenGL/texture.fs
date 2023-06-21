//
//  texture.fs
//  OpenGL
//
//  Created by Johannes Gehring on 19.06.23.
//  Copyright © 2023 Frank Deinzer. All rights reserved.
//

const char textureFragment[] = R"EOF(
#version 330
smooth in vec2 intTexCoord;
uniform sampler2D tex;
out vec4 outColor;
in vec3 viewDir;
in vec3 lightPosition;
in vec3 pos;
smooth in vec3 normal;
void main(){
    vec3 lightColor = vec3(1.0f, 1.0f, 1.0f);
    vec3 lightDir = lightPosition - pos;
    vec3 l = normalize(lightDir);
    vec3 n = normalize(normal);
    vec3 diff = max(dot(l, n), 0.0) * lightColor;
    vec3 reflectDir = reflect(-l, n);
    vec3 r = normalize(reflectDir);
    vec3 v = normalize(viewDir);
    float spec = pow(max(dot(v, r), 0.0), 320);
    vec3 specular = 0.7f * spec * lightColor;
    vec3 ambient = 0.1f *  lightColor;
    vec4 result = vec4((ambient+diff)+specular, 1.0f);
    outColor = result*texture(tex, intTexCoord);
}
)EOF";
