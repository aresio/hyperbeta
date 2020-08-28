#ifndef __PLTtextures__
#define __PLTtextures__

#include "PLTfiles.h"

#include <GL/glew.h>
#include <GL/gl.h>
#include "glm\glm.hpp"

/// Defines a single texture.
/*!
	This class in conceived in order to pass arguments to shaders.
*/
class PLTtexture {

public:
	//@{
	///name Constructors.
	PLTtexture();
	PLTtexture(int width, GLenum t);
	PLTtexture(int width, int height, GLenum t);
	PLTtexture(int width, int height, int depth, GLenum t);
	PLTtexture(const PLTtexture &);		// copy constructor.
	//@}

	

	/// Transfers the texture from disk to GPU memory.
	bool LoadTexture(char* path, GLenum format=GL_RGB, GLenum texelformat=GL_RGB);

	/// Transfers the texture from disk to main memory, before transferring it to a 3D texture.
	bool LoadFrame(char* path, unsigned int level, GLenum format=GL_RGB, GLenum texelformat=GL_RGB);
	bool LoadMultipleFrames(string path, string ext=".tga", GLenum format=GL_RGB, GLenum texelformat=GL_RGB);

	/// Biases the mipmap level.
	void SetBias(float);

	/// Sets filtering for minification and magnification.
	void SetFiltering(GLenum filtermin=GL_LINEAR_MIPMAP_LINEAR, GLenum filtermag=GL_LINEAR);

	/// Set lateral clamping mode.
	void SetClamping(GLenum clamps=GL_CLAMP_TO_EDGE, GLenum clampt=GL_CLAMP_TO_EDGE, GLenum clampr=GL_CLAMP_TO_EDGE);

	/// Set repeating of texture.
	void SetRepeat(GLenum wraps=GL_REPEAT, GLenum wrapt=GL_REPEAT);

	/// Set kind of modulation.
	void SetModulation(int kind = GL_MODULATE);	

	/// Set depth test.
	void SetDepthTest(bool on=true);

	/// Set coordinate replacement (for point sprites).
	void SetCoordReplace(bool on=true);

	/// Enables Cube Map coordinates generation.
	void SetCubeMapGeneration(bool on=true);

	/// Sets mipmaps generation.
	void SetMipmaps(bool on=true);

	// Sets the border color of the texture (in case of clamping).
	void SetBorderColor(glm::vec4 color);

	/// Binds a specific texture.
	void Bind(int TU=0);
	void UnBind();
	void UnBind(int );


	/// Creates an empty texture map.
	bool CreateEmpty(int type = GL_TEXTURE_2D, GLenum format = GL_RGBA8, GLenum texelformat = GL_RGB);
	bool ActualTextureBuild();

	//@{
	///name Getters.
	unsigned int GetWidth() { return this->TextureWidth; };
	unsigned int GetHeight() { return this->TextureHeight; };
	unsigned int GetDepth() { return this->TextureDepth; };
	GLuint GetTextureUnit() { return GL_TEXTURE0 + this->TextureUnit; };		// returns the EXACT enumerator, not the ordinal 
	int GetModulation() { return this->Modulation; };
	//@}


	/// Handler for this texture.
	GLuint TextureID;

	/// Type of texture we want.
	GLenum Type;

private:

	/// Service routine for loading texture from files.
	PLTimageInfo LoadTextureFromFile(char*);

	/// Pointer to pixel datas in main memory.
	void* MemoryData;

	/// Border width.
	GLuint Border;

	/// Texture dimensions.
	GLuint TextureWidth, TextureHeight, TextureDepth;

	/// Last texture units.
	GLuint TextureUnit;

	/// Kind of modulation set.
	int Modulation;

};


#endif