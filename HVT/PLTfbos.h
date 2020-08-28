#ifndef __PLTfbos__
#define __PLTfbos__

#include <GL/glew.h>
#include <GL/gl.h>

#include "PLTtextures.h"

/// Defines a single frame buffer object.
/*!
	The idea behind this is to create a "description" of an FBO, and activate it on request.
*/
class PLTfbo {

public:
	/// Constructor.
	PLTfbo(GLuint w, GLuint h);

	//@{
	///@name Attach to the FBO a new renderbuffer.
	bool AttachColorBuffer(GLuint number, bool multisampled = false, GLuint samples = 4);
	bool AttachDepthBuffer(bool multisampled = false, unsigned int samples = 4);
	bool AttachStencilBuffer(bool multisampled = false, unsigned int samples = 4);
	bool AttachTexture(GLuint TexID, PLTtexture* tex);
	//@}

	//@{
	///name Buffers controls.
	bool SetDrawBuffers(GLenum* d);
	void SetBuffers(GLenum, GLenum);
	//@}


	//@{
	///name Getters.
	GLuint GetWidth() { return this->FBOwidth; };
	GLuint GetHeight() { return this->FBOheight; };
	//@}


	void Activate();
	void Deactivate();

	
private:
	
	/// Array of every possible render buffer attachable.
	GLuint RBOs[20];

	
	/// Size of the FBO.
	GLuint FBOwidth, FBOheight;

	/// Depth texture (useful for shadow mapping)
	GLuint LocalDepthTexture;
	GLuint ColorTexture[16];

	/// Verify validity of FBO.
	bool ValidateRBO(GLuint);
	GLenum ValidateFBO();

	/// Handler to this.
	GLuint FBOhandler;


};



#endif