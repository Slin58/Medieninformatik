#include "Surface.h"

void Surface::splitToTriangle(){
    int index4 = indices[indices.size()-1];
    indices.pop_back();
    int index3 = indices[indices.size()-1];
    indices.pop_back();
    int index2 = indices[indices.size()-1];
    indices.pop_back();
    int index1 = indices[indices.size()-1];
    indices.pop_back();
    indices.push_back(index1);
    indices.push_back(index2);
    indices.push_back(index3);
    indices.push_back(index3);
    indices.push_back(index2);
    indices.push_back(index4);
}

void Surface::setBoundingVolume(){
    Vector meanPoint(0.0, 0.0, 0.0);
    double radius = 0;
    for(Vector &v : vertices){
        meanPoint = meanPoint.vadd(v);
        double vx = abs(v.x);
        double vy = abs(v.y);
        double vz = abs(v.z);
        double biggest = max(vx, max(vy, vz));
        radius = max(biggest, radius);
    }
    meanPoint = meanPoint.svmpy(1.0/vertices.size());
    printf("radius %f \n",radius);
    boundingVolume = {meanPoint.x, meanPoint.y, meanPoint.z, radius};
}

void Surface::normalizeAndScaleVertices(double scaling){
    double maxVal = 1.0;
    for(Vector &v : vertices){
        maxVal = max(maxVal, max(abs(v.x), max(abs(v.y), abs(v.z))));
    }
//    vector<Vector> result = {};
//    std::transform(vertices.begin(), vertices.end(), vertices.begin(), [&scaling](Vector v){return Vector(v.x*scaling, v.y*scaling, v.z*scaling);});
    for(Vector &v : vertices){
        v = Vector(v.x*scaling/maxVal, v.y*scaling/maxVal, v.z*scaling/maxVal);
    }
//    vertices = result;
}
