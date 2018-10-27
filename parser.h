#ifndef __HW1__PARSER__
#define __HW1__PARSER__

#include <string>
#include <vector>
#include <cmath>
#include <limits>
#include <iostream>

namespace parser
{
    //Notice that all the structures are as simple as possible
    //so that you are not enforced to adopt any style or design.
    struct Vec3f
    {
        float x, y, z;
        Vec3f(){};
        Vec3f(float _x, float _y, float _z){this->x=_x; this->y=_y; this->z=_z;};
        Vec3f(const Vec3f &v2){*this=v2;}
        float length() const
        {
          return sqrt(x*x + y*y + z*z);
        }
        Vec3f normalize() const
        {
          float length_inv = 1/(this->length());
          return (*this)*length_inv;
        }

        Vec3f elementwiseMultip(const Vec3f &v2) const
        {
          return Vec3f(this->x*v2.x, this->y*v2.y, this->z*v2.z);
        }

        void operator=(const Vec3f &v2){this->x=v2.x; this->y=v2.y; this->z=v2.z;}

        Vec3f operator+(const Vec3f &v2) const
        {
          return Vec3f(this->x+v2.x, this->y+v2.y, this->z+v2.z);
        }

        Vec3f operator-(const Vec3f &v2) const
        {
          return Vec3f(this->x-v2.x, this->y-v2.y, this->z-v2.z);
        }

        Vec3f crossProduct(const Vec3f &v2) const
        {
            return Vec3f(this->y*v2.z-this->z*v2.y, -(this->x*v2.z-this->z*v2.x), this->x*v2.y-this->y*v2.x);
        }

        //scaler_multiplication
        Vec3f operator*(float factor) const
        {
            return Vec3f(this->x*factor, this->y*factor, this->z*factor);
        }

        //dot_product
        float operator*(const Vec3f &v2) const
        {
          return (this->x*v2.x + this->y*v2.y + this->z*v2.z);
        }

        //negate
        Vec3f operator-() const
        {
          return Vec3f(-this->x, -this->y, -this->z);
        }
    };

    struct Vec3i
    {
        int r, g, b;
        Vec3i(){};
        Vec3i(int _r, int _g, int _b){r=_r; g=_g; b=_b;};
        Vec3i(const Vec3i &v2){r=v2.r; g=v2.g; b=v2.b;}
        Vec3i operator+(const Vec3i &v2)
        {
          return Vec3i(this->r+v2.r, this->g+v2.g, this->b+v2.b);
        }
        Vec3i operator-(const Vec3i &v2)
        {
          return Vec3i(this->r-v2.r, this->g-v2.g, this->b-v2.b);
        }
    };

    struct Vec4f
    {
        float l, r, b, t;
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
        Vec3f corner_a, corner_b, corner_c, normal;
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
        Vec3f center;
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

    struct ray
    {
        char c;
        float t;
        void *obj;
        Vec3f e,d;
        ray(){};
        ray(const float &_t, const Vec3f &_e, const Vec3f &_d){this->t=_t; this->e=_e; this->d=_d;};
        ray(const ray &r2){*this=r2;}
        void operator=(const ray &r2){this->t=r2.t; this->e=r2.e; this->d=r2.d;}
        void sphere_intersect(Sphere &sphere)
        {
            Vec3f ce = {this->e.x - sphere.center.x, this->e.y - sphere.center.y, this->e.z - sphere.center.z};
            float ce_squared = ce*ce,
                  d_squared = this->d*this->d,
                  d_dot_ce = this->d*ce;

            float discriminant = d_dot_ce*d_dot_ce - (d_squared * (ce_squared - sphere.radius*sphere.radius));
            if(discriminant<0)
                return;

            float discriminant_sqrt = sqrt(discriminant);
            float _t = (-d_dot_ce - discriminant_sqrt)/d_squared; // Small value of t (closest intersection point) is stored.
            if(_t<t)
            {
              this->t = _t;
              this->obj = &sphere;
              this->c='s';
            }
        }
        void mesh_intersect(Mesh &mesh, const float &t0, const float &t1)
        {
          for(auto &face : mesh.faces)
          {
            if(triangle_mesh_intersect_subroutine(face, t0, t1))
            {
              this->obj = &mesh;
              this->c='m';
            }
          }
        }

        void triangle_intersect(Triangle &triangle, const float &t0, const float &t1)
        {
          if(triangle_mesh_intersect_subroutine(triangle.indices, t0, t1))
          {
            this->obj = &triangle;
            this->c='t';
          }
        }

      private:
        bool triangle_mesh_intersect_subroutine(Face &corners, const float &t0, const float &t1)
        {
            Vec3f ab=corners.corner_a-corners.corner_b,
                  ac=corners.corner_a-corners.corner_c,
                  ae=corners.corner_a-this->e;
            float a = ab.x,
                  b = ab.y,
                  c = ab.z,
                  d = ac.x,
                  e = ac.y,
                  f = ac.z,
                  g = this->d.x,
                  h = this->d.y,
                  i = this->d.z,
                  j = ae.x,
                  k = ae.y,
                  l = ae.z,
                  eihf = e*i - h*f,
                  gfdi = g*f - d*i,
                  dheg = d*h - e*g,
                  akjb = a*k - j*b,
                  jcal = j*c - a*l,
                  blkc = b*l - k*c,
                  M = a*eihf + b*gfdi + c*dheg;

            float t = -(f*akjb + e*jcal + d*blkc)/M;
            if(t<=t0 || t>t1)
                return false;

            float gamma = (i*akjb + h*jcal + g*blkc)/M;
            if(gamma<0 || gamma>1)
                return false;

            float beta = (j*eihf + k*gfdi + l*dheg)/M;
            if(beta<0 || (beta>1-gamma))
                return false;
            if(t < this->t)
            {
              this->t = t;
              corners.normal=((corners.corner_b-corners.corner_a).crossProduct(corners.corner_c-corners.corner_a)).normalize();
              return true;
            }
            return false;
        }
    };
}

#endif
