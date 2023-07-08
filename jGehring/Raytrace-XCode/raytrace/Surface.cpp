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
    std::for_each(vertices.begin(), vertices.end(), [&meanPoint, &radius](Vector v){
        meanPoint = meanPoint.vadd(v);
        double vx = abs(v.x);
        double vy = abs(v.y);
        double vz = abs(v.z);
        double biggest = max(vx, max(vy, vz));
        radius = max(biggest, radius);
    });
    meanPoint = meanPoint.svmpy(1.0/vertices.size());
    printf("radius %f \n",radius);
    boundingVolume = {meanPoint.x, meanPoint.y, meanPoint.z, radius};
}

void Surface::normalizeAndScaleVertices(double scaling){
    
}
