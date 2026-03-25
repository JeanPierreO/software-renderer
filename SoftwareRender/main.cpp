#include <cmath>
#include <tuple>
#include "FileReader.h"
#include "tgaimage.h"
#include "Structs/Vec3.h"
#include <iostream>
#include "Structs/Geometry.h"
#include "Structs/Model.h"
#include <map>
#include <random>

constexpr int width = 800;
constexpr int height = 800;
constexpr TGAColor white = {255, 255, 255, 255}; // attention, BGRA order
constexpr TGAColor green = {0, 255, 0, 255};
constexpr TGAColor red = {0, 0, 255, 255};
constexpr TGAColor blue = {255, 128, 64, 255};
constexpr TGAColor yellow = {0, 200, 255, 255};

vector<float> projection_transform(vector<float> vertex)
{
    vector<float> vertex_transformed;

    float x_new = (vertex[0]+ 1.) * width / 2;
    vertex_transformed.push_back(x_new);
    float y_new = (vertex[1]+ 1.) * width / 2;
    vertex_transformed.push_back(y_new);

    return vertex_transformed;
}
vector<pair<float,float>> line(float ax, float ay, float bx, float by, TGAImage& buf,const TGAColor &c, vector<pair<float,float>>& vertices){
    //fix steep line
    bool steep = abs(bx-ax) < abs(by-ay);
    if(steep){// In the answer code he simply checked abs(bx-ax) < abs(by-ay)
        std::swap(ax,ay);
        std::swap(bx,by);
    }

    if(bx < ax){//to swap the values if the endpoint x is smaller than initial point
        std::swap(ax, bx);
        std::swap(ay, by);
    }
    int diff_x = bx - ax;
    int ierror = 0;
    int error_add = 2 * (bx-ax);
    int diff_y = std::abs(by-ay);
    int y_t = ay;
    for(float i{ax}; i <= bx; ++i){
        if(steep){
            buf.set(y_t, i,c);
            vertices.push_back({y_t, i});
        }else{
            buf.set(i, y_t,c);
            vertices.push_back({i, y_t});
        }
        ierror += 2 * diff_y;
        if(ierror > diff_x){
            y_t +=  by > ay ? 1: -1;
            ierror -= error_add;
        }
    }
    return vertices;
}
void sortVertexPairByY(vector<pair<float,float>> &vertices)
{
    pair<float,float> tempPair;
   

    for (int i = 0; i < vertices.size(); i++)
    {
        tempPair = vertices[i];
        for (int j = 0; j < vertices.size() - 1; j++)
        {
            if (vertices[j].second < vertices[j+1].second)
            {
                tempPair = vertices[j];
                vertices[j] = vertices[j+1];
                vertices[j+1] = tempPair;
            }
        }
    }
}
//Draws the outlines of a triangle first. Then takes all of those vertices and sorts them by the y.
//We then take that list of vertices pairs and add it to a map so we can get all of the x values for each y.
//We then draw a 1d line between the y and x's for each key and vector value list
void triangle_scanline(int ax, int ay, int bx, int by, int cx, int cy, TGAImage &framebuffer, TGAColor color) {
    vector<pair<float,float>> verticesOfTriangle;
    map<float, vector<float>> verticesOfTriangleMap;
    
    line(ax, ay, bx, by, framebuffer, color, verticesOfTriangle);
    line(bx, by, cx, cy, framebuffer, color, verticesOfTriangle);
    line(cx, cy, ax, ay, framebuffer, color, verticesOfTriangle);

    for (int i = 0; i < verticesOfTriangle.size(); i++)
    {
        verticesOfTriangleMap[verticesOfTriangle[i].second].push_back(verticesOfTriangle[i].first);
    }

    int index = 0;
    
    for (const auto& [key,value] : verticesOfTriangleMap)
    {
        if (verticesOfTriangleMap[key].size() < 2)
            continue;

        for (int ax = 0; ax < verticesOfTriangleMap[key].size() - 1; ax++)
        {
            line(verticesOfTriangleMap[key][ax], key, verticesOfTriangleMap[key][ax + 1], key, framebuffer, color, verticesOfTriangle);   
        }
    }
}

double signed_triangle_area(int ax, int ay, int bx, int by, int cx, int cy) {
    return .5*((by-ay)*(bx+ax) + (cy-by)*(cx+bx) + (ay-cy)*(ax+cx));
}

void triangle_boundingbox(int ax, int ay, int bx, int by, int cx, int cy, TGAImage &framebuffer, TGAColor color)
{
    //4,10 - 10,20 x1,y1 x2,y2
    int boundingBoxMinX = min(min(ax, bx), cx);
    int boundingBoxMinY = min(min(ay, by), cy);
    int boundingBoxMaxX = max(max(ax, bx), cx);
    int boundingBoxMaxY = max(max(ay, by), cy);

    double total_area = signed_triangle_area(ax, ay, bx, by, cx, cy);

    if (total_area<1) return;
    
    for (int y = boundingBoxMinY; y <= boundingBoxMaxY; y++)
    {
        for (int x = boundingBoxMinX; x <= boundingBoxMaxX; x++)
        {
            double alpha = signed_triangle_area(x, y, bx, by, cx, cy)/ total_area;
            double beta = signed_triangle_area(x, y, cx, cy, ax, ay)/ total_area;
            double gamma = signed_triangle_area(x, y, ax, ay, bx, by) / total_area;

            if (alpha<0 || beta<0 || gamma<0) continue;
            
            framebuffer.set(x, y, color);
        }
    }
    
    vector<pair<float,float>> verticesOfTriangle;
    
    line(ax, ay, bx, by, framebuffer, color, verticesOfTriangle);
    line(bx, by, cx, cy, framebuffer, color, verticesOfTriangle);
    line(cx, cy, ax, ay, framebuffer, color, verticesOfTriangle);
}

TGAColor GetRandomColor()
{
    static std::mt19937 gen(std::random_device{}());
    static std::uniform_int_distribution<int> dist(0, 255);

    return TGAColor{
        static_cast<uint8_t>(dist(gen)),
        static_cast<uint8_t>(dist(gen)),
        static_cast<uint8_t>(dist(gen)),
        255  // or dist(gen) if you want random alpha
    };
}

void processModelData(TGAImage& framebuffer)
{
    Model model("./diablo3_pose.obj");

    vector<vector<float>> vertices = model.get_vertices();
    vector<int> faces = model.get_faces();
    
    for (int i = 0; i < model.numberOfFaces(); i+=3)
    {
        vector<pair<float,float>> verticesOfTriangle;
        vector<float> v1 = vertices[faces[i]];
        vector<float> v2 = vertices[faces[i+1]];
        vector<float> v3 = vertices[faces[i+2]];

        vector<float> v1_transformed = projection_transform(v1);
        vector<float> v2_transformed = projection_transform(v2);
        vector<float> v3_transformed = projection_transform(v3);
        
        triangle_boundingbox(v1_transformed[0], v1_transformed[1], v2_transformed[0], v2_transformed[1], v3_transformed[0], v3_transformed[1],framebuffer, GetRandomColor());
    }
}

int main(int argc, char** argv)
{
    TGAImage framebuffer(width, height, TGAImage::RGB);

    // triangle_boundingbox(  7, 45, 35, 100, 45,  60, framebuffer, red);
    // triangle_boundingbox(120, 35, 90,   5, 45, 110, framebuffer, white);
    // triangle_boundingbox(115, 83, 80,  90, 85, 120, framebuffer, green);
        
    processModelData(framebuffer);
    framebuffer.write_tga_file("framebuffer.tga");
    return 0;
}
