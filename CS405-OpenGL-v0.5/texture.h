#ifndef TEXTURE_H
#define TEXTURE_H

//#include <GL/glew.h>
#include "Include/glad/glad.h"

#include "stb_image.h"

// Texture2D is able to store and configure a texture in OpenGL.
// It also hosts utility functions for easy management.
class Texture3D {
public:
	//GLuint getID() { return this->ID; }
	// Holds the ID of the texture object, used for all texture operations
	// to reference to this particular texture
	GLuint ID;

	GLuint getHeight() { return this->Height; }
	void setHeight(GLuint height) { this->Height = height; }
	GLuint getWidth() { return this->Width; }
	void setWidth(GLuint width) { this->Width = width; }

	GLuint getInternal_Format() { return this->Internal_Format; }
	void setInternal_Format(GLuint internal_format) { this->Internal_Format = internal_format; }

	GLuint getImage_Format() { return this->Image_Format; }
	void setImage_Format(GLuint image_format) { this->Image_Format = image_format; }

	GLuint getWarp_S() { return this->Wrap_S; }
	void setWarp_S(GLuint warp_s) { this->Wrap_S = warp_s; }
	GLuint getWarp_T() { return this->Wrap_T; }
	void setWarp_T(GLuint warp_t) { this->Wrap_T = warp_t; }
	GLuint getFilter_Min() { return this->Filter_Min; }
	void setFilter_Min(GLuint filter_min) { this->Filter_Min = filter_min; }
	GLuint getFilter_Max() { return this->Filter_Max; }
	void setFilter_Max(GLuint filter_max) { this->Filter_Max = filter_max; }

	// Constructor (sets default texture modes)
	Texture3D();
	// Generates texture from image data
	void Generate(GLuint width, GLuint height, unsigned char* data);
	// Binds the texture as the current active GL_TEXTURE_2D texture object
	void Bind() const;

private:
	// Holds the ID of the texture object, used for all texture operations
	// to reference to this particular texture
	//GLuint ID;
	// Texture image dimensions
	GLuint Width, Height;

	// Texture Format
	GLuint Internal_Format;
	GLuint Image_Format;

	// Texture configuration
	GLuint Wrap_S; // Wrapping mode on S axis
	GLuint Wrap_T; // Wrapping mode on T axis
	GLuint Filter_Min; // Filtering mode if texture pixels < screen pixels
	GLuint Filter_Max; // Filtering mode if texture pixels > screen pixels
};


Texture3D::Texture3D()
	: Width(0), Height(0), Internal_Format(GL_RGB), 
	Image_Format(GL_RGB), Wrap_S(GL_REPEAT), Wrap_T(GL_REPEAT), 
	Filter_Min(GL_LINEAR), Filter_Max(GL_LINEAR)
{
	glGenTextures(1, &this->ID);
}

void Texture3D::Generate(GLuint width, GLuint height, unsigned char* data)
{
	this->Width = width;
	this->Height = height;
	// Create Texture
	glBindTexture(GL_TEXTURE_2D, this->ID);
	glTexImage2D(GL_TEXTURE_2D, 0, this->Internal_Format, width, height, 0, this->Image_Format, GL_UNSIGNED_BYTE, data);
	// Set Texture wrap and filter modes
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, this->Wrap_S);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, this->Wrap_T);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, this->Filter_Min);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, this->Filter_Max);
	// Unbind texture
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture3D::Bind() const
{
	glBindTexture(GL_TEXTURE_2D, this->ID);
}

#endif 
