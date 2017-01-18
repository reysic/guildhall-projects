#include <vector>
#include <memory>
#include <cassert>
#include <iostream>
#include <string>
#include <cmath>
#include "SimpleSceneGraph.h"

namespace simpleSceneGraph {
	size_t Node::s_nodeCount = 0;

	void TransformNode::Strategy::updateBSphere(const matrix& m, BoundingSphere* bSphere)
	{
		//NOTHING
	}

}

//call from somewhere...
//The function should produce the following output.
/*
Node count is : 0
Building graph
Done building graph.Node count is : 5
Update traversal
RotateStrategy::update with angle = 0.01
TranslateStrategy::update with angle = 0.02
Cull traversal
Node::cull root
TransformNode::cull transform1
GeometryNode::cull
Node::cull geom1
TransformNode::cull transform2
GeometryNode::cull
Node::cull geom2
Cleaning up the graph, node count = 5
RotateStrategy::~RotateStrategy
TranslateStrategy::~TranslateStrategy
Final node count is : 0
*/
void trySimpleSceneGraph()
{
	//     using namespace simpleSceneGraph;
	//     std::cout << "Node count is: " << Node::getNodeCount() << std::endl;
	//     std::cout << "Building graph" << std::endl;
	// 
	//     Node* root = new Node("root");
	//     TransformNode* transform1(new TransformNode("transform1"));
	//     root->addChild(transform1);
	//     RotateStrategy* rotateStrategy = new RotateStrategy();
	//     transform1->setStrategy(rotateStrategy);
	//     GeometryNode* geom1= new GeometryNode("geom1");
	//     transform1->addChild(geom1);
	// 
	//     //second branch
	//     TransformNode* transform2 = new TransformNode("transform2");
	//     root->addChild(transform2);
	//     TranslateStrategy* translateStrategy = new TranslateStrategy();
	//     transform2->setStrategy(translateStrategy);
	//     GeometryNode* geom2 = new GeometryNode("geom2");
	//     transform2->addChild(geom2);
	//     std::cout << "Done building graph. Node count is: " << Node::getNodeCount() << std::endl;
	// 
	//     //let's iterate over children of the root node:
	//     Node::const_iterator_child it = root->begin_child(), ite = root->end_child();
	//     for (; it != ite; ++it)
	//         const std::string& name = (*it)->name();
	// 
	//     //create the traversal 
	//     Node::Traversal* trav = new Node::Traversal();
	//     matrix view;//compute it
	//     trav->initToViewMatrix(view);
	//     std::cout << "Update traversal" << std::endl;
	//     root->update();
	//     std::cout << "Cull traversal" << std::endl;
	//     root->cull(trav);
	// 
	//     std::cout << "Cleaning up the graph, node count = " << Node::getNodeCount() << std::endl;
	//     //In reality, you want to go with shared_ptr approach that is based on reference counting
	//     //in order to automate memory cleanup according to the RAII paradigm.
	//     delete trav;
	//     delete root;
	//     delete transform1;
	//     delete geom1;
	//     delete transform2;
	//     delete geom2;
	//     delete rotateStrategy;
	//     delete translateStrategy;
	// 
	//     std::cout << "Final node count is: " << Node::getNodeCount() << std::endl;
	//     assert(Node::getNodeCount() == 0);
}
