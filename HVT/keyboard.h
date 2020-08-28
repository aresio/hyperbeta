#ifndef __KEYBOARD__
#define __KEYBOARD__

#include <cstdio>
#include <time.h>

#include "renderer.h"
#include "service.h"
#include "audio.h"
#include "PLTfiles.h"


bool key_state[256] = { false };
bool speckey_state[256] = { false };
bool inhibited_key[256] = { false };
time_t key_press_time[256] = { 0 };
bool key_state_modifier;
unsigned int total_pressed = 0;
unsigned int totalspec_pressed = 0;




inline void update_key_modifier() {
	key_state_modifier = (glutGetModifiers() != 0);
}




/* Callback for functional keys */
void treat_functional_keys_up(int const key) {

	update_key_modifier();

	if (key == GLUT_KEY_F1) {
		switch_audio();
		BHandl.set_option("audio", use_audio);
	}
	if (key == GLUT_KEY_F2) {
		switch_fog();
		BHandl.set_option("fog", use_fog);
	}
	if (key == GLUT_KEY_F3) {
		switch_peptide();
		BHandl.set_option("selectpeptide", select_peptide);
	}
	if (key == GLUT_KEY_F4) {
		switch_diaphragm();
		BHandl.set_option("diaphragm", open_diaphragm);
	}
	if (key == GLUT_KEY_F5) {
		switch_fullscreen();
		BHandl.set_option("fullscreen", full_screen);
	}
	if (key == GLUT_KEY_F6) {
		switch_cloud();
		BHandl.set_option("grains", show_cloud);
	}
	if (key == GLUT_KEY_F7) {
		switch_components();
		BHandl.set_option("components", show_structures);
	}
	if (key == GLUT_KEY_F8) {
		switch_bb();
		BHandl.set_option("bbox", show_bb);
	}
	if (key == GLUT_KEY_F9) pause_animation();
	if (key == GLUT_KEY_F10) negative_throttle_animation();
	if (key == GLUT_KEY_F11) positive_throttle_animation();

}




/* Callback for key down event (before release) */
void key_down_func(unsigned char const key, int const x, int const y)
{	
	update_key_modifier();
    key_state[key] = true;
	total_pressed++;	
}

/* Callback for key up event (after release) */
void key_up_func(unsigned char const key, int const x, int const y)
{
	update_key_modifier();
    key_state[key] = false;
	total_pressed--;
		
	if (key=='o' || key=='O') {		
		switch_subcomponents();
		BHandl.set_option("subcomp", only_selected_component);
	}

	if (key=='b' || key=='B') {
		switch_rainbow();
		BHandl.set_option("rainbow", use_rainbow);
	}

	if (key == 'c' || key == 'C') {
		switch_skin();
		BHandl.set_option("rainbow", use_rainbow);
	}

	if (key == 'e' || key == 'E') {
		//saveTGA("screenshot.tga", screen_width, screen_height);
		screenshot_fbo->Activate();
	}

	if (key=='i' || key=='I') {
		switch_timeinverted();
		BHandl.set_option("inverttime", time_inverted);
	}

	if (key == 'u' || key == 'U') {
		show_statistics = !show_statistics;
	}

	// LoD balancing 
	if (key=='l' || key=='L') {
		use_lod_balancing = !use_lod_balancing;
		BHandl.set_option("lod", use_lod_balancing);
	}

	// toggle left panel
	if (key=='q' || key=='Q') {
		switch_toolbar();
	}

}

void speckey_down(int const key, int const x, int const y)
{
    speckey_state[key] = true;
	totalspec_pressed++;	
}
 
void speckey_up(int const key, int const x, int const y)
{
    speckey_state[key] = false;
	totalspec_pressed--;
	treat_functional_keys_up(key);
}


void keyfunbase() {

	//update_key_modifier();
	
	if (total_pressed+totalspec_pressed==0) return;

	// quit
	if (key_state[27]) 	exit(0);

	float delta = frames_motion_blur==1? 1 : 1.f/(frames_motion_blur/2);
	
	// MOVEMENT
	if (key_state['w'] || key_state['W']) {
		if (key_state_modifier) vel_mov.z+=movement_step*.2f*delta; 
		else 					vel_mov.z+=movement_step*delta; 
	}
	if (key_state['s'] || key_state['S']) {
		if (key_state_modifier) vel_mov.z-=movement_step*.2f*delta; 
		else					vel_mov.z-=movement_step*delta;
	}
	if (key_state['a'] || key_state['A']) {
		if (key_state_modifier) vel_mov.x+=movement_step*.2f*delta;
		else					vel_mov.x+=movement_step*delta; 
	}
	if (key_state['d'] || key_state['D']) {
		if (key_state_modifier) vel_mov.x-=movement_step*.2f*delta; 
		else					vel_mov.x-=movement_step*delta; 
	}	
	if (key_state['-']) {
		if (key_state_modifier) vel_mov.y-=movement_step*.2f*delta;
		else					vel_mov.y-=movement_step*delta;
	}
	if (key_state['+']) {
		if (key_state_modifier) vel_mov.y+=movement_step*.2f*delta;
		else					vel_mov.y+=movement_step*delta;
	}

	// reset position
	if (key_state['r'] || key_state['R']) reset_view();

	// automatic rotation
	if (key_state['x'] || key_state['X']) {
		if (key_state_modifier)	auto_rotate_x -= rotation_step*.05f*delta; 
		else					auto_rotate_x += rotation_step*.05f*delta; 
	}
	if (key_state['y'] || key_state['Y']) {
		if (key_state_modifier)	auto_rotate_y -= rotation_step*.05f*delta; 
		else					auto_rotate_y += rotation_step*.05f*delta; 
	}
	if (key_state['z'] || key_state['Z']) {
		if (key_state_modifier)	auto_rotate_z -= rotation_step*.05f*delta; 
		else					auto_rotate_z += rotation_step*.05f*delta; 
	}

	// Control FOV
	if (key_state[','] && fov>45) {
		fov-=0.003f;
		set_fov(fov);
	}
	if (key_state['.'] && fov<100){
		fov+=0.003f;
		set_fov(fov);
	}
	if (key_state['f'] && fov<100) {
		fov = 60.f;
		set_fov(fov);
	}


	// control aperture of diaphragm
	if ((key_state['j'] || key_state['J']) && aperture>0) {
		aperture -= 0.0005f;
	}
	if ((key_state['k'] || key_state['K']) && aperture < 10.f) {
		aperture +=0.0005f;
	}

	// circle of confusion detail
	if (key_state['8']) {
		user_frames_motion_blur = 8;
		frames_motion_blur = user_frames_motion_blur;
		BHandl.set_option("components", open_diaphragm);
	}
	if (key_state['9']) {
		user_frames_motion_blur = 15;
		frames_motion_blur = user_frames_motion_blur;
		BHandl.set_option("components", open_diaphragm);
	}
	if (key_state['0']) {
		user_frames_motion_blur = 24;
		frames_motion_blur = user_frames_motion_blur;
		BHandl.set_option("components", open_diaphragm);
	}

	if (key_state[13]) rewind_animation();

	// reset
	if (key_state[32]) {
			vel_mov = glm::vec3(0,0,0);
			vel_rot = glm::vec3(0,0,0);
			auto_rotate_x = auto_rotate_y = auto_rotate_z = 0;
	}
	
	if (speckey_state[GLUT_KEY_UP]) {
		if (key_state_modifier) vel_rot.x-=rotation_step*.2f*delta; 
		else					vel_rot.x-=rotation_step*delta; 
	}
	if (speckey_state[GLUT_KEY_DOWN]) {
		if (key_state_modifier) vel_rot.x+=rotation_step*.2f*delta; 
		else					vel_rot.x+=rotation_step*delta; 
	}
	if (speckey_state[GLUT_KEY_LEFT]) {
		if (key_state_modifier) vel_rot.y-=rotation_step*.2f*delta; 
		else					vel_rot.y-=rotation_step*delta;
	}
	if (speckey_state[GLUT_KEY_RIGHT]) {
		if (key_state_modifier) vel_rot.y+=rotation_step*.2f*delta; 
		else					vel_rot.y+=rotation_step*delta;
	}
	if (speckey_state[GLUT_KEY_PAGE_DOWN]) {
		if (key_state_modifier) vel_rot.z+=rotation_step*.2f*delta; 
		else					vel_rot.z+=rotation_step*delta;
	}
	if (speckey_state[GLUT_KEY_PAGE_UP]) {
		if (key_state_modifier) vel_rot.z-=rotation_step*.2f*delta; 
		else					vel_rot.z-=rotation_step*delta; 
	}

}


#endif