#ifndef __PLTcameras__
#define __PLTcameras__

// #include "PLTgeometry.h"
#include "glm\glm.hpp"

// Must define it before.
class PLTcamera;




/// The "camera" defines the viewer's POV.
/*!
	This class is useful for shadow mapping as well, handling light's POV.
*/
class PLTcamera  {

public:

	void Translate(glm::vec3 v);
	void Rotate(glm::vec3 r);

	/// Camera's position.
	glm::vec3 Position;

	/// Camera's "lookat" point.
	glm::vec3 Destination;

	/// Camera's zenith vector.
	glm::vec3 ZenithVector;

	//@{
	///@name Near and far planes for frustum culling. 
	float NearPlane;
	float FarPlane;
	//@}
	
	/// Field of vision.
	float FOV;

	//@{
	///@name Constructors.
	PLTcamera(unsigned int n) {
		this->AspectRatio = 1.0;
		
		this->Position=glm::vec3( 0.0f , 0.0f , 1.0f ) ;
		this->Destination=glm::vec3( 0.0f , 0.0f , 0.0f ); 

		this->FOV = 90; 
		this->NearPlane = 0.1f; 
		this->FarPlane = 100.0f; 
		ZenithVector=glm::vec3(0.0f , 1.0f , 0.0f); 
		this->CameraID=n; 
		gluLookAt(	Position.x, Position.y, Position.z,
					Destination.x, Destination.y, Destination.z,
					ZenithVector.x, ZenithVector.y, ZenithVector.z);
	
		// Frustum = new PLTviewFrustum();		
		// Frustum->CalculateViewingFrustum( GetPosition(), Destination, this->NearPlane, this->FarPlane, this->FOV);
		// Frustum->CalculateViewingFrustum( this );

		}; 
	//@}

	/// Applies the changes done.
	void Apply();	
	void ApplyMath();

	//@{
	///@name Sets the perspective and the clipping planes for this camera.		
	void SetPerspective(float);
	void SetPerspective(float,float);
	void SetPerspective(float,float,float);
	void ActualSetPerspective();
	//@}

	//@{
	///@name Sets the point of view of the current camera.	
	void SetPOV(glm::vec3, glm::vec3);
	//@}

	/// ID for the current camera, returned by Environment (TODO).
	unsigned int CameraID;

	/// Pointer to the rotation matrix.
	glm::vec3* PointerToRotationVector() { return &this->AbsoluteRotation; };



//private:

	/// When a modify is done, this flag is set to true.
	bool Changed;

	/// Matrix of spatial transformations.
	glm::mat4 CameraMatrix;

	/// Matrix of rotation.
	glm::mat4 RotationMatrix;

	/// Matrix of Translation.
	glm::mat4 TranslationMatrix;

	/// Vectors of camera movement.
	glm::vec3 TranslationVector, RotationVector;

	/// Vector of absolute rotation.
	glm::vec3 AbsoluteRotation, AbsolutePosition;

	/// Quenches down the movement.
	void QuenchMovement();

	/// Viewing frustum for this camera.
	//PLTviewFrustum* Frustum;

	/// width:height.
	float AspectRatio;



};





#endif