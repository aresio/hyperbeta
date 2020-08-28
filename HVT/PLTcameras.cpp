#include "stdafx.h"
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include "PLTcameras.h"
#include "PLTtweaks.h"
#include "PLTmath.h"

#include <math.h>
#include <iostream>
#include <string>

using namespace std;



void PLTcamera::Apply() {
	glMultMatrixf( this->CameraMatrix );
	QuenchMovement();
};



void PLTcamera::Translate(PLTvec3 v)  { 		
	this->TranslationVector += v;

	this->AbsolutePosition += v;
	
	this->TranslationMatrix = TranslationMatrixFromVector( this->TranslationVector );
	this->TranslationMatrix.Multiply( this->CameraMatrix );

	this->CameraMatrix = this->TranslationMatrix;

	Changed = true;
};



void PLTcamera::Rotate(PLTvec3 r) {
	this->RotationVector += r;
	
	this->AbsoluteRotation += r;

	this->RotationMatrix = RotationMatrixFromVector( -this->RotationVector );
	this->RotationMatrix.Multiply( this->CameraMatrix );

	this->CameraMatrix = this->RotationMatrix;

	Changed = true;
};



void PLTcamera::SetPerspective(float f) {
	// TODO: compute the aspect ratio		
	FOV = f;
	this->ActualSetPerspective();
}
void PLTcamera::SetPerspective(float n, float f) {
	// TODO: compute the aspect ratio	
	NearPlane = n;
	FarPlane = f;	
	ActualSetPerspective();
}
void PLTcamera::SetPerspective(float fo, float n, float f) {
	// TODO: compute the aspect ratio	
	FOV = fo;
	NearPlane = n;
	FarPlane = f;
	ActualSetPerspective();
}
void PLTcamera::ActualSetPerspective() {
	cout << " * camera " << this->CameraID << "'s fov: " << FOV << endl;
	// this->Frustum->CalculateViewingFrustum( GetPosition(), Destination, this->NearPlane, this->FarPlane, this->FOV);

	// this->Frustum->CalculateViewingFrustum( this );
	
	gluPerspective(FOV, AspectRatio, NearPlane, FarPlane);				
	Changed=true;
};



/*

void PLTcamera::SetPOV(PLTvec3 p, PLTvec3 d) {
	
	SetPosition ( p );
	Destination = d;	

	Frustum->CalculateViewingFrustum( GetPosition, Destination, this->NearPlane, this->FarPlane, this->FOV);

	Changed=true;
	// TODO: zenith	
}
*/


void PLTcamera::QuenchMovement() { 
	// restart movement.
	this->TranslationVector = PLTvec3(0,0,0);
	this->RotationVector = PLTvec3(0,0,0); 
};



void PLTviewFrustum::CalculateViewingFrustum( PLTcamera* c) {

	this->NearDistance = c->NearPlane;
	this->FarDistance = c->FarPlane;

	float ratio = c->AspectRatio;		

	this->NearPlaneSize.Y = 2.0f * tanf( PLT_DEG2RAD * c->FOV * 0.5f ) * c->NearPlane;
	this->NearPlaneSize.X = this->NearPlaneSize.Y * c->AspectRatio;

	this->FarPlaneSize.Y  = 2.0f * tanf( PLT_DEG2RAD * c->FOV * 0.5f ) * c->FarPlane;
	this->FarPlaneSize.X  = this->FarPlaneSize.Y * c->AspectRatio;

	cout << "   frustum - ";

	printf ("near: (%f, %f), far: (%f, %f)\n", this->NearPlaneSize.X, this->NearPlaneSize.Y, this->FarPlaneSize.X, this->FarPlaneSize.Y);

};



bool PLTviewFrustum::Contains( PLTentity* e ) {
	
	return this->Contains( e->GetBoundingSphereCenter() );

}




bool PLTviewFrustum::Contains( PLTvec3 p ) {
	
	// per ognuno dei piani del frustum
	for (unsigned int i=0; i<6; i++) {
		if ( this->Planes[i].SignedDistance ( p ) <0 )	
			return false;
	}
	return true;

}