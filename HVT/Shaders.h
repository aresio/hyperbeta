#ifndef __SHADERS__
#define __SHADERS__

#include <iostream>
#include <fstream>

#include <GL/GL.h>
#include <GL/glew.h>
#include <cassert>

class Shader {

public:

	Shader(GLint shader_type) {this->shaderId = glCreateShader(shader_type);};
	~Shader() {};

	bool load_source(std::string path) {
		
		this->ShaderCode.clear();
		
		std::ifstream ShaderStream(path, std::ios::in);

		if(ShaderStream.is_open()){
			std::string Line = "";
			while(getline(ShaderStream, Line)) this->ShaderCode += "\n" + Line;
			ShaderStream.close();
		}else{
			printf("ERROR: impossible to open %s.\n", path.c_str());
			//getchar();
			return 0;
		}

		/// std::cout << this->get_code() << std::endl;

		GLint size = this->ShaderCode.size();

		GLchar* const stringa = this->get_code();

		glShaderSource(this->shaderId, 1, &stringa, &size);
		if (GL_NO_ERROR != glGetError()) {
			printf("ERROR: cannot understand source code.\n");
			//getchar();
			exit(1);
		}

		glCompileShader( this->shaderId ) ; 
		GLint success = 0;
		glGetShaderiv(this->shaderId, GL_COMPILE_STATUS, &success);
		if (success==GL_FALSE) {
			printf("ERROR: cannot compile shader.\n");
			
			GLint maxLength = 0;
			glGetShaderiv(this->shaderId, GL_INFO_LOG_LENGTH, &maxLength);

			// The maxLength includes the NULL character
			GLchar* errorLog = (GLchar*) malloc (sizeof(GLchar) * maxLength);
			glGetShaderInfoLog(this->shaderId, maxLength, &maxLength, &errorLog[0]);

			std::cout << errorLog << std::endl;

			// Provide the infolog in whatever manor you deem best.
			// Exit with failure.
			glDeleteShader(this->shaderId); // Don't leak the shader.
			
			//getchar();
			return 0;
		}


	}

	GLint getId() { return this->shaderId; };
	

private:

	GLchar* const get_code() { return (GLchar*) this->ShaderCode.c_str(); };

	std::string ShaderCode;
	GLint shaderId;

};


class Program {

public:

	Program() { this->glslProgId = 0; };

	Program(Shader vs, Shader fs) {
		
		this->glslProgId = glCreateProgram();
		if (GL_NO_ERROR != glGetError()) {
			printf("ERROR: cannot create program.\n");
			//getchar();
			exit(1);
		}

		glAttachShader( this->glslProgId, vs.getId() );
		if (GL_NO_ERROR != glGetError()) {
			printf("ERROR: cannot attach vertex shader.\n");
			//getchar();
			exit(1);
		}
		glAttachShader( this->glslProgId, fs.getId() );
		if (GL_NO_ERROR != glGetError()) {
			printf("ERROR: cannot attach fragment shader.\n");
			//getchar();
			exit(1);
		}
		glLinkProgram ( this->glslProgId );	
		if (GL_NO_ERROR != glGetError()) {
			printf("ERROR: cannot link program.\n");
			//getchar();
			exit(1);
		}

		printf(" * Program successfully loaded, created, attached and linked.\n");

	}

	void enable() { glUseProgram(this->glslProgId); 
	
		GLint error = glGetError();
		if (GL_NO_ERROR != error) {
			switch(error) {
				case GL_INVALID_VALUE:
					printf("Error: invalid value.\n");
					break;
				case GL_INVALID_OPERATION:
					printf("Error: invalid operation.\n");
					break;
				default:
					printf("Error: unknown shader error.\n");
					break;
			}
			//getchar();
			exit(1);
		}
	
	};
	void disable() { glUseProgram(0); };

	GLint getId() { return this->glslProgId; };

private:

	GLint glslProgId;

};

#endif