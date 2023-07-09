//
//  ObjectReader.cpp
//  OpenGL
//
//  Created by Johannes Gehring on 08.05.23.
//

#include "WavefrontObject.hpp"

WavefrontObject::WavefrontObject(bool identicalNormals){
    vertices = {};
    normals = {};
    textures = {};
    faceElements = {};
    this->identicalNormals = identicalNormals;
}

vector<float> WavefrontObject::transformToBuffer(){
    vector<float> result = {};
    this->normalizeVertices();
    if(faceElements.size() > 0 && faceElements[0].z) this->hasNormals = true;
    else if(identicalNormals) this->createIdenticalNormals();
    else this->createAverageNormals();
    std::for_each(this->faceElements.begin(), this->faceElements.end(), [this, &result](glm::vec3 faceElement){
        int offset = 0 + (faceElement.x - 1) * 4;
        for(int i = offset; i < (offset + 4); i++){
            float f = this->vertices[i];
            result.push_back(f);
        }
        if(this->identicalNormals){
            offset = (0 + (faceElement.z - 1) * 3);
            for(int i = offset; i <= (offset + 2); i++){
                result.push_back(this->normals[i]);
            }
        }
        else{
            offset = 0 + (faceElement.x - 1) * 3;
            for(int i = offset; i <= (offset + 2); i++){
                result.push_back(this->normals[i]);
            }
        }
        if(this->hasTextures){
            offset = 0 + (faceElement.y - 1)*2;
            for(int i = offset; i <= (offset + 1); i++){
                result.push_back(this->textures[i]);
            }
        }
        
    });
    
    return result;
}

glm::vec3 splitFaceElement(string &token){
    char_separator<char> sep("");
    int fType = 0;
    if(token.rfind("//")!= std::string::npos){
        sep = char_separator<char>("//");
        fType = 1;
    } else if (token.find("/") != std::string::npos){
        sep = char_separator<char>("/");
        fType = 2;
    }
    tokenizer<char_separator<char>> tok(token, sep);
    vector<float> parts(std::distance(tok.begin(), tok.end()));
    transform(tok.begin(), tok.end(), parts.begin(), [](string s){return stof(s);});
    glm::vec3 faceElement;
    long partsSize = parts.size();
    if(partsSize == 1) faceElement = glm::vec3(parts[0], 0, 0);
    else if (partsSize == 2 && fType == 2) faceElement = glm::vec3(parts[0], parts[1], 0);
    else if (partsSize == 2 && fType == 1) faceElement = glm::vec3(parts[0], 0, parts[1]);
    else faceElement = glm::vec3(parts[0], parts[1], parts[2]);
    if(partsSize > 3) printf("ALARM! \n");
    return faceElement;
}

WavefrontObject readWavefrontObject(string path, bool identicalNormals){
    WavefrontObject result = WavefrontObject(identicalNormals);
    if(path == ""){
        return result;
    } else {
        fstream file;
        char_separator<char> sep(" ");
        file.open(path,ios::in);
        if (file.is_open()){
            string tp;
            while(getline(file, tp)){
                string line = trim_right_copy(tp);
                if(line == "") continue;
                
                tokenizer<char_separator<char>> tok(line, sep);
                tokenizer<char_separator<char>>::iterator beg=tok.begin();
                string lineStart = *beg;
                ++beg;
                if(lineStart == "v") {
                    bool needHomogenous = std::distance(beg, tok.end()) == 3;
                    for_each(beg, tok.end(), [&result](string s){
                        float val = lexical_cast<float>(s);
                        result.vertices.push_back(val);});
                    if(needHomogenous) result.vertices.push_back(1.0f);
                }
                else if (lineStart == "f"){
                    int count = 0;
                    for_each(beg, tok.end(), [&result, &count](string s){
                        result.faceElements.push_back(splitFaceElement(s));
                        count++;
                    });
                    if(count == 4) result.splitToTriangles();
                }
                else if (lineStart == "o") {result.objectTitle = *beg;}
                else if(lineStart == "vt"){
                    result.hasTextures = true;
                    int i = 0;
                    for(auto it = beg; i < 2; ++it){
                        float val = lexical_cast<float>(*it);
                        result.textures.push_back(val);
                        i++;
                    }
                }
                else if (lineStart == "vn"){
                    for_each(beg, tok.end(), [&result](string s){
                        float val = lexical_cast<float>(s);
                        result.normals.push_back(val);
                    });
                }
            }
        }
        file.close();
    }
    return result;
}


vector<glm::vec3> WavefrontObject::triangleNormalVectors() {
    vector<glm::vec3> normals = {};
    vector<glm::vec3> triangleVertices = {};
    for(int j = 0; j < this->faceElements.size(); j+=1){
        int normalIndex = div(j, 3).quot + 1;
        int offset = 0 + (this->faceElements[j].x - 1) * 4;
        glm::vec3 vertex = glm::vec3(this->vertices[offset], this->vertices[offset+1], this->vertices[offset+2]);
        triangleVertices.push_back(vertex);
        this->faceElements[j].z = normalIndex;
    }
    for(int i = 0; i < triangleVertices.size(); i+=3){
        glm::vec3 p0 = triangleVertices[i];
        glm::vec3 p1 = triangleVertices[i+1];
        glm::vec3 p2 = triangleVertices[i+2];
        glm::vec3 normal = glm::triangleNormal(p0, p1, p2);
        normals.push_back(normal);
    }
    return normals;
}

void WavefrontObject::createIdenticalNormals(){
    vector<glm::vec3> normals = triangleNormalVectors();
    for(glm::vec3 normal : normals){
        this->normals.push_back(normal.x);
        this->normals.push_back(normal.y);
        this->normals.push_back(normal.z);
    }
    this->hasNormals = true;
}

void WavefrontObject::createAverageNormals(){
    vector<glm::vec3> normals = triangleNormalVectors();
    auto numberOfVertices = this->vertices.size()/4;
    for(int i = 1; i <= numberOfVertices; i++){
        glm::vec3 vertexNormal(0.0);
        int count = 0;
        for(glm::vec3 face : faceElements){
            if(face.x == i) {
                vertexNormal += normals[face.z];
                face.z = i;
                count++;
            }
        }
        vertexNormal /= count;
        this->normals.push_back(vertexNormal.x);
        this->normals.push_back(vertexNormal.y);
        this->normals.push_back(vertexNormal.z);
    }
    this->hasNormals = true;
}

void WavefrontObject::normalizeVertices(){
    float xMax = 0;
    for(int i = 0; i < vertices.size(); i+=4){
        xMax = max(xMax, max(abs(vertices[i]), max(abs(vertices[i+1]), abs(vertices[i+2]))));
    }
    for(int i = 0; i < vertices.size(); i+=4){
        vertices[i] /= xMax;
        vertices[i+1] /= xMax;
        vertices[i+2] /= xMax;
    }
}

void WavefrontObject::splitToTriangles(){
    glm::vec3 index4 = faceElements[faceElements.size()-1];
    faceElements.pop_back();
    glm::vec3 index3 = faceElements[faceElements.size()-1];
    faceElements.pop_back();
    glm::vec3 index2 = faceElements[faceElements.size()-1];
    faceElements.pop_back();
    glm::vec3 index1 = faceElements[faceElements.size()-1];
    faceElements.pop_back();
    faceElements.push_back(index1);
    faceElements.push_back(index2);
    faceElements.push_back(index3);
    faceElements.push_back(index3);
    faceElements.push_back(index2);
    faceElements.push_back(index4);
}
