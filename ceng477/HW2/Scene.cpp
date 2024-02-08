#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <iomanip>
#include <cstring>
#include <string>
#include <vector>
#include <cmath>

#include<iostream>

#include "tinyxml2.h"
#include "Triangle.h"
#include "Helpers.h"
#include "Scene.h"

using namespace tinyxml2;
using namespace std;

/*
	Parses XML file
*/
Scene::Scene(const char *xmlPath)
{
	const char *str;
	XMLDocument xmlDoc;
	XMLElement *xmlElement;

	xmlDoc.LoadFile(xmlPath);

	XMLNode *rootNode = xmlDoc.FirstChild();

	// read background color
	xmlElement = rootNode->FirstChildElement("BackgroundColor");
	str = xmlElement->GetText();
	sscanf(str, "%lf %lf %lf", &backgroundColor.r, &backgroundColor.g, &backgroundColor.b);

	// read culling
	xmlElement = rootNode->FirstChildElement("Culling");
	if (xmlElement != NULL)
	{
		str = xmlElement->GetText();

		if (strcmp(str, "enabled") == 0)
		{
			this->cullingEnabled = true;
		}
		else
		{
			this->cullingEnabled = false;
		}
	}

	// read cameras
	xmlElement = rootNode->FirstChildElement("Cameras");
	XMLElement *camElement = xmlElement->FirstChildElement("Camera");
	XMLElement *camFieldElement;
	while (camElement != NULL)
	{
		Camera *camera = new Camera();

		camElement->QueryIntAttribute("id", &camera->cameraId);

		// read projection type
		str = camElement->Attribute("type");

		if (strcmp(str, "orthographic") == 0)
		{
			camera->projectionType = ORTOGRAPHIC_PROJECTION;
		}
		else
		{
			camera->projectionType = PERSPECTIVE_PROJECTION;
		}

		camFieldElement = camElement->FirstChildElement("Position");
		str = camFieldElement->GetText();
		sscanf(str, "%lf %lf %lf", &camera->position.x, &camera->position.y, &camera->position.z);

		camFieldElement = camElement->FirstChildElement("Gaze");
		str = camFieldElement->GetText();
		sscanf(str, "%lf %lf %lf", &camera->gaze.x, &camera->gaze.y, &camera->gaze.z);

		camFieldElement = camElement->FirstChildElement("Up");
		str = camFieldElement->GetText();
		sscanf(str, "%lf %lf %lf", &camera->v.x, &camera->v.y, &camera->v.z);

		camera->gaze = normalizeVec3(camera->gaze);
		camera->u = crossProductVec3(camera->gaze, camera->v);
		camera->u = normalizeVec3(camera->u);

		camera->w = inverseVec3(camera->gaze);
		camera->v = crossProductVec3(camera->u, camera->gaze);
		camera->v = normalizeVec3(camera->v);

		camFieldElement = camElement->FirstChildElement("ImagePlane");
		str = camFieldElement->GetText();
		sscanf(str, "%lf %lf %lf %lf %lf %lf %d %d",
			   &camera->left, &camera->right, &camera->bottom, &camera->top,
			   &camera->near, &camera->far, &camera->horRes, &camera->verRes);

		camFieldElement = camElement->FirstChildElement("OutputName");
		str = camFieldElement->GetText();
		camera->outputFilename = string(str);

		this->cameras.push_back(camera);

		camElement = camElement->NextSiblingElement("Camera");
	}

	// read vertices
	xmlElement = rootNode->FirstChildElement("Vertices");
	XMLElement *vertexElement = xmlElement->FirstChildElement("Vertex");
	int vertexId = 1;

	while (vertexElement != NULL)
	{
		Vec3 *vertex = new Vec3();
		Color *color = new Color();

		vertex->colorId = vertexId;

		str = vertexElement->Attribute("position");
		sscanf(str, "%lf %lf %lf", &vertex->x, &vertex->y, &vertex->z);

		str = vertexElement->Attribute("color");
		sscanf(str, "%lf %lf %lf", &color->r, &color->g, &color->b);

		this->vertices.push_back(vertex);
		this->colorsOfVertices.push_back(color);

		vertexElement = vertexElement->NextSiblingElement("Vertex");

		vertexId++;
	}

	// read translations
	xmlElement = rootNode->FirstChildElement("Translations");
	XMLElement *translationElement = xmlElement->FirstChildElement("Translation");
	while (translationElement != NULL)
	{
		Translation *translation = new Translation();

		translationElement->QueryIntAttribute("id", &translation->translationId);

		str = translationElement->Attribute("value");
		sscanf(str, "%lf %lf %lf", &translation->tx, &translation->ty, &translation->tz);

		this->translations.push_back(translation);

		translationElement = translationElement->NextSiblingElement("Translation");
	}

	// read scalings
	xmlElement = rootNode->FirstChildElement("Scalings");
	XMLElement *scalingElement = xmlElement->FirstChildElement("Scaling");
	while (scalingElement != NULL)
	{
		Scaling *scaling = new Scaling();

		scalingElement->QueryIntAttribute("id", &scaling->scalingId);
		str = scalingElement->Attribute("value");
		sscanf(str, "%lf %lf %lf", &scaling->sx, &scaling->sy, &scaling->sz);

		this->scalings.push_back(scaling);

		scalingElement = scalingElement->NextSiblingElement("Scaling");
	}

	// read rotations
	xmlElement = rootNode->FirstChildElement("Rotations");
	XMLElement *rotationElement = xmlElement->FirstChildElement("Rotation");
	while (rotationElement != NULL)
	{
		Rotation *rotation = new Rotation();

		rotationElement->QueryIntAttribute("id", &rotation->rotationId);
		str = rotationElement->Attribute("value");
		sscanf(str, "%lf %lf %lf %lf", &rotation->angle, &rotation->ux, &rotation->uy, &rotation->uz);

		this->rotations.push_back(rotation);

		rotationElement = rotationElement->NextSiblingElement("Rotation");
	}

	// read meshes
	xmlElement = rootNode->FirstChildElement("Meshes");

	XMLElement *meshElement = xmlElement->FirstChildElement("Mesh");
	while (meshElement != NULL)
	{
		Mesh *mesh = new Mesh();

		meshElement->QueryIntAttribute("id", &mesh->meshId);

		// read projection type
		str = meshElement->Attribute("type");

		if (strcmp(str, "wireframe") == 0)
		{
			mesh->type = WIREFRAME_MESH;
		}
		else
		{
			mesh->type = SOLID_MESH;
		}

		// read mesh transformations
		XMLElement *meshTransformationsElement = meshElement->FirstChildElement("Transformations");
		XMLElement *meshTransformationElement = meshTransformationsElement->FirstChildElement("Transformation");

		while (meshTransformationElement != NULL)
		{
			char transformationType;
			int transformationId;

			str = meshTransformationElement->GetText();
			sscanf(str, "%c %d", &transformationType, &transformationId);

			mesh->transformationTypes.push_back(transformationType);
			mesh->transformationIds.push_back(transformationId);

			meshTransformationElement = meshTransformationElement->NextSiblingElement("Transformation");
		}

		mesh->numberOfTransformations = mesh->transformationIds.size();

		// read mesh faces
		char *row;
		char *cloneStr;
		int v1, v2, v3;
		XMLElement *meshFacesElement = meshElement->FirstChildElement("Faces");
		str = meshFacesElement->GetText();
		cloneStr = strdup(str);

		row = strtok(cloneStr, "\n");
		while (row != NULL)
		{
			int result = sscanf(row, "%d %d %d", &v1, &v2, &v3);

			if (result != EOF)
			{
				mesh->triangles.push_back(Triangle(v1, v2, v3));
			}
			row = strtok(NULL, "\n");
		}
		mesh->numberOfTriangles = mesh->triangles.size();
		this->meshes.push_back(mesh);

		meshElement = meshElement->NextSiblingElement("Mesh");
	}
}

void Scene::assignColorToPixel(int i, int j, Color c)
{
	this->image[i][j].r = c.r;
	this->image[i][j].g = c.g;
	this->image[i][j].b = c.b;
}

/*
	Initializes image with background color
*/
void Scene::initializeImage(Camera *camera)
{
	if (this->image.empty())
	{
		for (int i = 0; i < camera->horRes; i++)
		{
			vector<Color> rowOfColors;
			vector<double> rowOfDepths;

			for (int j = 0; j < camera->verRes; j++)
			{
				rowOfColors.push_back(this->backgroundColor);
				rowOfDepths.push_back(1.01);
			}

			this->image.push_back(rowOfColors);
			this->depth.push_back(rowOfDepths);
		}
	}
	else
	{
		for (int i = 0; i < camera->horRes; i++)
		{
			for (int j = 0; j < camera->verRes; j++)
			{
				assignColorToPixel(i, j, this->backgroundColor);
				this->depth[i][j] = 1.01;
				this->depth[i][j] = 1.01;
				this->depth[i][j] = 1.01;
			}
		}
	}
}

/*
	If given value is less than 0, converts value to 0.
	If given value is more than 255, converts value to 255.
	Otherwise returns value itself.
*/
int Scene::makeBetweenZeroAnd255(double value)
{
	if (value >= 255.0)
		return 255;
	if (value <= 0.0)
		return 0;
	return (int)(value);
}

/*
	Writes contents of image (Color**) into a PPM file.
*/
void Scene::writeImageToPPMFile(Camera *camera)
{
	ofstream fout;

	fout.open(camera->outputFilename.c_str());

	fout << "P3" << endl;
	fout << "# " << camera->outputFilename << endl;
	fout << camera->horRes << " " << camera->verRes << endl;
	fout << "255" << endl;

	for (int j = camera->verRes - 1; j >= 0; j--)
	{
		for (int i = 0; i < camera->horRes; i++)
		{
			fout << makeBetweenZeroAnd255(this->image[i][j].r) << " "
				 << makeBetweenZeroAnd255(this->image[i][j].g) << " "
				 << makeBetweenZeroAnd255(this->image[i][j].b) << " ";
		}
		fout << endl;
	}
	fout.close();
}

/*
	Converts PPM image in given path to PNG file, by calling ImageMagick's 'convert' command.
*/
void Scene::convertPPMToPNG(string ppmFileName)
{
	string command;

	// TODO: Change implementation if necessary.
	command = "./magick convert " + ppmFileName + " " + ppmFileName + ".png";
	system(command.c_str());
}

Vec3 Scene::findV(Vec3 u){
	Vec3 v(u);
	if(abs(u.x)<abs(u.y) && abs(u.x)<abs(u.z)){
		v.x = 0;
		v.y = u.z;
		v.z = -u.y;
	} 
	else if(abs(u.y)<abs(u.z)){
		v.y = 0;
		v.x = u.z;
		v.z = -u.x;
	} 
	else{
		v.z = 0;
		v.y = u.x;
		v.x = -u.y;
	}
	return v;

}

Matrix4 Scene::rotate(Rotation rotation){
	//int idx = findmin3(rotation.ux, rotation.uy, rotation.uz);
	Vec3 u(rotation.ux, rotation.uy, rotation.uz);
	Vec3 v = findV(u);
	u=normalizeVec3(u);
	v=normalizeVec3(v);
	Vec3 w = crossProductVec3(u, v);
	double values[4][4] = {
		{u.x, u.y, u.z, 0},
		{v.x, v.y, v.z, 0},
		{w.x, w.y, w.z, 0},
		{  0,   0,   0, 1}
	};
	double valuesinv[4][4] = {
		{u.x, v.x, w.x, 0},
		{u.y, v.y, w.y, 0},
		{u.z, v.z, w.z, 0},
		{  0,   0,   0, 1}
	};
	double pi = 2*acos(0.0);
	rotation.angle *= pi/180;
	double valuesRotate[4][4] = {
		{1,                   0,                    0, 0},
		{0, cos(rotation.angle), -sin(rotation.angle), 0},
		{0, sin(rotation.angle), cos(rotation.angle), 0},
		{0,                   0,                    0, 1}
	};
	Matrix4 M(values);
	Matrix4 Minv(valuesinv);
	Matrix4 R(valuesRotate);
	Matrix4 T = multiplyMatrixWithMatrix(Minv, multiplyMatrixWithMatrix(R, M));
	return T;
}

Matrix4 Scene::translate(Translation translation){
	double values[4][4] = {
		{1, 0, 0, translation.tx},
		{0, 1, 0, translation.ty},
		{0, 0, 1, translation.tz},
		{0, 0, 0,              1}
	};
	Matrix4 T(values);
	return T;
}

Matrix4 Scene::scale(Scaling scaling){
	double values[4][4] = {
		{scaling.sx,          0,          0, 0},
		{         0, scaling.sy,          0, 0},
		{         0,          0, scaling.sz, 0},
		{         0,          0,          0, 1}
	};
	Matrix4 T(values);
	return T;
}
/*
	Transformations, clipping, culling, rasterization are done here.
*/

Matrix4 Scene::viewing_transformation(Camera *camera){
	Vec3 u = camera->u, v = camera->v, w = camera->w, e=camera->position;
	Matrix4 T;
	double camValues[4][4] = {
		{u.x, u.y, u.z, -(u.x*e.x + u.y*e.y + u.z*e.z)},
		{v.x, v.y, v.z, -(v.x*e.x + v.y*e.y + v.z*e.z)},
		{w.x, w.y, w.z, -(w.x*e.x + w.y*e.y + w.z*e.z)},
		{  0,   0,   0, 							 1}
	};
	Matrix4 Mcam(camValues);
	if(camera->projectionType == 0){ // orthographic
		double r=camera->right, l=camera->left, t=camera->top, b=camera->bottom, n=camera->near, f=camera->far;
		double orthValues[4][4] = {
			{2/(r-l),       0,        0, -((r+l)/(r-l))},
			{      0, 2/(t-b),        0, -((t+b)/(t-b))},
			{      0,       0, -2/(f-n), -((f+n)/(f-n))},
			{      0,       0,  	  0,			  1}
		};
		Matrix4 Morth(orthValues);
		T = multiplyMatrixWithMatrix(Morth, Mcam);
	}
	else{ // perspective
		double r=camera->right, l=camera->left, t=camera->top, b=camera->bottom, n=camera->near, f=camera->far;
		double perValues[4][4] = {
			{(2*n)/(r-l),           0,    (r+l)/(r-l),              0},
			{          0, (2*n)/(t-b),    (t+b)/(t-b),              0},
			{          0,           0, -((f+n)/(f-n)), (-2*f*n)/(f-n)},
			{      	   0,           0,  	       -1,			    0}
		};
		Matrix4 Mper(perValues);
		T = multiplyMatrixWithMatrix(Mper, Mcam);
	}
	return T;
}

Vec3 Scene::calculate_normal(Triangle triangle, vector<Vec3*> newVertices){
	Vec3 v1 = subtractVec3(*newVertices[triangle.vertexIds[1]], *newVertices[triangle.vertexIds[0]]);
	Vec3 v2 = subtractVec3(*newVertices[triangle.vertexIds[2]], *newVertices[triangle.vertexIds[0]]);
	Vec3 v3 = crossProductVec3(v1,v2);
	return v3;
}

Vec3 Scene::get_center(Triangle triangle, vector<Vec3*> newVertices){
	Vec3 v = addVec3(*newVertices[triangle.vertexIds[2]],addVec3(*newVertices[triangle.vertexIds[1]], *newVertices[triangle.vertexIds[0]]));
	v.x /= 3;
	v.y /= 3;
	v.z /= 3;
	return v;
}



//LOCKI KOD

Color Scene::mulColor(double s, Color c){
	Color c1;
	c1.r = c.r*s;
	c1.g = c.g*s;
	c1.b = c.b*s;
	return c1;
}

Color Scene::addColor(Color c1, Color c2){
	Color result;
	result.r = c1.r + c2.r;
	result.g = c1.g + c2.g;
	result.b = c1.b + c2.b;
	return result;
}

Color Scene::subColor(Color c1, Color c2){
	Color result;
	result.r = c1.r - c2.r;
	result.g = c1.g - c2.g;
	result.b = c1.b - c2.b;
	return result;
}

bool Scene::visiblee(double den, double num, double& te, double& tl){
	double t;
	if(den>0){
		t = num/den;
		if(t>tl){
			return false;
		}
		if(t>te){
			te = t;
		}

	}
	else if(den<0){
		t = num/den;
		if(t<te){
			return false;
		}
		if(t<tl){
			tl=t;
		}
	}
	else if(num>0){
		return false;
	}
	return true;
}

bool Scene::lineclipping(Vec3& p0, Vec3& p1, Color& c0, Color& c1){ // returns new p0 p1 as a pair
	double te=0,tl=1;
	double x0 = p0.x, oldx0=p0.x, oldy0=p0.y;
	double y0 = p0.y;
	double z0 = p0.z;
	double x1 = p1.x, oldx1=p1.x, oldy1=p1.y;
	double y1 = p1.y;
	double z1 = p1.z;
	double dx = x1-x0;
	double dy = y1-y0;
	double dz = z1-z0;
	double xmin=-1;
	double xmax=1;
	double ymin=-1;
	double ymax=1;
	double zmax=1;
	double zmin=-1;
	bool visible=false;
	pair<Vec3, Vec3> res;
	double alfa0, alfa1;
	Color newc0=c0, newc1=c1;
	if (visiblee(dx, xmin-x0, te, tl)){
		if (visiblee(-dx, x0-xmax, te, tl)){
			if (visiblee(dy, ymin-y0, te, tl)){
				if (visiblee(-dy, y0-ymax, te, tl)){
					if(visiblee(dz, zmin-z0, te, tl)){
						if(visiblee(-dz, z0-zmax, te, tl)){
							visible=true;
							if(tl<1){
								x1=x0+dx*tl;
								y1=y0+dy*tl;
								z1=z0+dz*tl;
							}
							if(te>0){
								x0=x0+dx*te;
								y0=y0+dy*te;
								z0=z0+dz*te;
							}
							if(oldx1 != oldx0){
								alfa0 = (x0-oldx0)/(oldx1-oldx0);
								newc0 = addColor(mulColor(1-alfa0, c0), mulColor(alfa0, c1));
								alfa1 = (x1-oldx0)/(oldx1-oldx0);
								newc1 = addColor(mulColor(1-alfa1, c0), mulColor(alfa1, c1));}
							else{
								alfa0 = (y0-oldy0)/(oldy1-oldy0);
								newc0 = addColor(mulColor(1-alfa0, c0), mulColor(alfa0, c1));
								alfa1 = (y1-oldy0)/(oldy1-oldy0);
								newc1 = addColor(mulColor(1-alfa1, c0), mulColor(alfa1, c1));
							}
						}
					}
					
				}
			}
		}
	}
	p0.x=x0;
	p0.y=y0;
	p0.z=z0;
	p1.x=x1;
	p1.y=y1;
	p1.z=z1;
	c0 = newc0;
	c1 = newc1;
	return visible;
}

Matrix4 Scene::viewport_transformation(Vec4 p, Camera *camera){
	double nx = camera->horRes;
	double ny = camera->verRes;
	double mvp[4][4] = {
		{     nx/2,       0,       0,  (nx-1)/2},
		{        0,    ny/2,       0,  (ny-1)/2},
		{        0,       0,     0.5,       0.5},
		{        0,       0,       0,         0},
	};
	Matrix4 Matrixvp(mvp);
	return Matrixvp;
}

double Scene::f01(int x, int y, int x0, int x1, int y0, int y1){
	double result;
	result = x*(y0-y1)+y*(x1-x0)+x0*y1-y0*x1;
	return result;
}

double Scene::f12(int x, int y, int x1, int x2, int y1, int y2){
	double result;
	result = x*(y1-y2)+y*(x2-x1)+x1*y2-y1*x2;
	return result;
}

double Scene::f20(int x, int y, int x2, int x0, int y2, int y0){
	double result;
	result = x*(y2-y0)+y*(x0-x2)+x2*y0-y2*x0;
	return result;
}

void Scene::draw(int x, int y, double z, Color c, Scene* scene){
	if(z < scene->depth[x][y] && z>=0 && z<=1){
		scene->depth[x][y] = z;
		c.r = makeBetweenZeroAnd255(c.r);
		c.g = makeBetweenZeroAnd255(c.g);
		c.b = makeBetweenZeroAnd255(c.b);
		assignColorToPixel(x,y,c);
	}
}


void Scene::triangle_rasterization(Triangle T, Scene* scene, vector<Vec3*> newVertices, std::vector<Color *> colorsOfNewVertices){
	double xmin,xmax,ymin,ymax;
	double x0,y0,z0,x1,y1,z1,x2,y2,z2;
	double z;
	double a,b,g,fnc12,fnc20,fnc01;
	x0 = newVertices[T.vertexIds[0]]->x;
	y0 = newVertices[T.vertexIds[0]]->y;
	z0 = newVertices[T.vertexIds[0]]->z;
	x1 = newVertices[T.vertexIds[1]]->x;
	y1 = newVertices[T.vertexIds[1]]->y;
	z1 = newVertices[T.vertexIds[1]]->z;
	x2 = newVertices[T.vertexIds[2]]->x;
	y2 = newVertices[T.vertexIds[2]]->y;
	z2 = newVertices[T.vertexIds[2]]->z;
	Color c0(*colorsOfNewVertices[T.vertexIds[0]]);
	Color c1(*colorsOfNewVertices[T.vertexIds[1]]);
	Color c2(*colorsOfNewVertices[T.vertexIds[2]]);

	Color c;
	if(x0 <= x1 && x0 <= x2) xmin = x0;
	else if(x1 <= x0 && x1 <= x2) xmin = x1;
	else if(x2 <= x0 && x2 <= x1) xmin = x2;

	if(y0 <= y1 && y0 <= y2) ymin = y0;
	else if(y1 <= y0 && y1 <= y2) ymin = y1;
	else if(y2 <= y0 && y2 <= y1) ymin = y2;	

	if(x0 >= x1 && x0 >= x2) xmax = x0;
	else if(x1 >= x0 && x1 >= x2) xmax = x1;
	else if(x2 >= x0 && x2 >= x1) xmax = x2;

	if(y0 >= y1 && y0 >=y2) ymax = y0;
	else if(y1 >= y0 && y1 >= y2) ymax = y1;
	else if(y2 >= y0 && y2 >= y1) ymax = y2;

	fnc01 = f01(x2, y2, x0, x1, y0, y1);
	fnc12 = f12(x0, y0, x1, x2, y1, y2);
	fnc20 = f20(x1, y1, x2, x0, y2, y0);
	for(int y=ymin;y<=ymax;y++){
		for(int x=xmin;x<=xmax;x++){
			a = f12(x, y, x1, x2, y1, y2)/fnc12;
			b = f20(x, y, x2, x0, y2, y0)/fnc20;
			g = f01(x, y, x0, x1, y0, y1)/fnc01;
			if(a >= 0 && b >= 0 && g >= 0){
				c = addColor(addColor(mulColor(a,c0),mulColor(b,c1)),mulColor(g,c2));
				z = a*z0+b*z1+g*z2;
				//if(x>1000) cout << x << " " << y << " " << z << " " << xmax << "\n";
				draw(x,y,z,c,scene);
			}
		}
	}	
}


void Scene::line_rasterization(Vec3 p0, Vec3 p1, Color c0, Color c1, Scene* scene){
	double x0=p0.x, y0=p0.y;
	double x1=p1.x, y1=p1.y;
	double z0=p0.z,z1=p1.z, z = z0;
	if(x1 < x0){
			double temp=x0;
			double temp1=y0;
			double temp2=z0;
			x0 = x1;
			y0 = y1;
			z0 = z1;
			y1 = temp1;
			x1 = temp;
			z1 = temp2;
			Color c = c0;
			c0.r = c1.r;
			c0.g = c1.g;
			c0.b = c1.b;
			c1.r = c.r;
			c1.g = c.g;
			c1.b = c.b;

		}
	if(x1 == x0){
		if(y1 < y0){
			double temp1=y0;
			double temp2=z0;
			y0 = y1;
			z0 = z1;
			y1 = temp1;
			z1 = temp2;
			Color c = c0;
			c0.r = c1.r;
			c0.g = c1.g;
			c0.b = c1.b;
			c1.r = c.r;
			c1.g = c.g;
			c1.b = c.b;
		}
		Color c=c0;
		Color dc = mulColor(1.0/(y1-y0),subColor(c1,c0));
		for(int y=y0;y<=y1;y++){
			draw(x0,y,z,c,scene);
			z += (z1-z0)/(y1-y0);
			c = addColor(c,dc);
		}
	}
	else{
		if((y1-y0)/(x1-x0) <= 1 && (y1-y0)/(x1-x0) >=0){
			//std::cout<< "(" <<x0 << " " << y0 << " " << z0 << ")" << " , " << "(" << x1 << " " << y1 << " " << z1 << ")" << std::endl;
			double d = (y0-y1)+0.5*(x1-x0);
			Color c=c0;
			Color dc = mulColor(1.0/(x1-x0),subColor(c1,c0));
			double y=y0,z = z0;
			for(int x=x0;x<=x1;x++){
				draw(x,y,z,c,scene);
				z += (z1-z0)/(x1-x0);
				if(d<0){
					y++;
					d += (y0-y1)+(x1-x0);
				}
				else{
					d += (y0-y1);
				}
				c = addColor(c,dc);
			}
		}
		else if ((y1-y0)/(x1-x0) > 1){
			if(y1 < y0){
			double temp=x0;
			double temp1=y0;
			double temp2=z0;
			x0 = x1;
			y0 = y1;
			z0 = z1;
			y1 = temp1;
			x1 = temp;
			z1 = temp2;
		}
			double d = (x0-x1)+0.5*(y1-y0);
			Color c=c0;
			Color dc = mulColor(1.0/(y1-y0),subColor(c1,c0));
			double x=x0,z = z0;
			for(int y=y0;y<=y1;y++){
				draw(x,y,z,c,scene);
				z += (z1-z0)/(y1-y0);
				if(d<0){
					x++;
					d += (x0-x1)+(y1-y0);
				}
				else{
					d += (x0-x1);
				}
				c = addColor(c,dc);
			}
		}
		else if(-(y1-y0)/(x1-x0) <= 1 && -(y1-y0)/(x1-x0) >=0){
			double real_y0 = y0;
			double real_y1 = y1;
			y0=-y0;
			y1=-y1;
			double d = (y0-y1)+0.5*(x1-x0);
			Color c=c0;
			Color dc = mulColor(1.0/(x1-x0),subColor(c1,c0));
			double y=y0,z = z0;
			for(int x=x0;x<=x1;x++){
				draw(x,-y,z,c,scene);
				z += (z1-z0)/(x1-x0);
				if(d<0){
					y++;
					d += (y0-y1)+(x1-x0);
				}
				else{
					d += (y0-y1);
				}
				c = addColor(c,dc);
			}
		}
		else if (-(y1-y0)/(x1-x0) > 1){
			y1=-y1;
			y0=-y0;
			if(y1 < y0){
			double temp=x0;
			double temp1=y0;
			double temp2=z0;
			x0 = x1;
			y0 = y1;
			z0 = z1;
			y1 = temp1;
			x1 = temp;
			z1 = temp2;
			Color c = c0;
			c0.r = c1.r;
			c0.g = c1.g;
			c0.b = c1.b;
			c1.r = c.r;
			c1.g = c.g;
			c1.b = c.b;
		}
			double d = (x0-x1)+0.5*(y1-y0);
			Color c=c0;
			Color dc = mulColor(1.0/(y1-y0),subColor(c1,c0));
			double x=x0,z = z0;
			for(int y=y0;y<=y1;y++){
				draw(x,-y,z,c,scene);
				z += (z1-z0)/(y1-y0);
				if(d<0){
					x++;
					d += (x0-x1)+(y1-y0);
				}
				else{
					d += (x0-x1);
				}
				c = addColor(c,dc);
			}
		}
		
		
	}
}


void Scene::forwardRenderingPipeline(Camera *camera)
{
	// TODO: Implement this function
	vector<Mesh*> transformed_meshes(meshes.size());
	for(int i=0; i<meshes.size(); i++){
		transformed_meshes[i] = new Mesh(*meshes[i]); 
	}
	vector<Vec3*> newVertices;
	std::vector<Color *> colorsOfNewVertices;
	int a = 0;
	for(int i=0; i<meshes.size(); i++){
		Mesh* mesh = meshes[i];
		vector<Triangle> newTriangles;
		
		for(int j=0; j<mesh->numberOfTriangles; j++){
			for(int idx: mesh->triangles[j].vertexIds){
				Vec3 point3(*vertices[idx-1]);
				Vec4 point4(point3.x, point3.y, point3.z, 1, point3.colorId);
				for(int k=0; k<mesh->numberOfTransformations; k++){
					Matrix4 transformMatrix;
					if(mesh->transformationTypes[k] == 'r'){
						transformMatrix = rotate(*rotations[mesh->transformationIds[k]-1]);
					}
					else if(mesh->transformationTypes[k] == 't'){
						transformMatrix = translate(*translations[mesh->transformationIds[k]-1]);
					}
					else if(mesh->transformationTypes[k] == 's'){
						transformMatrix = scale(*scalings[mesh->transformationIds[k]-1]);
					}
					point4 = multiplyMatrixWithVec4(transformMatrix, point4);
				}
				
				Vec3* ptr = new Vec3(point4.x, point4.y, point4.z, point4.colorId);
				newVertices.push_back(ptr);
				colorsOfNewVertices.push_back(colorsOfVertices[idx-1]);
			}
			Triangle newTriangle(a, a+1, a+2);
			a+=3;
			newTriangles.push_back(newTriangle);
		}
		transformed_meshes[i]->triangles = newTriangles;
		transformed_meshes[i]->numberOfTriangles = newTriangles.size();
		
	}


	for(vector<double> arr: depth){
		for(double num: arr)	
			num=1;
	}

	for(Mesh* mesh: transformed_meshes){
		for(Triangle triangle: mesh->triangles){
			if(cullingEnabled){
				//Orthographic bilmeyen  || (dotProductVec3(calculate_normal(triangle, newVertices), camera->gaze) > 0)
				if(camera->projectionType==1 && (dotProductVec3(calculate_normal(triangle, newVertices), subtractVec3(get_center(triangle, newVertices), camera->position)) > 0)){
					continue;
				}
				//if(camera->projectionType==0 && (dotProductVec3(calculate_normal(triangle, newVertices), inverseVec3(camera->gaze)) > 0)){
				//	continue;
				//}
			}
			//ortographic or perspective
			for(int id: triangle.vertexIds){
				Vec4 point(newVertices[id]->x, newVertices[id]->y, newVertices[id]->z, 1, newVertices[id]->colorId);
				point = multiplyMatrixWithVec4(viewing_transformation(camera), point);
				if(camera->projectionType==1){ //perspective divide
					point.x /= point.t;
					point.y /= point.t;
					point.z /= point.t;
				}
				newVertices[id]->x = point.x;
				newVertices[id]->y = point.y;
				newVertices[id]->z = point.z;
			}																						
			if(cullingEnabled && camera->projectionType==0 && (dotProductVec3(calculate_normal(triangle, newVertices), get_center(triangle, newVertices)) < 0)){
				continue;
			}
			//clipping
			//vector<pair<Vec3, Vec3>> lines;
			//vector<pair<Color, Color>> colorOfLines;

			//pair<Vec3, Vec3> l1, l2, l3;
			if(mesh->type == 0){ //wireframe
				//linelari clipleyip yeni bi listeye koy sonra o linelari rasterize et
				vector<Vec3> points(6);
				points[0] = *newVertices[triangle.vertexIds[0]];
				points[1] = *newVertices[triangle.vertexIds[1]];
				points[2] = *newVertices[triangle.vertexIds[1]];
				points[3] = *newVertices[triangle.vertexIds[2]];
				points[4] = *newVertices[triangle.vertexIds[2]];
				points[5] = *newVertices[triangle.vertexIds[0]];
				vector<Color> colors(6);
				colors[0] = *colorsOfNewVertices[triangle.vertexIds[0]];
				colors[1] = *colorsOfNewVertices[triangle.vertexIds[1]];
				colors[2] = *colorsOfNewVertices[triangle.vertexIds[1]];
				colors[3] = *colorsOfNewVertices[triangle.vertexIds[2]];
				colors[4] = *colorsOfNewVertices[triangle.vertexIds[2]];
				colors[5] = *colorsOfNewVertices[triangle.vertexIds[0]];
				bool line1 = lineclipping(points[0], points[1], colors[0], colors[1]);
				bool line2 = lineclipping(points[2], points[3], colors[2], colors[3]);
				bool line3 = lineclipping(points[4], points[5], colors[4], colors[5]);
				
				//viewport
				for(int i=0; i<6; i++){
					Vec4 point(points[i].x, points[i].y, points[i].z, 1);
					point = multiplyMatrixWithVec4(viewport_transformation(point, camera), point);
					points[i].x = point.x;
					points[i].y = point.y;
					points[i].z = point.z;
				}
				//rasterization
				if(line1) line_rasterization(points[0], points[1], colors[0], colors[1], this);
				if(line2) line_rasterization(points[2], points[3], colors[2], colors[3], this);
				if(line3) line_rasterization(points[4], points[5], colors[4], colors[5], this);
				
			}
			else{
				bool flag=false;
				
				for(int id: triangle.vertexIds){
					Vec4 point(newVertices[id]->x, newVertices[id]->y, newVertices[id]->z, 1);
					point = multiplyMatrixWithVec4(viewport_transformation(point, camera), point);
					newVertices[id]->x = point.x;
					newVertices[id]->y = point.y;
					newVertices[id]->z = point.z;
					if(newVertices[id]->x < 0 || newVertices[id]->x > camera->horRes || newVertices[id]->y < 0 || newVertices[id]->y > camera->verRes)
						flag = true;
				}
				
				if(flag) continue;
				
				triangle_rasterization(triangle, this, newVertices, colorsOfNewVertices);
			}

			//viewport: line ve vertexleri for ile bundan gecir

			//rasterization: triangle ve line icin olan fonksiyonlari cagir

		}
	}

}


