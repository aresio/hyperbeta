#include "stdafx.h" 

#include "PLTtextures.h"
#include "PLTfbos.h"

#include <iostream>

using namespace std;


PLTfbo::PLTfbo(GLuint width, GLuint height) {

	// first, generate FB and create handler
	glGenFramebuffers(1, &FBOhandler);
	glBindFramebuffer(GL_FRAMEBUFFER, FBOhandler);

	if (!glIsFramebuffer(FBOhandler)) {
		cout << " * ERROR: unable to create FBO\n";		
		return;
	}
	
	// clear references to yet unexistant RBOs
	memset(RBOs, 0, sizeof(RBOs));

	this->FBOwidth = width;
	this->FBOheight = height;

	glGenTextures(16, ColorTexture);

	cout << " * New empty FBO created (" << FBOhandler << ") \n";


	// ?
	glBindFramebufferEXT(GL_FRAMEBUFFER, 0);

};


/** 
	// TODO
*/
bool PLTfbo::AttachDepthBuffer(bool multisampled, GLuint samples) {

	// Create frame buffer object and bind it				
	glBindFramebuffer(GL_FRAMEBUFFER, FBOhandler);
					
		glGenRenderbuffers(1, &RBOs[18]);
		
		glBindRenderbuffer(GL_RENDERBUFFER, RBOs[18]);	
			
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, FBOwidth, FBOheight); 			
				
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, RBOs[18]);

	if (GLenum status=ValidateFBO()) {
		printf ("ERROR: can't add depth buffer to FBO, error code: %x\n", status); 		
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		return false;
		
	} else {
		cout << " * Depth buffer successfully added to FBO" << endl;		
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		return true;
	}

}


/** 
	Creates the frame buffer object for the convolution of shadows		
*/
bool PLTfbo::AttachColorBuffer(GLuint n, bool multisampled, GLuint samples) {

	// Create frame buffer object and bind it				
	glBindFramebuffer(GL_FRAMEBUFFER, FBOhandler);
					
		glGenRenderbuffers(1, &RBOs[n]);
		
		glBindRenderbuffer(GL_RENDERBUFFER, RBOs[n]);	
			
			glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA, FBOwidth, FBOheight); 			
				
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+n, GL_RENDERBUFFER, RBOs[n]);

	if (GLenum status=ValidateFBO()) {
		printf ("ERROR: can't add color buffer to FBO, error code: %x\n", status); 		
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		return false;
		
	} else {
		cout << " * Color buffer " << n << " successfully added to FBO" << endl;		
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		return true;
	}

}



bool PLTfbo::AttachTexture(GLenum target, PLTtexture* tex) {

	glBindFramebuffer(GL_FRAMEBUFFER, FBOhandler);
		glFramebufferTexture2D(GL_FRAMEBUFFER, target, tex->Type, tex->TextureID, 0);	
		GLuint status = glCheckFramebufferStatus(GL_FRAMEBUFFER); 
		cout << " * Texture for RTT attached \n";
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return (status==GL_FRAMEBUFFER_COMPLETE);		// TODO

}




// TODO: does it really work??
GLenum PLTfbo::ValidateFBO() {

	// check if everything worked correctly
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status==GL_FRAMEBUFFER_COMPLETE) {
		return 0;
	} else {
		return status;
	}

}

bool PLTfbo::ValidateRBO(GLuint r) {

	if (!glIsRenderbuffer(r))  {

		// error!!
		cout << " * ERROR: cannot create a new RBO\n";
		return false;

	};
	 
	return true;

};



bool PLTfbo::SetDrawBuffers(GLenum* d) {

	glBindFramebuffer(GL_FRAMEBUFFER, FBOhandler);

		//TODO: check existance
		glDrawBuffers(sizeof(d)/sizeof(GLenum), d);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return true;

};



void PLTfbo::Activate() {
	
	glBindFramebuffer(GL_FRAMEBUFFER, FBOhandler);
	
};



void PLTfbo::Deactivate() {
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

};



void PLTfbo::SetBuffers(GLenum read, GLenum write) {

	glBindFramebuffer(GL_FRAMEBUFFER, FBOhandler);
		glReadBuffer(read);
		glDrawBuffer(write);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

};