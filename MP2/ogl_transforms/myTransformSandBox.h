#ifndef MYTRANSFORMSANDBOX_H
#define MYTRANSFORMSANDBOX_H
#include "SimpleSceneGraph.h"
class myTransformSandBox
{
public:
	myTransformSandBox(void);
	~myTransformSandBox(void);

	//using the Strategy design pattern
	virtual void render();

	void Normalize(const float* input, float* output);
	void LookAt(simpleSceneGraph::TransformNode* from, simpleSceneGraph::TransformNode* to, float* output);
	void LookAt(simpleSceneGraph::TransformNode* from, simpleSceneGraph::BillboardNode* to, float* output);
	float* GetBillboard(simpleSceneGraph::TransformNode* from, simpleSceneGraph::TransformNode* to);

	bool g_lookAt = false;
	bool g_godEye = true;

	float SuperTc[16];
	float SuperRc[16];

protected:

	void renderAxis(float lenght);
	void CrossProduct(const float* a, const float* b, float* c);
	void CopyArray(const float* a, float* b);

	simpleSceneGraph::Node* _root;
	simpleSceneGraph::Node::Traversal* _trav;
	float cameraAngleDisplacement = 0.1f;
};

#endif //MYTRANSFORMSANDBOX_H