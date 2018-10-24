#include <iostream>
#include <cmath>
#include <limits>
#include "parser.h"
#include "ppm.h"

typedef unsigned char RGB[3];

using namespace std;
using namespace parser;

float irradiance(float I, float d)
{
    return I/(d*d);
}

void ambient_shading(float ref_coeff)
{
    ;
}

void diffuse_shading(float ref_coeff)
{
    ;
}


void specular_shading(float ref_coeff)
{
    ;
}

//Ray-Triangle intersection
//Parameters: r parameter is mutable, others are constant
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

//Dot product of two vectors
//Parameters: All are constant
float dot_product(const Vec3f &v1, const Vec3f &v2)
{
    return (v1.x*v2.x + v1.y*v2.y + v1.z*v2.z);
}

//Ray-Sphere intersection
//Parameters: r parameter is mutable, others are constant
bool sphere_intersect(ray &r, const Vec3f &center, const float &R)
{
    Vec3f ce = {r.e.x - center.x, r.e.y - center.y, r.e.z - center.z};
    float ce_squared = dot_product(ce, ce),
          d_squared = dot_product(r.d, r.d),
          d_dot_ce = dot_product(r.d, ce);
    
    float discriminant = d_dot_ce*d_dot_ce - (d_squared * (ce_squared - R*R));
    cout<<discriminant<<endl;
    if(discriminant<0)
        return false;

    float discriminant_sqrt = sqrt(discriminant);
    r.t = (-d_dot_ce - discriminant_sqrt)/d_squared; // Small value of t (closest intersection point) is stored.
    return true;
}

void printPoint(Vec3f p)
{
    cout<<"("<<p.x<<", "<<p.y<<", "<<p.z<<")"<<endl;
}

Vec3f crossProduct(const Vec3f &v1, const Vec3f &v2)
{
    return Vec3f(v1.y*v2.z-v1.z*v2.y, -(v1.x*v2.z-v1.z*v2.x), v1.x*v2.y-v1.y*v2.x);
}

Vec3f revert(const Vec3f &point)
{
    return Vec3f(-point.x, -point.y, -point.z);
}

Vec3f scalarMultiplication(const Vec3f &v, float factor)
{
    return Vec3f(v.x*factor, v.y*factor, v.z*factor);
}

int main(int argc, char* argv[])
{
    //Testing
    //ray r = {0, {3, 5, 0}, {0, -5, 0}};
    Vec3f corner_a={1,0,0}, corner_b={0,1,0}, corner_c={0,1,0}, center = {0, 0, 0};
    //cout<<triangle_intersect(r, corner_a, corner_b, corner_c, 0, numeric_limits<float>::infinity())<<endl;
    //cout<<sphere_intersect(r, center, 3)<<endl;
    printPoint(crossProduct(corner_a,corner_b));
    //End testing
    
    if(argc<2)
    {
        cout<<"Missing parameters"<<endl;
        exit(-1);
    }
    parser::Scene scene;

    scene.loadFromXml(argv[1]);

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
        string img_name = camera.image_name;
        int img_width = camera.image_width, 
            img_height = camera.image_height;
        Vec3f u = crossProduct(camera.up, revert(camera.gaze));
        float horizontal = (r-l)/img_width,
              vertical = (t-b)/img_height;
        unsigned char* image = new unsigned char [img_width * img_height * 3];
        int pixel_i = 0;
        Vec3f pixel_center = {l+horizontal*0.5, b+vertical*0.5, camera.gaze*camera.near_distance};
        for (int y = 0; y < img_height; ++y)
        {
            for (int x = 0; x < img_width; ++x)
            {
                //ray r;
                //r.e = eyepoint;
                //r.d = compute_ray(r.e, s);
                float tmin = numeric_limits<float>::infinity();
                */
                /*for(auto &mesh : scene.meshes)
                {

                }*/

                /*for(auto &triangle : scene.triangles)
                {
                  Material triangle_material = scene.materials[triangle.material_id];
                  Vec3f corner_a = scene.vertex_data[triangle.indices.v0_id], 
                        corner_b = scene.vertex_data[triangle.indices.v1_id], 
                        corner_c = scene.vertex_data[triangle.indices.v2_id];
                  cout<<"Triangle:"<<endl;
                  printPoint(corner_a);
                  printPoint(corner_b);
                  printPoint(corner_c);
                }

                for(auto &sphere : scene.spheres)
                {
                  Material sphere_material = scene.materials[sphere.material_id];
                  Vec3f sphere_center = scene.vertex_data[sphere.center_vertex_id];
                  cout<<"Sphere:"<<endl;
                  printPoint(sphere_center);
                }
                pixel_center.x += horizontal;
            }
            pixel_center.y += vertical;
        }
        write_ppm(argv[2], image, img_width, img_height);
        delete [] image;
    }*/
}
/*
//Notes:
-> A ray can intersect with multiple objects at the same time. Use closest t always.
-> 
*/