#ifndef __VBO__
#define __VBO__

#include <GL\GL.h>
#include <GL\glut.h>
#include <GL/glew.h>


#define Particles 32

GLuint geometry_array = 0;
GLuint color_array    = 0;
GLuint indices_array  = 0;

GLfloat geometry[Particles*3];	// 3 vertices per particle
GLfloat partcolors[Particles*3];	// 3 values per particle
GLuint indices[Particles];		// 1 index per particle


void update_vbo() {

	for (unsigned int i=0; i<Particles; i++) {
		geometry[i*3] = 3*sin(t/100+i);
		geometry[i*3+1] = 3*sin(t/50+i*2);;
		geometry[i*3+2] = 3*sin(t/200+i*3);;
	}

	for (unsigned int i=0; i<Particles; i++) {
		partcolors[i*3] = (float)i/Particles;
		partcolors[i*3+1] = 0;
		partcolors[i*3+2] = .5;
	}
		
	glBindBuffer(GL_ARRAY_BUFFER, geometry_array);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 3 * Particles, NULL, GL_STREAM_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat)*3*Particles, geometry);

	for (unsigned int i=0; i<Particles; i++) {
		geometry[i*3] = 10.f*sin(t/100+i);
		geometry[i*3+1] = (GLfloat)i;
		geometry[i*3+2] = (GLfloat)i;
	}
	
	glBindBuffer(GL_ARRAY_BUFFER, color_array);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 3 * Particles, NULL, GL_STREAM_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat)*3*Particles, partcolors);

}



void initialize_vbo() {
	
	for (unsigned int i=0; i<Particles; i++) {
		indices[i] = i;
	}

	update_vbo();


	// The VBO containing the positions and sizes of the particles	
	glGenBuffers(1, &geometry_array);
	
	glGenBuffers(1, &indices_array);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_array);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*Particles, NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(GLuint)*Particles, indices);
		
}


void vbo_render() {

	//Render
	// Step 1
	glBindBuffer(GL_ARRAY_BUFFER, geometry_array);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_array);

	// Step 2
	//glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	//glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

	// Step 3
	// glTexCoordPointer(3, GL_FLOAT, sizeof(GLfloat)*3, (float*)(sizeof(GLfloat)*5));
	// glNormalPointer(GL_FLOAT, sizeof(GLfloat)*8, (float*)(sizeof(GLfloat)*3));
	glVertexPointer(3, GL_FLOAT, sizeof(GLfloat)*3, NULL);
	glColorPointer(3, GL_FLOAT, sizeof(GLfloat)*3, NULL);

	// Step 4
	glDrawElements(GL_POINTS, Particles, GL_UNSIGNED_INT, NULL);

	// Step 5
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	//glDisableClientState(GL_NORMAL_ARRAY);
	//glDisableClientState(GL_TEXTURE_COORD_ARRAY);

}


#endif