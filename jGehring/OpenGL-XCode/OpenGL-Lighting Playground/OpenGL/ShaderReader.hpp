//
//  ShaderReader.hpp
//  OpenGL
//
//  Created by Johannes Gehring on 14.06.23.
//  Copyright © 2023 Frank Deinzer. All rights reserved.
//

#pragma once
#include <stdio.h>
#include "string"

using namespace::std;

char* readVertexShader(string filename);
char* readFragmentShader(string filename);

