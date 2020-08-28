#ifndef __OptionS__
#define __OptionS__

#include <gl/GL.h>
#include <GL\GLU.h>
// #include <GL\glut.h>
#include "glm\glm.hpp"

#include <map>

#include "graphics.h"
#include "audio.h"
#include "status.h"
//#include "displays.h"

#include "PLTtextures.h"



#define padding_options 0.05f

class Option {
	
public:

	// Option() { this->position = glm::vec2(0,0); this->size = glm::vec2(100,20); this->text = "Default Option";  };
	Option(
		glm::vec2 const pos,
		glm::vec2 const size,
		std::string const text,
		std::string const texture_on,
		std::string const texture_off,
		bool initial_state = false,
		bool use_border = true
	) { 
		this->position=pos; 
		this->size=size; 
		this->text = text; 
		this->texture_on.Type = GL_TEXTURE_2D;
		this->texture_off.Type = GL_TEXTURE_2D;
		this->texture_on.LoadTexture((char*)texture_on.c_str(), GL_RGBA8, GL_BGRA_EXT);
		this->texture_off.LoadTexture((char*)texture_off.c_str(), GL_RGBA8, GL_BGRA_EXT);
		this->texture_on.SetModulation(GL_DECAL);
		this->texture_off.SetModulation(GL_DECAL);
		this->texture_on.SetRepeat(GL_CLAMP_TO_BORDER, GL_CLAMP_TO_BORDER);
		this->texture_off.SetRepeat(GL_CLAMP_TO_BORDER, GL_CLAMP_TO_BORDER);
		this->texture_on.SetFiltering(GL_LINEAR, GL_LINEAR);
		this->texture_off.SetFiltering(GL_LINEAR, GL_LINEAR);
		this->texture_on.SetMipmaps(true);
		this->texture_off.SetMipmaps(true);
		if (use_border) {
			this->texture_on.SetBorderColor(glm::vec4(.7, .7, .7, 1));
			this->texture_off.SetBorderColor(glm::vec4(.7, .7, .7, 1));
		}
		else {
			this->texture_on.SetRepeat(GL_REPEAT, GL_REPEAT);
			this->texture_off.SetRepeat(GL_REPEAT, GL_REPEAT);
		}
		this->clicked = initial_state;
			
	};

	/// Draw the option box on the screen
	void render() { 		
				
		// enable texture		
		PLTtexture* selected;
		if (this->clicked) {
			selected = &(this->texture_on);
		} else {
			selected = &(this->texture_off);
		}
		selected->Bind();

		// draw box		
		glBegin(GL_QUADS);
			
			glTexCoord2f(-padding_options, 1.f+padding_options);
			glVertex2f( this->position.x, this->position.y + this->size.y );
			
			glTexCoord2f(-padding_options, -padding_options);
			glVertex2f(this->position.x, this->position.y);

			glTexCoord2f(1.f+padding_options, -padding_options);
			glVertex2f( this->position.x + this->size.x, this->position.y );
			
			glTexCoord2f(1.f + padding_options, 1.f + padding_options);
			glVertex2f(this->position.x + this->size.x, this->position.y + this->size.y);

		glEnd();

		// disable texture
		selected->UnBind();	
		
		
	};

	void click() { this->clicked = !this->clicked; };
	void set_click(bool v) { this->clicked = v; };

	std::string get_text() { return this->text; };

	bool click_detect(glm::vec2 pos) {

		if ( pos.x > this->position.x && pos.x < this->position.x + this->size.x ) {
			if ( pos.y > this->position.y && pos.y < this->position.y + this->size.y ) {
				// printf("Detected %s.\n", this->text.c_str());				
				return true;
			}
		}
		return false;
	};

	void set_position(glm::vec2 p) { this->position = p; }

private:

	glm::vec2 position;
	glm::vec2 size;
	std::string text;

	PLTtexture texture_on;
	PLTtexture texture_off;

	bool clicked;

};


class ButtonsHandler {

public:

	ButtonsHandler() { }

	void add_option( glm::vec2 const pos, 
		glm::vec2 const size, 
		std::string const text,
		std::string const texture_on,
		std::string const texture_off,
		void (*callback)(),
		bool initial_state,
		bool use_border) {
		Option* option = new Option( pos, size, text, texture_on, texture_off, initial_state, use_border );
		options_vector.push_back(option);
		this->options_names[option->get_text()] = this->options_vector.size()-1; 
		this->function_pointers.push_back(callback);
	};

	void set_option(std::string name, bool status ) {
		int id = this->options_names[name];
		this->options_vector[id]->set_click(status);		
	};

	int check_click(glm::vec2 pos) {

		if (!show_toolbar) return -1;

		for (unsigned int i=0; i<this->options_vector.size(); i++) {			
			if (options_vector[i]->click_detect(pos)) {
				if (this->function_pointers[i]!=NULL) this->function_pointers[i]();
				return i;
			}
		}

		return -1;

	};

	void render_all() {

		// switch to ortho		
		set_orthographic(true);

		RenderState.temporary_set_depthtest(false);
		RenderState.temporary_set_lighting(false);

		glBlendFunc(GL_ZERO, GL_SRC_COLOR);
		glEnable(GL_TEXTURE_2D);
		glColor4f(0, 0, 0, 0);


		for (unsigned int i=0; i<this->options_vector.size(); i++) 
			this->options_vector[i]->render();

		// return to normal projection and re-enable lighting and depth-test
		set_orthographic(false);

		RenderState.recover_depthtest();
		RenderState.recover_lighting();

		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDisable(GL_TEXTURE_2D);

	}
	
	std::vector<Option*> options_vector;

	std::map<std::string, int> options_names;

	std::vector< void (*)() > function_pointers;

};





#endif