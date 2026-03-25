#include <cmath>
#include <tuple>
#include "FileReader.h"
#include "tgaimage.h"
#include "Structs/Vec3.h"
#include <iostream>

#include "Structs/Geometry.h"
#include "Structs/Model.h"

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
void line(float ax, float ay, float bx, float by, TGAImage& buf,const TGAColor &c){
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
        }else{
            buf.set(i, y_t,c);
        }
        ierror += 2 * diff_y;
        if(ierror > diff_x){
            y_t +=  by > ay ? 1: -1;
            ierror -= error_add;
        }
    }

}

void processModelData(TGAImage& framebuffer)
{
    Model model("./diablo3_pose.obj");

    vector<vector<float>> vertices = model.get_vertices();
    vector<int> faces = model.get_faces();
    
    for (int i = 0; i < model.numberOfFaces(); i+=3)
    {
        vector<float> v1 = vertices[faces[i]];
        vector<float> v2 = vertices[faces[i+1]];
        vector<float> v3 = vertices[faces[i+2]];

        vector<float> v1_transformed = projection_transform(v1);
        vector<float> v2_transformed = projection_transform(v2);
        vector<float> v3_transformed = projection_transform(v3);

        line(v1_transformed[0], v1_transformed[1], v2_transformed[0], v2_transformed[1], framebuffer,red);
        line(v1_transformed[0], v1_transformed[1], v3_transformed[0], v3_transformed[1], framebuffer,red);
        line(v2_transformed[0], v2_transformed[1], v3_transformed[0], v3_transformed[1], framebuffer,red);
    }
}

int main(int argc, char** argv)
{
    TGAImage framebuffer(width, height, TGAImage::RGB);
    processModelData(framebuffer);
    framebuffer.write_tga_file("framebuffer.tga");

    return 0;
}
