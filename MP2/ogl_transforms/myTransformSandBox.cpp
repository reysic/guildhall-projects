
#include ".\mytransformsandbox.h"

#define STRICT
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <mmsystem.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include "geometry.h"
#include <cmath>
#include <utility> //swap

using namespace simpleSceneGraph;

TransformNode* sunTransform;
TransformNode* sunRotationNode;
TransformNode* earthTransform;
TransformNode* earthTransformRotate;
TransformNode* moonTransformNode;
TransformNode* moonTransformRotate;
BillboardNode* bbTransformNode;

TranslateStrategy* translateStrategyEarth;
TranslateStrategy* translateStrategyMoon;


float g_angle = 90.0;

myTransformSandBox::myTransformSandBox(void) : _root(nullptr), _trav(nullptr), cameraAngleDisplacement(0.5f)
{
	_root = new Node("root");

	
	//first branch
	sunTransform = new TransformNode("sunTransform");
	_root->addChild(sunTransform);
	sunRotationNode = new TransformNode("sunRotationNode");
	sunTransform->addChild(sunRotationNode);
	RotateStrategy* rotateStrategy = new RotateStrategy(90.0, 0.0, 0.0);
	sunRotationNode->setStrategy(rotateStrategy);

	GeometryNode* geom1 = new GeometryNode("geom1", GeometryNode::Sphere, 5.0, 2);
	sunRotationNode->addChild(geom1);

	
	earthTransform = new TransformNode("earthTransform");
	sunTransform->addChild(earthTransform);
	translateStrategyEarth = new TranslateStrategy(0.0, 10.0, 10.0); //orbit
	earthTransform->setStrategy(translateStrategyEarth);

	earthTransformRotate = new TransformNode("earthTransformRotate");
	earthTransform->addChild(earthTransformRotate);
	RotateStrategy* earthRotate = new RotateStrategy(90.0, 0.0, g_angle);
	earthTransformRotate->setStrategy(earthRotate);

	GeometryNode* geomEarf = new GeometryNode("geomEarf", GeometryNode::Sphere, 3.0);
	earthTransformRotate->addChild(geomEarf);

	moonTransformNode = new TransformNode("moonTransformNode");
	earthTransform->addChild(moonTransformNode);
	translateStrategyMoon = new TranslateStrategy(0.0, -4.0, 5.0);
	moonTransformNode->setStrategy(translateStrategyMoon);

	moonTransformRotate = new TransformNode("moonTransformRotate");
	moonTransformNode->addChild(moonTransformRotate);
	RotateStrategy* moonRotationStrategy = new RotateStrategy(0.0, 90.0, 0.0);
	moonTransformRotate->setStrategy(moonRotationStrategy);

	GeometryNode* geomMoon = new GeometryNode("geomMoon", GeometryNode::Sphere, 1.5, 3);
	moonTransformRotate->addChild(geomMoon);

	bbTransformNode = new BillboardNode("bbTransformNode");
	_root->addChild(bbTransformNode);

	GeometryNode* geomBb = new GeometryNode("geomBb", GeometryNode::Teapot, 1.5);
	bbTransformNode->addChild(geomBb);
	
	
	std::vector<NavigationPoint> navPointsForTeapot;
	navPointsForTeapot.push_back(NavigationPoint(Vector3(5.f, 0.f, 0.f), AxisAngle(0.f, Vector3(0.f, 0.f, 1.f))));
	navPointsForTeapot.push_back(NavigationPoint(Vector3(0.f, 0.f, -5.f), AxisAngle(M_PI_2, Vector3(0.f, 1.f, 0.f))));
	navPointsForTeapot.push_back(NavigationPoint(Vector3(-5.f, 0.f, 0.f), AxisAngle(M_PI, Vector3(0.f, 1.f, 0.f))));
	navPointsForTeapot.push_back(NavigationPoint(Vector3(0.f, 0.f, 5.f), AxisAngle(-M_PI_2, Vector3(0.f, 1.f, 0.f))));

	//T1
	TransformNode* teapotTransform = new TransformNode("teapotNavigator");
	_root->addChild(teapotTransform);
	NavigatorStrategy* navStrategy = new NavigatorStrategy(navPointsForTeapot);
	teapotTransform->setStrategy(navStrategy);

	//G1
	GeometryNode* teapot = new GeometryNode("teapot", GeometryNode::Teapot, 3.f);
	teapotTransform->addChild(teapot);

	_trav = new Node::Traversal();
	_trav->setCameraFrustumPlanes(63.0, 45.0, 0.1, 100.0);//see gluPerspective( 45.0, (GLdouble)nWidth / (GLdouble)nHeight, 0.1, 100.0);
	//_trav->setCameraFrustumPlanes(1000.0, 1000.0, 0.1, 100000.0);//see gluPerspective( 45.0, (GLdouble)nWidth / (GLdouble)nHeight, 0.1, 100.0);

}

myTransformSandBox::~myTransformSandBox(void)
{
	delete _trav;
	delete _root;
	Node::checkNodeCount();
}

void myTransformSandBox::renderAxis(float lenght)
{
	//Use Red, Green, Blue for X Y Z respectively
	glLineWidth(3.0f);
	glBegin(GL_LINES);
	//X axis
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(lenght, 0.0f, 0.0f);
	//Y axis
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, lenght, 0.0f);
	//Z axis
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, lenght);
	glEnd();
}

/*
ROW-major memory layout: from float[16] to matrix
0  1  2  3   |  0  1  2  3
4  5  6  7   |  4  5  6  7
8  9  10 11  |  8  9  10 11
12 13 14 15  |  12 13 14 15

Column-major:
0 4 8  12
1 5 9  13
2 6 10 14
3 7 11 15
*/
static void matrixMult(const float* lhs, const float* rhs, float* result)
{
	//assume that the arguments are valid float[16] buffers
	result[0] = lhs[0] * rhs[0] + lhs[1] * rhs[4] + lhs[2] * rhs[8] + lhs[3] * rhs[12];
	result[1] = lhs[0] * rhs[1] + lhs[1] * rhs[5] + lhs[2] * rhs[9] + lhs[3] * rhs[13];
	result[2] = lhs[0] * rhs[2] + lhs[1] * rhs[6] + lhs[2] * rhs[10] + lhs[3] * rhs[14];
	result[3] = lhs[0] * rhs[3] + lhs[1] * rhs[7] + lhs[2] * rhs[11] + lhs[3] * rhs[15];

	result[4] = lhs[4] * rhs[0] + lhs[5] * rhs[4] + lhs[6] * rhs[8] + lhs[7] * rhs[12];
	result[5] = lhs[4] * rhs[1] + lhs[5] * rhs[5] + lhs[6] * rhs[9] + lhs[7] * rhs[13];
	result[6] = lhs[4] * rhs[2] + lhs[5] * rhs[6] + lhs[6] * rhs[10] + lhs[7] * rhs[14];
	result[7] = lhs[4] * rhs[3] + lhs[5] * rhs[7] + lhs[6] * rhs[11] + lhs[7] * rhs[15];

	result[8] = lhs[8] * rhs[0] + lhs[9] * rhs[4] + lhs[10] * rhs[8] + lhs[11] * rhs[12];
	result[9] = lhs[8] * rhs[1] + lhs[9] * rhs[5] + lhs[10] * rhs[9] + lhs[11] * rhs[13];
	result[10] = lhs[8] * rhs[2] + lhs[9] * rhs[6] + lhs[10] * rhs[10] + lhs[11] * rhs[14];
	result[11] = lhs[8] * rhs[3] + lhs[9] * rhs[7] + lhs[10] * rhs[11] + lhs[11] * rhs[15];

	result[12] = lhs[12] * rhs[0] + lhs[13] * rhs[4] + lhs[14] * rhs[8] + lhs[15] * rhs[12];
	result[13] = lhs[12] * rhs[1] + lhs[13] * rhs[5] + lhs[14] * rhs[9] + lhs[15] * rhs[13];
	result[14] = lhs[12] * rhs[2] + lhs[13] * rhs[6] + lhs[14] * rhs[10] + lhs[15] * rhs[14];
	result[15] = lhs[12] * rhs[3] + lhs[13] * rhs[7] + lhs[14] * rhs[11] + lhs[15] * rhs[15];
}

void matrixTranspose(float* mat)
{
	//in-place transpose
	std::swap(mat[1], mat[4]);
	std::swap(mat[2], mat[8]);
	std::swap(mat[3], mat[12]);
	std::swap(mat[6], mat[9]);
	std::swap(mat[7], mat[13]);
	std::swap(mat[11], mat[14]);
}

int g_frameCount;

void myTransformSandBox::CrossProduct(const float* a, const float* b, float* c)
{
	c[0] = a[1] * b[2] - b[1] * a[2];
	c[1] = a[2] * b[0] - b[2] * a[0];
	c[2] = a[0] * b[1] - b[0] * a[1];
}

void myTransformSandBox::CopyArray(const float* a, float* b)
{
	b[0] = a[0];
	b[1] = a[1];
	b[2] = a[2];
	b[3] = a[3];

	b[4] = a[4];
	b[5] = a[5];
	b[6] = a[6];
	b[7] = a[7];

	b[8] = a[8];
	b[9] = a[9];
	b[10] = a[10];
	b[11] = a[11];

	b[12] = a[12];
	b[13] = a[13];
	b[14] = a[14];
	b[15] = a[15];
}


void myTransformSandBox::render()
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	cameraAngleDisplacement -= 0.001f;
	float angle = -1.14159f*0.1f;
	float earthOrbitRadius = 14.f;
	float earthOrbitSpeed = 0.08f;
	float moonOrbitRadius = 6.f;
	float moonOrbitSpeed = 0.10f;
	float bbOrbitRadius = 18.f;
	float bbOrbitSpeed = 0.16f;

	//All matrices are ROW (C++) major

	g_frameCount++;

	float Wup[3] = {
		0.f, 1.f, 0.f
	};

	//TranslateStrategy* earthOrbit = new TranslateStrategy(0.0,0.0,0.0);
	TranslateStrategy* earthOrbit = new TranslateStrategy(earthOrbitRadius * cos(earthOrbitSpeed * g_angle), 0.0, earthOrbitRadius * sin(earthOrbitSpeed * g_angle));
	earthTransform->setStrategy(earthOrbit);

	TranslateStrategy* moonOrbit = new TranslateStrategy( moonOrbitRadius * cos( moonOrbitSpeed * g_angle ), moonOrbitRadius * sin( moonOrbitSpeed * g_angle ), 0.0 );
	moonTransformNode->setStrategy(moonOrbit);

	TranslateStrategyBB* bbOrbit = new TranslateStrategyBB(bbOrbitRadius * cos(bbOrbitSpeed * g_angle), 0.0, bbOrbitRadius * sin(bbOrbitSpeed * g_angle));
	bbTransformNode->setStrategy(bbOrbit);

	g_angle += 0.05f;
	RotateStrategy* rotateStrategy = new RotateStrategy(90.0, 0.0, g_angle);
	sunRotationNode->setStrategy(rotateStrategy);
	RotateStrategy* rotateStrategy2 = new RotateStrategy(90.0, 0.0, -g_angle * 5);
	earthTransformRotate->setStrategy(rotateStrategy2);
	RotateStrategy* rotateStrategy3 = new RotateStrategy(90.0, 0.0, g_angle * 2);
	moonTransformRotate->setStrategy(rotateStrategy3);
	// 	float Tc[] = { 1.0f, 0.0f, 0.0f, 0.0f,
	// 		 				0.0f, 1.0f, 0.0f, 0.0f,
	// 		 				0.0f, 0.0f, 1.0f, 0.0f,
	// 		 				0.f, 0.f, 0.0f, 1.0f };//Camera position in the world

	//Camera position in the World
	float Tc[] = { 1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.f, 5.f, 40.0f, 1.0f };//Camera position in the world

	if (g_lookAt)
	{
		float TcLookAt[] = { 1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			earthOrbit->m_x, earthOrbit->m_y, earthOrbit->m_z, 1.0f };//Camera position in the world

		//CopyArray(TcLookAt, Tc);
		_root->update();
		_trav->initToViewMatrix(matrix());
		_root->updateModels(_trav);

		CopyArray(moonTransformNode->GetModel()._m, Tc);
		
		CopyArray(Tc, SuperTc);
		float RcLookAt[16];
		LookAt(moonTransformNode, bbTransformNode, RcLookAt);
		CopyArray(RcLookAt, SuperRc);
	}
	else if (g_godEye)
	{
		_root->update();
		_trav->initToViewMatrix(matrix());
		//_root->updateModels(_trav);

		//CopyArray(earthTransform->GetModel()._m, Tc);
		matrix superMatrix = matrix(Tc);
		CopyArray(Tc, SuperTc);
		float Rc[] = { 1.f, 0.f, 0.f, 0.0f,
			0.f, 1.f, 0.f, 0.0f,
			0.f, 0.f, 1.f, 0.0f,
			0.0f, .0f, 0.0f, 1.0f };
		CopyArray(Rc, SuperRc);
	}

	// 	float Tc[] = { 1.0f, 0.0f, 0.0f, 0.0f,
	// 		0.0f, 1.0f, 0.0f, 0.0f,
	// 		0.0f, 0.0f, 1.0f, 0.0f,
	// 		75.0f * cos(g_frameCount * (3.14159 / 180)), 75.0f * sin(g_frameCount * (3.14159 / 180)), 40.0f, 1.0f };//Camera position in the world


	float Tteay[] = { 1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		25.0f * cos(cameraAngleDisplacement * 5.f), 0.0f, 25.0f * sin(cameraAngleDisplacement * 5.f), 1.0f };

	// 	float Kbb[3] = {
	// 		Tc[12] - Tteay[12], Tc[13] - Tteay[13], Tc[14] - Tteay[14]
	// 	};
	// 
	// 	float KbbNorm[3];
	// 
	// 	Normalize(Kbb, KbbNorm);
	// 
	// 	float Ibb[3];
	// 
	// 	CrossProduct(Wup, KbbNorm, Ibb);
	// 
	// 	float IbbNorm[3];
	// 
	// 	Normalize(Ibb, IbbNorm);
	// 
	// 	float JbbNorm[3];
	// 
	// 	CrossProduct(KbbNorm, IbbNorm, JbbNorm);
	// 
	// 	float Rteay[] = { IbbNorm[0], IbbNorm[1], IbbNorm[2], 0.0f,
	// 		JbbNorm[0], JbbNorm[1], JbbNorm[2], 0.0f,
	// 		KbbNorm[0], KbbNorm[1], KbbNorm[2], 0.0f,
	// 		0.0f, 0.0f, 0.0f, 1.0f };

	float Rteay[] = {1.f, 0.f, 0.f, 0.0f,
		0.f, 1.f, 0.f, 0.0f,
		0.f, 0.f, 1.f, 0.0f,
		0.0f, .0f, 0.0f, 1.0f};

	//Look at

	float Kc[3] = {
		Tc[12] - Tteay[12], Tc[13] - Tteay[13], Tc[14] - Tteay[14]
	};

	float KcNorm[3];

	Normalize(Kc, KcNorm);

	float Xc[3] = {
		0.f, 0.f, 0.f
	};

	CrossProduct(Wup, KcNorm, Xc);

	float XcNorm[3];

	Normalize(Xc, XcNorm);

	float YcNorm[3];

	CrossProduct(KcNorm, XcNorm, YcNorm);

	//Camera orientation (basis) in the world. 
	//Note that using the canonical space rotation about X matrix (row-major!) works out.
	float Rc[] = { 1.f, 0.f, 0.f, 0.0f,
				0.f, 1.f, 0.f, 0.0f,
				0.f, 0.f, 1.f, 0.0f,
				0.0f, .0f, 0.0f, 1.0f };

//  	float Rc[] = { XcNorm[0], XcNorm[1], XcNorm[2], 0.0f,
//  		YcNorm[0], YcNorm[1], YcNorm[2], 0.0f,
//  		KcNorm[0], KcNorm[1], KcNorm[2], 0.0f,
//  		0.0f, 0.0f, 0.0f, 1.0f };

	//Rc*Tc is Camera->World. We need World->Camera (its inverse)
	//Inverse is transpose for orthogonal
	matrixTranspose(SuperRc);
	//Inverse for translation is negation
	SuperTc[12] = -SuperTc[12];
	SuperTc[13] = -SuperTc[13];
	SuperTc[14] = -SuperTc[14];
	// (Rc*Tc).inverse = Tc.inverse * Rc.transpose
	float view[16];
//	matrixMult(Tc, Rc, view);
	matrixMult(SuperTc, SuperRc, view);

	glLoadMatrixf(view);
	renderAxis(10.0f);

	float Mty[16];
	matrixMult(Rteay, Tteay, Mty);

	float modelView[16];
	matrixMult(Mty, view, modelView);
	glLoadMatrixf(modelView);
	glColor3f(sin(cameraAngleDisplacement * 10 + 4), sin(cameraAngleDisplacement * 10 + 0), sin(cameraAngleDisplacement * 10 + 2));
	renderWireTeapot(3.0);



	// 
	// 	float Tteax[] = { 1.0f, 0.0f, 0.0f, 0.0f,
	// 		0.0f, 1.0f, 0.0f, 0.0f,
	// 		0.0f, 0.0f, 1.0f, 0.0f,
	// 		 10.0f, 5.0f * cos(cameraAngleDisplacement * 5.f), 5.0f * sin(cameraAngleDisplacement * 5.f),  1.0f };//Camera position in the world
	// 
	// 	float Rteax[] = { 1.0f, 0.0f, 0.0f, 0.0f,
	// 		0.0f, cos(angle + cameraAngleDisplacement), sin(angle + cameraAngleDisplacement), 0.0f,
	// 		0.0f, -sin(angle + cameraAngleDisplacement), cos(angle + cameraAngleDisplacement), 0.0f,
	// 		0.0f, 0.0f, 0.0f, 1.0f };
	// 	float Mtx[16];
	// 	matrixMult(Rteax, Tteax, Mtx);
	// 
	// 	matrixMult(Mtx, view, modelView);
	// 	glLoadMatrixf(modelView);
	// 	glColor3f(sin(cameraAngleDisplacement * 10 + 0), sin(cameraAngleDisplacement * 10 + 2), sin(cameraAngleDisplacement * 10 + 4));
	// 	renderWireTeapot(3.0);
	// 
	// 
	// 	float Tteaz[] = { 1.0f, 0.0f, 0.0f, 0.0f,
	// 		0.0f, 1.0f, 0.0f, 0.0f,
	// 		0.0f, 0.0f, 1.0f, 0.0f,
	// 		5.0f * cos(cameraAngleDisplacement * 5.f), 5.0f * sin(cameraAngleDisplacement * 5.f), 10.0f, 1.0f };//Camera position in the world
	// 
	// 	float Rteaz[] = { cos(angle + cameraAngleDisplacement), -sin(angle + cameraAngleDisplacement),  0.0f, 0.0f,
	// 		sin(angle + cameraAngleDisplacement), cos(angle + cameraAngleDisplacement), 0.0f, 0.0f,
	// 		 0.0f, 0.0f, 1.0f, 0.0f,
	// 		0.0f, 0.0f, 0.0f, 1.0f };
	// 	float Mtz[16];
	// 	matrixMult(Rteaz, Tteaz, Mtz);
	// 
	// 	matrixMult(Mtz, view, modelView);
	// 	glLoadMatrixf(modelView);
	// 	glColor3f(sin(cameraAngleDisplacement * 10 + 2), sin(cameraAngleDisplacement * 10 + 4), sin(cameraAngleDisplacement * 10 + 0));
	// 	renderWireTeapot(3.0);
	// 
	// 
	// 	float Ttea2[] = { 1.0f, 0.0f, 0.0f, 0.0f,
	// 		0.0f, 1.0f, 0.0f, 0.0f,
	// 		0.0f, 0.0f, 1.0f, 0.0f,
	// 		4.0f * cos(cameraAngleDisplacement * 10.f), 0.0f, 4.0f * sin(cameraAngleDisplacement * 10.f), 1.0f};//Camera position in the world
	// 
	// 	float T2[16];
	// 	matrixMult(Ttea2, Tteaz, T2);
	// 
	// 	matrixMult(T2, view, modelView);
	// 	glLoadMatrixf(modelView);
	// 	glColor3f(sin(cameraAngleDisplacement * 21 + 0), sin(cameraAngleDisplacement * 21 + 2), sin(cameraAngleDisplacement * 21 + 4));
	// 	renderWireTeapot(1.0);
	// 
	// 	float Ttea4[] = { 1.0f, 0.0f, 0.0f, 0.0f,
	// 		0.0f, 1.0f, 0.0f, 0.0f,
	// 		0.0f, 0.0f, 1.0f, 0.0f,
	// 		0.0f, 3.0f * cos(cameraAngleDisplacement * 20.f), 3.0f * sin(cameraAngleDisplacement * 20.f),  1.0f };//Camera position in the world
	// 
	// 	float T4[16];
	// 	matrixMult(Ttea4, T2, T4);
	// 
	// 	matrixMult(T4, view, modelView);
	// 	glLoadMatrixf(modelView);
	// 	glColor3f(sin(cameraAngleDisplacement * 31 + 0), sin(cameraAngleDisplacement * 31 + 2), sin(cameraAngleDisplacement * 31 + 4));
	// 	renderWireTeapot(0.5);


	// 	float Tbase[] = { 1.0f, 0.0f, 0.0f, 0.0f,
	// 		0.0f, 1.0f, 0.0f, 0.0f,
	// 		0.0f, 0.0f, 1.0f, 0.0f,
	// 		0.0f, 0.0f, 0.0f, 1.0f };//Camera position in the world
	// 
	// 	float Rbase[] = { cos(angle + cameraAngleDisplacement), 0.0f, -sin(angle + cameraAngleDisplacement), 0.0f,
	// 		0.0f, 1.0f, 0.0f, 0.0f,
	// 		sin(angle + cameraAngleDisplacement), 0.0f, cos(angle + cameraAngleDisplacement), 0.0f,
	// 		0.0f, 0.0f, 0.0f, 1.0f };
	// 
	// 	float Mbase[16];
	// 	matrixMult(Rbase, Tbase, Mbase);
	// 
	// 	float modelView[16];
	// 	matrixMult(Mbase, view, modelView);
	// 	glLoadMatrixf(modelView);
	// 	glColor3f(sin(cameraAngleDisplacement * 10 + 4), sin(cameraAngleDisplacement * 10 + 0), sin(cameraAngleDisplacement * 10 + 2));
	// 	renderWireCylinder(1.0f, 10.0f, 10.0f, 1.0f);
	// 
	// 	float Tforearm[] = { 1.0f, 0.0f, 0.0f, 0.0f,
	// 		0.0f, 1.0f, 0.0f, 0.0f,
	// 		0.0f, 0.0f, 1.0f, 0.0f,
	// 		0.0f, 0.0f, 10.0f, 1.0f };//Camera position in the world
	// 
	// 
	// 	float Rforearm[] = { cos(angle + cameraAngleDisplacement), 0.0f, -sin(angle + cameraAngleDisplacement), 0.0f,
	// 		0.0f, 1.0f, 0.0f, 0.0f,
	// 		sin(angle + cameraAngleDisplacement), 0.0f, cos(angle + cameraAngleDisplacement), 0.0f,
	// 		0.0f, 0.0f, 0.0f, 1.0f 
	// 	};
	// 
	// 	float L1[16];
	// 	matrixMult(Tforearm, Tbase, L1);
	// 
	// 	float L1R1[16];
	// 	matrixMult(L1, Rforearm, L1R1);
	// 
	// 	float R2L1R1[16];
	// 	matrixMult(Rbase, L1R1, R2L1R1);
	// 
	// 	matrixMult(R2L1R1, view, modelView);
	// 	glLoadMatrixf(modelView);
	// 	glColor3f(sin(cameraAngleDisplacement * 10 + 4), sin(cameraAngleDisplacement * 10 + 0), sin(cameraAngleDisplacement * 10 + 2));
	// 	renderWireCylinder(1.0f, 5.0f, 10.0f, 1.0f);
	// 
	// 	float Tm[] = { 1.0f, 0.0f, 0.0f, 0.0f,
	// 		0.0f, 1.0f, 0.0f, 0.0f,
	// 		0.0f, 0.0f, 1.0f, 0.0f,
	// 		4.0f * cos(cameraAngleDisplacement * 10.f), 0.0f, 4.0f * sin(cameraAngleDisplacement * 10.f), 1.0f}; //Camera position in the world
	// 
	// 
	// 	float Rm[] = { cos(angle + cameraAngleDisplacement), 0.0f, -sin(angle + cameraAngleDisplacement), 0.0f,
	// 		0.0f, 1.0f, 0.0f, 0.0f,
	// 		sin(angle + cameraAngleDisplacement), 0.0f, cos(angle + cameraAngleDisplacement), 0.0f,
	// 		0.0f, 0.0f, 0.0f, 1.0f
	// 	};
	// 
	// 	float L2[] = { 1.0f, 0.0f, 0.0f, 0.0f,
	// 		0.0f, 1.0f, 0.0f, 0.0f,
	// 		0.0f, 0.0f, 1.0f, 0.0f,
	// 		0.0f, 0.0f, 5.0f, 1.0f };//Camera position in the world
	// 
	// 	//float L2[16];
	// 	//matrixMult(Tforearm, L1, L2);
	// 
	// 	float RmTm[16];
	// 	matrixMult(Rm, Tm, RmTm);
	// 
	// 	float RmTmL2[16];
	// 	matrixMult(RmTm, L2, RmTmL2);
	// 
	// 
	// 	float OrbitModelView[16];
	// 	matrixMult(RmTmL2, modelView, OrbitModelView);
	// 	glLoadMatrixf(OrbitModelView);
	// 	glColor3f(sin(cameraAngleDisplacement * 10 + 4), sin(cameraAngleDisplacement * 10 + 0), sin(cameraAngleDisplacement * 10 + 2));
	// 
	// 	renderWireSphere(1.0f, 10.0f, 10.0f);




	//_root->update();
	_root->updateBSpheres();
	_trav->initToViewMatrix(matrix(view));
	int TODO; //not
	_root->updateModels(_trav);
	_root->cull(_trav); //View has to be known by this point

}

void myTransformSandBox::Normalize(const float* input, float* output)
{
	float length = sqrt(input[0] * input[0] + input[1] * input[1] + input[2] * input[2]);
	output[0] = input[0] / length;
	output[1] = input[1] / length;
	output[2] = input[2] / length;
}

void myTransformSandBox::LookAt(TransformNode* from, TransformNode* to, float* output)
{
	//Look at

	float Wup[3] = {
		0.f, 1.f, 0.f
	};
	_root->updateModels(_trav);
	matrix lookFrom = from->GetModel();
	matrix lookTo = to->GetModel();
	float Kc[3] = {
		lookFrom._m[12] - lookTo._m[12], lookFrom._m[13] - lookTo._m[13], lookFrom._m[14] - lookTo._m[14]
	};

	float KcNorm[3];

	Normalize(Kc, KcNorm);

	float Xc[3] = {
		0.f, 0.f, 0.f
	};

	CrossProduct(Wup, KcNorm, Xc);

	float XcNorm[3];

	Normalize(Xc, XcNorm);

	float YcNorm[3];

	CrossProduct(KcNorm, XcNorm, YcNorm);

	//Camera orientation (basis) in the world. 
	//Note that using the canonical space rotation about X matrix (row-major!) works out.
// 	float Rc[] = { 1.f, 0.f, 0.f, 0.0f,
// 		0.f, 1.f, 0.f, 0.0f,
// 		0.f, 0.f, 1.f, 0.0f,
// 		0.0f, .0f, 0.0f, 1.0f };

	float Rc[] = { XcNorm[0], XcNorm[1], XcNorm[2], 0.0f,
		YcNorm[0], YcNorm[1], YcNorm[2], 0.0f,
		KcNorm[0], KcNorm[1], KcNorm[2], 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f };
	CopyArray(Rc, output);

}

void myTransformSandBox::LookAt(simpleSceneGraph::TransformNode* from, simpleSceneGraph::BillboardNode* to, float* output)
{
	//Look at

	float Wup[3] = {
		0.f, 1.f, 0.f
	};
	_root->updateModels(_trav);
	matrix lookFrom = from->GetModel();
	matrix lookTo = to->GetModel();
	float Kc[3] = {
		lookFrom._m[12] - lookTo._m[12], lookFrom._m[13] - lookTo._m[13], lookFrom._m[14] - lookTo._m[14]
	};

	float KcNorm[3];

	Normalize(Kc, KcNorm);

	float Xc[3] = {
		0.f, 0.f, 0.f
	};

	CrossProduct(Wup, KcNorm, Xc);

	float XcNorm[3];

	Normalize(Xc, XcNorm);

	float YcNorm[3];

	CrossProduct(KcNorm, XcNorm, YcNorm);

	//Camera orientation (basis) in the world. 
	//Note that using the canonical space rotation about X matrix (row-major!) works out.
	// 	float Rc[] = { 1.f, 0.f, 0.f, 0.0f,
	// 		0.f, 1.f, 0.f, 0.0f,
	// 		0.f, 0.f, 1.f, 0.0f,
	// 		0.0f, .0f, 0.0f, 1.0f };

	float Rc[] = { XcNorm[0], XcNorm[1], XcNorm[2], 0.0f,
		YcNorm[0], YcNorm[1], YcNorm[2], 0.0f,
		KcNorm[0], KcNorm[1], KcNorm[2], 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f };

	CopyArray(Rc, output);
}

float* myTransformSandBox::GetBillboard(simpleSceneGraph::TransformNode* from, simpleSceneGraph::TransformNode* to)
{
	float Wup[3] = {
		0.f, 1.f, 0.f
	};

	_root->updateModels(_trav);
	matrix lookFrom = from->GetModel();
	matrix lookTo = to->GetModel();
	float Kbb[3] = {
		lookFrom._m[12] - lookTo._m[12], lookFrom._m[13] - lookTo._m[13], lookFrom._m[14] - lookTo._m[14]
	};

	float KbbNorm[3];

	Normalize(Kbb, KbbNorm);

	float Ibb[3];

	CrossProduct(Wup, KbbNorm, Ibb);

	float IbbNorm[3];

	Normalize(Ibb, IbbNorm);

	float JbbNorm[3];

	CrossProduct(KbbNorm, IbbNorm, JbbNorm);

	float Rteay[] = { IbbNorm[0], IbbNorm[1], IbbNorm[2], 0.0f,
		JbbNorm[0], JbbNorm[1], JbbNorm[2], 0.0f,
		KbbNorm[0], KbbNorm[1], KbbNorm[2], 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f };

	return Rteay;
}
