#pragma once
#include <string>
#include <vector>
#include "Geometry.h"

using namespace std;

class Model
{
public:
    Model(const string fileName);

    int numberOfVertices()
    {
        return vertices_xyz.size();
    }
    int numberOfFaces()
    {
        return faces_xyz.size();
    }

    vec3 vertex(const int i) const;
    vec3 vertex(const int iface, const int numOfvert) const;

    vector<vector<float>> get_vertices(){
        return vertices_xyz;
    }

    vector<int> get_faces(){
        return faces_xyz;
    }
    
private:
    vector<vector<float>> vertices_xyz;
    vector<int> faces_xyz;
    int verticesArraySize;
    int facesArraySize;
    int verticesNum;
    int facesNum;
};
