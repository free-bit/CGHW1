#include <iostream>
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


//Dot product of two vectors
//Parameters: All are constant

//Ray-Sphere intersection
//Parameters: r parameter is mutable, others are constant

void printPoint(Vec3f p)
{
    cout<<"("<<p.x<<", "<<p.y<<", "<<p.z<<")"<<endl;
}

Vec3f revert(const Vec3f &point)
{
    return Vec3f(-point.x, -point.y, -point.z);
}

int main(int argc, char* argv[])
{
    //Testing
    /*ray r = {0, {-6, 0, 0}, {5.11, -6.37, 0}};
    Sphere sphere;
    sphere.center=Vec3f(0,0,0);
    sphere.radius=3;
    cout<<r.sphere_intersect(sphere)<<endl;
    cout<<r.t<<endl;*/
    //End testing
    // #if 0
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

    for(auto &camera : scene.cameras)
    {
        string img_name = camera.image_name;
        int img_width = camera.image_width,
            img_height = camera.image_height;
        Vec3f u_axis = camera.up.crossProduct(-camera.gaze);
        float horizontal = (camera.near_plane.r-camera.near_plane.l)/img_width,
              vertical = (camera.near_plane.t-camera.near_plane.b)/img_height;
        cout<<horizontal<<endl;
        cout<<vertical<<endl;

        unsigned char* image = new unsigned char [img_width * img_height * 3];

        int pixel_channel = 0;
        int count_true=0,count_false=0;
        ray r;
        r.e = camera.position; //eyepoint
        float v = camera.near_plane.b+vertical*0.5;
        for (int y = 0; y < img_height; y++)
        {
          float u = camera.near_plane.l+horizontal*0.5;
            for (int x = 0; x < img_width; x++)
            {
                //r.obj = NULL;
                r.t = numeric_limits<float>::infinity();
                r.d = camera.gaze*camera.near_distance + u_axis*u - camera.up*v;
                for(auto &mesh : scene.meshes)
                {
                  r.mesh_intersect(mesh, 0, numeric_limits<float>::infinity());
                }

                for(auto &triangle : scene.triangles)
                {
                  r.triangle_intersect(triangle, 0, numeric_limits<float>::infinity());
                }

                for(auto &sphere : scene.spheres)
                {
                  r.sphere_intersect(sphere);
                }
                if(r.t<numeric_limits<float>::infinity())
                {
                  Material material;
                  int material_id;
                  switch(r.c)
                  {
                    case 's':
                      material_id=((Sphere *) r.obj)->material_id;
                      break;
                    case 'm':
                      material_id=((Mesh *) r.obj)->material_id;
                      break;
                    case 't':
                      material_id=((Triangle *) r.obj)->material_id;
                      break;
                    default:
                      cout<<"error"<<endl;
                  }
                  material_id--; //zero-indexed
                  material.ambient = scene.materials[material_id].ambient; //ambient coeff
                  material.diffuse = scene.materials[material_id].diffuse;
                  material.specular = scene.materials[material_id].specular;
                  material.mirror = scene.materials[material_id].mirror;
                  material.phong_exponent = scene.materials[material_id].phong_exponent;
                  Vec3f ambient_color(scene.ambient_light.x*material.ambient.x,scene.ambient_light.y*material.ambient.y,scene.ambient_light.z*material.ambient.z);
                  //TODO: might require clamping
                  image[pixel_channel++]=ambient_color.x;
                  image[pixel_channel++]=ambient_color.y;
                  image[pixel_channel++]=ambient_color.z;
                }
                else
                {
                  //TODO: might require clamping
                  image[pixel_channel++]=scene.background_color.r;
                  image[pixel_channel++]=scene.background_color.g;
                  image[pixel_channel++]=scene.background_color.b;
                }
                u += horizontal;
            }
            v += vertical;
        }
        write_ppm(img_name.c_str(), image, img_width, img_height);
        delete [] image;
    }
    // #endif
}
/*
//Notes:
-> A ray can intersect with multiple objects at the same time. Use closest t always.
*/
