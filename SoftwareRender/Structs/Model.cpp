#include <fstream>
#include <sstream>
#include "model.h"
#include <iostream>

using namespace std;

Model::Model(const string filename) {
    ifstream in;
    in.open(filename, ifstream::in);
    if (in.fail()) return;
    string line;
    while (getline(in, line))
    {
        if (line.substr(0,2) == "v ")
        {
            string s = line.substr(2, line.size());
            float x, y, z;

            vector<float> newVertex;

            sscanf_s(s.c_str(), "%f %f %f", &x, &y, &z);

            newVertex.push_back(x);
            newVertex.push_back(y);
            newVertex.push_back(z);

            vertices_xyz.push_back(newVertex);
        }
    }

    verticesArraySize = vertices_xyz.size();
    verticesNum = verticesArraySize / 3;

    in.clear();
    in.seekg(0);

    string face;

    while (getline(in, face))
    {
        if (face[0] == 'f')
        {
            string s = face.substr(2, face.size());
            string delimiter = " ";
            int face_index = 0;
            size_t pos = s.find(delimiter);
            while (pos != string::npos)
            {
                string temp = s.substr(0, pos);
                sscanf_s(temp.c_str(), "%d/*/*", &face_index);
                faces_xyz.push_back(face_index - 1);
                s.erase(0, pos + delimiter.length());
                pos = s.find(delimiter);
            }

            sscanf_s(s.c_str(), "%d/*/*", &face_index);
            faces_xyz.push_back(face_index - 1);
        }
    }
    
    facesArraySize = faces_xyz.size();
    facesNum = facesArraySize / 3;
    
    std::cerr << "# v# " << numberOfVertices() << " f# "  << numberOfFaces() << std::endl;
}