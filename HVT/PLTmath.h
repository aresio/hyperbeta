#ifndef __PLTmath__
#define __PLTmath__

#include <math.h>
#include <PLTvectors.h>

#define sign2(x) (( x > 0 ) - ( x < 0 ))



/**
	Returns the angle between x axis and a direction vector described by two positions.
*/
double PLTangleFromVector(PLTvec3d v1, PLTvec3d v2);


/**
	Returns the distance, with Euclidean metric, between two points in space.
*/
double PLT3DpointsDistance(PLTvec3d v1, PLTvec3d v2);
float PLT3DpointsDistance(PLTvec3 v1, PLTvec3 v2);
double PLT3DpointsSquaredDistance(PLTvec3d v1, PLTvec3d v2);
float PLT3DpointsSquaredDistance(PLTvec3 v1, PLTvec3 v2);


/**
	Returns the distance with Euclidean metric.
*/
/*static float PLT2DpointsDistance(PLTvec3 v1, PLTvec3 v2) { return sqrt( pow(v2.X-v1.X, 2) + pow(v2.Y-v1.Y, 2) ); };
static double PLT2DpointsDistance(PLTvec3d v1, PLTvec3d v2) { return sqrt( pow(v2.X-v1.X, 2) + pow(v2.Y-v1.Y, 2) ); };*/
float PLT2DpointsDistance(PLTvec3 v1, PLTvec3 v2);
double PLT2DpointsDistance(PLTvec3d v1, PLTvec3d v2);

/**
	Takes a rotation vector and returns the associate homogeneous matrix.
*/
PLTmat4 RotationMatrixFromVector(PLTvec3 v);


/**
	Takes a translation vector and returns the associate homogeneous matrix.
*/
PLTmat4 TranslationMatrixFromVector(PLTvec3 v);


/**
	Takes a scale vector and returns the associate homogeneous matrix.
*/
PLTmat4 ScaleMatrixFromVector(PLTvec3 v);


/**
	Takes a number and returns the smallest-greater power of 2 number.
*/
unsigned int GreaterPowerOf2(int);



/**
	Carmack's fast inverse square root functions. eg.: (float)(1.0/sqrt(number), sqrt(number)*number.
*/
float Q_rsqrt( float number );
float SquareRootFloat(float number);
double SquareRootDouble(double number);


PLTvec3 PLTcrossProduct(PLTvec3 a, PLTvec3 b);
float   PLTDotProduct(PLTvec3 a, PLTvec3 b);


PLTvec2 Normalize(PLTvec2 v);
PLTvec3 Normalize(PLTvec3 v);


#endif
