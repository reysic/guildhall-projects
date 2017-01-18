#pragma once
#include <vector>
#include <memory>
#include <cassert>
#include <iostream>
#include <string>
#include <cmath>
#include "geometry.h"
#include "Vector3.hpp"
#include "MathUtils.hpp"
#include "AxisAngle.hpp"
#include "Quaternion.hpp"
#include "CatmullRom.hpp"
#include "SlerpSegment.hpp"

//A simple reference implementation of a scene graph.
//The point was to keep it as simple as possible while 
//demonstrating core elements of the idea behind a scene graph and traversals.
namespace simpleSceneGraph {
	
	//replace with your own matrix class
	struct matrix {

		matrix()
		{
			_m[0] = 1.0; _m[1] = 0.0; _m[2] = 0.0; _m[3] = 0.0;
			_m[4] = 0.0; _m[5] = 1.0; _m[6] = 0.0; _m[7] = 0.0;
			_m[8] = 0.0; _m[9] = 0.0; _m[10] = 1.0; _m[11] = 0.0;
			_m[12] = 0.0; _m[13] = 0.0; _m[14] = 0.0; _m[15] = 1.0;

		}

		matrix(const float* src)
		{
			for (size_t i = 0; i < 16; ++i)
				_m[i] = src[i];
		}

		matrix&  operator=(const matrix& rhs)
		{
			_m[0] = rhs._m[0];
			_m[1] = rhs._m[1];
			_m[2] = rhs._m[2];
			_m[3] = rhs._m[3];
			_m[4] = rhs._m[4];
			_m[5] = rhs._m[5];
			_m[6] = rhs._m[6];
			_m[7] = rhs._m[7];
			_m[8] = rhs._m[8];
			_m[9] = rhs._m[9];
			_m[10] = rhs._m[10];
			_m[11] = rhs._m[11];
			_m[12] = rhs._m[12];
			_m[13] = rhs._m[13];
			_m[14] = rhs._m[14];
			_m[15] = rhs._m[15];
			return *this;
		}

		inline friend void mult(const matrix& l, const matrix& r, matrix& res)
		{
			assert(&l != &res && &r != &res);//prevent in-place overwrites
			//assume that the arguments are valid float[16] buffers
			res._m[0] = l._m[0] * r._m[0] + l._m[1] * r._m[4] + l._m[2] * r._m[8] + l._m[3] * r._m[12];
			res._m[1] = l._m[0] * r._m[1] + l._m[1] * r._m[5] + l._m[2] * r._m[9] + l._m[3] * r._m[13];
			res._m[2] = l._m[0] * r._m[2] + l._m[1] * r._m[6] + l._m[2] * r._m[10] + l._m[3] * r._m[14];
			res._m[3] = l._m[0] * r._m[3] + l._m[1] * r._m[7] + l._m[2] * r._m[11] + l._m[3] * r._m[15];

			res._m[4] = l._m[4] * r._m[0] + l._m[5] * r._m[4] + l._m[6] * r._m[8] + l._m[7] * r._m[12];
			res._m[5] = l._m[4] * r._m[1] + l._m[5] * r._m[5] + l._m[6] * r._m[9] + l._m[7] * r._m[13];
			res._m[6] = l._m[4] * r._m[2] + l._m[5] * r._m[6] + l._m[6] * r._m[10] + l._m[7] * r._m[14];
			res._m[7] = l._m[4] * r._m[3] + l._m[5] * r._m[7] + l._m[6] * r._m[11] + l._m[7] * r._m[15];

			res._m[8] = l._m[8] * r._m[0] + l._m[9] * r._m[4] + l._m[10] * r._m[8] + l._m[11] * r._m[12];
			res._m[9] = l._m[8] * r._m[1] + l._m[9] * r._m[5] + l._m[10] * r._m[9] + l._m[11] * r._m[13];
			res._m[10] = l._m[8] * r._m[2] + l._m[9] * r._m[6] + l._m[10] * r._m[10] + l._m[11] * r._m[14];
			res._m[11] = l._m[8] * r._m[3] + l._m[9] * r._m[7] + l._m[10] * r._m[11] + l._m[11] * r._m[15];

			res._m[12] = l._m[12] * r._m[0] + l._m[13] * r._m[4] + l._m[14] * r._m[8] + l._m[15] * r._m[12];
			res._m[13] = l._m[12] * r._m[1] + l._m[13] * r._m[5] + l._m[14] * r._m[9] + l._m[15] * r._m[13];
			res._m[14] = l._m[12] * r._m[2] + l._m[13] * r._m[6] + l._m[14] * r._m[10] + l._m[15] * r._m[14];
			res._m[15] = l._m[12] * r._m[3] + l._m[13] * r._m[7] + l._m[14] * r._m[11] + l._m[15] * r._m[15];

		}
		inline friend void mult(const Vector3& l, const matrix& r, Vector3& res)
		{
			res.x = l.x * r._m[0] + l.y * r._m[4] + l.z * r._m[8] + 1.f * r._m[12];
			res.y = l.x * r._m[1] + l.y * r._m[5] + l.z * r._m[9] + 1.f * r._m[13];
			res.z = l.x * r._m[2] + l.y * r._m[6] + l.z * r._m[10] + 1.f * r._m[14];
		}
		static inline void NormalizeSS(const float* input, float* output)
		{
			float length = sqrt(input[0] * input[0] + input[1] * input[1] + input[2] * input[2]);
			output[0] = input[0] / length;
			output[1] = input[1] / length;
			output[2] = input[2] / length;
		}
		static inline void CrossProductSS(const float* a, const float* b, float* c)
		{
			c[0] = a[1] * b[2] - b[1] * a[2];
			c[1] = a[2] * b[0] - b[2] * a[0];
			c[2] = a[0] * b[1] - b[0] * a[1];
		}


		float  _m[16];
	};

	class BoundingSphere
	{
	public:
		float m_R;

		Vector3 m_center;

		BoundingSphere(float radius = -1.0f, Vector3 center = Vector3())
		{
			m_R = radius;
			m_center = center;
		}

		bool empty() const
		{
			return m_R == -1.0f;
		}

		BoundingSphere extendBy(Vector3& vertex)
		{
			Vector3 vecToPoint = m_center - vertex;
			float distanceToVertex = vecToPoint.GetMagnitude();

			float distSquared = distanceToVertex * distanceToVertex;
			float radiusSquared = m_R * m_R;
			if (radiusSquared < distSquared)
			{
				Vector3 pointOnRadius = vecToPoint.GetNormalized() * m_R;
				Vector3 pointOnRadiusPosition = m_center + pointOnRadius;
				Vector3 newCenter = (pointOnRadiusPosition + vertex) / 2.f;
				float newRadius = (distanceToVertex + m_R) / 2.f;

				return BoundingSphere(newRadius, newCenter);
			}

			return BoundingSphere(m_R, m_center);
		}

		BoundingSphere extendBy(const BoundingSphere& other)
		{

			if (this->empty())
				return other;
			if (other.empty())
				return BoundingSphere(m_R, m_center);
			Vector3 vecToCenters = other.m_center - m_center;
			float distanceToCenter = vecToCenters.GetMagnitude();

			BoundingSphere bigSphere = BoundingSphere(other.m_R, other.m_center);
			BoundingSphere littleSphere = BoundingSphere(m_R, m_center);

			if (bigSphere.m_R < littleSphere.m_R)
			{
				bigSphere = BoundingSphere(m_R, m_center);
				littleSphere = BoundingSphere(other.m_R, other.m_center);

			}
			if (bigSphere.m_R >(littleSphere.m_R + distanceToCenter))
			{
				return bigSphere;
			}
			else
			{
				Vector3 dirToOtherRadius = vecToCenters.GetNormalized() * other.m_R;
				Vector3 pointOnOtherRadius = dirToOtherRadius + other.m_center;

				return (this->extendBy(pointOnOtherRadius));
			}
		}

		BoundingSphere transformBy(const matrix& mtx)
		{
			int TODO;
			//                                1 for homogeneous coordinate
			//                                transform as a position
			// newCenter = [ctrX, ctrY, ctrZ, 1] * mtx
			Vector3 newCenter;
			mult(m_center, mtx, newCenter);
			return BoundingSphere(m_R, newCenter);
		}
	};

	class Node {
	public:
		matrix superMatrix;
		typedef std::vector<Node*> Children;
		typedef Children::const_iterator const_iterator_child;

		struct Traversal {

			Traversal()
			{
			}
			matrix              _view;
			//matrix              _invView; //could also be handy for other algorithms
			std::vector<matrix> _stack;

			struct PlaneEquation {
			public:
				Vector3	_normal;
				Vector3 _p0;
				bool isOutside(const BoundingSphere& sphere) const {
					//float d = dot(_normal, (sphere.ctr - _p0) = gives us signed distance to the plane

					float d = DotProduct(_normal, (sphere.m_center - _p0));

					if (d > sphere.m_R)
						return true;
					return false;
				}
			};
			std::vector<PlaneEquation>	_frustumPlanes;

			void setCameraFrustumPlanes(float hfov, float vfov, float nearDist, float farDist)
			{
				_frustumPlanes.clear();
				//int TODO;
				//Set _frustumPlanes based on the vars
				PlaneEquation nearPlane;
				nearPlane._p0 = Vector3(0.f, 0.f, -nearDist);
				nearPlane._normal = Vector3(0.f, 0.f, 1.f);
				_frustumPlanes.push_back(nearPlane);


				PlaneEquation farPlane;
				farPlane._p0 = Vector3(0.f, 0.f, -farDist);
				farPlane._normal = Vector3(0.f, 0.f, -1.f);
				_frustumPlanes.push_back(farPlane);

				hfov = hfov * M_PI / 180.0;//Degree->Radians
				vfov = vfov * M_PI / 180.0;//Degree->Radians
				PlaneEquation rightPlane;
				rightPlane._p0 = Vector3(0.f, 0.f, 0.f);
				rightPlane._normal = Vector3(cos(hfov/2.f), 0.f, sin(hfov/2.f));
				_frustumPlanes.push_back(rightPlane);

				PlaneEquation leftPlane;
				leftPlane._p0 = Vector3(0.f, 0.f, 0.f);
				leftPlane._normal = Vector3(-cos(hfov / 2.f), 0.f, sin(hfov / 2.f));
				_frustumPlanes.push_back(leftPlane);

				PlaneEquation topPlane;
				topPlane._p0 = Vector3(0.f, 0.f, 0.f);
				topPlane._normal = Vector3(0.f, cos(vfov / 2.f), sin(vfov / 2.f));
				_frustumPlanes.push_back(topPlane);

				PlaneEquation botPlane;
				botPlane._p0 = Vector3(0.f, 0.f, 0.f);
				botPlane._normal = Vector3(0.f, -cos(vfov / 2.f), sin(vfov / 2.f));
				_frustumPlanes.push_back(botPlane);
			}

			void initToViewMatrix(const matrix& view)
			{
				_view = view;
				_stack.erase(_stack.begin(), _stack.end());
				_stack.push_back(view);
			}

			void pushBillboardMatrixStack(const matrix& modelMatrix)
			{
				matrix mv = _stack.back();
				_stack.push_back(_view);
				mult(modelMatrix, mv, _stack.back());
			}

			void pushMatrixStack(const matrix& modelMatrix)
			{
				matrix mv = _stack.back();
				_stack.push_back(mv);
				mult(modelMatrix, mv, _stack.back());
			}

			void popMatrixStack()
			{
				_stack.pop_back();
			}

			const matrix& topMatrixStack() const { return _stack.back(); }

			bool isVisible(const BoundingSphere& nodeSphere)
			{
				BoundingSphere sphere = nodeSphere;
				sphere = sphere.transformBy(topMatrixStack());
				//frustum check
				auto it = _frustumPlanes.begin(), ite = _frustumPlanes.end();
				for (; it != ite; ++it) {
					bool isOutside = it->isOutside(sphere);
					if (isOutside)
						return false;
				}
				return true;
			}
		};

		Node(const char* name) : _name(name), _parent(nullptr)
		{
			s_nodeCount++; //bookkeeping
		}

		//NOTE: Virtual Destructor for the base class.
		//See Scott Meyers, Effective C++, Item 7
		virtual ~Node()
		{
			destroy();
			assert(s_nodeCount >= 1);  s_nodeCount--;//bookkeeping
		}

		void addChild(Node* child)
		{
			child->setParent(this);
			_children.push_back(child);
		}

		BoundingSphere GetBSphere()
		{
			return _sphere;
		}

		virtual void update() //not const-correct, changes scene graph.
		{
			Children::const_iterator it = _children.begin(), ite = _children.end();
			for (; it != ite; ++it)
				(*it)->update();

		}

		virtual void cull(Traversal* trav) const //<- Note const-correctness!
		{
			std::cout << "Node::cull " << _name << std::endl;

			if (!trav->isVisible(_sphere))
				return;
			Children::const_iterator it = _children.begin(), ite = _children.end();
			for (; it != ite; ++it)
				(*it)->cull(trav);
		}

		virtual void updateModels(Traversal* trav)
		{
			Children::const_iterator it = _children.begin(), ite = _children.end();
			for (; it != ite; ++it)
				(*it)->updateModels(trav);
		}

		virtual void updateBSpheres()
		{
			_sphere = BoundingSphere();
			Children::iterator it = _children.begin(), ite = _children.end();
			for (; it != ite; ++it)
			{
				BoundingSphere otherSphere = (*it)->_sphere;
				(*it)->updateBSpheres();
				_sphere = _sphere.extendBy((*it)->GetBSphere());
			}
		}

		const std::string& name() const { return _name; }

		const_iterator_child   begin_child() const { return _children.begin(); }

		const_iterator_child   end_child()  const { return _children.end(); }

		//For bookkeeping purposes, to show that shared pointers cleanup automatically!
		static size_t getNodeCount()
		{
			return s_nodeCount;
		}

		void destroy()
		{
			Children::iterator itc = _children.begin(), itce = _children.end();
			for (; itc != itce; ++itc) {
				(*itc)->destroy();
				delete *itc;
			}
			_children.clear();
		}

		static void checkNodeCount() { assert(s_nodeCount == 0); }

	protected:
		void    setParent(Node* parent) { _parent = parent; }

		Node*           _parent;
		Children        _children;
		std::string     _name;
		BoundingSphere  _sphere;
		
		//static variables are BAD for mulithreading, btw.
		static size_t s_nodeCount;
	};

	class TransformNode : public Node {
	public:

		//The Strategy Design pattern
		class Strategy {
		public:
			Strategy(){}
			virtual ~Strategy(){}

			virtual void update(matrix& m) = 0;
			virtual void updateBSphere(const matrix& m, BoundingSphere* bSphere);
		};

		TransformNode(const char* name) : Node(name), _strategy(nullptr) {}

		~TransformNode(){}

		void setStrategy(Strategy* s) { _strategy = s; }

		virtual void update()
		{
			if (_strategy)
				_strategy->update(_transform);
			Node::update();
		}

		virtual void updateModels(Traversal* trav)
		{
			trav->pushMatrixStack(_transform);
			Children::const_iterator it = _children.begin(), ite = _children.end();
			_model = trav->topMatrixStack();
			for (; it != ite; ++it)
				(*it)->updateModels(trav);
			trav->popMatrixStack();
		}

		virtual void updateBSpheres()
		{
			_sphere = BoundingSphere();
			Children::const_iterator it = _children.begin(), ite = _children.end();
			for (; it != ite; ++it)
			{
				(*it)->updateBSpheres();
				_sphere = _sphere.extendBy((*it)->GetBSphere());
			}
			//also needs tranform the result by _transform
			_sphere = _sphere.transformBy(_transform);
		}

		matrix		_model;

		matrix GetModel() const
		{
			return _model;
		}

		virtual void cull(Traversal* trav) const
		{
			std::cout << "TransformNode::cull " << _name << std::endl;
			//NOTE that we do NOT push stack before we do the frustum check.
			//This is because updateBSpheres already applied _transform
			if (!trav->isVisible(_sphere))
				return;
			trav->pushMatrixStack(_transform);
			Children::const_iterator it = _children.begin(), ite = _children.end();
			for (; it != ite; ++it)
				(*it)->cull(trav);
			trav->popMatrixStack();
		}

	protected:
		matrix  _transform;
		Strategy*  _strategy;
	};

	class BillboardNode : public Node {
	public:

		//The Strategy Design pattern
		class Strategy {
		public:
			Strategy(){}
			virtual ~Strategy(){}

			virtual void update(matrix& m) = 0;
		};

		BillboardNode(const char* name) : Node(name), _strategy(nullptr) {}

		~BillboardNode(){}

		void setStrategy(Strategy* s) { _strategy = s; }

		virtual void update()
		{
			if (_strategy)
				_strategy->update(_transform);
			Node::update();
		}

		virtual void updateModels(Traversal* trav)
		{
			trav->pushMatrixStack(_transform);
			Children::const_iterator it = _children.begin(), ite = _children.end();
			_model = trav->topMatrixStack();
			for (; it != ite; ++it)
				(*it)->updateModels(trav);
			trav->popMatrixStack();
		}

		virtual void updateBSpheres()
		{
			_sphere = BoundingSphere();
			Children::const_iterator it = _children.begin(), ite = _children.end();
			for (; it != ite; ++it)
			{
				(*it)->updateBSpheres();
				_sphere = _sphere.extendBy((*it)->GetBSphere());
			}
		}

		matrix		_model;

		matrix GetModel() const
		{
			return _model;
		}

		virtual void cull(Traversal* trav) const
		{
			std::cout << "TransformNode::cull " << _name << std::endl;
			matrix bbMatrix(_transform);

			float Wup[3] = {
				0.f, 1.f, 0.f
			};
			matrix camera = trav->_view;

			float Kbb[3] = {
				camera._m[12] - _transform._m[12], camera._m[13] - _transform._m[13], camera._m[14] - _transform._m[14]
			};

			float KbbNorm[3];

			matrix::NormalizeSS(Kbb, KbbNorm);

			float Ibb[3];

			matrix::CrossProductSS(Wup, KbbNorm, Ibb);

			float IbbNorm[3];

			matrix::NormalizeSS(Ibb, IbbNorm);

			float JbbNorm[3];

			matrix::CrossProductSS(KbbNorm, IbbNorm, JbbNorm);

			float Rc[] = { IbbNorm[0], IbbNorm[1], IbbNorm[2], 0.0f,
				JbbNorm[0], JbbNorm[1], JbbNorm[2], 0.0f,
				KbbNorm[0], KbbNorm[1], KbbNorm[2], 0.0f,
				0.0f, 0.0f, 0.0f, 1.0f };

			matrix Tbb = matrix();
			Tbb._m[12] = _transform._m[12];
			Tbb._m[13] = _transform._m[13];
			Tbb._m[14] = _transform._m[14];
			matrix Rbb = matrix(Rc);
			matrix Mbb;
			mult(Rbb, Tbb, Mbb);

			trav->pushBillboardMatrixStack(Mbb);

			Children::const_iterator it = _children.begin(), ite = _children.end();
			for (; it != ite; ++it)
				(*it)->cull(trav);
			trav->popMatrixStack();
		}

	protected:
		matrix  _transform;
		Strategy*  _strategy;
	};

	class GeometryNode : public Node {
	public:

		enum Type { Teapot, Sphere, Cylinder };

		GeometryNode(const char* name, Type type = Teapot, double size = 1.0, int color = 1) : Node(name), _type(type), _size(size), _color(color) {
			_sphere = BoundingSphere((float)_size * 2.f, Vector3::ZERO);
		}

		virtual void updateBSpheres()
		{
			int TODO;
			//build bsphere from geometry (vertex) data
			//technically, also call on children.
			_sphere.m_R = _size;
			
		}

		virtual void cull(Traversal* trav) const
		{
			std::cout << "GeometryNode::cull" << std::endl;
			if (!trav->isVisible(_sphere))
				return;
			glLoadMatrixf(trav->topMatrixStack()._m);
			switch (_type) {
			case Teapot:
				glColor3f(1.0f, 0.0f, 1.0f);
				renderWireTeapot((int)_size);
				break;
			case Sphere:
				if ( _color == 2 )
				{
					// Yellow
					glColor3f( 1.0f, 1.0f, 0.0f );
				}
				else if ( _color == 3 )
				{
					// White
					glColor3f( 1.0f, 1.0f, 1.0f );
				}
				else
				{
					glColor3f( 0.0f, 1.0f, 1.0f );
				}
				renderWireSphere((int)_size, 10.0, 10.0);
				break;
			case Cylinder:
				glColor3f(0.0f, 1.0f, 1.0f);
				renderWireCylinder((int)_size, 10.0, 10.0, 10.0);
				break;
			default:
				;
			}
			Children::const_iterator it = _children.begin(), ite = _children.end();
			for (; it != ite; ++it)
				(*it)->cull(trav);

		}

	protected:
		Type    _type;
		double _size;
		int _color;
	};
	class SlerpStrategy : public TransformNode::Strategy {
	public:
		SlerpStrategy(float angle1, const Vector3& axis1, float angle2, const Vector3& axis2, float t, float dt)
			: m_q1(angle1, axis1)
			, m_q2(angle2, axis2)
			, m_t(t)
			, m_dt(dt)
		{
			float cosTheta = m_q1.DotProduct(m_q2);

			if (cosTheta < 0.f) {
				m_q2 = -1.f * m_q2;
				cosTheta = -1.f * cosTheta;
			}

			m_theta = acos(cosTheta);
		}

		virtual void Update(float deltaSeconds, matrix& m) {
			m_t += m_dt;

			if (m_t > 1.f) {
				m_dt *= -1.f;
				m_t = 1.f - (m_t - 1.f);
			}
			else if (m_t < 0.f) {
				m_dt *= -1.f;
				m_t = -m_t;
			}

			//SLERP COMPUTATION AWW YEEE
			Quaternion intermediateQuaternion = (m_q1 * sin((1.f - m_t) * m_theta) + m_q2 * sin(m_t * m_theta));
			intermediateQuaternion = intermediateQuaternion * (1 / sin(m_theta));

			intermediateQuaternion.ToRotationMatrix(m._m);

			//qm = (q1 * sin((1 - t)theta) + q2 * sin(t * theta)) / sin(theta)
		}
	private:
		Quaternion m_q1;
		Quaternion m_q2;

		float m_t;
		float m_dt;

		float m_theta;
	};

	class NavigationPoint {
	public:
		NavigationPoint() : m_position(), m_axisAngle() {}
		NavigationPoint(const Vector3& position, const AxisAngle& axisAngle) : m_position(position), m_axisAngle(axisAngle) {}

	public:
		Vector3 m_position;
		AxisAngle m_axisAngle;
	};

	class NavigatorStrategy : public TransformNode::Strategy {
	public:
		NavigatorStrategy() : m_navPoints() {}
		NavigatorStrategy(std::vector<NavigationPoint> navPoints) : m_navPoints(navPoints) {
			for (unsigned int i = 0; i < m_navPoints.size(); i++) {
				m_segments.resize(m_navPoints.size());
				m_splines.resize(m_navPoints.size());


				if ((i + 1) == m_navPoints.size()) {
					m_segments[i] = SlerpSegment(Quaternion(m_navPoints[i].m_axisAngle.m_angle, m_navPoints[i].m_axisAngle.m_axis), Quaternion(m_navPoints[0].m_axisAngle.m_angle, m_navPoints[0].m_axisAngle.m_axis));
				}
				else {
					m_segments[i] = SlerpSegment(Quaternion(m_navPoints[i].m_axisAngle.m_angle, m_navPoints[i].m_axisAngle.m_axis), Quaternion(m_navPoints[i + 1].m_axisAngle.m_angle, m_navPoints[i + 1].m_axisAngle.m_axis));
				}

				int firstPointForSpline = i - 1;
				if (firstPointForSpline < 0)
					firstPointForSpline = m_navPoints.size() - 1;

				int secondPointForSpline = i;

				int thirdPointForSpline = i + 1;
				int fourthPointForSpline = i + 2;

				if (thirdPointForSpline >= m_navPoints.size()) {
					thirdPointForSpline = 0;
					fourthPointForSpline = 1;
				}

				if (fourthPointForSpline >= m_navPoints.size()) {
					fourthPointForSpline = 0;
				}

				m_splines[i] = CatmullRomSpline(m_navPoints[firstPointForSpline].m_position, m_navPoints[secondPointForSpline].m_position, m_navPoints[thirdPointForSpline].m_position, m_navPoints[fourthPointForSpline].m_position);
			}

			currSlerpSegment = m_segments.begin();
			currSpline = m_splines.begin();

			m_interpolatingParameterForSplines = 0.f;
			m_interpolatingParameterForSlerp = 0.f;
			m_deltaStep = 0.01f;
		}

		virtual void update(matrix& m) {
			ComputeDeltaStep();
			UpdateRotation(m);
			UpdatePosition(m);
		}

		void UpdateRotation(matrix& m) {
			//Switch to next slerp segment if interpolating parameter over 1
			Vector3 rot_i = (*currSpline).GetTangent(m_interpolatingParameterForSplines);
			rot_i.SetNormalized();
			Vector3 rot_k = CrossProduct(rot_i, Vector3(0.f, 1.f, 0.f));
			rot_k.SetNormalized();
			Vector3 rot_j = CrossProduct(rot_k, rot_i);

			m._m[0] = rot_i.x;
			m._m[1] = rot_i.y;
			m._m[2] = rot_i.z;

			m._m[3] = 0.f;

			m._m[4] = rot_j.x;
			m._m[5] = rot_j.y;
			m._m[6] = rot_j.z;

			m._m[7] = 0.f;

			m._m[8] = rot_k.x;
			m._m[9] = rot_k.y;
			m._m[10] = rot_k.z;

			m._m[11] = 0.f;

			m._m[15] = 1.f;

			m._m[12] = 0.f;
			m._m[13] = 0.f;
			m._m[14] = 0.f;

			return;

			// 		Quaternion intermediateQuaternionStep = (*currSlerpSegment)(m_interpolatingParameterForSlerp);
			// 		intermediateQuaternionStep.ToRotationMatrix(m);
			// 
			// 		m_interpolatingParameterForSlerp += m_deltaStep;
			// 		if (m_interpolatingParameterForSlerp > 1.f) {
			// 			m_interpolatingParameterForSlerp = m_interpolatingParameterForSlerp - 1.f;
			// 			currSlerpSegment++;
			// 
			// 			if (currSlerpSegment == m_segments.end()) {
			// 				currSlerpSegment = m_segments.begin();
			// 			}
			// 		}
		}

		void UpdatePosition(matrix& m) {
			//Switch to next spline if interpolating parameter over 1
			(*currSpline).FillInPositionMatrixForPositionOnSpline(m_interpolatingParameterForSplines, m._m);

			m_interpolatingParameterForSplines += m_deltaStep;
			if (m_interpolatingParameterForSplines > 1.f) {
				m_interpolatingParameterForSplines = m_interpolatingParameterForSplines - 1.f;
				currSpline++;

				if (currSpline == m_splines.end()) {
					currSpline = m_splines.begin();
				}
			}
		}

		void ComputeDeltaStep() {
			float tangentLength = (*currSpline).GetNormOfTangent(m_interpolatingParameterForSplines);

			float t = m_interpolatingParameterForSplines;
			float desiredDist = 0.05f;
			m_deltaStep = 0.f;

			for (unsigned int i = 0; i < 100; i++) {
				float currTangentLength = (*currSpline).GetNormOfTangent(t);
				float dt = desiredDist / (100 * currTangentLength);

				t += dt;
				m_deltaStep += dt;
			}

			m_deltaStep = desiredDist / tangentLength;
		}

	private:
		std::vector<NavigationPoint> m_navPoints;
		std::vector<CatmullRomSpline> m_splines;
		std::vector<SlerpSegment> m_segments;

		std::vector<SlerpSegment>::const_iterator currSlerpSegment;
		std::vector<CatmullRomSpline>::const_iterator currSpline;

		float m_interpolatingParameterForSplines;
		float m_interpolatingParameterForSlerp;

		float m_deltaStep;
	};

	//Some sample transformation strategies, to play with
	class RotateStrategy : public TransformNode::Strategy {
	public:
		const double degreesToRad = (3.14159 / 180.0);
		RotateStrategy(double pitch, double heading, double roll) : h(heading * degreesToRad), p(pitch * degreesToRad), r(roll * degreesToRad) {}

		~RotateStrategy()
		{
			std::cout << "RotateStrategy::~RotateStrategy" << std::endl;
		}

		virtual void update(matrix& m)
		{
			matrix z;
			//Rotation about Z axis
			z._m[0] = cos(r);		z._m[1] = -sin(r);		z._m[2] = 0.0;		z._m[3] = 0.0;
			z._m[4] = sin(r);		z._m[5] = cos(r);		z._m[6] = 0.0;		z._m[7] = 0.0;
			z._m[8] = 0.0;			z._m[9] = 0.0;			z._m[10] = 1.0;		z._m[11] = 0.0;
			z._m[12] = 0.0;			z._m[13] = 0.0;			z._m[14] = 0.0;		z._m[15] = 1.0;

			matrix y;
			//Rotation about y axis
			y._m[0] = cos(h);		y._m[1] = 0.0;			y._m[2] = sin(h);	y._m[3] = 0.0;
			y._m[4] = 0.0;			y._m[5] = 1.0;			y._m[6] = 0.0;		y._m[7] = 0.0;
			y._m[8] = -sin(h);		y._m[9] = 0.0;			y._m[10] = cos(h);	y._m[11] = 0.0;
			y._m[12] = 0.0;			y._m[13] = 0.0;			y._m[14] = 0.0;		y._m[15] = 1.0;

			matrix x;
			//Rotation about x axis
			x._m[0] = 1.0;			x._m[1] = 0.0;			x._m[2] = 0.0;		x._m[3] = 0.0;
			x._m[4] = 0.0;			x._m[5] = cos(p);		x._m[6] = -sin(p);	x._m[7] = 0.0;
			x._m[8] = 0.0;			x._m[9] = sin(p);		x._m[10] = cos(p);	x._m[11] = 0.0;
			x._m[12] = 0.0;			x._m[13] = 0.0;			x._m[14] = 0.0;		x._m[15] = 1.0;

			matrix yx;
			mult(y, x, yx);
			mult(z, yx, m);
		}

		virtual void updateBSpheres(const matrix& m, BoundingSphere* bSphere)
		{
			//NOTHING
		}

	private:
		double h;
		double p;
		double r;
	};

	class TranslateStrategy : public TransformNode::Strategy {
	public:
		TranslateStrategy(double x, double y, double z) : m_x(x), m_y(y), m_z(z) {}

		~TranslateStrategy()
		{
			std::cout << "TranslateStrategy::~TranslateStrategy" << std::endl;
		}

		virtual void update(matrix& m)
		{
			m._m[0] = 1.0;  m._m[1] = m._m[2] = m._m[3] = 0.0;
			m._m[4] = 0.0;  m._m[5] = 1.0; m._m[6] = 0.0;  m._m[7] = 0.0;
			m._m[8] = 0.0;  m._m[9] = 0.0; m._m[10] = 1.0; m._m[11] = 0.0;
			m._m[12] = m_x;  m._m[13] = m_y;  m._m[14] = m_z; m._m[15] = 1.0;
		}

		virtual void updateBSphere(const matrix& m, BoundingSphere* bSphere)
		{
			bSphere->transformBy(m);
		}

	public:
		double  m_x;
		double  m_y;
		double  m_z;
	};

	class TranslateStrategyBB : public BillboardNode::Strategy {
	public:
		TranslateStrategyBB(double x, double y, double z) : m_x(x), m_y(y), m_z(z) {}

		~TranslateStrategyBB()
		{
			std::cout << "TranslateStrategyBB::~TranslateStrategyBB" << std::endl;
		}

		virtual void update(matrix& m)
		{
			m._m[0] = 1.0;  m._m[1] = m._m[2] = m._m[3] = 0.0;
			m._m[4] = 0.0;  m._m[5] = 1.0; m._m[6] = 0.0;  m._m[7] = 0.0;
			m._m[8] = 0.0;  m._m[9] = 0.0; m._m[10] = 1.0; m._m[11] = 0.0;
			m._m[12] = m_x;  m._m[13] = m_y;  m._m[14] = m_z; m._m[15] = 1.0;
		}

		virtual void updateBSphere(const matrix& m, BoundingSphere* bSphere)
		{
			bSphere->transformBy(m);
		}

	public:
		double  m_x;
		double  m_y;
		double  m_z;
	};

	class OrbitStrategy : public TransformNode::Strategy {
	public:
		OrbitStrategy() : _angle(0.0), _radius(1.0) {}

		~OrbitStrategy()
		{
			std::cout << "OrbitStrategy::~OrbitStrategy" << std::endl;
		}

		virtual void update(matrix& m)
		{
			_angle += 0.02;
			std::cout << "TranslateStrategy::update with angle=" << _angle << std::endl;
			//Say, rotation about Z axis
			m._m[0] = 1.0;  m._m[1] = m._m[2] = m._m[3] = 0.0;
			m._m[4] = 0.0;  m._m[5] = 1.0; m._m[6] = 0.0;  m._m[7] = 0.0;
			m._m[8] = 0.0;  m._m[9] = 0.0; m._m[10] = 1.0; m._m[11] = 0.0;
			m._m[12] = _radius*cos(_angle);  m._m[13] = _radius*sin(_angle);  m._m[14] = 0.0; m._m[15] = 1.0;
		}

		virtual void updateBSphere(const matrix& m, BoundingSphere* bSphere)
		{
			bSphere->transformBy(m);
		}

	private:
		double  _angle;
		double  _radius;
	};
}//namespace 
