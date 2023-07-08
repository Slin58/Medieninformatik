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
#include <glm/gtx/normal.hpp>
#include "fstream"
#include<boost/tokenizer.hpp>
#include<boost/lexical_cast.hpp>
#include<boost/algorithm/string.hpp>
#include <string_view>
#include "iostream"
#include "numeric"

using namespace::std;
using namespace::boost;

class WavefrontObject{
public:
    WavefrontObject(bool identicalNormals);
    bool identicalNormals;
    string objectTitle;
    vector<float> vertices;
    vector<float> normals;
    vector<float> textures;
    bool hasNormals = false;
    bool hasTextures = false;
    vector<glm::vec3> faceElements;
    vector<float> transformToBuffer();
    void createIdenticalNormals();
    void createAverageNormals();
    vector<glm::vec3> triangleNormalVectors();
    void normalizeVertices();
};

WavefrontObject readWavefrontObject(string path = "", bool identicalNormals=false);
glm::vec3 splitFaceElement(string &token);
void drawCube();


