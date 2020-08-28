#ifndef __DISTRIBUTIONS__
#define __DISTRIBUTIONS__

#include <GL\GL.h>
#include "graphics.h"
#include "loader.h"


void display_statistics(int  frame, bool const inverted) {

	if (inverted) frame = (vec_freq.size()-1) - frame;

	int n = vec_freq[frame].size();
	auto it = vec_freq[frame].begin();
	

	GLfloat* col = use_dark_skin ? (GLfloat*)clear_color_bright : (GLfloat*)clear_color_dark;

	// adapting info text
	displayText((left_display + width_display)+ 80, 33 + vec_freq[frame].size() * 20  , col[0], col[1], col[2], "Grain components", GLUT_BITMAP_HELVETICA_18);

	// print all grains statistics
	for (it = vec_freq[frame].begin(); it != vec_freq[frame].end(); it++, n--) {
		displayText((left_display + width_display) + 80, 10 + n * 20, col[0], col[1], col[2],
			((*it).first).c_str(), GLUT_BITMAP_HELVETICA_18);

		unsigned int frequenza = (*it).second;

		displayText((left_display + width_display) + 80 + max_length_grain_name*18 , 10 + n * 20, col[0], col[1], col[2],
			((std::to_string(frequenza))+"%").c_str(), GLUT_BITMAP_HELVETICA_18);
	}
}


/// Draw the overall statistics on the display
void draw_distribution(bool const inverted) {

	float conv_larg_display1 = width_display/(vec_structures.size());
	float conv_alte_display1 = height_display/max_structures;
	
	float conv_larg_display2 = width_display/vec_structures.size();
	float conv_alte_display2 = height_display/max_triples;
	
	float conv_larg_display3 = width_display/violet_green_ratio.size();
	float conv_alte_display3 = height_display/1.;
	
	set_orthographic(true);
	
	RenderState.temporary_set_lighting(false);
	RenderState.temporary_set_depthtest(false);
	
	// background service display
	glColor4f(1.0f, 1.0f, 1.0f, 0.6f);	
	glBegin(GL_QUADS);		
		glVertex2f(0, (GLfloat)screen_height);
		glVertex2f(0, 0);
		glVertex2f((GLfloat)width_display+60, 0);
		glVertex2f((GLfloat)width_display + 60, (GLfloat)screen_height);
	glEnd();

	// values
	glBegin(GL_LINE_STRIP);			
			
		glColor3f(.5f, .1f, .1f);	

		if (!inverted) 
			for (unsigned int sn=0; sn< vec_structures.size(); sn++ ) {
				glVertex2f(margin_display_left+sn*conv_larg_display1, screen_height -margin_display_top - (height_display+20) + conv_alte_display1 * vec_structures[sn].size());
			}
		else
			for (unsigned int sn=0; sn< vec_structures.size(); sn++ ) {
				glVertex2f(margin_display_left+sn*conv_larg_display1, screen_height -margin_display_top - (height_display+20) + conv_alte_display1 * vec_structures[vec_structures.size()-sn-1].size());
			}


	glEnd();
	glBegin(GL_LINE_STRIP);			

		if (!inverted) 
			for (unsigned int sn=0; sn< triples_per_snapshot.size(); sn++ ) {
				glVertex2f(margin_display_left+sn*conv_larg_display2, screen_height -margin_display_top  - (height_display*2+50) + conv_alte_display2 * triples_per_snapshot[sn]);				
			}
		else {
			for (unsigned int sn=0; sn< vec_structures.size(); sn++ ) {
				glVertex2f(margin_display_left+sn*conv_larg_display2, screen_height -margin_display_top  - (height_display*2+50) + conv_alte_display2 * triples_per_snapshot[triples_per_snapshot.size()-sn-1]);				
			}
		}

	glEnd();
	glBegin(GL_LINE_STRIP);			

		if (!inverted) 
			for (unsigned int sn=0; sn< vec_structures.size(); sn++ ) {
				glVertex2f(margin_display_left+sn*conv_larg_display2, 
					screen_height -margin_display_top  - (height_display*3+80) + conv_alte_display3 * violet_green_ratio[sn]);				
			}
		else 
			for (unsigned int sn=0; sn< vec_structures.size(); sn++ ) {
				glVertex2f(margin_display_left+sn*conv_larg_display2, 
					screen_height -margin_display_top  - (height_display*3+80) + conv_alte_display3 * violet_green_ratio[vec_structures.size()-sn-1]);				
			}

	glEnd();
	
	// cartesian axes
	glColor4f(.1f, .1f, .1f, .9f);	
	glBegin(GL_LINES); 
	
		glVertex2f((GLfloat)margin_display_left,					(GLfloat)(screen_height-(height_display+20)-margin_display_top));
		glVertex2f((GLfloat)margin_display_left,					(GLfloat)(screen_height-20-margin_display_top));
		glVertex2f((GLfloat)margin_display_left,					(GLfloat)(screen_height-(height_display+20)-margin_display_top));
		glVertex2f((GLfloat)(margin_display_left+width_display),	(GLfloat)(screen_height-(height_display+20)-margin_display_top));

		glVertex2f((GLfloat)margin_display_left,					screen_height-(height_display*2+50) -margin_display_top);
		glVertex2f((GLfloat)margin_display_left,					screen_height-50-(height_display)   -margin_display_top);
		glVertex2f((GLfloat)margin_display_left,					screen_height-(height_display*2+50) -margin_display_top);
		glVertex2f((GLfloat)margin_display_left+width_display,		screen_height-(height_display*2+50) -margin_display_top);

		
		glVertex2f((GLfloat)margin_display_left,					screen_height-(height_display*3+80)-margin_display_top);
		glVertex2f((GLfloat)margin_display_left,					screen_height-80-(height_display*2)-margin_display_top);
		glVertex2f((GLfloat)margin_display_left,					screen_height-(height_display*3+80)-margin_display_top);
		glVertex2f((GLfloat)margin_display_left+width_display,		screen_height-(height_display*3+80)-margin_display_top);
		

	glEnd();

	// labels on axes
	displayText((GLfloat)margin_display_left-50, (GLfloat)screen_height-margin_display_top-12, 0.3f, 0.3f, 0.3f, "Number of components", GLUT_BITMAP_8_BY_13);
	displayText((GLfloat)margin_display_left-50, (GLfloat)screen_height-margin_display_top-height_display-42, 0.3f, 0.3f, 0.3f, "Number of triples", GLUT_BITMAP_8_BY_13);
	displayText((GLfloat)margin_display_left-50, (GLfloat)screen_height-margin_display_top-height_display*2-72, 0.3f, 0.3f, 0.3f, "Ratio beta:all grains", GLUT_BITMAP_8_BY_13);

	set_orthographic(false);

	RenderState.recover_lighting();
	RenderState.recover_depthtest();

}




/// Draw the points on the statistics for the current frame
void update_points_distribution(bool  const inverted) {

	RenderState.temporary_set_depthtest(false);

	set_orthographic(true);

	unsigned int total_frames = vec_structures.size();

	float conv_larg_display1 = width_display/total_frames;
	float conv_alte_display1 = height_display/max_structures;

	float conv_larg_display2 = width_display/total_frames;
	float conv_alte_display2 = height_display/max_triples;

	float conv_larg_display3 = width_display/total_frames;
	float conv_alte_display3 = height_display/1.;

	
	// good
	glColor3f(1,0,0);	
	glBegin(GL_POINTS); 
		unsigned int frame_to_render = inverted? total_frames-actual_frame-1 : actual_frame;		
		glVertex2f(margin_display_left+actual_frame*conv_larg_display1, screen_height -margin_display_top - (height_display+20) + conv_alte_display1 * vec_structures[frame_to_render ].size());
		glVertex2f(margin_display_left+actual_frame*conv_larg_display2, screen_height -margin_display_top - (height_display*2+50) + conv_alte_display2 * triples_per_snapshot[frame_to_render ]);
		glVertex2f(margin_display_left+actual_frame*conv_larg_display3, screen_height -margin_display_top  - (height_display*3+80) + conv_alte_display3 * violet_green_ratio[frame_to_render ]);
	glEnd();

	unsigned int current_structures = vec_structures[frame_to_render].size();
	unsigned int current_triples = triples_per_snapshot[frame_to_render];
	auto        current_ratio = std::round(violet_green_ratio[frame_to_render]*100);

	// information on display
	displayText((GLfloat)margin_display_left - 50, (GLfloat)screen_height - margin_display_top - 30, 0.3f, 0.3f, 0.3f, std::to_string(current_structures).c_str(), GLUT_BITMAP_HELVETICA_18);
	displayText((GLfloat)margin_display_left - 50, (GLfloat)screen_height - margin_display_top - height_display - 60, 0.3f, 0.3f, 0.3f, std::to_string(current_triples).c_str(), GLUT_BITMAP_HELVETICA_18);
	displayText((GLfloat)margin_display_left - 50, (GLfloat)screen_height - margin_display_top - height_display - 160, 0.3f, 0.3f, 0.3f, std::to_string((int)current_ratio).c_str()+std::string("%"), GLUT_BITMAP_HELVETICA_18);


	set_orthographic(false);

	RenderState.recover_depthtest();

}


void create_displaylist_display() {
	displaylist_displays = glGenLists(1);
	glNewList( displaylist_displays, GL_COMPILE);
		draw_distribution(time_inverted);
	glEndList();
}




#endif