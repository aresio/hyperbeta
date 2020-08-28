#include "stdafx.h"

#include "PLTtextures.h"
#include "PLTfiles.h"
//#include "PLTmath.h"

#include <iostream>
#include <sstream>
#include <string>
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include "glm\glm.hpp"

using std::cout;


unsigned int GreaterPowerOf2(int n) {

	unsigned int t=2;
	while ( (unsigned int)n > t )	t*=2;
	return t;

};


/* 
	TODO:	binding/unbinding textures is an expensive strategy.
			shoud keep track of bindings and avoid redundancy.
*/
PLTtexture::PLTtexture() {
	this->TextureWidth=0;
	this->TextureHeight=0;	
	this->TextureDepth=0;
	this->TextureUnit=0;
	glGenTextures(1, &this->TextureID);	
	// cout << "WARNING: empty texture created" << endl;
};




PLTtexture::PLTtexture(int width, GLenum t) {

	// Create a new texture.
	glGenTextures(1, &this->TextureID);

	this->Type = GL_TEXTURE_1D;
	this->TextureWidth=width;
	this->TextureHeight=0;	
	this->TextureDepth=0;

	glBindTexture(this->Type, this->TextureID);
		ActualTextureBuild();
	glBindTexture(this->Type, 0);

};



PLTtexture::PLTtexture(int width, int height, GLenum t) {

	// Create a new texture.
	glGenTextures(1, &this->TextureID);

	this->Type = t;
	this->TextureWidth=width;
	this->TextureHeight=height;	
	this->TextureDepth=0;

	glBindTexture(this->Type, this->TextureID);
		ActualTextureBuild();
	glBindTexture(this->Type, 0);
	
};



PLTtexture::PLTtexture(int width, int height, int depth, GLenum t) {

	if ( depth != GreaterPowerOf2(depth) ) {
		cout << "ERROR: depth must be a power of 2.\n";
	} else {

		// Create a new texture.
		glGenTextures(1, &this->TextureID);

		this->Type = t;
		this->TextureWidth=width;
		this->TextureHeight=height;	
		this->TextureDepth=depth;

		glBindTexture(this->Type, this->TextureID);
			ActualTextureBuild();
		glBindTexture(this->Type, 0);

	}

};




bool PLTtexture::ActualTextureBuild() {

	this->MemoryData=0;
	this->Border = 0;

	/* a couple of defaults */
	// glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );	
	this->Modulation = GL_MODULATE;

	glTexParameterf( this->Type, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
	glTexParameterf( this->Type, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameterf( this->Type, GL_TEXTURE_WRAP_S, GL_REPEAT );
	glTexParameterf( this->Type, GL_TEXTURE_WRAP_T, GL_REPEAT );

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	// default
	this->TextureUnit = 0;

	return true;

};






bool PLTtexture::CreateEmpty(int type, GLenum format, GLenum texelformat) {

	// TODO ABSOLUTELY!!
	TextureWidth = this->TextureWidth; 
	TextureHeight = this->TextureHeight;
	TextureDepth = this->TextureDepth;
	
	glBindTexture(type, this->TextureID);

		// glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		switch (type) {
		
			case GL_TEXTURE_1D:
				glTexImage1D(GL_TEXTURE_1D, 0, format, 
					TextureWidth, 0, texelformat, GL_UNSIGNED_BYTE, NULL);
			break;
			case GL_TEXTURE_2D:
				glTexImage2D(GL_TEXTURE_2D, 0, format, 
					TextureWidth, TextureHeight, 0, texelformat, GL_UNSIGNED_BYTE, NULL);
				glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
			break;
			case GL_TEXTURE_3D:
				glTexImage3D(GL_TEXTURE_3D, 0, format, 
					TextureWidth, TextureHeight, TextureDepth, 0, texelformat, GL_UNSIGNED_BYTE, NULL);
			break;

		};


		// TODO: choose mipmaps or not?
		glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);

		SetFiltering();

	glBindTexture(type, 0);

	return true;	// TODO

};




bool PLTtexture::LoadTexture(char* path, GLenum format, GLenum texelformat) {

	// TODO: LOD mipmaps
	// TODO: border, color formats
	
	GLenum err ;
	
	err = glGetError();

	// Let's start binding our previously generated texture.
	glBindTexture(this->Type, this->TextureID);

	// mm..
	// glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	PLTimageInfo iminfo = LoadTextureFromFile(path);

	// err = glGetError();

	/* 
	   Let's load from specified path our data.
	   This call will:
	   - check for existance/validity of the specified texture map
	   - load data into main memory
	   - set the size and texel/pixel formats
	 */
	if (!iminfo.Valid) {
		cout << " * ERROR: cannot load " << path << " texture\n";
		return false;
	};

	this->TextureWidth=iminfo.Width;
	this->TextureHeight=iminfo.Height;
	this->TextureDepth=iminfo.Depth;
	this->MemoryData=iminfo.DataPointer;

	

	switch (Type) {
	
		case GL_TEXTURE_1D:
			glTexImage1D(GL_TEXTURE_1D, 0, format, 
				TextureWidth, 0, texelformat, GL_UNSIGNED_BYTE, MemoryData);
		break;
		case GL_TEXTURE_2D:
			err = glGetError();

				gluBuild2DMipmaps( GL_TEXTURE_2D, format, TextureWidth, TextureHeight,
                   texelformat, GL_UNSIGNED_BYTE, MemoryData );

			//glTexImage2D(GL_TEXTURE_2D, 0, format, 
			//	TextureWidth, TextureHeight, 0, texelformat, GL_UNSIGNED_BYTE, MemoryData);
			err = glGetError();			
			if (err!=0)	cout << "ERROR: check glTexImage2D" << endl;
		break;
		case GL_TEXTURE_3D:
			glTexImage3D(GL_TEXTURE_3D, 0, format, 
				TextureWidth, TextureHeight, TextureDepth, 0, texelformat, GL_UNSIGNED_BYTE, MemoryData);
		break;

		// cubemaps
		case GL_TEXTURE_CUBE_MAP_POSITIVE_X:
		case GL_TEXTURE_CUBE_MAP_NEGATIVE_X:
		case GL_TEXTURE_CUBE_MAP_POSITIVE_Y:
		case GL_TEXTURE_CUBE_MAP_NEGATIVE_Y:
		case GL_TEXTURE_CUBE_MAP_POSITIVE_Z:
		case GL_TEXTURE_CUBE_MAP_NEGATIVE_Z:
			glTexImage2D(Type, 0, format, 
				TextureWidth, TextureHeight, 0, texelformat, GL_UNSIGNED_BYTE, MemoryData);
		break;

		default: // ??
			cout << "ERROR: unrecognized texture type." << endl;
		break;
	};

	// defaults
	SetMipmaps();
	SetFiltering();
	SetClamping();
	SetRepeat();
	SetModulation();

	//if (type==GL_TEXTURE_2D) 
	
	// cout << " * Texture " << path << " loaded" << endl;

	// Dispose everything
	glBindTexture(Type, 0);

	return true;
}



bool PLTtexture::LoadMultipleFrames(string path, string ext, GLenum format, GLenum texelformat) {

	std::ostringstream o;
	PLTimageInfo iminfo;
	GLuint TotalMemorySize;
	GLbyte* pBits;

	string dest = path + stringify(1) + ext;

	iminfo = PLTreadTGAHeader(dest.c_str());
	TotalMemorySize = iminfo.ImageSize*this->GetDepth();


	if (1) {
		cout << "\n * Retrieving informations from file:  " << dest.c_str() << endl;

		cout << "   Every texture's size is: " << iminfo.ImageSize << " bytes\n";
		cout << "   Textures size: " << iminfo.Width << "x" << iminfo.Height << " pixels\n";
		cout << "   3D texture is composed of " << this->GetDepth() << " slices, we allocate " << TotalMemorySize << " bytes\n";
	}


	pBits = (GLbyte *) malloc( TotalMemorySize * sizeof(GLbyte) );
	

	for (unsigned int i=0; i<this->GetDepth(); i++) {

		string dest = path + stringify(i+1) + ext;	
		
		// cout << "Importing texture " << dest << endl;

		iminfo = LoadTextureFromFile((char *)dest.c_str());

		if (!iminfo.Valid) {
			cout << " * ERROR: cannot load " << path << " texture\n";
			return false;	
		}

		memcpy(pBits+iminfo.ImageSize*i, iminfo.DataPointer, iminfo.ImageSize);

	}

	glBindTexture(this->Type, this->TextureID);
		
		glTexImage3D(GL_TEXTURE_3D, 0, format, 
			TextureWidth, TextureHeight, TextureDepth, 0, texelformat, GL_UNSIGNED_BYTE, pBits);


		SetMipmaps();
		SetFiltering();
		SetClamping();
		SetRepeat();
		SetModulation();

	glBindTexture(this->Type, 0);

	free(pBits);
	

	return true;

};




void PLTtexture::SetFiltering(GLenum filtermin, GLenum filtermag) {
	glBindTexture(Type, TextureID);
		glTexParameteri(this->Type, GL_TEXTURE_MIN_FILTER, filtermin);
		glTexParameteri(this->Type, GL_TEXTURE_MAG_FILTER, filtermag);
	glBindTexture(Type, 0);
}



void PLTtexture::SetBias(float lev) {
	glBindTexture(Type, TextureID);
		glTexEnvf(GL_TEXTURE_FILTER_CONTROL, GL_TEXTURE_LOD_BIAS, lev);
	glBindTexture(Type, 0);
}



void PLTtexture::SetMipmaps(bool m) {
	glBindTexture(Type, TextureID);
		glTexParameteri(this->Type, GL_GENERATE_MIPMAP, m);
	glBindTexture(Type, 0);
}



void PLTtexture::SetRepeat(GLenum wraps, GLenum wrapt) {
	glBindTexture(Type, TextureID);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wraps);
		glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapt);
	glBindTexture(Type, 0);
}



void PLTtexture::SetModulation(int kind) {
	glBindTexture(this->Type, this->TextureID);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, (GLfloat)kind);
		this->Modulation = kind;
	glBindTexture(this->Type, 0);
};


void PLTtexture::SetBorderColor(glm::vec4 color) {
	glBindTexture(this->Type, this->TextureID);
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, (float*)&(color));
	glBindTexture(this->Type, 0);
}


PLTimageInfo PLTtexture::LoadTextureFromFile(char* path) {

	PLTimageInfo iminfo;

	switch (RecognizeExtension(path)) {

		// Targa files 
		case PLT_TGA_FILE:
			// cout << " * Recognized TARGA file, loading...";
			iminfo = LoadTarga(path);
			if (!iminfo.Valid) {
				cout << " ERROR! " << path << " is not a valid TGA file.\n";
				return iminfo;
			};

			// cout << " OK\n";

			
			
		break;

		// Unknown type
		default:
			cout << " * ERROR: unsupported texture file (" << path << ")\n";	
			iminfo.Valid=false;
			return iminfo;
		break;

	}

	return iminfo;
}



void PLTtexture::SetClamping(GLenum clamps, GLenum clampt, GLenum clampr) {

	glBindTexture(this->Type, this->TextureID);
		switch (this->Type) {
			case GL_TEXTURE_3D:
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, clampr);
			case GL_TEXTURE_2D:
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, clampt);
			case GL_TEXTURE_1D:
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, clamps);
			break;
		};
	glBindTexture(this->Type, 0);
	
}



void PLTtexture::SetDepthTest(bool on) {

	glBindTexture(this->Type, this->TextureID);
		glTexParameteri(this->Type, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY);
		glTexParameteri(this->Type, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);	
	glBindTexture(this->Type, 0);

};



void PLTtexture::Bind(int TU) {	
	glBindTexture(this->Type, this->TextureID);
	glActiveTexture(GL_TEXTURE0 + TU);
	this->TextureUnit = TU;	// salvo l'ordinale, non l'enumeratore
};

void PLTtexture::UnBind(int TU) {
	glActiveTexture(GL_TEXTURE0 + TU);	
	glBindTexture(this->Type, 0);
};

void PLTtexture::UnBind() {
	glActiveTexture( GetTextureUnit() );	// determino l'enumeratore dal numerale
	glBindTexture(this->Type, 0);
};




void PLTtexture::SetCoordReplace(bool on) {
	glBindTexture(this->Type, this->TextureID);
		glTexEnvi(GL_POINT_SPRITE, GL_COORD_REPLACE, on);
	glBindTexture(this->Type, 0);
};



void PLTtexture::SetCubeMapGeneration(bool on) {
	glBindTexture(this->Type, this->TextureID);
		/*glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
		glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
		glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);*/
		/// TOGLIMI
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_GENERATE_MIPMAP, GL_TRUE);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	glBindTexture(this->Type, 0); 
};



PLTtexture::PLTtexture(const PLTtexture &t) {
	memcpy (this, &t, sizeof(*this));
};