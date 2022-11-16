#pragma once
#pragma once

#include <vector>
#include <string>
#include "../Gateware/Gateware.h"
#define GATEWARE_ENABLE_MATH  // Math Library
#include "h2bParser.h"
#include "../Assets/FSLogo.h"



class Model 
{

	GW::SYSTEM::GWindow win;
	GW::GRAPHICS::GOpenGLSurface ogl;
	// what we need at a minimum to draw a triangle
	GLuint vertexArray = 0;
	GLuint vertexBufferObject = 0;
	// TODO: Part 1g
	GLuint indiciesBuffer = 0;
	
	// TODO: Part 2c
	GLuint uboBuffer = 0;
	// TODO: Part 2a
	
	GW::MATH::GVECTORF lightDir = { -1,-1,-2,0 };
	GW::MATH::GVECTORF lightColor = { 0.9f,0.9f,1,1 };
	GW::MATH::GVECTORF lightAmbient = { 0.25f,0.25f,0.35f,1 };
	float fov = G_DEGREE_TO_RADIAN_F(65.0f); float fNear = .1f; float fFar = 100.0f; float AR;
	std::chrono::steady_clock::time_point prevTime;
	std::chrono::microseconds FPS;
	float ftheta = 0;
	// TODO: Part 2b
	struct UBO_DATA
	{
		// global scene data, updated each frame
		GW::MATH::GVECTORF sunDirection, sunColor; //lighting info
		GW::MATH::GMATRIXF viewMatrix, projectionMatrix; // viewing info
		//sub-mesh transform and material data, updated each draw
		GW::MATH::GMATRIXF world; // final world space transform
		OBJ_ATTRIBUTES material; // color/texture of surface
		GW::MATH::GVECTORF sunAmbient, camPos;
	} UBO;
public:
	Model() {}
		GW::MATH::GMATRIXF worldPosition = {};
		//GW::I::GCollisionImplementation col;
		std::string Name = {};
		std::string NamePath = {};
		H2B::Parser parser = {};
		std::chrono::steady_clock::time_point prevTimeInput;
		std::chrono::microseconds FPSInput;
		unsigned int screenHeight;
		unsigned int screenWidth;
			GW::MATH::GAABBMMF BB;
		
		bool UploadModelDataToGPU(GW::SYSTEM::GWindow _win, GW::GRAPHICS::GOpenGLSurface _ogl)
		{
			//int cm = 1;
			/*for (int cm = 0; cm < models.size(); cm++)
			{*/

			win = _win;
			ogl = _ogl;
			
			////Camera
			GW::MATH::GVECTORF cameraPos = { 0.75f,0.25f,1.5f };
			
			// TODO: Part 2b

			UBO.sunDirection = lightDir;
			UBO.sunColor = lightColor;
		//	UBO.viewMatrix = viewMat;
		//	UBO.projectionMatrix = projMat;
		//	UBO.world = worldMat;
			
			OBJ_ATTRIBUTES* obj = (OBJ_ATTRIBUTES*)&parser.materials[0].attrib;
			UBO.material = *obj;
			
			//UBO.material = (OB)lvlData.parsers[0].materials[0].attrib;


			// TODO: Part 4e
			UBO.sunAmbient = lightAmbient;
			UBO.camPos = cameraPos;


			


			// Link Needed OpenGL API functions
			LoadExtensions();
			// In debug mode we link openGL errors to the console
	//#ifndef NDEBUG
	//		glEnable(GL_DEBUG_OUTPUT);
	//		glDebugMessageCallback(MessageCallback, 0);
	//#endif


			glGenVertexArrays(1, &vertexArray);
			glGenBuffers(1, &vertexBufferObject);
			glBindVertexArray(vertexArray);
			glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
			//glBufferData(GL_ARRAY_BUFFER, sizeof(parser.vertices), (void*)&parser.vertices, GL_STATIC_DRAW);
			glBufferData(GL_ARRAY_BUFFER, sizeof(H2B::VERTEX) * parser.vertices.size(), parser.vertices.data(), GL_STATIC_DRAW);
			// TODO: Part 1g
			glGenBuffers(1, &indiciesBuffer);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indiciesBuffer);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * parser.indices.size(), parser.indices.data(), GL_STATIC_DRAW);
			//// TODO: Part 2c
			glGenBuffers(1, &uboBuffer);
			glBindBuffer(GL_UNIFORM_BUFFER, uboBuffer);
			glBufferData(GL_UNIFORM_BUFFER, sizeof(UBO), &UBO, GL_DYNAMIC_DRAW);
			glBindBuffer(GL_UNIFORM_BUFFER, 0);

			//glBindBuffer(GL_UNIFORM_BUFFER, uboBuffer);
			GLuint binding_point_index = 0;
			glBindBufferBase(GL_UNIFORM_BUFFER, binding_point_index, uboBuffer);
			GLvoid* p = glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
			memcpy(p, &UBO, sizeof(UBO));
			glUnmapBuffer(GL_UNIFORM_BUFFER);

			
			return 0;
		}
		void DrawModel(GLuint shaderExecutable,GW::MATH::GMATRIXF viewMat, GW::MATH::GMATRIXF projMat )
		{
			
			//int cm = 1;
			//Get Delta Time
			std::chrono::steady_clock::time_point currTime = std::chrono::high_resolution_clock::now();
			float deltaTime = std::chrono::duration_cast<std::chrono::microseconds>(currTime - prevTime).count() / 100000.0f;
			ftheta = deltaTime * 5;
			prevTime = currTime;
			// TODO: Part 2a
			

				// setup the pipeline
			glUseProgram(shaderExecutable);
				//World
			UBO.viewMatrix = viewMat;
				UBO.projectionMatrix = projMat;
			GLint locationW = glGetUniformLocation(shaderExecutable, "world");
			
			glUniformMatrix4fv(locationW, 1, GL_FALSE, (GLfloat*)&worldPosition);
			//Camera
			GLint locationV = glGetUniformLocation(shaderExecutable, "viewMatrix");
			glUniformMatrix4fv(locationV, 1, GL_FALSE, (GLfloat*)&viewMat);
			//Projection
			GLint locationP = glGetUniformLocation(shaderExecutable, "projectionMatrix");
			glUniformMatrix4fv(locationP, 1, GL_FALSE, (GLfloat*)&projMat);

			// TODO: Part 1e
			glBindVertexArray(vertexArray);
			glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
			
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(H2B::VERTEX), (void*)0);
			glEnableVertexAttribArray(0);
			
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(H2B::VERTEX), (void*)12);
			glEnableVertexAttribArray(1);
			
			glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(H2B::VERTEX), (void*)24);
			glEnableVertexAttribArray(2);
			
			// now we can draw
			// TODO: Part 1d
			// TODO: Part 1h

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indiciesBuffer);
			//glUseProgram(shaderExecutable);
			// TODO: Part 2e
			unsigned int block_index = glGetUniformBlockIndex(shaderExecutable, "UBO");
			// TODO: Part 2f
			GLuint binding_point_index = 0;
			glBindBufferBase(GL_UNIFORM_BUFFER, binding_point_index, uboBuffer);
			// TODO: Part 2g
			glUniformBlockBinding(shaderExecutable, block_index, 0);
			// TODO: Part 3b
		//	for (int i = 0; i < lvlData.parsers[cm].meshCount; i++)
			for (int i = 0; i < parser.meshCount; i++)
			{

				// TODO: Part 4d
				
				UBO.world = worldPosition;
				
				OBJ_ATTRIBUTES* obj = (OBJ_ATTRIBUTES*)&parser.materials[i].attrib;
				UBO.material = *obj;

				glBindBuffer(GL_ARRAY_BUFFER, uboBuffer);
				// get pointer
				void* ptr = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
				// now copy data into memory
				memcpy(ptr, &UBO, sizeof(UBO));
				// make sure to tell OpenGL we're done with the pointer
				glUnmapBuffer(GL_ARRAY_BUFFER);
				glUseProgram(shaderExecutable);
				// TODO: Part 4e
			
				glDrawElements(GL_TRIANGLES, parser.batches[i].indexCount, GL_UNSIGNED_INT, (GLvoid*)(sizeof(unsigned int) * parser.batches[i].indexOffset));
				
				/*}
					}*/
					// some video cards(cough Intel) need this set back to zero or they won't display
			}
			//glBindVertexArray(0);
		}
		void drawFur(GLuint shaderExecutableFur, GW::MATH::GMATRIXF viewMat, GW::MATH::GMATRIXF projMat)
		{
			GLint locationVF = glGetUniformLocation(shaderExecutableFur, "view");
			glUniformMatrix4fv(locationVF, 1, GL_FALSE, (GLfloat*)&viewMat);
			//Projection
			GLint locationPF = glGetUniformLocation(shaderExecutableFur, "projection");
			glUniformMatrix4fv(locationPF, 1, GL_FALSE, (GLfloat*)&projMat);
			//Model
			GW::MATH::GMATRIXF mod = { 1 };
			GLint locationMF = glGetUniformLocation(shaderExecutableFur, "model");
			glUniformMatrix4fv(locationMF, 1, GL_FALSE, (GLfloat*)&worldPosition);

			glBindVertexArray(vertexArray);
			glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);

			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(H2B::VERTEX), (void*)0);
			glEnableVertexAttribArray(0);

			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(H2B::VERTEX), (void*)12);
			glEnableVertexAttribArray(1);

			glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(H2B::VERTEX), (void*)24);
			glEnableVertexAttribArray(2);
			
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indiciesBuffer);
			
			glDrawElements(GL_TRIANGLES, parser.batches[1].indexCount, GL_UNSIGNED_INT, (GLvoid*)(sizeof(unsigned int) * parser.batches[1].indexOffset));
			/*for (int i = 0; i < parser.meshes.size(); i++)
			{
				glDrawElements(GL_TRIANGLES, parser.batches[i].indexCount, GL_UNSIGNED_INT, (GLvoid*)(sizeof(unsigned int) * parser.batches[i].indexOffset));
			}*/
		}
		
	~Model()
	{
		// free resources
		if (vertexArray != NULL)
		{

		glDeleteVertexArrays(1, &vertexArray);
		glDeleteBuffers(1, &vertexBufferObject);
		// TODO: Part 1g
		glDeleteBuffers(1, &indiciesBuffer);
		/*glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
		glDeleteProgram(shaderExecutable);*/
		// TODO: Part 2c
		glDeleteBuffers(1, &uboBuffer);
		}
		
	}


private:
	// Modern OpenGL API Functions must be queried before use
	PFNGLCREATESHADERPROC				glCreateShader = nullptr;
	PFNGLSHADERSOURCEPROC				glShaderSource = nullptr;
	PFNGLCOMPILESHADERPROC				glCompileShader = nullptr;
	PFNGLGETSHADERIVPROC				glGetShaderiv = nullptr;
	PFNGLGETSHADERINFOLOGPROC			glGetShaderInfoLog = nullptr;
	PFNGLATTACHSHADERPROC				glAttachShader = nullptr;
	PFNGLDETACHSHADERPROC				glDetachShader = nullptr;
	PFNGLDELETESHADERPROC				glDeleteShader = nullptr;
	PFNGLCREATEPROGRAMPROC				glCreateProgram = nullptr;
	PFNGLLINKPROGRAMPROC				glLinkProgram = nullptr;
	PFNGLUSEPROGRAMPROC					glUseProgram = nullptr;
	PFNGLGETPROGRAMIVPROC				glGetProgramiv = nullptr;
	PFNGLGETPROGRAMINFOLOGPROC			glGetProgramInfoLog = nullptr;
	PFNGLGENVERTEXARRAYSPROC			glGenVertexArrays = nullptr;
	PFNGLBINDVERTEXARRAYPROC			glBindVertexArray = nullptr;
	PFNGLGENBUFFERSPROC					glGenBuffers = nullptr;
	PFNGLBINDBUFFERPROC					glBindBuffer = nullptr;
	PFNGLBUFFERDATAPROC					glBufferData = nullptr;
	PFNGLENABLEVERTEXATTRIBARRAYPROC	glEnableVertexAttribArray = nullptr;
	PFNGLDISABLEVERTEXATTRIBARRAYPROC	glDisableVertexAttribArray = nullptr;
	PFNGLVERTEXATTRIBPOINTERPROC		glVertexAttribPointer = nullptr;
	PFNGLGETUNIFORMLOCATIONPROC			glGetUniformLocation = nullptr;
	PFNGLUNIFORMMATRIX4FVPROC			glUniformMatrix4fv = nullptr;
	PFNGLDELETEBUFFERSPROC				glDeleteBuffers = nullptr;
	PFNGLDELETEPROGRAMPROC				glDeleteProgram = nullptr;
	PFNGLDELETEVERTEXARRAYSPROC			glDeleteVertexArrays = nullptr;
	PFNGLDEBUGMESSAGECALLBACKPROC		glDebugMessageCallback = nullptr;
	// TODO: Part 2d
	PFNGLGETUNIFORMBLOCKINDEXPROC       glGetUniformBlockIndex = nullptr;
	PFNGLBINDBUFFERBASEPROC	            glBindBufferBase = nullptr;
	PFNGLUNIFORMBLOCKBINDINGPROC        glUniformBlockBinding = nullptr;
	PFNGLMAPBUFFERPROC                  glMapBuffer = nullptr;
	PFNGLUNMAPBUFFERPROC                glUnmapBuffer = nullptr;

	// Modern OpenGL API functions need to be queried
	void LoadExtensions()
	{
		ogl.QueryExtensionFunction(nullptr, "glCreateShader", (void**)&glCreateShader);
		ogl.QueryExtensionFunction(nullptr, "glShaderSource", (void**)&glShaderSource);
		ogl.QueryExtensionFunction(nullptr, "glCompileShader", (void**)&glCompileShader);
		ogl.QueryExtensionFunction(nullptr, "glGetShaderiv", (void**)&glGetShaderiv);
		ogl.QueryExtensionFunction(nullptr, "glGetShaderInfoLog", (void**)&glGetShaderInfoLog);
		ogl.QueryExtensionFunction(nullptr, "glAttachShader", (void**)&glAttachShader);
		ogl.QueryExtensionFunction(nullptr, "glDetachShader", (void**)&glDetachShader);
		ogl.QueryExtensionFunction(nullptr, "glDeleteShader", (void**)&glDeleteShader);
		ogl.QueryExtensionFunction(nullptr, "glCreateProgram", (void**)&glCreateProgram);
		ogl.QueryExtensionFunction(nullptr, "glLinkProgram", (void**)&glLinkProgram);
		ogl.QueryExtensionFunction(nullptr, "glUseProgram", (void**)&glUseProgram);
		ogl.QueryExtensionFunction(nullptr, "glGetProgramiv", (void**)&glGetProgramiv);
		ogl.QueryExtensionFunction(nullptr, "glGetProgramInfoLog", (void**)&glGetProgramInfoLog);
		ogl.QueryExtensionFunction(nullptr, "glGenVertexArrays", (void**)&glGenVertexArrays);
		ogl.QueryExtensionFunction(nullptr, "glBindVertexArray", (void**)&glBindVertexArray);
		ogl.QueryExtensionFunction(nullptr, "glGenBuffers", (void**)&glGenBuffers);
		ogl.QueryExtensionFunction(nullptr, "glBindBuffer", (void**)&glBindBuffer);
		ogl.QueryExtensionFunction(nullptr, "glBufferData", (void**)&glBufferData);
		ogl.QueryExtensionFunction(nullptr, "glEnableVertexAttribArray", (void**)&glEnableVertexAttribArray);
		ogl.QueryExtensionFunction(nullptr, "glDisableVertexAttribArray", (void**)&glDisableVertexAttribArray);
		ogl.QueryExtensionFunction(nullptr, "glVertexAttribPointer", (void**)&glVertexAttribPointer);
		ogl.QueryExtensionFunction(nullptr, "glGetUniformLocation", (void**)&glGetUniformLocation);
		ogl.QueryExtensionFunction(nullptr, "glUniformMatrix4fv", (void**)&glUniformMatrix4fv);
		ogl.QueryExtensionFunction(nullptr, "glDeleteBuffers", (void**)&glDeleteBuffers);
		ogl.QueryExtensionFunction(nullptr, "glDeleteProgram", (void**)&glDeleteProgram);
		ogl.QueryExtensionFunction(nullptr, "glDeleteVertexArrays", (void**)&glDeleteVertexArrays);
		ogl.QueryExtensionFunction(nullptr, "glDebugMessageCallback", (void**)&glDebugMessageCallback);
		// TODO: Part 2d
		ogl.QueryExtensionFunction(nullptr, "glGetUniformBlockIndex", (void**)&glGetUniformBlockIndex);
		ogl.QueryExtensionFunction(nullptr, "glBindBufferBase", (void**)&glBindBufferBase);
		ogl.QueryExtensionFunction(nullptr, "glUniformBlockBinding", (void**)&glUniformBlockBinding);
		ogl.QueryExtensionFunction(nullptr, "glMapBuffer", (void**)&glMapBuffer);
		ogl.QueryExtensionFunction(nullptr, "glUnmapBuffer", (void**)&glUnmapBuffer);
	}
};
