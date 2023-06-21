//
//  ObjectReader.hpp
//  OpenGL
//
//  Created by Johannes Gehring on 08.05.23.
//
#pragma once
#define ObjectReader_hpp

#include <stdio.h>
#include "string"
#include <glm/glm.hpp>
#include "fstream"
#include<boost/tokenizer.hpp>
#include<boost/lexical_cast.hpp>
#include<boost/algorithm/string.hpp>
#include <string_view>
#include "iostream"

using namespace::std;
using namespace::boost;

class WavefrontObject{
public:
    WavefrontObject();
    string objectTitle;
    vector<float> vertices;
    vector<float> normals;
    vector<float> textures;
    bool hasNormals = false;
    vector<glm::vec3> faceElements;
    vector<float> transformToBuffer();
};

WavefrontObject readWavefrontObject(string path = "");
glm::vec3 splitFaceElement(string &token);


