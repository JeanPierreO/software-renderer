#pragma once
#include <string>
#include <vector>

#include "Structs/Face.h"
#include "Structs/Vec3.h"

using namespace std;

class FileReader
{
public:
    vector<string> ReadFile(char definitionToPull);
    vector<Vec3> GetVertices();
    vector<Face> GetFaces();
};
