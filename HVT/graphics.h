#ifndef __GRAPHICS__
#define __GRAPHICS__

#include <GL\GL.h>
#include <GL\GLU.h>
#include <GL\glut.h>
#include <GL\wglew.h>

#include "glm\glm.hpp"
#include "glm\gtc\matrix_transform.hpp"
#include "glm\gtc\type_ptr.hpp"

#include "data.h"
#include "PLTtools.h"
#include "audio.h"
#include "status.h"
#include "PLTfbos.h"
#include "Shaders.h"
#include "Settings.h"
#include "colors.h"
#include "buttons.h"

#include <cmath>

#define open_diaphragm frames_motion_blur>2

Program* phongshading;


glm::vec3 color[10];

// camera control
glm::vec3 vel_rot(0,0,0);
glm::vec3 vel_mov(0,0,0);

// model view control
glm::mat4 modelview ;
glm::vec3 initial_translation;
glm::mat4 axis_rotat;

// automatic rotation of objects around axes
float auto_rotate_x = 0.0f;
float auto_rotate_y = 0.0f;
float auto_rotate_z = 0.0f;

// rendering variables
float t=0;
unsigned int actual_frame = 0;
unsigned int screen_width=1024;
unsigned int screen_height=768;
int current_frame = 0;
int frame_count = 0;
int snapshots = 0;
int frames_interpolation = 10;
int step_snap = 0;
bool temp_pause = false;

// spheres with GLU
GLUquadric* Q = gluNewQuadric();
GLUquadric* Qwire = gluNewQuadric();
GLUquadric* Qcilynder = gluNewQuadric();



PLTfbo* scene_fbo;
PLTfbo* accum_fbo;
PLTtexture* scene_fbo_texture;
GLuint FramebufferName;
GLuint renderedTexture;

// LoD spheres
std::vector<GLUquadric*> spheres_lod_vector;
std::vector<GLUquadric*> spheres_lod_vector2;

// sphere with VBO
GLuint ballVbo;

// Frame buffer objects
PLTfbo* screenshot_fbo;
char* path_screenshot;

// bounding box
glm::vec3 min_coord(DBL_MAX,DBL_MAX,DBL_MAX);
glm::vec3 max_coord(-DBL_MAX,-DBL_MAX,-DBL_MAX);
float maxdistance;

// display lists
GLuint displaylist_index;
GLuint dlsphere;
GLuint dlsphere_2;
GLuint displaylist_displays;
std::vector<GLuint> dl_lodspheres;
std::vector<GLuint> dl_lodspheres2;


const float width_display = 240.0f;
const float height_display = 70.0f;
float left_display = 0;
float top_display = 0;
const int margin_display_left = 60;
const int margin_display_top = 20;



void set_orthographic(bool enabled) {

	if (enabled) {

		// save previous matrix which contains the 
		//settings for the perspective projection
		glMatrixMode(GL_PROJECTION);		
		glPushMatrix();						
		glLoadIdentity();		
		gluOrtho2D(0, screen_width, 0, screen_height);

		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();

	} else {
		
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();

	};

};



void create_lod_spheres() {

	float disc = (float)(maxlod-minlod)/(lod_levels-1);

	for (unsigned int i=0; i<lod_levels; i++) {

		float local_lod = minlod+(lod_levels-(i+1))*disc;
		spheres_lod_vector.push_back( gluNewQuadric() );		
		dl_lodspheres.push_back( glGenLists(1) );
		glNewList( dl_lodspheres.back(), GL_COMPILE);
		
		gluSphere( spheres_lod_vector.back(), spheres_size, (GLint)local_lod, (GLint)local_lod );
		glEndList();
	}

	for (unsigned int i = 0; i<lod_levels; i++) {

		float local_lod = minlod + (lod_levels - (i + 1))*disc;
		spheres_lod_vector2.push_back(gluNewQuadric());
		dl_lodspheres2.push_back(glGenLists(1));
		glNewList(dl_lodspheres2.back(), GL_COMPILE);

		gluSphere(spheres_lod_vector2.back(), spheres_size*1.15, (GLint)local_lod, (GLint)local_lod);
		glEndList();
	}
}

/* Recalculates the position of the camera in world coordinates */
glm::vec3 get_position_of_camera() {
	glm::mat3 rotMat(modelview);
	glm::vec3 d(modelview[3]);
	glm::vec3 retVec = -d * rotMat;
	return retVec;
}


/* Calculate a rotation matrix according to a rotation vector */
glm::mat4 RotationMatrixFromVector(glm::vec3 v) {

	// ragionarci su TODO
	float theta = 1.0;

	glm::mat4 ret;

	
	float costheta = cos(theta*v.z);
	float sintheta = sin(theta*v.z);
	glm::mat4 mz ( costheta, sintheta, 0, 0, 
				   -sintheta, costheta, 0, 0, 
				   0, 0, 1, 0,
				   0, 0, 0, 1 );

	costheta = cos(theta*v.x);
	sintheta = sin(theta*v.x);
	glm::mat4 mx ( 1, 0, 0, 0,
				   0, costheta, -sintheta, 0, 
				   0, sintheta, costheta, 0, 
				   0, 0, 0, 1 );

	costheta = cos(theta*v.y);
	sintheta = sin(theta*v.y);
	glm::mat4 my ( costheta, 0, sintheta, 0, 
				   0, 1., 0, 0,
				   -sintheta, 0, costheta, 0, 
				   0, 0, 0, 1. );

	ret = ret*mz;
	ret = ret*mx;
	ret = ret*my;

	return ret;

};


/* TODO */
void draw_axes() {
		
}


/* Create a display list for a sphere (prevents useless primitive issues by storing the geometries on the GPU) */
void create_displaylist_sphere() {
	dlsphere = glGenLists(1);
	glNewList( dlsphere, GL_COMPILE);		
		gluSphere(Q, spheres_size, (GLint)detail_spheres, (GLint)detail_spheres);
	glEndList();

	dlsphere_2 = glGenLists(1);
	glNewList( dlsphere_2, GL_COMPILE);
		glScalef(1.1f, 1.1f, 1.1f);
		gluSphere(Q, spheres_size, (GLint)detail_spheres, (GLint)detail_spheres);
	glEndList();

}


void redefine_autorotation_matrix() {
	axis_rotat = glm::mat4();

	static float local_rotate_x = 0;
	static float local_rotate_y = 0;
	static float local_rotate_z = 0;
	
	local_rotate_x += auto_rotate_x*(frames_motion_blur);
	local_rotate_y += auto_rotate_y*(frames_motion_blur);
	local_rotate_z += auto_rotate_z*(frames_motion_blur);

	axis_rotat = glm::rotate( axis_rotat, local_rotate_x, glm::vec3(1.,0.,0.) );
	axis_rotat = glm::rotate( axis_rotat, local_rotate_y, glm::vec3(0.,1.,0.) );
	axis_rotat = glm::rotate( axis_rotat, local_rotate_z, glm::vec3(0.,0.,1.) );
	axis_rotat = glm::translate(axis_rotat, initial_translation);
}


/* Sets the field of vision */
void set_fov(float fov) {
	glMatrixMode   (GL_PROJECTION);
	glLoadIdentity ();
	gluPerspective( fov, (float)screen_width/screen_height, nplane, fplane);
	glMatrixMode (GL_MODELVIEW);
	//printf(" * New FOV: %f.\n", fov);
}


/* Applies the requested perspective */
void apply_perspective(int w, int h) {
	if(h == 0) h = 1;
	float ratio = 1.0f * w / h;
	gluPerspective(fov,ratio,nplane,fplane);
}


/* Displays text on the screen at the selected coordinates with the selected colors */
void displayText( float x, float y, float r, float g, float b, std::string stri, void* type=GLUT_BITMAP_HELVETICA_18 ) {
	int j = strlen( stri.c_str() );
	glColor3f( r, g, b );
	glRasterPos2f( x, y );
	for( int i = 0; i < j; i++ ) {
		glutBitmapCharacter( type, stri[i] );
	}
}


void reset_view() {
	//modelview = glm::translate( glm::mat4(), glm::vec3(-(min_coord.x+max_coord.x)/2,-(min_coord.y+max_coord.y)/2,-(min_coord.z+max_coord.z)/2 -10) );	
	//modelview = glm::translate( modelview, -initial_translation);
	modelview = glm::mat4();
}


/* OpenGL initialization*/
void init() {

	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_diffuse);
	//glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	glLightfv(GL_LIGHT1, GL_DIFFUSE, light2_diffuse);
	glLightfv(GL_LIGHT1, GL_SPECULAR, light2_diffuse);
	//glLightfv(GL_LIGHT1, GL_POSITION, light2_position);

	glMaterialfv(GL_FRONT, GL_SPECULAR, white_specular );
	glMaterialf (GL_FRONT, GL_SHININESS, 64.f );

	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	//glEnable(GL_LIGHTING);
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);
	glLineWidth(spheres_size * 40.f);
	glPointSize(10.0);
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);
	// glEnable(GL_DEPTH_TEST);
	
	RenderState.set_lighting(true);
	RenderState.set_depthtest(true);

	glEnable(GL_POINT_SMOOTH);
	glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);

	glMatrixMode(GL_PROJECTION);
	gluPerspective( fov, (float)screen_width/screen_height, nplane, fplane);
	glMatrixMode(GL_MODELVIEW);

	reset_view();
		
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LEQUAL);
	glDepthRange(0.0f, 1.0f);

	glEnable (GL_BLEND); 
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	 // glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

	if (use_dark_skin) {
		glClearColor( clear_color_dark->x, clear_color_dark->y, clear_color_dark->y, clear_color_dark->w);
	}
	else {
		glClearColor(clear_color_bright->x, clear_color_bright->y, clear_color_bright->y, clear_color_bright->w);
	}

	printf(" * GPU in use: %s.\n", glGetString(GL_RENDERER));
		
    GLint  iMultiSample = 0;
    GLint  iNumSamples = 0;
    glGetIntegerv(GL_SAMPLE_BUFFERS, &iMultiSample);
    glGetIntegerv(GL_SAMPLES, &iNumSamples);
	printf(" * Multisample: %d samples: %d.\n", iMultiSample, iNumSamples);


	GLint MajorVersion = 0;
	GLint MinorVersion = 0;
	glGetIntegerv(GL_MAJOR_VERSION, &MajorVersion);
	glGetIntegerv(GL_MINOR_VERSION, &MinorVersion);
	printf(" * OpenGL version: %d.%d.\n", MajorVersion, MinorVersion);

	printf(" * GLSL version: %s.\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
	
	
	GLint err = glewInit();
	if (GLEW_OK != err)
	{	 
	  fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
	  exit(101);
	}

/*
	GLint success = 0;			
	Shader newvertexshader = Shader(GL_VERTEX_SHADER);
	Shader newfragmentshader = Shader(GL_FRAGMENT_SHADER);
	newvertexshader.load_source("shaders/blinnphong.vert");
	newfragmentshader.load_source("shaders/blinnphong.frag");
	phongshading = new Program( newvertexshader, newfragmentshader );
	*/
	
/*	
	glValidateProgram(phongshading->getId());
	GLint status;
    glGetProgramiv(phongshading->getId(), GL_VALIDATE_STATUS, &status);
	if (status == GL_FALSE) {
        printf("ERROR: cannot validate shader.\n");
		getchar();
		exit(1);
	}*/
	

	// create_vbo_sphere();

	// fogging
	GLfloat* fogColor = use_dark_skin? 	(GLfloat*)clear_color_dark : (GLfloat*)clear_color_bright;
	glFogi(GL_FOG_MODE, GL_LINEAR);
	glFogfv(GL_FOG_COLOR, fogColor);   
	// glFogf(GL_FOG_DENSITY, 0.07f);           // How Dense Will The Fog Be
	glHint(GL_FOG_HINT, GL_NICEST);       // Fog Hint Value
	
	
	color[0]= glm::vec3(1,0,0);
	color[1]= glm::vec3(0,1,0);
	color[2]= glm::vec3(0,0,1);
	color[3]= glm::vec3(1,1,0);
	color[4]= glm::vec3(1,0,1);
	color[5]= glm::vec3(0,1,1);
	color[6]= glm::vec3(1,0.5,0);
	color[7]= glm::vec3(1,0.5,1);
	color[8]= glm::vec3(1,0.5,0.5);
	color[9]= glm::vec3(.5,0.5,1);
	
	create_lod_spheres();

	/*
	scene_fbo = new PLTfbo(1024,1024);
	scene_fbo->AttachColorBuffer(0);
	scene_fbo->AttachDepthBuffer();
	
	scene_fbo_texture = new PLTtexture(1024,1024,GL_TEXTURE_2D);	
	scene_fbo_texture->SetRepeat(GL_CLAMP_TO_BORDER, GL_CLAMP_TO_BORDER);
	scene_fbo_texture->SetModulation(GL_DECAL);
	scene_fbo_texture->SetFiltering(GL_LINEAR, GL_LINEAR);
	scene_fbo_texture->SetMipmaps(true);

	scene_fbo->AttachTexture(0, scene_fbo_texture);
	
	accum_fbo = new PLTfbo(screen_width, screen_height);
	accum_fbo->AttachColorBuffer(0);
	*/
	
	/*
	// generate FBO
	FramebufferName = 0;
	glGenFramebuffers(1, &FramebufferName);
	glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);

	// The texture we're going to render to	
	glGenTextures(1, &renderedTexture);

	// "Bind" the newly created texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, renderedTexture);

	// Give an empty image to OpenGL ( the last "0" )
	glTexImage2D(
		GL_TEXTURE_2D, 
		0,
		GL_RGB, 
		screen_width, 
		screen_height, 
		0,
		GL_RGB, 
		GL_UNSIGNED_BYTE, 
		0);

	// Poor filtering. Needed !
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	// The depth buffer
	GLuint depthrenderbuffer;
	glGenRenderbuffers(1, &depthrenderbuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depthrenderbuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, screen_width, screen_height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthrenderbuffer);

	// Set "renderedTexture" as our colour attachement #0
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, renderedTexture, 0);

	// Set the list of draw buffers.
	GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
	glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers
	
	// Always check that our framebuffer is ok
	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		printf("ERROR: cannot created framebuffer. \n");
		getchar();
		exit(0);
	}
	*/

	wglSwapIntervalEXT(0);

	RenderState.set_fog(false);
	RenderState.set_culling(true);

	//glLightModelf(GL_LIGHT_MODEL_LOCAL_VIEWER, 1);

	printf("*** Initialization completed.\n");


}


void apply_modelview() {

	GLdouble viewport[4];
	glGetDoublev(GL_VIEWPORT, viewport);	
	float aspect = (GLfloat)viewport[2] / (GLfloat)viewport[3];
	float top = (GLfloat)nplane / (cos(fov) / sin(fov));
	float bottom = -top;
	float right = top * aspect;
	float left = -right;
	float xwsize = right - left;
	float ywsize = top - bottom;
	float fov2 = ((fov*pi) / 180.0f) / 2.0f;
	float eyedx=0, eyedy=0;
	if (picked_depth != -1) {
		if (frames_motion_blur == 15) {
			eyedx = jitters15[frame_motion_blur].x*aperture;
			eyedy = jitters15[frame_motion_blur].y*aperture;
		}
		else if (frames_motion_blur == 8) {
			eyedx = jitters8[frame_motion_blur].x*aperture;
			eyedy = jitters8[frame_motion_blur].y*aperture;
		}
		else if (frames_motion_blur == 24) {
			eyedx = jitters24[frame_motion_blur].x*aperture;
			eyedy = jitters24[frame_motion_blur].y*aperture;
		}
	}

	float dx,dy;
	static float newdepth = 10;

	if (picked_depth!=-1) {
		newdepth += (picked_depth-newdepth)*2e-2f;
		dx = -(eyedx*nplane/newdepth);
		dy = -(eyedy*nplane/newdepth);
	} else {
		dx = -(eyedx*nplane/dist_focus);
		dy = -(eyedy*nplane/dist_focus);
	}
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(left + dx, right + dx, bottom + dy, top + dy, nplane, fplane);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

		glTranslatef(		
		-eyedx, 
		-eyedy, 
		0
	);

	glMultMatrixf( glm::value_ptr( modelview ) );
				
}


#endif