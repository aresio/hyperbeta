#include "stdafx.h" 
#include <GL/glew.h>
#include <GL/gl.h>

#include <PLTmath.h>
#include <PLTmeshes.h>
#include "PLTgeometry.h"
#include "PLTvectors.h"
#include <PLTshaders.h>


using namespace std;



bool PLTtriangle::FormedBy (PLTvec4 t) {

	/*cout << "----------------TRIANGOLO----------------" << endl;
	cout << "Vertice: " << t << endl;*/

	if ( t == this->Vertexes[0] ) return true;

	// cout << this->Vertexes[0] << endl;

	if ( t == this->Vertexes[1] ) return true;

	// cout << this->Vertexes[1] << endl;

	if ( t == this->Vertexes[2] ) return true;

	// cout << this->Vertexes[2] << endl;

	return false;
}





bool PLTtriangle::operator<(const PLTtriangle& tri) const {
	return this->Center.X < tri.Center.X;
}



bool PLTtriangle::OnTheLeft(PLTvec4 v) {
	return this->Center.X + this->Radius < v.X;
}



PLTtriangle::PLTtriangle(PLTvec4 p1, PLTvec4 p2, PLTvec4 p3) {
	this->Vertexes[0]=p1; this->Vertexes[1]=p2; this->Vertexes[2]=p3;
	CalculateCircle();
	// cout << "Creato triangolo " << this->Center.X << " / " << this->Radius << endl;
}


void PLTtriangle::CalculateCircle() {


	// valori di ritorno dei determinanti
	float m11, m12, m13, m14;

	// Matrici 3x3 
	PLTmat3 A;
	
	// troviamo M_11/
	for (int i=0; i<3; i++) {
		A.Set(0,i, this->Vertexes[i].X);
		A.Set(1,i, this->Vertexes[i].Y);
		A.Set(2,i, 1);
	}
	m11 = (float)A.Determinant();

	// troviamo M_12
	for (int i=0; i<3; i++) {
		A.Set(0,i, this->Vertexes[i].X * this->Vertexes[i].X + this->Vertexes[i].Y * this->Vertexes[i].Y );
		A.Set(1,i, this->Vertexes[i].Y );
		A.Set(2,i, 1);
	}
	m12 = (float)A.Determinant();

	// troviamo M_13
	for (int i=0; i<3; i++) {
		A.Set(0,i, this->Vertexes[i].X * this->Vertexes[i].X + this->Vertexes[i].Y * this->Vertexes[i].Y );
		A.Set(1,i, this->Vertexes[i].X );
		A.Set(2,i, 1);
	}
	m13 = (float)A.Determinant();

	// troviamo M_14	
	for (int i=0; i<3; i++) {
		A.Set(0,i, this->Vertexes[i].X * this->Vertexes[i].X + this->Vertexes[i].Y * this->Vertexes[i].Y );
		A.Set(1,i, this->Vertexes[i].X );
		A.Set(2,i, this->Vertexes[i].Y );
	}
	m14 = (float)A.Determinant();
	


	// se M_11 è uguale a zero, allora  i punti sono allineati e non esistono cerchi
	if (m11 == 0) {
		cout << " * ERROR: points aligned - no circles possible" << endl;
		this->Radius = 0;
		return;
	}

	this->Center.X = 0.5f * m12 / m11;
	this->Center.Y = -0.5f * m13 / m11;

	// what follows is a distance. should it be computed in some other way??
	//this->Radius = sqrt( this->Center.X * this->Center.X + this->Center.Y * this->Center.Y + m14/m11 );
	this->Radius = SquareRootFloat( this->Center.X * this->Center.X + this->Center.Y * this->Center.Y + m14/m11 );

	 
	/*cout << " * Circle from triangle created: center in " << this->Center.X << ", " << this->Center.Y;
	cout << " and radius of " << this->Radius << endl;*/
}



bool PLTtriangle::ContainsVertex(PLTvec4 v) {
	return ( PLT2DpointsDistance( v, this->Center ) <= this->Radius );
}





double* ReturnRGB(double valore, double max_value, double min_value) {

	double coeff =  (valore-min_value)*PI/(max_value-min_value);
	
	double tupla[3];

	tupla[0] = max(0.0f,cos(coeff));
	tupla[1] = max(0.0f,cos(coeff-PI*0.5));
	tupla[2] = max(0.0f,sin(coeff-PI*0.5));

	return tupla;
}


const float* ReturnRGBf(float valore, float max_value, float min_value) {

	float coeff =  (valore-min_value)*PIF/(max_value-min_value);

	float tupla[3];

	/*tupla[0] = max(0.0f,cosf(coeff));	
	tupla[1] = max(0.0f,cosf(coeff-PIF*0.5f));
	tupla[2] = max(0.0f,sinf(coeff-PIF*0.5f));*/

	tupla[0] = cosf(coeff);	
	tupla[1] = cosf(coeff-PIF*0.5f);
	tupla[2] = sinf(coeff-PIF*0.5f);
		
	return tupla;
}



void NewReturnRGBf( float v, float min, float max, float* ret ) {
	float coeff =  (v-min)*PIF/(max-min);
	ret[0] = max(0.0f,cosf(coeff));	
	ret[1] = max(0.0f,cosf(coeff-PIF*0.5f));
	ret[2] = max(0.0f,sinf(coeff-PIF*0.5f));
}


void NewReturnGrayf( float v, float min, float max, float* ret ) {
	float coeff =  (v-min)*PIF/(max-min);
	ret[0]=ret[1]=ret[2]=1-coeff+.1;
}


/// perché è quì?!
bool PLTvec3::operator<(const PLTvec3& v) const {
	if (this->X< v.X)	return true;
	if (this->X> v.X)	return false;
	if (this->Y< v.Y)	return true;
	if (this->Y> v.Y)	return false;
	return (this->Z < v.Z);
}






bool PLTmesh::SetAttribute(char* nome, int size, const GLvoid* pointer, int data_type, bool normalized) {

	GLint loc = glGetAttribLocation(this->Shader->GetProgramID(), nome );
	//		GLint loc = glGetAttribLocation(this->Shader->GetProgramID(), "tangent" );
	
		// save the location
		this->int_att[ nome ] = loc;	
	
		// should check TODO
		glEnableVertexAttribArray(loc);

		// bind data
		glVertexAttribPointer( loc, size, data_type, normalized, 0, pointer );

	return loc!=-1;

};



PLTplane::PLTplane(PLTvec3 p0, PLTvec3 p1, PLTvec3 p2)  {

	PLTvec3 v = p1-p0;
	PLTvec3 u = p2-p0;

	PLTvec3 n = PLTcrossProduct( v,u );
	
	this->NormalVector = Normalize(n);

	this->DistanceFromOrigin = - PLTDotProduct( this->NormalVector, p0 );

};



float PLTplane::SignedDistance( PLTvec3 point )  {

	return PLTDotProduct( this->NormalVector , point ) - this->DistanceFromOrigin ;	

}



PLTplane::PLTplane() {
	// ?
};
