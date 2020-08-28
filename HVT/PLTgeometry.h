#pragma once

#ifndef __PLTgeometry___
#define __PLTgeometry___

// #include "PLTtextures.h"
// #include "PLTmatrixes.h"
#include "PLTshaders.h"
#include "PLTthings.h"
#include "PLTmaterials.h"

#include <GL/glew.h>
#include <GL/gl.h>

#include <vector>
#include <iostream>
#include <map>

using namespace std;


double* ReturnRGB (double, double max=1, double min=0);
const float*  ReturnRGBf(float, float max=1, float min=0);
void NewReturnRGBf(float,float,float,float*);
void NewReturnGrayf(float,float,float,float*);

class PLTshaderProgram;

/**
	A triangle is, basically, a triplette of vertexes.
*/
struct PLTtriangle {

public:

	/// Constructors.
	PLTtriangle() {};
	PLTtriangle(PLTvec4 p1, PLTvec4 p2, PLTvec4 p3);
	PLTtriangle(const PLTvec4* p) { PLTtriangle(p[0], p[1], p[2]); };

	/// A triangle is defined by 3 vertexes.
	PLTvec4 Vertexes[3];

	/// Every vertex has its own color.
	PLTcolor VertexesColors[3];

	/// Check if the current triangle contains the specified vertex.
	bool ContainsVertex(PLTvec4 v);
	
	/// Overload operator "<" for using set from STL
	bool operator<(const PLTtriangle& tri) const;

	/// Overload operator "=" for assignments
	// PLTtriangle* operator=(const PLTtriangle& tri)  { PLTtriangle(tri.Vertexes[0], tri.Vertexes[1], tri.Vertexes[2]); return this; }

	/// Check if the current triangle stays completely on the left of the specified vertex.
	bool OnTheLeft(PLTvec4 v);

	/// Computes the encompassing circle for this triangle.
	void CalculateCircle();
	
	/// Returns true if one of this triangle's vertexes is t.
	bool FormedBy (PLTvec4 t);


	/// Center of the encompassing circle.
	PLTvec3 Center;

	/// Radius of the encompassing circle.
	float Radius;	
	
};


//! A plane separates the 3D space in two proper sub-spaces.
class PLTplane {

public: 

	/// Constructors.
	PLTplane();
	PLTplane( PLTvec3, PLTvec3, PLTvec3 );

	/// Distance from an arbitrary point.
	float SignedDistance(PLTvec3 point);

private: 

	/// Normal of the plane.
	PLTvec3 NormalVector;

	/// Distance from POV?
	float DistanceFromOrigin;

};






#endif