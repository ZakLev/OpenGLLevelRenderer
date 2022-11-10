#pragma once
#pragma once
#include <vector>
#include <string>
#include "../Gateware/Gateware.h"
#define GATEWARE_ENABLE_MATH  // Math Library
#include "h2bParser.h"
#include "../Gateware/Gateware.h"



class Model // Object Oriented not Data Oriented -- except they are in seperate vectors instead of a model object
{

	GW::SYSTEM::GWindow win;
	GW::GRAPHICS::GOpenGLSurface ogl;
	GW::MATH::GMatrix matMath;
	GLuint vertexArray = 0;
	GLuint vertexBufferObject = 0;
	GLuint indiciesBuffer = 0;
	GLuint vertexShader = 0;
	GLuint fragmentShader = 0;
	GLuint shaderExecutable = 0;
	GLuint uboBuffer = 0;
	//struct UBO_DATA
	//{
	//	// global scene data, updated each frame
	//	GW::MATH::GVECTORF sunDirection, sunColor; //lighting info
	//	GW::MATH::GMATRIXF viewMatrix, projectionMatrix; // viewing info
	//	//sub-mesh transform and material data, updated each draw
	//	GW::MATH::GMATRIXF world; // final world space transform
	//	OBJ_ATTRIBUTES material; // color/texture of surface
	//	GW::MATH::GVECTORF sunAmbient, camPos;
	//} UBO;
public:
	Model() {}
	GW::MATH::GMATRIXF worldPosition = {};
	std::string Name = {};
	std::string NamePath = {};
	H2B::Parser parser = {};
	/*GW::MATH::GMATRIXF Model::GetWorldPosition() { return worldPosition; };
	void Model::SetWorldPosition( GW::MATH::GMATRIXF& wp) { worldPosition = wp; };
	*/
	bool LoadModelData(const char* filePath) {
		return parser.Parse(filePath);
	}
	bool UploadModelDataToGPU(GW::SYSTEM::GWindow _win, GW::GRAPHICS::GOpenGLSurface _ogl)
	{

		
	}
	//void DrawModel()
	//{
	//	
	//	// setup the pipeline
	//		//World
	//	GLint locationW = glGetUniformLocation(shaderExecutable, "world");
	//	//glUniformMatrix4fv(locationW, 1, GL_FALSE, (GLfloat*)&worldMat);
	//	//glUniformMatrix4fv(locationW, 1, GL_FALSE, (GLfloat*)&lvlData.worldPositions[cm]);
	//	glUniformMatrix4fv(locationW, 1, GL_FALSE, (GLfloat*)&worldPosition);
	//	//Camera
	//	GLint locationV = glGetUniformLocation(shaderExecutable, "viewMatrix");
	//	glUniformMatrix4fv(locationV, 1, GL_FALSE, (GLfloat*)&viewMat);
	//	//Projection
	//	GLint locationP = glGetUniformLocation(shaderExecutable, "projectionMatrix");
	//	glUniformMatrix4fv(locationP, 1, GL_FALSE, (GLfloat*)&projMat);

	//	// TODO: Part 1e
	//	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
	//	glEnableVertexAttribArray(0);
	//	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
	//	glUseProgram(shaderExecutable);
	//	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)12);
	//	glEnableVertexAttribArray(1);
	//	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
	//	glUseProgram(shaderExecutable);
	//	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)24);
	//	glEnableVertexAttribArray(2);
	//	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
	//	glUseProgram(shaderExecutable);
	//	// now we can draw
	//	glBindVertexArray(vertexArray);
	//	glUseProgram(shaderExecutable);
	//	// TODO: Part 1d
	//	// TODO: Part 1h

	//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indiciesBuffer);
	//	glUseProgram(shaderExecutable);
	//	// TODO: Part 2e
	//	unsigned int block_index = glGetUniformBlockIndex(shaderExecutable, "UBO");
	//	// TODO: Part 2f
	//	GLuint binding_point_index = 0;
	//	glBindBufferBase(GL_UNIFORM_BUFFER, binding_point_index, uboBuffer);
	//	// TODO: Part 2g
	//	glUniformBlockBinding(shaderExecutable, block_index, 0);
	//	// TODO: Part 3b
	//
	//	for (int i = 0; i < parser.meshCount; i++)
	//	{

	//		UBO.world = worldPosition;
	//		//UBO.world = GW::MATH::GIdentityMatrixF;
	//			
	//		OBJ_ATTRIBUTES* obj = (OBJ_ATTRIBUTES*)&models[cm].parser.materials[i].attrib;
	//		UBO.material = *obj;

	//		glBindBuffer(GL_ARRAY_BUFFER, uboBuffer);
	//		// get pointer
	//		void* ptr = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	//		// now copy data into memory
	//		memcpy(ptr, &UBO, sizeof(UBO));
	//		// make sure to tell OpenGL we're done with the pointer
	//		glUnmapBuffer(GL_ARRAY_BUFFER);

	//		
	//		glDrawElements(GL_TRIANGLES, models[cm].parser.batches[i].indexCount, GL_UNSIGNED_INT, (GLvoid*)(sizeof(unsigned int) * models[cm].parser.batches[i].indexOffset));
	//}
};
