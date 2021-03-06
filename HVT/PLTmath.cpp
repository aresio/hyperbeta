#include "stdafx.h"

#include <math.h>

#include "PLTgeometry.h"
// #include "PLTpoints.h"


double PLTangleFromVector(PLTvec3d v1, PLTvec3d v2) {
	return atan2(v2.Y-v1.Y, v2.X-v1.X); 
};


unsigned int GreaterPowerOf2(int n) {

	unsigned int t=2;
	while ( (unsigned int)n > t )	t*=2;
	return t;

};


PLTmat4 RotationMatrixFromVector(PLTvec3 v) {

	// ragionarci su TODO
	float theta = 1.0;

	PLTmat4 ret;
	
	ret.LoadIdentity();

	float costheta = cos(theta*v.Z);
	float sintheta = sin(theta*v.Z);
	PLTmat4 mz = { costheta, sintheta, 0, 0, 
				   -sintheta, costheta, 0, 0, 
				   0, 0, 1, 0,
				   0, 0, 0, 1 };

	costheta = cos(theta*v.X);
	sintheta = sin(theta*v.X);
	PLTmat4 mx = { 1, 0, 0, 0,
				   0, costheta, -sintheta, 0, 
				   0, sintheta, costheta, 0, 
				   0, 0, 0, 1 };

	costheta = cos(theta*v.Y);
	sintheta = sin(theta*v.Y);
	PLTmat4 my = { costheta, 0, sintheta, 0, 
				   0, 1, 0, 0,
				   -sintheta, 0, costheta, 0, 
				   0, 0, 0, 1 };

	ret.Multiply(mz);
	ret.Multiply(mx);
	ret.Multiply(my);

	return ret;

};



PLTmat4 TranslationMatrixFromVector(PLTvec3 v) {

	PLTmat4 TempM;

	// creiamo la traslazione (punto 2)
	TempM.LoadIdentity();
	TempM[12]=v.X; 
	TempM[13]=v.Y;  
	TempM[14]=v.Z;	

	return TempM;

};




PLTmat4 ScaleMatrixFromVector(PLTvec3 v) {

	PLTmat4 TempM;

	// creiamo lo scaling (punto 2)
	TempM.LoadIdentity();
	TempM[0]=v.X; 
	TempM[5]=v.Y;  
	TempM[10]=v.Z;	
	TempM[15]=1.0f;

	return TempM;

};




float Q_rsqrt( float number )  {
  long i;
  float x2, y;
  const float threehalfs = 1.5F;

  x2 = number * 0.5F;
  y  = number;
  i  = * ( long * ) &y;  // evil floating point bit level hacking
  i  = 0x5f3759df - ( i >> 1 ); // what the fuck?
  y  = * ( float * ) &i;
  y  = y * ( threehalfs - ( x2 * y * y ) ); // 1st iteration
  // y  = y * ( threehalfs - ( x2 * y * y ) ); // 2nd iteration, this can be removed

  #ifndef Q3_VM
  #ifdef __linux__
    assert( !isnan(y) ); // bk010122 - FPE?
  #endif
  #endif
  return y;
}

float SquareRootFloat(float number) {
    long i;
    float x, y;
    const float f = 1.5F;

    x = number * 0.5F;
    y  = number;
    i  = * ( long * ) &y;
    i  = 0x5f3759df - ( i >> 1 );
    y  = * ( float * ) &i;
    y  = y * ( f - ( x * y * y ) );
    y  = y * ( f - ( x * y * y ) );
    return number * y;
}
double SquareRootDouble(double number) {
    long  i;
    double x, y;
    const double f = 1.5F;

    x = number * 0.5F;
    y  = number;
    i  = * ( long  * ) &y;
    i  = 0x5f3759df - ( i >> 1 );
    y  = * ( double * ) &i;
    y  = y * ( f - ( x * y * y ) );
    y  = y * ( f - ( x * y * y ) );
    return number * y;
}



float PLT2DpointsDistance(PLTvec3 v1, PLTvec3 v2) { return SquareRootFloat( pow(v2.X-v1.X, 2) + pow(v2.Y-v1.Y, 2) ); };
double PLT2DpointsDistance(PLTvec3d v1, PLTvec3d v2) { return SquareRootDouble( pow(v2.X-v1.X, 2) + pow(v2.Y-v1.Y, 2) ); };

float PLT3DpointsDistance(PLTvec3 v1, PLTvec3 v2) { return SquareRootFloat( pow(v2.X-v1.X, 2) + pow(v2.Y-v1.Y, 2) + pow(v2.Z-v1.Z, 2) ); };
double PLT3DpointsDistance(PLTvec3d v1, PLTvec3d v2) { return SquareRootDouble( pow(v2.X-v1.X, 2) + pow(v2.Y-v1.Y, 2) + pow(v2.Z-v1.Z, 2) ); };

float PLT3DpointsSquaredDistance(PLTvec3 v1, PLTvec3 v2) { return pow(v2.X-v1.X, 2) + pow(v2.Y-v1.Y, 2) + pow(v2.Z-v1.Z, 2) ; };
double PLT3DpointsSquaredDistance(PLTvec3d v1, PLTvec3d v2) { return pow(v2.X-v1.X, 2) + pow(v2.Y-v1.Y, 2) + pow(v2.Z-v1.Z, 2) ; };



/// Vectorial product. (a2b3 − a3b2, a3b1 − a1b3, a1b2 − a2b1).
PLTvec3 PLTcrossProduct(PLTvec3 a, PLTvec3 b) { return PLTvec3( a.Y*b.Z-a.Z*b.Y, a.Z*b.X-a.X*b.Z, a.X*b.Y-a.Y*b.X );  };

/// Scalar product.
float PLTDotProduct(PLTvec3 a, PLTvec3 b) { return a.X*b.X+a.Y*b.Y+a.Z*b.Z;  };



PLTvec3 Normalize(PLTvec3 v) {
	float magnitude = SquareRootFloat( pow(v.X, 2)+pow(v.Y, 2)+pow(v.Z,2) );
	return PLTvec3(v.X/magnitude, v.Y/magnitude, v.Z/magnitude);
};

PLTvec2 Normalize(PLTvec2 v) {
	float magnitude = SquareRootFloat( pow(v.X, 2)+pow(v.Y, 2) );
	return
		magnitude == 0?	PLTvec2(0,0) : PLTvec2(v.X/magnitude, v.Y/magnitude);
};