#ifndef __PLTfiles__
#define __PLTfiles__

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include <GL/glew.h>
#include <GL/gl.h>

using namespace std;

#define _CRT_SECURE_NO_DEPRECATE 1


/// This structure is returned by the texture loading functions.
struct PLTimageInfo {

	bool Valid;
	
	GLint Width;		
	GLint Height;
	GLint Depth;	
	GLenum Format;			// eg.: GL_BGRA_EXT
	GLint Components;		// eg.: GL_RGBA8
	GLint DataKind;			// eg.: GL_UNSIGNED_BYTE

	GLbyte* DataPointer;

	GLuint	ImageSize;		// size in bytes of the image

};



/// Define targa header.
#pragma pack(1)
typedef struct
    {
    GLbyte	identsize;              // Size of ID field that follows header (0)
    GLbyte	colorMapType;           // 0 = None, 1 = paletted
    GLbyte	imageType;              // 0 = none, 1 = indexed, 2 = rgb, 3 = grey, +8=rle
    unsigned short	colorMapStart;          // First colour map entry
    unsigned short	colorMapLength;         // Number of colors
    unsigned char 	colorMapBits;   // bits per palette entry
    unsigned short	xstart;                 // image x origin
    unsigned short	ystart;                 // image y origin
    unsigned short	width;                  // width in pixels
    unsigned short	height;                 // height in pixels
    GLbyte	bits;                   // bits per pixel (8 16, 24, 32)
    GLbyte	descriptor;             // image descriptor
    } TGAHEADER;
#pragma pack(8)


#define GL_BGR_EXT                          0x80E0
#define GL_BGRA_EXT                         0x80E1



enum { PLT_TGA_FILE, PLT_MTL_FILE };



/// Recognize files's extension.
unsigned int RecognizeExtension(string path);

/// Returns the extension of the specified file.
string InsulateExtension(string path);

/// Loads a TARGA file. Returns an "image informations" structure.
PLTimageInfo LoadTarga(char*);

/// Loads a TARGA file. Returns a pointer to the memory area.
GLbyte *gltLoadTGA(const char *szFileName, GLint *iWidth, GLint *iHeight, GLint *iComponents, GLenum *eFormat, unsigned int *iSize);

/// saves a TARGA file. 
//void saveTGA(const char* filename, GLint width, GLint height);


/// Parses a TARGA file header, returning its informations in a "image informations" structure.
PLTimageInfo PLTreadTGAHeader(const char * szFileName);

/// Turns a number into a std::string.
string stringify(double x);


void Tokenize(const string& , vector<string>& , string tok = " ");



inline double PLTconvertToDouble(const std::string& s) {
	std::istringstream i(s);
	double x;
	i >> x;
	return x;
};



inline float PLTconvertToFloat(const std::string& s) {
	std::istringstream i(s);
	float x;
	i >> x;
	return x;
};


#endif