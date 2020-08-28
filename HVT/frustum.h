#pragma once
#ifndef __FRUSTUM__
#define __FRUSTUM__

// #include <GL\freeglut.h>
#include "glm\glm.hpp"

double** frustum;

void create_frustum() {

	frustum = (double**)malloc(sizeof(double*) * 6);

	for (unsigned int i = 0; i < 6; i++) {
		double* piece = (double*) malloc(sizeof(double) *4);
		frustum[i] = piece;
	}

}

bool testpoint(glm::vec3 x)
{
	int i;
	float distance;

	for (i = 0; i < 6; i++)
	{
		distance = (GLfloat) ( (frustum[i][0] * x.x) + (frustum[i][1] * x.y) + (frustum[i][2] * x.z) + frustum[i][3] );
		if (distance < -5e-1)
			return false;
	}
	return true;
}

void extractfrustum()
{
	int i;
	GLfloat proj[16];
	GLfloat modl[16];
	double clip[16];
	float m0, m1, m2, m3;

	// Get current matricies
	glGetFloatv(GL_PROJECTION_MATRIX, proj);
	glGetFloatv(GL_MODELVIEW_MATRIX, modl);

	// Combine them
	for (i = 0; i < 4; i++)
	{
		m0 = modl[i * 4 + 0];
		m1 = modl[i * 4 + 1];
		m2 = modl[i * 4 + 2];
		m3 = modl[i * 4 + 3];
		clip[i * 4 + 0] = m0 * proj[0] + m1 * proj[4] + m2 * proj[8] + m3 * proj[12];
		clip[i * 4 + 1] = m0 * proj[1] + m1 * proj[5] + m2 * proj[9] + m3 * proj[13];
		clip[i * 4 + 2] = m0 * proj[2] + m1 * proj[6] + m2 * proj[10] + m3 * proj[14];
		clip[i * 4 + 3] = m0 * proj[3] + m1 * proj[7] + m2 * proj[11] + m3 * proj[15];
	}

	frustum[0][0] = clip[3] - clip[0];
	frustum[0][1] = clip[7] - clip[4];
	frustum[0][2] = clip[11] - clip[8];
	frustum[0][3] = clip[15] - clip[12];

	frustum[1][0] = clip[3] + clip[0];
	frustum[1][1] = clip[7] + clip[4];
	frustum[1][2] = clip[11] + clip[8];
	frustum[1][3] = clip[15] + clip[12];

	frustum[2][0] = clip[3] + clip[1];
	frustum[2][1] = clip[7] + clip[5];
	frustum[2][2] = clip[11] + clip[9];
	frustum[2][3] = clip[15] + clip[13];

	frustum[3][0] = clip[3] - clip[1];
	frustum[3][1] = clip[7] - clip[5];
	frustum[3][2] = clip[11] - clip[9];
	frustum[3][3] = clip[15] - clip[13];

	frustum[4][0] = clip[3] - clip[2];
	frustum[4][1] = clip[7] - clip[6];
	frustum[4][2] = clip[11] - clip[10];
	frustum[4][3] = clip[15] - clip[14];

	frustum[5][0] = clip[3] + clip[2];
	frustum[5][1] = clip[7] + clip[6];
	frustum[5][2] = clip[11] + clip[10];
	frustum[5][3] = clip[15] + clip[14];
}

#endif