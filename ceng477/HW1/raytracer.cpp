#include <iostream>
#include <limits>
#include <cmath>
#include "parser.h"
#include "ppm.h"

typedef unsigned char RGB[3];

parser::Vec3f negVec3(parser::Vec3f vec1){
    vec1.x = -vec1.x;
    vec1.y = -vec1.y;
    vec1.z = -vec1.z;
    return vec1;
}

parser::Vec3f addVec3(parser::Vec3f vec1, parser::Vec3f vec2){
    vec2.x += vec1.x;
    vec2.y += vec1.y;
    vec2.z += vec1.z;
    return vec2;
}

parser::Vec3f subVec3(parser::Vec3f vec1, parser::Vec3f vec2){
    vec1.x -= vec2.x;
    vec1.y -= vec2.y;
    vec1.z -= vec2.z;
    return vec1;
}

parser::Vec3f divVec3(parser::Vec3f vec2, float num){
    vec2.x /= num;
    vec2.y /= num;
    vec2.z /= num;
    return vec2;
}

parser::Vec3f mulVec3(parser::Vec3f vec2, float num){
    vec2.x *= num;
    vec2.y *= num;
    vec2.z *= num;
    return vec2;
}

parser::Vec3f parallelmul(parser::Vec3f v1, parser::Vec3f v2){
    v1.x *= v2.x;
    v1.y *= v2.y;
    v1.z *= v2.z;
    return v1;
}

parser::Vec3f crossProduct(parser::Vec3f vec1, parser::Vec3f vec2){
    parser::Vec3f vec3;
    vec3.x = (vec1.y*vec2.z) - (vec1.z*vec2.y);
    vec3.y = (vec1.z*vec2.x) - (vec1.x*vec2.z);
    vec3.z = (vec1.x*vec2.y) - (vec1.y*vec2.x);
    return vec3;
}

float dotProduct(parser::Vec3f vec1, parser::Vec3f vec2){
    float result = vec1.x*vec2.x + vec1.y*vec2.y + vec1.z*vec2.z;
    return result;
}

void calculateNormals(std::vector<parser::Vec3f>& triangleNormals, parser::Scene& scene){
    for(auto triangle: scene.triangles){
        parser::Vec3f v1 = subVec3(scene.vertex_data[triangle.indices.v1_id-1], scene.vertex_data[triangle.indices.v0_id-1]);
        parser::Vec3f v2 = subVec3(scene.vertex_data[triangle.indices.v2_id-1], scene.vertex_data[triangle.indices.v0_id-1]);
        parser::Vec3f v3 = crossProduct(v1,v2);
        triangleNormals.push_back(v3);
    }
}

void calculateMeshNormals(std::vector<std::vector<parser::Vec3f>>& meshNormals, parser::Scene& scene){
    int size = scene.meshes.size();
    for(int i=0; i<size; i++){
        int sizemesh = scene.meshes[i].faces.size();
        for(int j=0; j<sizemesh; j++){
            parser::Face currFace = scene.meshes[i].faces[j];
            parser::Vec3f v1 = subVec3(scene.vertex_data[currFace.v1_id-1], scene.vertex_data[currFace.v0_id-1]);
            parser::Vec3f v2 = subVec3(scene.vertex_data[currFace.v2_id-1], scene.vertex_data[currFace.v0_id-1]);
            parser::Vec3f v3 = crossProduct(v1,v2);
            meshNormals[i].push_back(v3);
        }
        
    }
}

struct Object{
    char type; //0:sphere, 1:triangle, 2:mesh
    int material_id;
    int id;
    parser::Vec3f intersectionPoint;
    float t;
    int meshface = -1; //-1 ise mesh diil
};

bool RayIntersectsSphere(parser::Vec3f e,
                         parser::Vec3f d,
                         parser::Scene& scene,
                         Object& obj,
                         parser::Sphere sphere)
{
    parser::Vec3f c = scene.vertex_data[sphere.center_vertex_id-1];

    parser::Vec3f oc = subVec3(e, c);
    double discriminant = dotProduct(d, oc)*dotProduct(d, oc) - dotProduct(d, d) * (dotProduct(oc, oc) - sphere.radius*sphere.radius)    ;
    
    if(discriminant<0){
        return false;
    }
    discriminant = sqrt(discriminant); 
    
    if(discriminant == 0){
        obj.t = -dotProduct(d, oc)/dotProduct(d, d);
    }
    else{
        // bura da verimsiz
        obj.t = (-dotProduct(d, oc)-discriminant)/dotProduct(d, d);
    }
    obj.intersectionPoint = addVec3(e,mulVec3(d, obj.t));
    obj.type = 0;
    obj.material_id = sphere.material_id;
    return true;
}                         

bool RayIntersectsTriangle(parser::Vec3f e, 
                           parser::Vec3f d, 
                           parser::Scene& scene,
                           Object& obj,
                           parser::Face& T)
{
    parser::Vec3f v0 = scene.vertex_data[T.v0_id-1];
    parser::Vec3f v1 = scene.vertex_data[T.v1_id-1];  
    parser::Vec3f v2 = scene.vertex_data[T.v2_id-1];
    parser::Vec3f e1, e2, n, s, k;
    float epsilon = 0.0000001;
    float a, f_val, u, v;
    e1 = subVec3(v1, v0);
    e2 = subVec3(v2, v0);
    n = crossProduct(d, e2);
    a = dotProduct(e1, n);
    if (a > -epsilon && a < epsilon)
        return false; 
    s = subVec3(e, v0);
    f_val = 1.0 / a;
    u = f_val * dotProduct(s, n);
    if (u < 0.0 || u > 1.0)
        return false;
    k = crossProduct(s, e1);
    v = f_val * dotProduct(d, k);
    if (v < 0.0 || u + v > 1.0)
        return false;
    obj.t = f_val * dotProduct(e2, k);
    if (obj.t > epsilon) 
    {
        obj.intersectionPoint = addVec3(e, mulVec3(d, obj.t));
        return true;
    }
    else 
        return false;
}



bool RayIntersectsObject(parser::Vec3f e,
                         parser::Vec3f d,
                         parser::Scene& scene,
                         Object& obj)
{
    bool intersect = false;
    float tmin = std::numeric_limits<float>::infinity();
    obj.t = std::numeric_limits<float>::infinity();
    Object returnObj;

    for(int k=0; k< scene.spheres.size(); k++){
        if(RayIntersectsSphere(e, d, scene, obj, scene.spheres[k])){
            if(obj.t<tmin && obj.t > 0){
                obj.id = k;
                obj.meshface = -1;
                tmin = obj.t;
                intersect = true;
                returnObj = obj;
            }
        }
    }

    for(int k=0; k<scene.triangles.size(); k++){
        if(RayIntersectsTriangle(e, d, scene, obj, scene.triangles[k].indices)){
            if(obj.t<tmin && obj.t > 0){
                tmin = obj.t;
                intersect = true;
                obj.id = k;
                obj.type = 1;
                obj.material_id = scene.triangles[k].material_id;
                obj.meshface = -1;
                returnObj = obj;
            }
        }
    }

    for(int k=0; k<scene.meshes.size(); k++){
        for(int l=0; l<scene.meshes[k].faces.size(); l++){
            if(RayIntersectsTriangle(e, d, scene, obj, scene.meshes[k].faces[l])){
                if(obj.t<tmin && obj.t > 0){
                    tmin = obj.t;
                    intersect = true;
                    obj.id = k;
                    obj.type = 2;
                    obj.material_id = scene.meshes[k].material_id;
                    obj.meshface = l;
                    returnObj = obj;
                }
            }
        }
    }
    obj = returnObj;
    return intersect;
}

bool inShadow(parser::PointLight& light,
              parser::Vec3f& intersectionPoint,
              parser::Scene& scene)
{
    parser::Vec3f e, d;
    e = light.position;
    d = subVec3(intersectionPoint, e);
    Object obj;
    if(RayIntersectsObject(e, d, scene, obj)){
        if(obj.t<1) return true;
    }
    return false;
}


parser::Vec3i clamp(parser::Vec3f color, int s, int f){
    parser::Vec3i real_color;
    real_color.x = color.x; real_color.y = color.y; real_color.z = color.z;
    if(color.x > 255) real_color.x = 255;
    else if(color.x < 0) real_color.x = 0;
    if(color.y > 255) real_color.y = 255;
    else if(color.y < 0) real_color.y = 0;
    if(color.z > 255) real_color.z = 255;
    else if(color.z < 0) real_color.z = 0;
    return real_color;
}

void normalization(parser::Vec3f& n){
    float sum = sqrt(n.x*n.x+n.y*n.y+n.z*n.z);
    if(sum==0) return;
    n.x = (n.x/sum);
    n.y = (n.y/sum);
    n.z = (n.z/sum);
}

parser::Vec3f calculateNormal(Object obj, parser::Scene scene, std::vector<parser::Vec3f>& triangleNormals, std::vector<std::vector<parser::Vec3f>>& meshNormals){
    if(obj.type == 0){
        parser::Vec3f n = addVec3(negVec3(scene.vertex_data[scene.spheres[obj.id].center_vertex_id-1]), obj.intersectionPoint);
        normalization(n);
        return n;
    }
    else if(obj.type == 1){
        parser::Vec3f n = triangleNormals[obj.id];
        normalization(n);
        return n;
    }


    else if(obj.type == 2){
        parser::Vec3f n = meshNormals[obj.id][obj.meshface];
        normalization(n);
        return n;        
    }
}

int return_normal(parser::Vec3f normal, parser::Vec3f vec){
    if (dotProduct(normal, vec) < 0) return 1;
    return 0;
}

parser::Vec3f computeColor(parser::Vec3f d, int depth, parser::Scene scene, Object obj, parser::Vec3f e, std::vector<parser::Vec3f>& triangleNormals, std::vector<std::vector<parser::Vec3f>>& meshNormals);

parser::Vec3f applyShading(parser::Vec3f d, int depth, parser::Scene scene, Object obj, parser::Vec3f e, std::vector<parser::Vec3f>& triangleNormals, std::vector<std::vector<parser::Vec3f>>& meshNormals){
    parser::Vec3f color; color.x = 0; color.y=0; color.z=0; // ambient light
    
    parser::Vec3f normal = calculateNormal(obj, scene, triangleNormals, meshNormals);
    parser::Vec3f normalnegd = negVec3(d);
    normalization(normalnegd);
    color = parallelmul(scene.ambient_light, scene.materials[obj.material_id-1].ambient);
    if(scene.materials[obj.material_id-1].is_mirror){
        parser::Vec3f reflectRay;reflectRay.x = 0; reflectRay.y =0; reflectRay.z =0;
        parser::Vec3f point2 = addVec3(obj.intersectionPoint, mulVec3(normal, scene.shadow_ray_epsilon));
        parser::Vec3f wo = negVec3(d);  
        Object temp_obj;
        float zero = 0;
        normalization(wo);
        reflectRay = addVec3(negVec3(wo), mulVec3(mulVec3(normal,2), std::max(zero ,dotProduct(normal, wo))));
        normalization(reflectRay); 
        depth++;
        color = addVec3(parallelmul(computeColor(reflectRay, depth, scene, temp_obj, point2, triangleNormals, meshNormals), scene.materials[obj.material_id-1].mirror),color);
    }    
    for(int i=0;i<scene.point_lights.size();i++){
        parser::Vec3f epsilon_point = addVec3(obj.intersectionPoint, mulVec3(normal, scene.shadow_ray_epsilon));
        if(!inShadow(scene.point_lights[i], epsilon_point, scene)){

            parser::Vec3f w = addVec3(scene.point_lights[i].position, negVec3(obj.intersectionPoint));
            parser::Vec3f specShading;
            parser::Vec3f light_distance = subVec3(scene.point_lights[i].position, obj.intersectionPoint);
            parser::Vec3f h;
            parser::Vec3f wio;
            parser::Vec3f diffuseShading;
            parser::Vec3f negd = negVec3(d);
            normalization(negd);
            normalization(w);
            float wiodistance; 
            float distance = sqrt(pow(light_distance.x,2) + pow(light_distance.y,2) + pow(light_distance.z,2));
            wio = addVec3(negd, w);
            wiodistance = sqrt(wio.x*wio.x+wio.y*wio.y+wio.z*wio.z);
            h  = divVec3(wio, wiodistance);
            normalization(h);
            float zero = 0;
            diffuseShading = divVec3(parallelmul(mulVec3(scene.materials[obj.material_id-1].diffuse, std::max(zero, dotProduct(w, normal)) ), scene.point_lights[i].intensity), distance*distance);
            color = addVec3(color, diffuseShading);
            specShading = divVec3(parallelmul(mulVec3(scene.materials[obj.material_id-1].specular, pow(std::max(zero, dotProduct(normal , h)), scene.materials[obj.material_id-1].phong_exponent)), scene.point_lights[i].intensity), distance*distance);
            color = addVec3(color, specShading);
        }
    }
    return color;
}

parser::Vec3f computeColor(parser::Vec3f d, int depth, parser::Scene scene, Object obj, parser::Vec3f e, std::vector<parser::Vec3f>& triangleNormals, std::vector<std::vector<parser::Vec3f>>& meshNormals){
    if(depth > scene.max_recursion_depth){
        parser::Vec3f vec;
        vec.x = 0; vec.y = 0; vec.z = 0;
        return vec;
    }
    if(RayIntersectsObject(e, d, scene, obj)){
        return applyShading(d, depth, scene, obj, e, triangleNormals, meshNormals);
    }
    else if(depth == 0){
        parser::Vec3f temp;
        temp.x = scene.background_color.x;temp.y = scene.background_color.y;temp.z = scene.background_color.z;
        return temp;
    }
    else{
        parser::Vec3f vec;
        vec.x = 0; vec.y = 0; vec.z = 0;
        return vec;
    }
}



int main(int argc, char* argv[])
{
    parser::Scene scene;
    scene.loadFromXml(argv[1]);
    std::vector<parser::Camera> cameras = scene.cameras;
    std::vector<parser::Vec3f> triangleNormals;
    calculateNormals(triangleNormals, scene);
    std::vector<std::vector<parser::Vec3f>> meshNormals(scene.meshes.size());
    calculateMeshNormals(meshNormals, scene);
    for(auto camera: cameras){
        int width = camera.image_width;
        int height = camera.image_height;
        unsigned char* image = new unsigned char [width * height * 3];
        
        #pragma omp parallel for
        for (int j = 0; j < height; ++j)
        {
            for (int i = 0; i < width; ++i)
            {
                parser::Vec3f e = camera.position;
        parser::Vec3f m, q, s, d;
        parser::Vec3f v = camera.up;
        parser::Vec3f w = negVec3(camera.gaze);
        parser::Vec3f u = crossProduct(v, w);

        m = addVec3(e, mulVec3(camera.gaze, camera.near_distance));
        
        q = addVec3(addVec3(m, mulVec3(u, camera.near_plane.x)), mulVec3(v, camera.near_plane.w));
        
        float su = (camera.near_plane.y - camera.near_plane.x)/width;
        float sv = (camera.near_plane.w - camera.near_plane.z)/height;
                s = addVec3(addVec3(q, mulVec3(u, ((i+0.5)*su))),mulVec3(v, -((j+0.5)*sv)));
                d = subVec3(s, e);
                //normalize ??
                int depth = 0;
                parser::Vec3i color;
                Object obj;
                color = clamp(computeColor(d, depth, scene, obj, e, triangleNormals, meshNormals),0 , 255);
                image[(int)3*(i+j*width)] = color.x; 
                image[(int)3*(i+j*width)+1] = color.y; 
                image[(int)3*(i+j*width)+2] = color.z;
            }
        }
        const char* str = camera.image_name.c_str();
        write_ppm(str, image, camera.image_width, camera.image_height);
    }
    return 0;

}
