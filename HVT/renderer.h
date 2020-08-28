#ifndef __RENDERER__
#define __RENDERER__

#include <vector>
#include <algorithm>
#include <numeric>
#include <unordered_set>

#include "keyboard.h"
#include "mouse.h"
#include "graphics.h"
#include "loader.h"
#include "frustum.h"
#include "Settings.h"
#include "displays.h"
#include "colors.h"

#include <GL\GL.h>
#include "colorconversion\colorspace.h"

#define SOUND_GUI "sounds/click.wav"
#define SOUND_NORMAL "sounds/click.wav"
#define SOUND_ANORMAL1 "sounds/fx-01.wav"
#define SOUND_ANORMAL2 "sounds/fx-01.wav"
#define SOUND_SLOW "sounds/descending.wav"
#define SOUND_FAST "sounds/ascending.wav"


enum { RENDER_MODE, SELECTION_MODE, SELECTION_MODE2, SELECTION_MODE3 };

#define NO_PICK 16777215

int pickedID  = NO_PICK;
int right_pickedID = NO_PICK;

glm::vec3 followed_object;

const float poljitter = 3e-2f;
const float max_lod_dist = 50;
const float min_lod_dist = 0;

GLint selected_grain = -1;
GLint sel_compo = -1;


unsigned int rendered_frames = 0;


void pause_animation() {
	temp_pause = !temp_pause;
	BHandl.set_option("pause", temp_pause);
};
void positive_throttle_animation() { 
	temp_pause = false;
	step_snap++; 
}
void negative_throttle_animation() { 
	step_snap--; 
	temp_pause = false;
}
void rewind_animation() { 
	current_frame = 0; step_snap = 0; 
};
void gotoend_animation() { 
	step_snap = 0; current_frame = point_clouds.size(); 
}


inline unsigned int const get_frame_to_render(bool inverted) {
	return 
		inverted? 
		point_clouds.size()-actual_frame-1 : actual_frame;
}


glm::vec4 id_to_color (unsigned int id) {
	int r = (id & 0x000000FF) >>  0;
	int g = (id & 0x0000FF00) >>  8;
	int b = (id & 0x00FF0000) >> 16;
	return glm::vec4(r/255., g/255., b/255., 1.0);	
}



void switch_timeinverted() {
	if (time_inverted) {
		time_inverted = false;
	}
	else {
		time_inverted = true;
	}
	create_displaylist_display();
	rendered_frames = 0;
	play_sound(SOUND_GUI);
}


void switch_peptide() {
	if (select_peptide) {
		select_peptide = false;
	}
	else {
		select_peptide = true;
	}
	play_sound(SOUND_NORMAL);
}


void switch_rainbow() {
	use_rainbow = !use_rainbow;
	play_sound(SOUND_NORMAL);
}


void switch_skin() {
	use_dark_skin = !use_dark_skin;
	glm::vec4* col = use_dark_skin ? clear_color_dark : clear_color_bright;
	glClearColor(col->x, col->y, col->y, col->w);
	glFogfv(GL_FOG_COLOR, (GLfloat*)col);
	play_sound(SOUND_NORMAL);
}


/* Switches the distance fogging */
void switch_fog() {
	if (use_fog)	glDisable(GL_FOG);
	else			glEnable(GL_FOG);
	use_fog = !use_fog;
	play_sound(SOUND_NORMAL);
}


/* Switches the display of grains cloud */
void switch_cloud() {
	show_cloud = !show_cloud;
	play_sound(SOUND_NORMAL);
}


/* Switches the display of subcomponents */
void switch_components() {
	show_structures = !show_structures;
	play_sound(SOUND_NORMAL);
}


/* Switches the bounding box */
void switch_bb() {
	show_bb = !show_bb;
	play_sound(SOUND_NORMAL);
}


/* Switches the display of the selected subcomponent */
void switch_subcomponents() {
	only_selected_component = !only_selected_component;
	play_sound(SOUND_NORMAL);
}


/* Switches the simulated depth of field */
void switch_diaphragm() {
	if (!(open_diaphragm)) {
		frames_motion_blur = user_frames_motion_blur;
		printf(" * Diaphragm now wide open.\n");
	}
	else {
		frames_motion_blur = 1;
		printf(" * Diaphragm now almost closed.\n");
	}
	//printf(" * Using dof with %d frames.\n", frames_motion_blur);
	play_sound(SOUND_ANORMAL1);
}


/* Switches the visibility of the toolbar */
void switch_toolbar() {
	show_toolbar = !show_toolbar;
	if (!show_toolbar) {
		left_display = -width_display;
	}
	else {
		left_display = 0;
	}
	play_sound(SOUND_ANORMAL1);
}

void switch_lod() {
	use_lod_balancing = !use_lod_balancing;
	play_sound(SOUND_ANORMAL1);
}



/* Switches fullscreen display */
void switch_fullscreen() {

	static glm::vec2 before;

	full_screen = !full_screen;
	if (full_screen) {
		GLint m_viewport[4];
		glGetIntegerv(GL_VIEWPORT, m_viewport);
		before.x = (GLfloat)m_viewport[2];
		before.y = (GLfloat)m_viewport[3];
		glutFullScreen();
	}
	else {
		glutReshapeWindow((GLint)before.x, (GLint)before.y);
		glutPositionWindow(20, 20);
	}
	play_sound(SOUND_GUI);

}


template <typename T>
vector<size_t> sort_indexes(const vector<T> &v) {

	// initialize original index locations
	vector<size_t> idx(v.size());
	std::iota(idx.begin(), idx.end(), 0);

	// sort indexes based on comparing values in v
	sort(idx.begin(), idx.end(),
		[&v](size_t i1, size_t i2) {return v[i1] < v[i2]; });

	return idx;
}

template <typename T>
vector<glm::vec3> sort_indexes_triple(const vector<T> &v) {

	// initialize original index locations
	vector<size_t> idx(v.size());
	std::iota(idx.begin(), idx.end(), 0);

	// sort indexes based on comparing values in v
	sort(idx.begin(), idx.end(),
		[&v](size_t i1, size_t i2) {return v[i1][2] < v[i2][2]; });

	vector<glm::vec3> remix;

	for (auto i : idx) {
		remix.push_back(glm::vec3(v[i][0], v[i][1], v[i][2]));
	}

	return remix;
}


std::string position_to_string(glm::vec3 pos) {
	return std::to_string(pos.x) + std::string("-") + std::to_string(pos.y) + std::string("-") + std::to_string(pos.z);
}


/* Returns a vector with the indices of the selected grains sorted by distance wrt camera */
void distance_sorted_stuctures(std::vector<glm::vec3>* sorted_indices, unsigned int frame) {

	glm::vec3 camera_position = get_position_of_camera();
	std::unordered_set<std::string> seenpoints;

	std::vector<glm::vec3> distances;
	for (unsigned int i = 0; i < vec_structures[frame].size(); i++) {
		for (unsigned int j = 0; j < vec_structures[frame][i]->size(); j++) {
			std::unordered_set<std::string>::const_iterator got = seenpoints.find(position_to_string(vec_structures[frame][i]->at(j)));
			if ( got == seenpoints.end() ) {
				float distance = glm::distance(camera_position, vec_structures[frame][i]->at(j));
				distances.push_back(glm::vec3(i, j, distance));
				seenpoints.insert(position_to_string(vec_structures[frame][i]->at(j)));
			}
		}		
	}

	for (auto i : sort_indexes_triple(distances)) {
		sorted_indices->push_back(i);
	}

}


/* Returns a vector with the indices of the selected grains sorted by distance wrt camera */
void distance_sorted_selected_peptide(std::vector<int>* sorted_indices, unsigned int frame) {
	unsigned int selected_group = pickedID / grouplength;

	glm::vec3 camera_position = get_position_of_camera();
	unsigned int offset = grouplength*selected_group;

	std::vector<float> distances;
	
	for (unsigned int i = offset; i < offset + grouplength; i++) {
		float distance = glm::distance(camera_position, point_clouds[frame][i]);
		distances.push_back(distance);
	}

	for (auto i : sort_indexes(distances)) {
		sorted_indices->push_back(i + offset);
	}

}


/* Get 3D coordinates based on window coordinates (e.g., mouse) */
glm::vec3 map2DPoint(glm::vec2 point) {

	GLdouble pos3D_x, pos3D_y, pos3D_z;

	GLdouble model_view[16];
	glGetDoublev(GL_MODELVIEW_MATRIX, model_view);

	GLdouble projection[16];
	glGetDoublev(GL_PROJECTION_MATRIX, projection);

	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);

	
	gluUnProject(point.x, point.y, 0.01, model_view, projection, viewport, &pos3D_x, &pos3D_y, &pos3D_z);

	return glm::vec3(pos3D_x, pos3D_y, pos3D_z);
	
}



/* Draws the bounding box of the animation */
void draw_bounding_box() {

	RenderState.temporary_set_lighting(false);
	RenderState.temporary_set_culling(false);
	//RenderState.temporary_set_fog(false);

	glColor4f(0.5f, 0.5f, 0.5f, 0.2f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); 
	glBegin(GL_QUADS);

		// sud
		glVertex3f(min_coord.x, min_coord.y, min_coord.z);
		glVertex3f(max_coord.x, min_coord.y, min_coord.z);
		glVertex3f(max_coord.x, max_coord.y, min_coord.z);
		glVertex3f(min_coord.x, max_coord.y, min_coord.z);

		// ovest
		glVertex3f(min_coord.x, min_coord.y, min_coord.z);
		glVertex3f(min_coord.x, min_coord.y, max_coord.z);
		glVertex3f(min_coord.x, max_coord.y, max_coord.z);
		glVertex3f(min_coord.x, max_coord.y, min_coord.z);

		// est
		glVertex3f(max_coord.x, min_coord.y, min_coord.z);
		glVertex3f(max_coord.x, min_coord.y, max_coord.z);
		glVertex3f(max_coord.x, max_coord.y, max_coord.z);
		glVertex3f(max_coord.x, max_coord.y, min_coord.z);

		// nord
		glVertex3f(min_coord.x, min_coord.y, max_coord.z);
		glVertex3f(max_coord.x, min_coord.y, max_coord.z);
		glVertex3f(max_coord.x, max_coord.y, max_coord.z);
		glVertex3f(min_coord.x, max_coord.y, max_coord.z);


	glEnd();
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); 

	RenderState.recover_lighting();
	RenderState.recover_culling();
	// RenderState.recover_fog();

}


/* Draw the sticks connecting the triples */
void drawsticks(bool inverted) {

	unsigned int frame_to_render = get_frame_to_render(inverted);
	
	if (only_selected_component)	glColor4fv((float*)&(*structure_colors_barely - glm::vec4(0.1f)));	
	else							glColor4fv((float*)&(*structure_colors - glm::vec4(0.1f)));	
	
	RenderState.temporary_set_lighting(false);
	
		
	glBegin(GL_QUADS);

		glPushMatrix();
			for (unsigned int i=0; i<vec_structures[frame_to_render].size(); i++) {
				if (sel_compo == i) continue;
				for (unsigned int j=0; j<vec_structures[frame_to_render][i]->size(); j+=3) {
			
					glVertex3f(vec_structures[frame_to_render][i]->at(j).x, vec_structures[frame_to_render][i]->at(j).y, vec_structures[frame_to_render][i]->at(j).z);
					glVertex3f(vec_structures[frame_to_render][i]->at(j).x+poljitter, vec_structures[frame_to_render][i]->at(j).y+poljitter, vec_structures[frame_to_render][i]->at(j).z+poljitter);

					glVertex3f(vec_structures[frame_to_render][i]->at(j+1).x, vec_structures[frame_to_render][i]->at(j+1).y, vec_structures[frame_to_render][i]->at(j+1).z);
					glVertex3f(vec_structures[frame_to_render][i]->at(j+1).x+poljitter, vec_structures[frame_to_render][i]->at(j+1).y+poljitter, vec_structures[frame_to_render][i]->at(j+1).z+poljitter);

					glVertex3f(vec_structures[frame_to_render][i]->at(j+1).x, vec_structures[frame_to_render][i]->at(j+1).y, vec_structures[frame_to_render][i]->at(j+1).z);
					glVertex3f(vec_structures[frame_to_render][i]->at(j+2).x+poljitter, vec_structures[frame_to_render][i]->at(j+2).y+poljitter, vec_structures[frame_to_render][i]->at(j+2).z+poljitter);
					
					glVertex3f(vec_structures[frame_to_render][i]->at(j+1).x, vec_structures[frame_to_render][i]->at(j+1).y, vec_structures[frame_to_render][i]->at(j+1).z);
					glVertex3f(vec_structures[frame_to_render][i]->at(j+2).x+poljitter, vec_structures[frame_to_render][i]->at(j+2).y+poljitter, vec_structures[frame_to_render][i]->at(j+2).z+poljitter);
					
				}
			}
		glPopMatrix();

		
		if (sel_compo>-1) {
	
			glColor4fv((float*)&(*selected_structure_color - glm::vec4(0.1f)));	
			
			for (unsigned int j=0; j<vec_structures[frame_to_render][sel_compo]->size(); j+=3) {

				glVertex3f(vec_structures[frame_to_render][sel_compo]->at(j).x, vec_structures[frame_to_render][sel_compo]->at(j).y, vec_structures[frame_to_render][sel_compo]->at(j).z);
				glVertex3f(vec_structures[frame_to_render][sel_compo]->at(j).x+poljitter, vec_structures[frame_to_render][sel_compo]->at(j).y+poljitter, vec_structures[frame_to_render][sel_compo]->at(j).z+poljitter);
		
				glVertex3f(vec_structures[frame_to_render][sel_compo]->at(j+1).x, vec_structures[frame_to_render][sel_compo]->at(j+1).y, vec_structures[frame_to_render][sel_compo]->at(j+1).z);
				glVertex3f(vec_structures[frame_to_render][sel_compo]->at(j+1).x+poljitter, vec_structures[frame_to_render][sel_compo]->at(j+1).y+poljitter, vec_structures[frame_to_render][sel_compo]->at(j+1).z+poljitter);
				
				glVertex3f(vec_structures[frame_to_render][sel_compo]->at(j+1).x, vec_structures[frame_to_render][sel_compo]->at(j+1).y, vec_structures[frame_to_render][sel_compo]->at(j+1).z);
				glVertex3f(vec_structures[frame_to_render][sel_compo]->at(j+1).x+poljitter, vec_structures[frame_to_render][sel_compo]->at(j+1).y+poljitter, vec_structures[frame_to_render][sel_compo]->at(j+1).z+poljitter);
				
				glVertex3f(vec_structures[frame_to_render][sel_compo]->at(j+2).x, vec_structures[frame_to_render][sel_compo]->at(j+2).y, vec_structures[frame_to_render][sel_compo]->at(j+2).z);
				glVertex3f(vec_structures[frame_to_render][sel_compo]->at(j+2).x+poljitter, vec_structures[frame_to_render][sel_compo]->at(j+2).y+poljitter, vec_structures[frame_to_render][sel_compo]->at(j+2).z+poljitter);

			}

		}

	glEnd();


	RenderState.recover_lighting();
	//glEnable(GL_LIGHTING);

}


/* LOD balancing */
int get_lod_index(glm::vec4 a, glm::mat4* full_transformations) {
	glm::vec4 b = *full_transformations*a;	
	int lod_index = 0;
	if (use_lod_balancing) {
		float dist = glm::distance(glm::vec4(0.f), b);
		if (dist > max_lod_dist) dist = max_lod_dist;
		if (dist < min_lod_dist) dist = min_lod_dist;
		lod_index = lod_levels * (dist / (max_lod_dist + min_lod_dist)) - 1;
	}
	return lod_index;
}



void draw_selected_peptide(GLint mode, bool inverted) {

	unsigned int frame_to_render = get_frame_to_render(inverted);

	// selected peptides
	if (mode != SELECTION_MODE2  && pickedID != NO_PICK) {

		std::vector<int> sorted_indices;
		distance_sorted_selected_peptide(&sorted_indices, frame_to_render);

		glColor4fv((float*)peptide_color);

		for (auto i : sorted_indices) {

			glPushMatrix();
			glm::vec3 a(point_clouds[frame_to_render][i]);
			glTranslatef(a.x, a.y, a.z);

			gluSphere(Q, spheres_size*1.5f, 20, 20);

			// glColor4fv((float*)cloud_colors);
			glPopMatrix();

		}

	}

}


/* Draw the grains cloud */
void drawpoints(GLint mode, bool inverted) {

	unsigned int frame_to_render = get_frame_to_render(inverted);

	glm::mat4 full_transformations = modelview*axis_rotat;

	if (pickedID == NO_PICK) selected_grain = -1;

	if (mode==0) glColor4fv((float*)cloud_colors);	

	unsigned int total_peptides = point_clouds[frame_to_render].size() / grouplength;

	
	//phongshading->enable();
	
	for (unsigned int i=0; i<point_clouds[frame_to_render].size(); i++) {
	
		glm::vec4 a(point_clouds[frame_to_render][i],1);		
		
		glPushMatrix();
					
			int lod_index = get_lod_index(a, &full_transformations);
			glTranslatef(a.x, a.y, a.z);			
			
			// this draws each object with a specific color, in order to implemetn
			// color-based picking. DO NOT TOUCH THIS!!
			if (mode==SELECTION_MODE2) glColor4fv((float*)&(id_to_color(i)));	

			else if (use_rainbow) {
				float hue = 280.f*((GLfloat)(i / grouplength) / total_peptides);
				HsvColor* hsv = NULL;
				hsv = Hsv_Create(hue, 0.9f, 0.9f);
				RgbFColor* rgbF = RgbF_CreateFromHsv(hsv->H, hsv->S, hsv->V);
				glColor3f(rgbF->R, rgbF->G, rgbF->B);
				free(hsv);
				free(rgbF);
			}
	
			if (testpoint(a)) glCallList( dl_lodspheres[lod_index] );

		glPopMatrix();		

		// selecting 
		if (mode!=SELECTION_MODE2) {

			if (pickedID == i) {
				selected_grain = i;

				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

				glPushMatrix();
				glm::vec3 a(point_clouds[frame_to_render][i]);
				glTranslatef(a.x, a.y, a.z);
				glRotatef(t*frames_motion_blur*2.5f, 0.2f, 0.3f, 0.4f);
				glColor4fv((GLfloat*)selected_grain_color);

				followed_object = a;

				gluSphere(Q, spheres_size*1.25f, 10, 10);

				glColor4fv((float*)cloud_colors);
				glPopMatrix();

				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

			}
			
		}				
	}

	// phongshading->disable();

	//draw_selected_peptide(mode, inverted);
			
}



/// Draw the beta-structures
void drawstructures(GLint const mode, bool const inverted) {

	unsigned int frame_to_render = get_frame_to_render(inverted);
	
	glm::mat4 full_transformations = modelview*axis_rotat;

	GLint offset = point_clouds[frame_to_render].size();
	sel_compo = right_pickedID==NO_PICK? -1 : right_pickedID-offset;
	
	if (only_selected_component)	glColor4fv((float*)structure_colors_barely );	
	else							glColor4fv((float*)structure_colors);	
	
	static std::vector<glm::vec3> distances;
	if (rendered_frames% 100 == 0) {
		distances.clear();
		distance_sorted_stuctures(&distances, frame_to_render);
	}

	for (unsigned int i = 0; i < distances.size(); i++) {

		//if (mode == SELECTION_MODE2) glColor4fv((float*)&(id_to_color(i + offset)));

		int sub = distances[i][0];
		int grain = (int) distances[i][1];
		glm::vec3 a(vec_structures[frame_to_render][sub]->at(grain));
		if (testpoint(a)) {
			glPushMatrix();
				glTranslatef(a.x, a.y, a.z);
				int lod_index = 1;
				if (use_lod_balancing)	lod_index = get_lod_index(glm::vec4(a, 1), &full_transformations);
				if (testpoint(a)) glCallList(dl_lodspheres2[lod_index]);
				glCallList(dl_lodspheres2[lod_index]);
			glPopMatrix();
		}
	}


	// paint all structures
	if (mode == SELECTION_MODE2) {
		for (unsigned int i = 0; i < vec_structures[frame_to_render].size(); i++) {

			// necessary for selection mode! DO NOT TOUCH!!!
			glColor4fv((float*)&(id_to_color(i + offset)));

			for (unsigned int j = 0; j < vec_structures[frame_to_render][i]->size(); j++) {
				glm::vec3 a(vec_structures[frame_to_render][i]->at(j));
				glPushMatrix();
				glTranslatef(a.x, a.y, a.z);
				int lod_index = 1;
				if (use_lod_balancing)	lod_index = get_lod_index(glm::vec4(a, 1), &full_transformations);

				if (testpoint(a)) glCallList(dl_lodspheres2[lod_index]);
				glPopMatrix();
			}
		}
	}


	if (sel_compo<0) return;

	// selected structure
	if (mode!=SELECTION_MODE2) {

		glColor4fv((float*)selected_structure_color);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		for (unsigned int j=0; j<vec_structures[frame_to_render][sel_compo]->size(); j++) {			
			glm::vec3 a(vec_structures[frame_to_render][sel_compo]->at(j));

			glPushMatrix();
				glTranslatef(a.x, a.y, a.z);								
				glRotatef(t*frames_motion_blur*3.0f, 0.4f, 0.3f, 0.2f);
				gluSphere(Q, spheres_size*1.2f, 10, 10);
			glPopMatrix();
			
		}
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		
	}

}



void create_options() {

	/*
	F1: toggle audio
	F2: toggle fog
	F3: toggle peptide selection
	F4: toggle dof
	F5: full screen
	F6: toggle grains cloud
	F7: toggle structures/components
	F8: toggle bounding box
	F9: pause
	F10: play backwards
	F11: play forward
	F12: NA
	*/

	// F1
	BHandl.add_option(glm::vec2(20, 200), glm::vec2(32, 32),
		"audio",
		"images/speaker_on.tga",
		"images/speaker_off.tga",
		NULL,
		false,
		true
	);

	// F2
	BHandl.add_option(glm::vec2(60, 200), glm::vec2(32, 32),
		"fog",
		"images/fog_on.tga",
		"images/fog_off.tga",
		switch_fog,
		false,
		true
	);

	// F3 
	BHandl.add_option(glm::vec2(100, 200), glm::vec2(32, 32),
		"selectpeptide",
		"images/peptide_on.tga",
		"images/peptide_off.tga",
		switch_peptide,
		false,
		true
	);

	// F4
	BHandl.add_option(glm::vec2(140, 200), glm::vec2(32, 32),
		"diaphragm",
		"images/diaphragm_on.tga",
		"images/diaphragm_off.tga",
		switch_diaphragm,
		false,
		true
	);

	// F5	
	BHandl.add_option(glm::vec2(180, 200), glm::vec2(32, 32),
		"fullscreen",
		"images/fullscreen_on.tga",
		"images/fullscreen_off.tga",
		switch_fullscreen,
		false,
		true
	);

	// F6
	BHandl.add_option(glm::vec2(220, 200), glm::vec2(32, 32),
		"grains",
		"images/grains_on.tga",
		"images/grains_off.tga",
		switch_cloud,
		false,
		true
	);

	// F7
	BHandl.add_option(glm::vec2(20, 160), glm::vec2(32, 32),
		"components",
		"images/components_on.tga",
		"images/components_off.tga",
		switch_components,
		false,
		true
	);

	// F8
	BHandl.add_option(glm::vec2(60, 160), glm::vec2(32, 32),
		"bbox",
		"images/boundingbox_on.tga",
		"images/boundingbox_off.tga",
		switch_bb,
		false,
		true
	);

	// ??
	BHandl.add_option(glm::vec2(100, 160), glm::vec2(32, 32),
		"subcomp",
		"images/chain_on.tga",
		"images/chain_off.tga",
		switch_subcomponents,
		false,
		true
	);

	// I
	BHandl.add_option(glm::vec2(140, 160), glm::vec2(32, 32),
		"inverttime",
		"images/timeinverted_on.tga",
		"images/timeinverted_off.tga",
		switch_timeinverted,
		false,
		true
	);

	// B
	BHandl.add_option(glm::vec2(180, 160), glm::vec2(32, 32),
		"rainbow",
		"images/rainbow_on.tga",
		"images/rainbow_off.tga",
		switch_rainbow,
		false,
		true
	);

	// L
	BHandl.add_option(glm::vec2(220, 160), glm::vec2(32, 32),
		"lod",
		"images/lod_on.tga",
		"images/lod_off.tga",
		switch_lod,
		false,
		true
	);

	// F9
	BHandl.add_option(glm::vec2(100, 240), glm::vec2(32, 32),
		"pause",
		"images/play_on.tga",
		"images/play_off.tga",
		pause_animation,
		false,
		false
	);

	// F?
	BHandl.add_option(glm::vec2(20, 240), glm::vec2(32, 32),
		"home",
		"images/backward.tga",
		"images/backward.tga",
		rewind_animation,
		false,
		false
	);

	// F?
	BHandl.add_option(glm::vec2(180, 240), glm::vec2(32, 32),
		"end",
		"images/forward.tga",
		"images/forward.tga",
		gotoend_animation,
		false,
		false
	);


	// slow down
	BHandl.add_option(glm::vec2(60, 240), glm::vec2(32, 32),
		"backward",
		"images/slowdown.tga",
		"images/slowdown.tga",
		negative_throttle_animation,
		false,
		false
	);

	// hurry up
	BHandl.add_option(glm::vec2(140, 240), glm::vec2(32, 32),
		"forward",
		"images/hurryup.tga",
		"images/hurryup.tga",
		positive_throttle_animation,
		false,
		false
	);



	// logo
	BHandl.add_option(glm::vec2(screen_width/2-64, 0), glm::vec2(256, 256),
		"logo",
		"images/hyperbeta2.tga",
		"images/hyperbeta2.tga",
		NULL,
		false,
		false
	);

	BHandl.set_option("fullscreen", full_screen);
	BHandl.set_option("fog", use_fog);
	BHandl.set_option("components", show_structures);
	BHandl.set_option("grains", show_cloud);
	BHandl.set_option("bbox", show_bb);
	BHandl.set_option("subcomp", only_selected_component);
	BHandl.set_option("audio", use_audio);
	BHandl.set_option("diaphragm", open_diaphragm);
	BHandl.set_option("inverttime", time_inverted);
	BHandl.set_option("selectpeptide", select_peptide);
	BHandl.set_option("lod", use_lod_balancing);
	// BHandl.set_option("logo", use_lod_balancing);


}





#endif 