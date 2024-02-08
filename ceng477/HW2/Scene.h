#ifndef SCENE_H
#define SCENE_H
#include "Vec3.h"
#include "Vec4.h"
#include "Color.h"
#include "Rotation.h"
#include "Scaling.h"
#include "Translation.h"
#include "Camera.h"
#include "Mesh.h"
#include "Matrix4.h"
class Scene
{
public:
	Color backgroundColor;
	bool cullingEnabled;

	std::vector<std::vector<Color> > image;
	std::vector<std::vector<double> > depth;
	std::vector<Camera *> cameras;
	std::vector<Vec3 *> vertices;
	std::vector<Color *> colorsOfVertices;
	std::vector<Scaling *> scalings;
	std::vector<Rotation *> rotations;
	std::vector<Translation *> translations;
	std::vector<Mesh *> meshes;

	Scene(const char *xmlPath);

	void assignColorToPixel(int i, int j, Color c);
	void initializeImage(Camera *camera);
	int makeBetweenZeroAnd255(double value);
	void writeImageToPPMFile(Camera *camera);
	void convertPPMToPNG(std::string ppmFileName);

	Vec3 findV(Vec3 u);
	Matrix4 rotate(Rotation rotation);
	Matrix4 translate(Translation translation);
	Matrix4 scale(Scaling scaling);
	Matrix4 viewing_transformation(Camera *camera);
	Vec3 calculate_normal(Triangle triangle, std::vector<Vec3*> newVertices);
	Vec3 get_center(Triangle triangle, std::vector<Vec3*> newVertices);
	Color mulColor(double s, Color c);
	Color addColor(Color c1, Color c2);
	Color subColor(Color c1, Color c2);
	bool visiblee(double den, double num, double& te, double& tl);
	bool lineclipping(Vec3& p0, Vec3& p1, Color& c0, Color& c1);
	Matrix4 viewport_transformation(Vec4 p, Camera *camera);
	double f01(int x, int y, int x0, int x1, int y0, int y1);
	double f12(int x, int y, int x1, int x2, int y1, int y2);
	double f20(int x, int y, int x2, int x0, int y2, int y0);
	void draw(int x, int y, double z, Color c, Scene* scene);
	void triangle_rasterization(Triangle T, Scene* scene, std::vector<Vec3*> newVertices, std::vector<Color *> colorsOfNewVertices);
	void line_rasterization(Vec3 p0, Vec3 p1, Color c0, Color c1, Scene* scene);

	void forwardRenderingPipeline(Camera *camera);
};

#endif