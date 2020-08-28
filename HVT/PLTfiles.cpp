#include "stdafx.h"

#include "PLTfiles.h"

#include <iostream>
#include <sstream>
#include <string>

using namespace std;


unsigned int RecognizeExtension(string path) {

	if (InsulateExtension(path)=="TGA") 
		return PLT_TGA_FILE;

	if (InsulateExtension(path)=="MTL")
		return PLT_MTL_FILE;

	cout << "ERROR: file format not recognized\n";
	return 0;

};



string InsulateExtension(string path) {
	// ultimo punto	
	string::size_type last = path.find_last_of('.');
	for (unsigned int i=0; i<path.length(); i++)
		path[i] = toupper(path[i]);
	return path.substr(last+1);
};



PLTimageInfo LoadTarga(char* path) {
	
	PLTimageInfo i; 		
	
	// dirty... TODO
	i.DataPointer = gltLoadTGA(path, &i.Width, &i.Height, &i.Components, &i.Format, &i.ImageSize);  	
	i.DataKind = GL_UNSIGNED_BYTE;
	
	if (i.DataPointer) {
		// cout << " texture loaded in main memory\n";
		i.Valid = true;
	} else {
		// cout << " ERROR! texture invalid \n";
		i.Valid = false;
	};
	

	return i;
};

//
//void saveTGA(const char* filename, GLint windowWidth, GLint windowHeight) {
//
//	screenshot_fbo->Activate();
//	TGAHEADER savetga;
//	savetga.identsize = 0;
//	savetga.colorMapType = 0;
//	savetga.imageType = 2;
//	savetga.colorMapStart = 0;
//	savetga.colorMapLength = 0;
//	savetga.colorMapBits = 0;
//	savetga.xstart = 0;
//	savetga.ystart = 0;
//	savetga.width = windowWidth;
//	savetga.height = windowHeight;
//	savetga.bits = 24;
//	savetga.descriptor = 8;
//
//	const int numberOfPixels = windowWidth * windowHeight * 3;
//	unsigned char* pixels = new GLubyte [numberOfPixels];
//
//	glPixelStorei(GL_PACK_ALIGNMENT, 1);
//	glReadBuffer(GL_FRONT);
//	glReadPixels(0, 0, windowWidth, windowHeight, GL_BGR_EXT, GL_UNSIGNED_BYTE, pixels);
//
//	FILE *outputFile = fopen(filename, "w");
//
//	fwrite(&savetga, sizeof(savetga), 1, outputFile);
//	fwrite(pixels, numberOfPixels, 1, outputFile);
//	fclose(outputFile);
//
//	printf("Finish writing to file.\n");
//}
//




////////////////////////////////////////////////////////////////////
// Allocate memory and load targa bits. Returns pointer to new buffer,
// height, and width of texture, and the OpenGL format of data.
// Call free() on buffer when finished!
// This only works on pretty vanilla targas... 8, 24, or 32 bit color
// only, no palettes, no RLE encoding.
GLbyte *gltLoadTGA(const char *szFileName, GLint *iWidth, GLint *iHeight, GLint *iComponents, GLenum *eFormat, unsigned int *iSize)
    {
    FILE *pFile;					// File pointer
    TGAHEADER tgaHeader;			// TGA file header
    unsigned long lImageSize;		// Size in bytes of image
    short sDepth;					// Pixel depth;
    GLbyte	*pBits = NULL;          // Pointer to bits
    
    // Default/Failed values
    *iWidth = 0;
    *iHeight = 0;
    *eFormat = GL_BGR_EXT;
    *iComponents = GL_RGB8;
    
    // Attempt to open the file
	fopen_s(&pFile, szFileName, "rb");
	
	
    if(pFile == NULL)
        return NULL;
        
    // Read in header (binary)
    fread(&tgaHeader, 18/* sizeof(TGAHEADER)*/, 1, pFile);
    
    // Do byte swap for big vs little endian
#ifdef __APPLE__
    BYTE_SWAP(tgaHeader.colorMapStart);
    BYTE_SWAP(tgaHeader.colorMapLength);
    BYTE_SWAP(tgaHeader.xstart);
    BYTE_SWAP(tgaHeader.ystart);
    BYTE_SWAP(tgaHeader.width);
    BYTE_SWAP(tgaHeader.height);
#endif

        
    // Get width, height, and depth of texture
    *iWidth = tgaHeader.width;
    *iHeight = tgaHeader.height;
    sDepth = tgaHeader.bits / 8;
    
    // Put some validity checks here. Very simply, I only understand
    // or care about 8, 24, or 32 bit targas.
    if(tgaHeader.bits != 8 && tgaHeader.bits != 24 && tgaHeader.bits != 32)
        return NULL;

    // Calculate size of image buffer
    lImageSize = tgaHeader.width * tgaHeader.height * sDepth;

	*iSize = lImageSize;
    
    // Allocate memory and check for success
    pBits = (GLbyte *) malloc(lImageSize * sizeof(GLbyte));
    if(pBits == NULL)
        return NULL;
    
    // Read in the bits
    // Check for read error. This should catch RLE or other 
    // weird formats that I don't want to recognize
    if(fread(pBits, lImageSize, 1, pFile) != 1)
        {
        free(pBits);
        return NULL;
        }
    
    // Set OpenGL format expected
    switch(sDepth)
        {
        case 3:     // Most likely case
            *eFormat = GL_BGR_EXT;
            *iComponents = GL_RGB8;
            break;
        case 4:
            *eFormat = GL_BGRA_EXT;
            *iComponents = GL_RGBA8;
            break;
        case 1:
            *eFormat = GL_LUMINANCE;
            *iComponents = GL_LUMINANCE8;
            break;
        };
        
    
    // Done with File
    fclose(pFile);
        
    // Return pointer to image data
    return pBits;
}





////////////////////////////////////////////////////////////////////
// Allocate memory and load targa bits. Returns pointer to new buffer,
// height, and width of texture, and the OpenGL format of data.
// Call free() on buffer when finished!
// This only works on pretty vanilla targas... 8, 24, or 32 bit color
// only, no palettes, no RLE encoding.
PLTimageInfo PLTreadTGAHeader (const char *szFileName)
    {
    FILE *pFile;					// File pointer
    TGAHEADER tgaHeader;			// TGA file header
//    unsigned long lImageSize;		// Size in bytes of image
    short sDepth;					// Pixel depth;
    GLbyte	*pBits = NULL;          // Pointer to bits
	PLTimageInfo ret;
    
    // Default/Failed values
	ret.Width = 0;
	ret.Height = 0;
	ret.Format = GL_BGR_EXT;
	ret.Components = GL_RGB8;
    
    // Attempt to open the fil    
	fopen_s(&pFile, szFileName, "rb");
	
	
	if(pFile == NULL) {
		cout << "ERROR: empty TGA file\n";
		ret.Valid = false;
        return ret;
	}
        
    // Read in header (binary)
    fread(&tgaHeader, 18/* sizeof(TGAHEADER)*/, 1, pFile);
    
    // Do byte swap for big vs little endian
#ifdef __APPLE__
    BYTE_SWAP(tgaHeader.colorMapStart);
    BYTE_SWAP(tgaHeader.colorMapLength);
    BYTE_SWAP(tgaHeader.xstart);
    BYTE_SWAP(tgaHeader.ystart);
    BYTE_SWAP(tgaHeader.width);
    BYTE_SWAP(tgaHeader.height);
#endif

        
    // Get width, height, and depth of texture
	ret.Width = tgaHeader.width;
	ret.Height = tgaHeader.height;	
    sDepth = tgaHeader.bits / 8;
    
    // Put some validity checks here. Very simply, I only understand
    // or care about 8, 24, or 32 bit targa's.
	if(tgaHeader.bits != 8 && tgaHeader.bits != 24 && tgaHeader.bits != 32) {
		cout << "ERROR: TGA format unknown\n"; 
		ret.Valid = false;
        return ret;
	}

    // Calculate size of image buffer
	ret.ImageSize = tgaHeader.width * tgaHeader.height * sDepth;

    // Set OpenGL format expected
    switch(sDepth)
        {
        case 3:     // Most likely case
			ret.Format = GL_BGR_EXT;
			ret.Components = GL_RGB8;
            break;
        case 4:
            ret.Format = GL_BGRA_EXT;
            ret.Components = GL_RGBA8;
            break;
        case 1:
            ret.Format = GL_LUMINANCE;
            ret.Components = GL_LUMINANCE8;
            break;
        };
        
    
    // Done with File
    fclose(pFile);
        
    // Return pointer to image data
    // return pBits;
	ret.Valid = true;
	return ret;
};




string stringify(double x)
 {
   std::ostringstream o;
   if (!(o << x))
     cout << "argh\n";
   return o.str();
 };



void Tokenize(const string& src, vector<string>& tokens, string tok )
{       

	bool trim=false; 
	string null_subst="";
        
		size_t pre_index = 0, index = 0, len = 0;   
		while( ( index = src.find_first_of( tok , pre_index ) ) != string::npos )
        {       
                if( (len = index-pre_index)!=0 )   
                        tokens.push_back(src.substr(pre_index, len));
                else if(trim==false)   
                        tokens.push_back(null_subst);
                pre_index = index+1;
        }   
        string endstr = src.substr(pre_index);   
        if( trim==false ) tokens.push_back( endstr.empty()?
                        
                        null_subst:endstr );   
        else if( !endstr.empty() ) tokens.push_back(endstr);
        
	return;
}