#include "stdafx.h" 

#include "GL/gl.h"
#include "glm\glm.hpp"
#include "glm\gtc\type_ptr.hpp"

// #include "renderer.h"

#include "PLTtools.h"
// #include "PLTgeometry.h"
// #include "PLTopengl.h"
// #include "PLTmath.h"

#include <string>
#include <sstream>


using namespace std;

extern glm::mat4 modelview;


PLTlegend::PLTlegend() {
	this->Steps = 10; 
	this->Position.X = 10; 
	this->Position.Y = 230; 
	this->Size = 20; 
	// this->Length = this->Steps/30+0.005; 
	this->Length = 160; 
	this->Horizontal = true; 
}



void PLTlegend::Render() {

	ostringstream oss_max, oss_min, oss_cap;

	oss_max << "max (" << this->Max << ")";
	oss_min << "min (" << this->Min << ")";
	oss_cap << this->Caption;

	glPushMatrix(); 

		glLoadIdentity();

		glPushAttrib(GL_ENABLE_BIT | GL_DEPTH_BUFFER_BIT);

			glDisable(GL_LIGHTING);
			glDisable(GL_DEPTH_TEST);
		
			// glOrtho(-1, 1, -1, 1, -1, 1);

			double pezzo = this->Length/this->Steps;

			glBegin(GL_QUADS);
			for (unsigned int i=0; i<this->Steps; i++) {

//				glColor3dv(ReturnRGB(i, this->Steps, 0));
				
				if (this->Horizontal) {
					glVertex3d(this->Position.X+i,		this->Position.Y, 0.1 );
					glVertex3d(this->Position.X+i, 		this->Position.Y+this->Size, 0.1 );				
				} else {						
					glVertex3d(this->Position.X,				pezzo*i+this->Position.Y, 0.1 );
					glVertex3d(this->Position.X+this->Size,		pezzo*i+this->Position.Y, 0.1 );
					glVertex3d(this->Position.X+this->Size,		pezzo*i+this->Position.Y+pezzo, 0.1 );
					glVertex3d(this->Position.X,				pezzo*i+this->Position.Y+pezzo, 0.1 );										
				}
						
			}
			glEnd();
			
			glColor3d(.8, .8, .8);		

			if (this->Horizontal) {
				glRasterPos3d(this->Position.X+(float)this->Steps/3000, this->Position.Y-this->Size, 0.1);								
				// NeHePrint(1, oss_max.str().c_str());				
				glRasterPos3d(this->Position.X-0.01, this->Position.Y-this->Size, 0.1);								
				//NeHePrint(1, oss_min.str().c_str());
				glRasterPos3d(this->Position.X-0.01, this->Position.Y+this->Size+0.01, 0.1);								
				//NeHePrint(0, oss_cap.str().c_str());
			} else {				
				glRasterPos3d(this->Position.X, this->Position.Y+this->Length+5, 0.1);								
				// NeHePrint(1, oss_max.str().c_str());				
				glRasterPos3d(this->Position.X, this->Position.Y-10, 0.1);								
				//NeHePrint(1, oss_min.str().c_str());
				glRasterPos3d(this->Position.X, this->Position.Y+this->Length+20, 0.1);								
				//NeHePrint(0, oss_cap.str().c_str());
			}
		
		glPopAttrib();	// lighting, depth test
		
	glPopMatrix();

};



void PLTgyroscope::Render() {

	// glMultMatrixf(RotationMatrixFromVector(*this->RotationVector));		

	glm::mat3 rotMat(  modelview  );
	
		
		
		glLoadIdentity();
		glMultMatrixf( glm::value_ptr(rotMat)  );		

		glBegin(GL_LINES);	
			
			// asse x (verde)
			glColor3f(0.2f,1.0f,.2f);	glVertex3d(0,0,0); glVertex3d(20,0,0);				

			// asse y (rosso)
			glColor3f(1.0f,.2f,.2f);	glVertex3d(0,20,0); glVertex3d(0,0,0);

			// asse z (blu)
			glColor3f(0.2f,.2f,1.0f);	glVertex3d(0,0,0); glVertex3d(0,0,20);
				
		glEnd();

		// freccia asse x (verde)
		glPushMatrix();				
			glColor3f(0.2f,1.0f,.2f);		
			glTranslated (20,0,0);
			glRotated(90,0,1,0);		
			gluCylinder( arrow, 5, 0, 10, 20, 20);
		glPopMatrix();

		// freccia asse y (rosso)
		glPushMatrix();				
			glColor3f(1.0f,.2f,.2f);															
			glTranslated(0,20,00);						
			glRotated(-90,1,0,0);								
			gluCylinder( arrow, 5, 0, 10, 20, 20);
		glPopMatrix();
		
		// freccia asse z (blu)
		glPushMatrix();	
			glColor3f(0.2f,.2f,1.0f);		
			glTranslated (0,0,20);
			glRotated(90,0,0,1);	
			gluCylinder( arrow, 5, 0, 10, 20, 20);
		glPopMatrix();
		
		/*
		glColor3f(.5,.5,.5);
		glRasterPos3d(35,0,0);								
		NeHePrint(1, "x");	
		glRasterPos3d(0,35,0);								
		NeHePrint(1, "y");	
		glRasterPos3d(0,0,35);								
		NeHePrint(1, "z");	
		*/

	glPopMatrix();


};


