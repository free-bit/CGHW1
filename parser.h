#ifndef __HW1__PARSER__
#define __HW1__PARSER__

#include <string>
#include <vector>

namespace parser
{
    //Notice that all the structures are as simple as possible
    //so that you are not enforced to adopt any style or design.
    struct Vec3f
    {
        Vec3f(){};
        Vec3f(float _x, float _y, float _z){x=_x; y=_y; z=_z;};
        float x, y, z;
    };

    struct Vec3i
    {
        Vec3i(){};
        Vec3i(int _x, int _y, int _z){x=_x; y=_y; z=_z;};
        int x, y, z;
    };

    struct Vec4f
    {
        float x, y, z, w;
    };

    struct ray
    {
        float t;
        Vec3f e,d;
    };

    struct Camera
    {
        Vec3f position;
        Vec3f gaze;
        Vec3f up;
        Vec4f near_plane;
        float near_distance;
        int image_width, image_height;
        std::string image_name;
    };

    struct PointLight
    {
        Vec3f position;
        Vec3f intensity;
    };

    struct Material
    {
        Vec3f ambient;
        Vec3f diffuse;
        Vec3f specular;
        Vec3f mirror;
        float phong_exponent;
    };

    struct Face
    {
        int v0_id;
        int v1_id;
        int v2_id;
    };

    struct Mesh
    {
        int material_id;
        std::vector<Face> faces;
    };

    struct Triangle
    {
        int material_id;
        Face indices;
    };

    struct Sphere
    {
        int material_id;
        int center_vertex_id;
        float radius;
    };

    struct Scene
    {
        //Data
        Vec3i background_color;

        float shadow_ray_epsilon;

        int max_recursion_depth;
        // Specifies how many bounces the ray makes off of mirror-like objects. 
        // Applicable only when a material has nonzero MirrorReflectance value. 
        // Primary rays are assumed to start with zero bounce count.

        std::vector<Camera> cameras;

        Vec3f ambient_light;
        // Radiance
        
        std::vector<PointLight> point_lights;
        // Light source
        
        std::vector<Material> materials;
        // Use material_id's of each object

        std::vector<Vec3f> vertex_data;
        // Use vertex_id's of each triangle

        std::vector<Mesh> meshes;
        // Object (triangles)
        // Each mesh is composed of several faces. 
        // A face is actually a triangle which contains three vertices.

        std::vector<Triangle> triangles;
        // Object

        std::vector<Sphere> spheres;
        // Object

        //Functions
        void loadFromXml(const std::string& filepath);
    };
}

#endif
