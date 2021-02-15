#pragma once

#include "glm\glm.hpp"

/// Renderer flags
bool full_screen = false;
bool use_fog = true;
bool show_structures = true;
bool show_cloud = true;
bool show_beta_sheet = false;
bool show_bb = true;
bool only_selected_component = false;
bool time_inverted = false;
bool select_peptide = false;
bool use_rainbow = false;
bool show_toolbar = true;
bool use_lod_balancing = true;
bool approximate_accumbuffer = true;
bool prepare_to_highquality = false;
bool show_statistics = true;
bool simulate_handheld = false;
bool use_dark_skin = false;
bool show_tweakbar = false;

// constants for automatic accumulation buffer control and level of detail balancing
const float threshold_movement_hires = 1e-3f;
const float threshold_autorotate_hires = 1e-4f;
const int lod_levels = 20;
const int minlod = 4;
const int maxlod = 32;

// constants used for the rendering
const float D2R = 3.1415f / 180.f;
const GLfloat pi = 3.1415926535897932f;
const float movement_step = 0.1f;
const float rotation_step = 0.007f;
const float detail_spheres = 16.f;
const float spheres_size = 0.08f;
float fov = 60.0f;
const float nplane = 0.1f;
const float fplane = 150.0f;
const float inertia_movement = 0.8f;
const float inertia_rotation = 0.8f;

// depth of field / motion blur settings
int frame_motion_blur = 0;
int frames_motion_blur = 8;
int user_frames_motion_blur = 15;
float dist_focus = 10.;
GLfloat picked_depth = -1;
float aperture = 0.05f;


// jittering for DoF
const glm::vec2 jitters8[8] = {
	glm::vec2(-0.334818, 0.435331),
	glm::vec2(0.286438, -0.393495),
	glm::vec2(0.459462, 0.141540),
	glm::vec2(-0.414498, -0.192829),
	glm::vec2(-0.183790, 0.082102),
	glm::vec2(-0.079263, -0.317383),
	glm::vec2(0.102254, 0.299133),
	glm::vec2(0.164216, -0.054399)
};

const glm::vec2 jitters15[15] = {
	glm::vec2(0.285561, 0.188437),
	glm::vec2(0.360176, -0.065688),
	glm::vec2(-0.111751, 0.275019),
	glm::vec2(-0.055918, -0.215197),
	glm::vec2(-0.080231, -0.470965),
	glm::vec2(0.138721, 0.409168),
	glm::vec2(0.384120, 0.458500),
	glm::vec2(-0.454968, 0.134088),
	glm::vec2(0.179271, -0.331196),
	glm::vec2(-0.307049, -0.364927),
	glm::vec2(0.105354, -0.010099),
	glm::vec2(-0.154180, 0.021794),
	glm::vec2(-0.370135, -0.116425),
	glm::vec2(0.451636, -0.300013),
	glm::vec2(-0.370610, 0.387504) };

const glm::vec2 jitters24[24] = {
	glm::vec2(0.030245,  0.136384),
	glm::vec2(0.018865, -0.348867),
	glm::vec2(-0.350114, -0.472309),
	glm::vec2(0.222181,  0.149524),
	glm::vec2(-0.393670, -0.266873),
	glm::vec2(0.404568,  0.230436),
	glm::vec2(0.098381,  0.465337),
	glm::vec2(0.462671,  0.442116),
	glm::vec2(0.400373, -0.212720),
	glm::vec2(-0.409988,  0.263345),
	glm::vec2(-0.115878, -0.001981),
	glm::vec2(0.348425, -0.009237),
	glm::vec2(-0.464016,  0.066467),
	glm::vec2(-0.138674, -0.468006),
	glm::vec2(0.144932, -0.022780),
	glm::vec2(-0.250195,  0.150161),
	glm::vec2(-0.181400, -0.264219),
	glm::vec2(0.196097, -0.234139),
	glm::vec2(-0.311082, -0.078815),
	glm::vec2(0.268379,  0.366778),
	glm::vec2(-0.040601,  0.327109),
	glm::vec2(-0.234392,  0.354659),
	glm::vec2(-0.003102, -0.154402),
	glm::vec2(0.297997, -0.417965)
};

std::string project_name;

/*
bool full_screen = false;
bool use_fog = true;
bool show_structures = true;
bool show_cloud = true;
bool show_beta_sheet = false;
bool show_bb = true;
bool only_selected_component = false;
bool time_inverted = false;
bool select_peptide = false;
bool use_rainbow = false;
bool show_toolbar = true;
bool use_lod_balancing = true;
bool approximate_accumbuffer = true;
bool prepare_to_highquality = false;
bool show_statistics = true;
bool simulate_handheld = false;
bool use_dark_skin = false;
bool show_tweakbar = false;
*/


void load_settings() {
	std::cout << " * Trying to load last used settings...";

	ifstream inputfile;
	inputfile.open(".settings");

	std::string riga,  value;
	int v;
	char discard[10];
	bool status;
	
	if (inputfile.good()) {
		std::cout << " file exists, parsing...";

		while (!inputfile.eof()) {

			//std::cout << std::endl;

			std::getline(inputfile, riga);
			sscanf(riga.c_str(), "%s : %d", &discard, &v);

			status = (v==1);

			if (std::string(discard) == "SHOWFOG") {
				use_fog = status;
			}
			else if (std::string(discard) == "SHOWSTRUC") {
				show_structures = status;
			}
			else if (std::string(discard) == "SHOWBOBOX") {
				show_bb = status;
			}
			else if (std::string(discard) == "SHOWCLOUD") {
				show_cloud = status;
			}
			else if (std::string(discard) == "SHOWBETA") {
				show_beta_sheet = status;
			}
			else if (std::string(discard) == "DARKTHEME") {
				use_dark_skin = status;
			}
			else if (std::string(discard) == "RAINBOW") {
				use_rainbow = status;
			}
			else if (std::string(discard) == "SELECTPEP") {
				select_peptide = status;
			}
			else {
				cout << "WARNING: " << discard << " option not supported.\n";
			}

			//std::cout << discard << " : " << status << std::endl;

		}
				
		std::cout << " done.\n";
	}
	
	inputfile.close();

}

void save_settings() {
	std::cout << " * Trying to save last used settings...";
	
	ofstream outputfile;
	outputfile.open(".settings");
	
	outputfile << "SHOWFOG   : " << use_fog << std::endl;
	outputfile << "SHOWSTRUC : " << show_structures << std::endl;
	outputfile << "SHOWCLOUD : " << show_cloud << std::endl;
	outputfile << "SHOWBOBOX : " << show_bb << std::endl;
	outputfile << "SHOWBETA  : " << show_beta_sheet << std::endl;
	outputfile << "DARKTHEME : " << use_dark_skin << std::endl;
	outputfile << "RAINBOW   : " << use_rainbow << std::endl;
	outputfile << "SELECTPEP : " << select_peptide << std::endl;
	

	outputfile.close();

}