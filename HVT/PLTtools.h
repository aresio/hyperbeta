#ifndef __PLTtools__
#define __PLTtools__

#include <vector>

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include "glm/glm.hpp"

struct PLTcolor;

struct PLTvec3;

class PLTenvironment;



class PLTlegend {

public:

	/// Number of steps to visualize.
	double Steps;
	
	/// Position in space.
	struct Pos {
		double X, Y;
	} Position;

	/// Width of the legenda.
	double Size;

	/// Length of the legenda.
	double Length;

	/// Minimum and maximum values of the scale.
	double Min, Max;

	/// Default constructor.
	PLTlegend();

	/// Renders this legend.
	void Render(); 

	/// Style of the legend.
	bool Horizontal;

	/// Holds the caption for this legend.
	char * Caption;

private:

	
};




class PLTgyroscope {

public:

	PLTgyroscope() { arrow = gluNewQuadric(); };

	void Render();

	GLUquadric* arrow;

	glm::vec3* RotationVector;

};




static float Random() {
	return (float)rand()/RAND_MAX;
};



#endif 
