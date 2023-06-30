//
//  ObjectReader.cpp
//  OpenGL
//
//  Created by Johannes Gehring on 08.05.23.
//

#include "WavefrontObject.hpp"

WavefrontObject::WavefrontObject() {
    vertices = {};
    normals = {};
    textures = {};
    faceElements = {};
}

vector<float> WavefrontObject::transformToBuffer() {
    vector<float> result = {};
    if (faceElements.size() > 0 && faceElements[0].z) this->hasNormals = true;

    std::for_each(this->faceElements.begin(), this->faceElements.end(), [this, &result](glm::vec3 faceElement) {
        int offset = 0 + (faceElement.x - 1) * 4;
        for (int i = offset; i < (offset + 4); i++) {
            float f = this->vertices[i];
            result.push_back(f);
        }
        if (this->hasNormals) {
            offset = 0 + (faceElement.z - 1) * 3;
            for (int i = faceElement.z - 1; i < (faceElement.z + 2); i++) {
                result.push_back(this->normals[i]);
            }
        }
        if (this->hasTextures) {
            offset = 0 + (faceElement.y - 1) * 2;
            for (int i = faceElement.y - 1; i < (faceElement.y + 1); i++) {
                result.push_back(this->textures[i]);
            }
        }

        });
    return result;
}

glm::vec3 splitFaceElement(string& line) {
    int fType = 0;
    vector<float> parts = {};

    if (line.find("//") != std::string::npos) {
        fType = 1;
    }
    else if (line.find("/") != std::string::npos) {
        fType = 2;
    }
    else {
        fType = 3; 
    }
    if (fType == 3) {
        vector<string> tokenizedLine = split(line.c_str(), ' ');
        tokenizedLine.erase(tokenizedLine.begin());
        for (string substring : tokenizedLine) {
            parts.push_back(stof(substring));
        }
    }
    else if (fType == 2) {
        vector<string> tokenizedLine = split(line.c_str(), ' ');
        tokenizedLine.erase(tokenizedLine.begin());
        for (string substring : tokenizedLine) {
            vector<string> furtherTokenizedLine = split(line.c_str(), '/');
            parts.push_back(stof(furtherTokenizedLine[0])); //vertex
            parts.push_back(stof(furtherTokenizedLine[1])); //normal
            parts.push_back(stof(furtherTokenizedLine[2])); //texture
        }
    }
    else if (fType == 1) {
        vector<string> tokenizedLine = split(line.c_str(), ' ');
        tokenizedLine.erase(tokenizedLine.begin());

        for (string s : tokenizedLine) {
            vector<string> furtherTokenizedLine = split(s.c_str(), '//');
            parts.push_back(stof(furtherTokenizedLine[0])); //vertex
            parts.push_back(stof(furtherTokenizedLine[1])); //texture
        }
    }
    
    glm::vec3 faceElement;
    long partsSize = parts.size();
    if (fType == 2) faceElement = glm::vec3(parts[0], parts[1], parts[2]);
    else if (fType == 1) faceElement = glm::vec3(parts[0], 0, parts[1]);
    else if(fType == 3) faceElement = glm::vec3(parts[0], 0, 0);
    return faceElement;
}

WavefrontObject readWavefrontObject(string path) {
    WavefrontObject result = WavefrontObject();
    std::ifstream input(fPath);

    if (path == "") {
        return result;
    }
    string line = "";
    getline(input, line);

    while (getline(input, line))
    {
        if (line.find("v ") != std::string::npos) {
            vector<string> tokenizedLine = split(line.c_str(), ' ');
            tokenizedLine.erase(tokenizedLine.begin());
            for (string s : tokenizedLine) {
                result.vertices.push_back(stof(s);
            }
        }
        else if (line.find("vt ") != std::string::npos) {
            result.hasTextures = true;
            vector<string> tokenizedLine = split(line.c_str(), ' ');
            tokenizedLine.erase(tokenizedLine.begin());
            for (string s : tokenizedLine) {
                result.textures.push_back(stof(val));
            }            
        }
        else if (line.find("vt ") != std::string::npos) { 
            vector<string> tokenizedLine = split(line.c_str(), ' ');
            result.objectTitle = tokenizedLine[1];
        }
        else if (line.find("vn ") != std::string::npos) {
            result.hasNormals = true; 
            vector<string> tokenizedLine = split(line.c_str(), ' ');
            tokenizedLine.erase(tokenizedLine.begin());
            for (string s : tokenizedLine) {
                result.normals.push_back(stof(s));
            }
        }
        else if (line.find("f ") != std::string::npos) { // do triangle 
            vector<string> tokenizedLine = split(line.c_str(), ' ');
            tokenizedLine.erase(tokenizedLine.begin());
            for (string s : tokenizedLine) {
                result.faceElements.push_back(splitFaceElement(s));
                }
        }
       
    }
    return result;
}