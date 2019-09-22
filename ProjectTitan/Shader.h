#pragma once

#include <glad/glad.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader
{
private:
	unsigned int _id;

public:
	// Return the shader program ID
	unsigned int getId();

	// constructor reads and builds the shader
	Shader(const GLchar* vertexPath, const GLchar* fragmentPath);

	// use/activate the shader
	void use();

	// utility uniform functions
	void setBool(const std::string& name, bool value) const;
	void setInt(const std::string& name, int value) const;
	void setFloat(const std::string& name, float value) const;
};
