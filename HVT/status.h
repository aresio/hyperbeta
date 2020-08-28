#ifndef __STATUS__
#define __STATUS__

#include <GL\GL.h>

#define OFF false 
#define ON true 

class RenderingState {

public:

	RenderingState() { this->lighting = true; this->culling = true; this->depthtest = true; this->fog = false; }

	void temporary_set_lighting(bool how) { 
		if (how) glEnable(GL_LIGHTING);
		else     glDisable(GL_LIGHTING);
		this->lighting_backup = this->lighting;
	}
	void recover_lighting() {
		this->lighting = this->lighting_backup;
		this->autoset_lighting();
	}
	void set_lighting(bool how) {
		this->lighting = how;
		this->autoset_lighting();
	}
	void autoset_lighting() {
		if (this->lighting) glEnable(GL_LIGHTING);
		else				glDisable(GL_LIGHTING);
	}

	void temporary_set_culling(bool how) {
		if (how) glEnable(GL_CULL_FACE);
		else     glDisable(GL_CULL_FACE);
		this->culling_backup = this->culling;
	}
	void recover_culling() {
		this->culling = this->culling_backup;
		this->autoset_culling();
	}
	void set_culling(bool how) {
		this->culling = how;
		this->autoset_culling();
	}
	void autoset_culling() {
		if (this->culling) glEnable(GL_CULL_FACE);
		else				glDisable(GL_CULL_FACE);
	}

	void temporary_set_fog(bool how) { 
		if (how) glEnable(GL_FOG);
		else     glDisable(GL_FOG);
		this->fog_backup = this->fog;
	}
	void recover_fog() {
		this->fog = this->fog_backup;
		this->autoset_fog();
	}
	void set_fog(bool how) {
		this->fog = how;
		this->autoset_fog();
	}

	void temporary_set_depthtest(bool how) { 
		if (how) glEnable(GL_DEPTH_TEST);
		else     glDisable(GL_DEPTH_TEST);
		this->fog_backup = this->fog;
	}
	void recover_depthtest() {
		this->fog = this->fog_backup;
		this->autoset_depthtest();
	}
	void set_depthtest(bool how) {
		this->fog = how;
		this->autoset_depthtest();
	}

private:


	void autoset_fog() {
		if (this->fog) glEnable(GL_FOG);
		else		   glDisable(GL_FOG);
	}

	void autoset_depthtest() {
		if (this->depthtest) glEnable(GL_DEPTH_TEST);
		else				glDisable(GL_DEPTH_TEST);
	}

	bool lighting;
	bool lighting_backup;

	bool culling;
	bool culling_backup;

	bool depthtest;
	bool depthtest_backup;	

	bool fog;
	bool fog_backup;	

};

RenderingState RenderState;

#endif