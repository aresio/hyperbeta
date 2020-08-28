#ifndef __MOUSE__
#define __MOUSE__

#include <GL\GL.h>

#include "keyboard.h"

glm::vec2 last_mouse_click ;
bool left_clicked = false;
bool right_clicked = false;
bool shift_left_clicked = false;
bool shift_right_clicked = false;

void mousefun( GLint const button, GLint const state, GLint const x, GLint const y) {
		
	// update_key_modifier();

	if (state==1) {
		last_mouse_click = glm::vec2(x,y);
		if (button == 0) {
			left_clicked = true;
			//shift_left_clicked = key_state_modifier;
		}
		else {
			right_clicked = true;
			//shift_left_clicked = key_state_modifier;
		}
	} else {
		left_clicked = false;
		right_clicked = false;
		shift_left_clicked = false;
		shift_right_clicked = false;
	}

	if (state==GLUT_UP) {
		int clicked_option = BHandl.check_click( glm::vec2(last_mouse_click.x, screen_height-last_mouse_click.y) );
		if (clicked_option!=-1) {
			BHandl.options_vector[clicked_option]->click();
			left_clicked = false;
			right_clicked = false;
		}
	}
	

}




#endif