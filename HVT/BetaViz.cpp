/* 
* Copyright (C) 2010 Marco S. Nobile - All Rights Reserved
* You may use, distribute and modify this code under the
* terms of the GPL3 license.
* 
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
* If you need information or help using hyperbeta please contact me at: m.s.nobile@tue.nl. 
*/



#include "stdafx.h"
#include <GL/glew.h>
#include <GL/freeglut.h>

#include <vector>
#include <string>
#include <exception>
#include <math.h>
#include <algorithm>

#include "service.h"
#include "Settings.h"
#include "keyboard.h"
#include "renderer.h"
#include "buttons.h"
#include "loader.h"
#include "displays.h"
#include "PLTfbos.h"

#include "glm\glm.hpp"
#include "glm\gtc\matrix_transform.hpp"
#include "glm\gtc\type_ptr.hpp"


// Specify that you want to force any available NVIDIA card
extern "C" {
	_declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
}


/************ RENDERING MODES ***********
This method actually performs the rendering of the scene. 
According to the current selection mode, it renders a subset of the objects
using specific coded colors. */
void actual_render(int mode = 0) {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	apply_modelview();

	if (mode!=SELECTION_MODE2 && mode!=SELECTION_MODE3) {
		glLightfv(GL_LIGHT0, GL_POSITION, light_position);
		glLightfv(GL_LIGHT1, GL_POSITION, light2_position);
	}
	
	glMultMatrixf(glm::value_ptr(axis_rotat));

	switch (mode) {
	
		// mouse left button: select grain/peptide
		case SELECTION_MODE2: 
			if (show_cloud) draw_points(SELECTION_MODE2, time_inverted);					
			break;

		// mouse right button: select structure
		case SELECTION_MODE3:
			if (show_structures) if (mode!=SELECTION_MODE) draw_structures(SELECTION_MODE2, time_inverted);
			break;

		// normal rendering
		default: 
			
			RenderState.temporary_set_fog(use_fog);
			if (show_cloud) draw_points(0, time_inverted);
			if (show_bb) draw_bounding_box();
			if (show_structures) {
				if (mode!=SELECTION_MODE) draw_structures(0, time_inverted);
				if (mode!=SELECTION_MODE) draw_sticks(time_inverted);
			}
			if (show_beta_sheet) {
				if (mode != SELECTION_MODE) {
					draw_boxes(time_inverted);
				}
			}
			if (select_peptide) {
				draw_selected_peptide(0, time_inverted);
			}
			break;
	}	
}


/* Update the count of frames per second */
int update_fps() {
	static unsigned int base_time = 0;
	static unsigned int frames = 0;
	static int fps = 0;
	unsigned int time = glutGet(GLUT_ELAPSED_TIME);
	if ((time - base_time) > 1000.0)
	{
		fps = frames*1000 / (time - base_time);
		base_time = time;
		frames = 0;
	}
	else {
		frames++;
	}
	return fps;
}


/* Displays the FPS on the bottom of the screen */
void display_fps() {
	int fps = update_fps();
	displayText((GLfloat)screen_width - 60, 20.f, .7f, .7f, .7f, std::to_string(fps) + std::string("fps").c_str(), GLUT_BITMAP_8_BY_13);
}


/* Picks a grain in the scene */
void grains_picking() {

	RenderState.temporary_set_lighting(false);
	RenderState.temporary_set_fog(false);

	if (left_clicked) {
		actual_render(SELECTION_MODE2);

		glFlush();
		glFinish();
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		unsigned char data[4];

		glReadPixels((GLint)last_mouse_click.x, (GLint)(screen_height - last_mouse_click.y), 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, data);
		pickedID = data[0] + data[1] * 256 + data[2] * 256 * 256;
		if (data[3] == 0) {
			pickedID = 16777215;
		}
		
	} else if (right_clicked) {
		actual_render(SELECTION_MODE3);

		glFlush();
		glFinish();
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		unsigned char data[4];

		glReadPixels((GLint)last_mouse_click.x, (GLint)screen_height - (GLint)last_mouse_click.y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, data);
		right_pickedID = data[0] + data[1] * 256 + data[2] * 256 * 256;
	}

	RenderState.recover_lighting();
	RenderState.recover_fog();

}


/* Main display func */
void display() {
		
	if (rendered_frames%5==1)	extractfrustum();


	/********************** 
			RENDER 
     **********************/

	actual_render();
		
	// accumulate buffers for DoF simulation
	if (open_diaphragm) {
		if (frame_motion_blur>=frames_motion_blur) {
			frame_motion_blur=0;			
			glAccum(GL_RETURN, 1.0);		
			if (prepare_to_highquality) {
				user_frames_motion_blur = 24;
				frames_motion_blur = user_frames_motion_blur;
			}
			else {
				user_frames_motion_blur = 8;
				frames_motion_blur = user_frames_motion_blur;
			}
		} else if (frame_motion_blur==0) {
			frame_motion_blur++;								
			glAccum(GL_LOAD, 1.0f / frames_motion_blur);			
			return;
		} else {
			frame_motion_blur++;			
			glAccum(GL_ACCUM, 1.0f / frames_motion_blur);
			return;
		}
	}


	// handle mouse click events
	if (left_clicked | right_clicked) {
		grains_picking();
		left_clicked = right_clicked = false;
		glAccum(GL_RETURN, 1.0);		
	}
	

	// The code below is executed only every #frames_motion_blur frames.
	t+=1.f/(frames_motion_blur);
	redefine_autorotation_matrix();
	
	GLint offset = point_clouds[actual_frame].size();

	glm::mat4 full_transformations = modelview*axis_rotat;

	unsigned int frame_to_render = get_frame_to_render(time_inverted);

	// correct focus
	if ( selected_grain!=-1 ) {				
		if (open_diaphragm) {
			glm::vec4 a(point_clouds[frame_to_render ][selected_grain],1);		
			glm::vec4 b = full_transformations*a;
			picked_depth  = glm::distance( b , glm::vec4(0,0,0,1) );			
			frames_motion_blur = user_frames_motion_blur;
		}
	} else if ( sel_compo>-1 ) {				
		if (open_diaphragm) {
			unsigned int grains = vec_structures[frame_to_render ][sel_compo]->size();
			glm::vec4 a(vec_structures[frame_to_render ][sel_compo]->at(grains/2),1);		
			glm::vec4 b = full_transformations*a;
			picked_depth  = glm::distance( b , glm::vec4(0,0,0,1) );			
			frames_motion_blur = user_frames_motion_blur;
		}
	}
	else {
		picked_depth = -1;
	}

	
	unsigned int prev = actual_frame;
	actual_frame = ((float)snapshots/frame_count)*current_frame;
	if (prev!=actual_frame) {
		rendered_frames = -1;
		if (pickedID>=offset) {
			pickedID = NO_PICK;	
			right_pickedID = NO_PICK;
		}	
	}

	float rythm = abs(sin((t*frames_motion_blur/10.0f)));
		
	if (pickedID != NO_PICK) {
		glm::vec3 ret = map2DPoint(glm::vec2(screen_width - 120, screen_height - 60));
		glBegin(GL_LINES);
			glColor4f(.6f, .4f, .0f, .7f);
			glLineWidth(spheres_size * 20.0f);
			glVertex3f(ret.x, ret.y, ret.z);
			glVertex3f(followed_object.x, followed_object.y, followed_object.z);
			
			glColor4f(.6f, .4f, .0f, 0.3f);
			glLineWidth(spheres_size * 60.f);
			glVertex3f(ret.x, ret.y, ret.z);
			glVertex3f(followed_object.x, followed_object.y, followed_object.z);
		glEnd();
		glLineWidth(spheres_size * 40);
	}

	set_orthographic(true);
	RenderState.temporary_set_depthtest(false);
	RenderState.temporary_set_lighting(false);
	RenderState.temporary_set_fog(false);

	// display the static part of displays
	if (show_toolbar) {
		
		glCallList(displaylist_displays);
		BHandl.render_all();
		update_points_distribution(time_inverted);
	
		// information about animation
		displayText(20.0f, 40.f, 0.1f, 0.1f, 0.2f, std::string("Frame: " + std::to_string(current_frame) + std::string("/") + std::to_string(frame_count)) + std::string(" (") + std::to_string(snapshots) + std::string(" snaps.)"));
		displayText(20.0f, 20.f, 0.1f, 0.1f, 0.2f, std::string("Time: " + std::to_string(actual_frame) + std::string(" (") + std::to_string(snapshots)) + std::string(") ns"));

		if (show_structures)
			displayText(20.f, 60.f, structure_colors->r*rythm, structure_colors->g*rythm, structure_colors->b, std::string("Components"));
		else
			displayText(20.f, 60.f, structure_colors->r*.3f, structure_colors->g*.3f, structure_colors->b*.3f, std::string("Components"));

		if (select_peptide)
			displayText(20.f, 100.f, peptide_color->r*rythm, peptide_color->g*rythm, peptide_color->b, std::string("Peptide"));
		else
			displayText(20.f, 100.f, peptide_color->r*.3f, peptide_color->g*.3f, peptide_color->b*.3f, std::string("Peptide"));

		if (show_cloud)
			displayText(20.f, 80.f, cloud_colors->r*rythm, cloud_colors->g*rythm, cloud_colors->b, std::string("Grains"));
		else
			displayText(20.f, 80.f, cloud_colors->r*.3f, cloud_colors->g*.3f, cloud_colors->g*.3f, std::string("Grains"));

		displayText(20, 130, 0, 0, 1, project_name.c_str(), GLUT_BITMAP_HELVETICA_12);
	}

	

	// draw fps
	
	// info selected grain
	if (!(selected_grain<0)) {		
		RenderState.temporary_set_lighting(false);
		displayText((GLfloat)screen_width - 110, (GLfloat)screen_height - 40, 0, 0, 0, vec_metadata[0][selected_grain].c_str(), GLUT_BITMAP_8_BY_13);
		displayText((GLfloat)screen_width - 111, (GLfloat)screen_height - 39, 1, 1, 1, vec_metadata[0][selected_grain].c_str(), GLUT_BITMAP_8_BY_13);
		displayText((GLfloat)screen_width - 110, (GLfloat)screen_height - 60, 0.3f, 0.15f, 0.2f, std::string("Peptide #") + vec_metadata3[0][selected_grain].c_str(), GLUT_BITMAP_8_BY_13);
		displayText((GLfloat)screen_width - 111, (GLfloat)screen_height - 59, 1.f, 1.f, 1.f, std::string("Peptide #") + vec_metadata3[0][selected_grain].c_str(), GLUT_BITMAP_8_BY_13);
		
		displayText((GLfloat)screen_width - 110, (GLfloat)screen_height - 80, 0.3f, 0.15f, 0.2f, std::string("Grain #") + std::to_string(selected_grain%grouplength), GLUT_BITMAP_8_BY_13);
		displayText((GLfloat)screen_width - 111, (GLfloat)screen_height - 79, 1.f, 1.f, 1.f, std::string("Grain #") + std::to_string(selected_grain%grouplength), GLUT_BITMAP_8_BY_13);

		displayText((GLfloat)screen_width - 110, (GLfloat)screen_height - 100, 0.3f, 0.15f, 0.2f, std::string("Index #") + std::to_string(selected_grain), GLUT_BITMAP_8_BY_13);
		displayText((GLfloat)screen_width - 111, (GLfloat)screen_height - 99, 1.f, 1.f, 1.f, std::string("Index #") + std::to_string(selected_grain), GLUT_BITMAP_8_BY_13);

		RenderState.recover_lighting();
	}

	// statistics about grains
	if (show_statistics) {
		display_statistics(actual_frame, time_inverted);
	}

	set_orthographic(false);

	RenderState.recover_depthtest();
	RenderState.recover_lighting();
	RenderState.recover_fog();
	
	rendered_frames += 1;
	

	// Double buffering update
	glutSwapBuffers();


}




void reshapefun(int w, int h) {

	if (w < 800 || h < 600) {
		glutReshapeWindow(800, 600);
	}

	// Use the Projection Matrix
	glMatrixMode(GL_PROJECTION);

    // Reset Matrix
	glLoadIdentity();

	// Set the viewport to be the entire window
	glViewport(0, 0, w, h);

	// Set the correct perspective.
	apply_perspective(w,h);

	// Get Back to the Modelview
	glMatrixMode(GL_MODELVIEW);

	screen_width=w; screen_height=h;

	int id = BHandl.options_names["logo"];
	BHandl.options_vector[id]->set_position( glm::vec2(w/2-64, screen_height - 148) );
	
	create_displaylist_display();

	std::string concat(" Settings size='300 200' color='96 96 96' alpha=200 position='");
	concat += std::to_string(w - 330);
	concat += " ";
	concat += std::to_string(100);
	concat += "' ";
	//std::cout << concat << std::endl;
	// TwDefine(concat.c_str()); // change default tweak bar size and color
		
}



void switch_displaylist() {
	// not supported yet
}


void initialize_fbos() {
	screenshot_fbo = new PLTfbo(screen_width, screen_height);
	screenshot_fbo->AttachDepthBuffer();
	screenshot_fbo->AttachColorBuffer(0);
	screenshot_fbo->Activate();
	screenshot_fbo->Deactivate();
	std::cout << " * Frame buffer objects initialized." << std::endl;
}



void mouseMove(int x, int y) { 			
}


// Periodic update (every ms milliseconds) of the velocities in the scene
void update_vels(int ms) {

	// process key presses
	keyfunbase();

	// inertial update of camera movement and rotation
	vel_mov *= inertia_movement;
	vel_rot *= inertia_rotation;

	// Increases the level of realism, but it wastes computations
	if (simulate_handheld) {
		glm::vec3 jitter;
		jitter.x = (-0.5f + (float)rand() / RAND_MAX)*1e-4f;
		jitter.y = (-0.5f + (float)rand() / RAND_MAX)*1e-4f;
		jitter.z = (-0.5f + (float)rand() / RAND_MAX)*1e-4f;
		vel_mov += jitter;
	}

	glm::mat4 temp;
	temp = glm::translate(glm::mat4(), vel_mov);
	temp = glm::rotate(temp, vel_rot.x, glm::vec3(1., 0., 0.));
	temp = glm::rotate(temp, vel_rot.y, glm::vec3(0., 1., 0.));
	temp = glm::rotate(temp, vel_rot.z, glm::vec3(0., 0., 1.));

	// Accumulation buffer is computationally expensive. During 
	// movements it is not necessary to maintain a high quality 
	// of the rendering.
	if (approximate_accumbuffer) {
		if ((glm::length(vel_mov) + glm::length(vel_rot) < threshold_movement_hires) && (glm::length(auto_rotate_x) + glm::length(auto_rotate_y) + glm::length(auto_rotate_z) < threshold_autorotate_hires)) {
			prepare_to_highquality = true;
		}
		else {
			prepare_to_highquality = false;
		}
	}

	modelview = temp * modelview;

	if (!temp_pause) {
		current_frame = (current_frame + step_snap) % frame_count;
	}	
	if (current_frame<0) {
		current_frame = frame_count + current_frame;
	}

	glutTimerFunc(ms, update_vels, ms);
}


int main(int argc, char** argv) {

	if (argc<4) {
		printf("ERROR: specify a directory containing the GROMACS input files, the prefix of the files and the number of snapshots.\n");
		exit(1);
	}

	logo();

	unsigned int first_snapshot = atoi(argv[3]);
	unsigned int last_snapshot = atoi(argv[4]);
	snapshots = last_snapshot - first_snapshot;
	frame_count = snapshots * frames_interpolation;

	printf(" * Requested %d snapshots. By using %d frame per snapshot will show %d frames.\n", snapshots, frames_interpolation, frame_count);
	printf(" * Opening folder: %s.\n", argv[1]);
	printf("   files prefix  : %s.\n", argv[2]);
	printf("   number of MD snapshots to be loaded: %d.\n", snapshots);

	std::string full_path = std::string(argv[1]);
	size_t pos = full_path.find_last_of("/\\");
	project_name =  (full_path.substr(pos+1));

	if (!get_group_length(full_path, &grouplength)) {
		printf("Cannot process input folder, closing down.\n");
		exit(6);
	}

	printf("   length of peptides: %d.\n", grouplength);

	for (unsigned int i=first_snapshot; i<last_snapshot; i++) {

		printf(" * Opening frame [%d].\n", i);

		std::vector<glm::vec3> nuvola;
		std::vector<std::string> metadata;
		std::vector<std::string> metadata2;
		std::vector<std::string> metadata3;
		std::vector<std::vector<glm::vec3>*> stru;
		std::vector<std::vector<unsigned int>*> boxes;

		unsigned int total_grains = load_point_cloud(&nuvola, &metadata, &metadata2, &metadata3, argv[1]+std::string("/")+argv[2]+"_"+std::to_string(i));
		
		point_clouds.push_back(nuvola);
		vec_metadata.push_back(metadata);
		vec_metadata2.push_back(metadata2);
		vec_metadata3.push_back(metadata3);

		std::map<std::string, float> freq;

		unsigned int total_triples = load_structures(&stru, argv[1] + std::string("/output_") + argv[2] + "_" + std::to_string(i), metadata, &freq, &boxes);
	
		vec_freq.push_back(freq);

		if (total_triples>max_triples) max_triples = total_triples;
		triples_per_snapshot.push_back(total_triples);
		vec_structures.push_back(stru);
		vec_boxes.push_back(boxes);

		violet_green_ratio.push_back((float)(total_triples/3)/total_grains);

	}

	printf(" * Total detected peptides: %zd.\n", point_clouds[0].size() / grouplength);

	maxdistance = glm::distance(max_coord, min_coord);

	printf(" * The animation is contained in coordinates: (%f, %f, %f) - (%f, %f, %f).\n", min_coord.x, min_coord.y, min_coord.z, max_coord.x, max_coord.y, max_coord.z);
	printf("   maximum distance: %f.\n", maxdistance);
	printf(" * Max structures in a snapshot: %d.\n", max_structures);
	
	glutInit(&argc, (char**)argv);
	glutSetOption(GLUT_MULTISAMPLE, 8);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_ALPHA |  GLUT_DEPTH | GLUT_MULTISAMPLE);
	glEnable(GL_MULTISAMPLE);
	glHint(GL_MULTISAMPLE_FILTER_HINT_NV, GL_NICEST);
	glutInitWindowSize(screen_width,screen_height);
	glutCreateWindow((std::string("HyperBeta ")+std::string(VERSION)).c_str());
	
	create_displaylist_display();
	create_displaylist_sphere();

	glutDisplayFunc(display);

	glutSpecialFunc(speckey_down);
	glutSpecialUpFunc(speckey_up);
	glutKeyboardFunc(key_down_func);
	glutKeyboardUpFunc(key_up_func);

	glutMouseFunc(mousefun);
	
	glutReshapeFunc(reshapefun);
	 
	glutIdleFunc(display);	  
	glutTimerFunc(10, update_vels, 10);

	load_settings();
	create_options();

	init();

	glFogf(GL_FOG_START, 10.f);
	glFogf(GL_FOG_END, maxdistance*1.05f);
	
	create_frustum();

	initialize_fbos();

	glutMainLoop();

	return 0;
}

