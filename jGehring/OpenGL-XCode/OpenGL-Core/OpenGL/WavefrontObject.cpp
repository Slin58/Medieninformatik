//
//  ObjectReader.cpp
//  OpenGL
//
//  Created by Johannes Gehring on 08.05.23.
//

#include "WavefrontObject.hpp"

WavefrontObject::WavefrontObject(){
    vertices = {};
    normals = {};
    textures = {};
    faceElements = {};
}

vector<float> WavefrontObject::transformToBuffer(){
    vector<float> result = {};
    if(faceElements.size() > 0 && faceElements[0].z) this->hasNormals = true;
    
    std::for_each(this->faceElements.begin(), this->faceElements.end(), [this, &result](glm::vec3 faceElement){
        int offset = 0 + (faceElement.x - 1) * 4;
        for(int i = offset; i < (offset + 4); i++){
            float f = this->vertices[i];
            result.push_back(f);
        }
        if(this->hasNormals){
            offset = 0 + (faceElement.z - 1) * 3;
            for(int i = faceElement.z - 1; i < (faceElement.z + 2); i++){
                result.push_back(this->normals[i]);
            }
        }
        if(this->hasTextures){
            offset = 0 + (faceElement.y - 1)*2;
            for(int i = faceElement.y - 1; i < (faceElement.y + 1); i++){
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
    return faceElement;
}

WavefrontObject readWavefrontObject(string path){
    WavefrontObject result = WavefrontObject();
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
                    for_each(beg, tok.end(), [&result](string s){
                        result.faceElements.push_back(splitFaceElement(s));
                    });
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

