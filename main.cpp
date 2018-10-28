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

Vec3f ds_shading(const Vec3f &v, //v is used for both wi & h
                 const Vec3f &n,
                 const Vec3f &coeff,
                 const Vec3f &I,
                 const float &r,
                 const float &p=1)
{
  return coeff.elementwiseMultip((I*(1/(r*r)))*(pow(max((float)0, n*v),p)));
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

void intersection_test(ray &r,
                       vector<Mesh> &meshes,
                       vector<Triangle> &triangles,
                       vector<Sphere> &spheres,
                       float shadow_ray_epsilon)
{
  for(auto &mesh : meshes)
  {
    r.mesh_intersect(mesh, shadow_ray_epsilon);
  }

  for(auto &triangle : triangles)
  {
    r.triangle_intersect(triangle, shadow_ray_epsilon);
  }

  for(auto &sphere : spheres)
  {
    r.sphere_intersect(sphere, shadow_ray_epsilon);
  }
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

        unsigned char* image = new unsigned char [img_width * img_height * 3];

        int pixel_channel = 0;
        // int count_true=0,count_false=0;
        float shadow_ray_epsilon = scene.shadow_ray_epsilon;
        ray r;
        r.c = '\0';
        r.obj = NULL;
        r.e = camera.position; //eyepoint
        float v = camera.near_plane.b+vertical*0.5;
        for (int y = 0; y < img_height; y++)
        {
          float u = camera.near_plane.l+horizontal*0.5;
            for (int x = 0; x < img_width; x++)
            {
                image[pixel_channel]=0;
                image[pixel_channel+1]=0;
                image[pixel_channel+2]=0;
                //r.obj = NULL;
                r.t = numeric_limits<float>::infinity();
                r.d = camera.gaze*camera.near_distance + u_axis*u - camera.up*v;
                //intersection test for all objects in the scene
                intersection_test(r,
                                  scene.meshes,
                                  scene.triangles,
                                  scene.spheres,
                                  shadow_ray_epsilon);

                if(r.t<numeric_limits<float>::infinity())
                {
                  Material material;
                  int material_id;

                  Vec3f point_of_intersection(r.e + r.d*r.t);
                  Vec3f normal;

                  switch(r.c)
                  {
                    case 's':
                    {
                      Sphere* ptr = ((Sphere *) r.obj);
                      material_id = ptr->material_id;
                      normal = (point_of_intersection-ptr->center)*(1/ptr->radius); //Normalized
                      break;
                    }
                    case 'm':
                    {
                      Mesh* ptr = ((Mesh *) r.obj);
                      material_id = ptr->material_id;
                      normal = ptr->faces[0].normal;
                      break;
                    }
                    case 't':
                    {
                      Triangle* ptr = ((Triangle *) r.obj);
                      material_id = ptr->material_id;
                      normal = ptr->indices.normal;
                      break;
                    }
                    default:
                      cout<<"error"<<endl;
                  }
                  material_id--; //zero-indexed
                  material.ambient = scene.materials[material_id].ambient;  //ambient coeff
                  material.diffuse = scene.materials[material_id].diffuse;  //diffuse coeff
                  material.specular = scene.materials[material_id].specular;//specular coeff
                  material.mirror = scene.materials[material_id].mirror;    //mirror coeff
                  material.phong_exponent = scene.materials[material_id].phong_exponent;

                  //Ambient component calculation
                  Vec3f accumulator(scene.ambient_light.elementwiseMultip(material.ambient));
                  // TODO: might require clamping

                  for(auto &light : scene.point_lights)
                  {
                    ray shadowRay(point_of_intersection,
                                  light.position-point_of_intersection);
                    float distance2light = shadowRay.d.length();        //float distance to point light
                    Vec3f vector2light(shadowRay.d*(1/distance2light)); //Normalized vector

                    //intersection test for all objects in the scene
                    intersection_test(shadowRay,
                                      scene.meshes,
                                      scene.triangles,
                                      scene.spheres,
                                      shadow_ray_epsilon);
                    //object is in shadow
                    // cout<<"Shadow t:"<<shadowRay.t<<endl;
                    // cout<<"Distance2Light:"<<distance2light<<endl;
                    if(shadowRay.t<1)
                    {
                      continue;
                    }
                    //object is not in shadow, calculate diffuse component
                    Vec3f diffuse_component = ds_shading(vector2light,
                                                         normal,
                                                         material.diffuse,
                                                         light.intensity,
                                                         distance2light);
                    Vec3f specular_component = ds_shading((vector2light+(-r.d)).normalize(),
                                                          normal,
                                                          material.specular,
                                                          light.intensity,
                                                          distance2light,
                                                          material.phong_exponent);
                    accumulator=accumulator+diffuse_component+specular_component;
                  }
                  image[pixel_channel]=image[pixel_channel]+accumulator.x > 255 ? 255 : image[pixel_channel]+accumulator.x;
                  image[pixel_channel+1]=image[pixel_channel+1]+accumulator.y > 255 ? 255 : image[pixel_channel+1]+accumulator.y;
                  image[pixel_channel+2]=image[pixel_channel+2]+accumulator.z > 255 ? 255 : image[pixel_channel+2]+accumulator.z;

                  //...
                  pixel_channel+=3;
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
//TODO: Eliminate unnecessary computing of normals in mesh.
//TODO: Valgrind error check
*/
