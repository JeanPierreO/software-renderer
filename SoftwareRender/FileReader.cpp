#include "FileReader.h"
#include <fstream>
#include <iostream>
#include <sstream>

#include "Structs/Face.h"

vector<string> Split(const string& line)
{
    vector<string> tokens;
    string token;
    stringstream ss(line);

    while (ss >> token)
    {
        tokens.push_back(token);
    }

    return tokens;
}


vector<string> SplitSlash(const string& input)
{
    vector<string> tokens;
    stringstream ss(input);
    string part;

    while (getline(ss, part, '/'))
    {
        tokens.push_back(part);
    }

    return tokens;
}

vector<string> FileReader::ReadFile(char definitionToPull)
{
    ifstream file("./Model.txt");

    if (!file.is_open())
    {
        cout << "Failed to open file\n";
    }

    vector<string> lines;
    string line;

    while (getline(file, line))
    {
        if (!line.empty() && line[0] == definitionToPull)
        {
            lines.push_back(line);
        }
    }

    file.close();

    return lines;
}

vector<Vec3> FileReader::GetVertices()
{
    vector<string> lines = ReadFile('v');
    vector<Vec3> vertices;

    for (auto& line : lines)
    {
        if (line.compare(0, 2, "v ") == 0)
        {
            line.erase(0, 2);

            stringstream ss(line);

            Vec3 v;
            ss >> v.x >> v.y >> v.z;

            vertices.push_back(v);
        }
    }

    return vertices;
}

vector<Face> FileReader::GetFaces()
{
    vector<string> lines = ReadFile('f');
    vector<Face> faces;

    for (auto& line : lines)
    {
        if (line.compare(0, 1, "f") == 0)
        {
            line.erase(0, 1);
            
            vector<string> splitFaces = Split(line);
            
            for (auto& face : splitFaces)
            {
                vector<string> tokens = SplitSlash(face);
                
                Face f;

                f.index = stoi(tokens[0]);
                
                faces.push_back(f);
            }
        }
    }
    
    return faces;
}
