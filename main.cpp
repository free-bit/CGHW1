#include <iostream>
#include <cmath>
#include <limits>
#include "parser.h"
#include "ppm.h"

typedef unsigned char RGB[3];

using namespace std;
using namespace parser;

void compute_ray()
{
    ;
}

void diffuse_shading()
{
    ;
}

void ambient_shading()
{
    ;
}

void specular_shading()
{
    ;
}

bool triangle_intersect(ray &r, const Vec3f &corner_a, const Vec3f &corner_b, const Vec3f &corner_c, const float &t0, const float &t1)
{ 
    float a = corner_a.x - corner_b.x,
          b = corner_a.y - corner_b.y,
          c = corner_a.z - corner_b.z,
          d = corner_a.x - corner_c.x,
          e = corner_a.y - corner_c.y,
          f = corner_a.z - corner_c.z,
          g = r.d.x,
          h = r.d.y,
          i = r.d.z,
          j = corner_a.x - r.e.x,
          k = corner_a.y - r.e.y,
          l = corner_a.z - r.e.z,
          eihf = e*i - h*f,
          gfdi = g*f - d*i,
          dheg = d*h - e*g,
          akjb = a*k - j*b,
          jcal = j*c - a*l,
          blkc = b*l - k*c,
          M = a*eihf + b*gfdi + c*dheg;

    float t = -(f*akjb + e*jcal + d*blkc)/M;
    cout<<t<<endl;
    if(t<0 || t>t1)
        return false;
    
    float gamma = (i*akjb + h*jcal + g*blkc)/M;
    if(gamma<0 || gamma>1)
        return false;

    float beta = (j*eihf + k*gfdi + l*dheg)/M;
    if(beta<0 || (beta>1-gamma))
        return false; 

    r.t = t; //Store t
    return true;
}

void sphere_intersect()
{
    ;
}

int main(int argc, char* argv[])
{
    ray r = {0, {2, 0, 0}, {-2, 0, 3}};
    Vec3f corner_a={0,-1,0}, corner_b={0,0,2}, corner_c={0,1,0};
    cout<<triangle_intersect(r, corner_a, corner_b, corner_c, 0, numeric_limits<float>::infinity())<<endl;
    // Sample usage for reading an XML scene file
    //parser::Scene scene;

    //scene.loadFromXml(argv[1]);

    // The code below creates a test pattern and writes
    // it to a PPM file to demonstrate the usage of the
    // ppm_write function.

    /* Remove later
    const RGB BAR_COLOR[8] =
    {
        { 255, 255, 255 },  // 100% White
        { 255, 255,   0 },  // Yellow
        {   0, 255, 255 },  // Cyan
        {   0, 255,   0 },  // Green
        { 255,   0, 255 },  // Magenta
        { 255,   0,   0 },  // Red
        {   0,   0, 255 },  // Blue
        {   0,   0,   0 },  // Black
    };

    int width = 640, height = 480;
    int columnWidth = width / 8;

    unsigned char* image = new unsigned char [width * height * 3];

    int i = 0;
    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            int colIdx = x / columnWidth;
            image[i++] = BAR_COLOR[colIdx][0];
            image[i++] = BAR_COLOR[colIdx][1];
            image[i++] = BAR_COLOR[colIdx][2];
        }
    } Remove later*/

    //MAIN

    /*for(auto &camera : scene.cameras)
    {
        string img_name=camera.image_name;
        int img_width = camera.image_width, img_height = camera.image_height;
        unsigned char* image = new unsigned char [img_width * img_height * 3];
        for(auto &pixel : image)
        {
            compute_ray();
            int tmin=INT_MAX;
            for(auto &mesh : scene.meshes)
            {

            }

            for(auto &triangle : scene.triangles)
            {
                
            }

            for(auto &spheres : scene.spheres)
            {
                
            }

        }

    }*/

    //write_ppm(argv[2], image, width, height);

}
