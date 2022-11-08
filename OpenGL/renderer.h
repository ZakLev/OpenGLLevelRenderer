// minimalistic code to draw a single triangle, this is not part of the API.
// TODO: Part 1b
 #include "../Assets/FSLogo.h"
#include "LevelData.h"
// Simple Vertex Shader
const char* vertexShaderSource = R"(
#version 330 // GLSL 3.30
// an ultra simple glsl vertex shader
// TODO: Part 2b
struct OBJ_ATTRIBUTES
{
	vec3    Kd; // diffuse reflectivity
	float	    d; // dissolve (transparency) 
	vec3    Ks; // specular reflectivity
	float       Ns; // specular exponent
	vec3    Ka; // ambient reflectivity
	float       sharpness; // local reflection map sharpness
	vec3    Tf; // transmission filter
	float       Ni; // optical density (index of refraction)
	vec3    Ke; // emissive reflectivity
	uint    illum; // illumination model
};

// The following GLSL syntax specefies a uniform buffer
// where the matricies contained within are row major.
layout(row_major) uniform UBO_DATA
{
    vec4 sunDirection, sunColor;
    mat4 viewMatrix, projectionMatrix;
    mat4 world;
    OBJ_ATTRIBUTES material;
    vec4 sunAmbient, camPos;
};
// TODO: Part 4e
// TODO: Part 1f
// TODO: Part 4a
layout(location = 0) in vec3 local_pos;
layout(location = 1) in vec3 local_uvw;
layout(location = 2) in vec3 local_nrm;
out vec3 world_nrm;
out vec3 worldPos;
void main()
{
	// TODO: Part 1f
	//gl_Position = vec4(local_pos, 1);
//gl_UVs = vec3(UV);
//gl_Norm = vec3(Norm);
	// TODO: Part 1h
//gl_Position.y -= 0.75;
	// TODO: Part 2h
vec4 pos4 = vec4(local_pos, 1);
 vec4 positionW = pos4 * world;
 vec4 positionV = positionW* viewMatrix;
 vec4 position = positionV* projectionMatrix;
gl_Position = vec4(position);
worldPos = vec3(positionW);
	// TODO: Part 4b
	vec3 nrm = vec3(vec4(local_nrm, 0.0) * world);
    world_nrm = nrm;
}
)";
// Simple Fragment Shader
const char* fragmentShaderSource = R"(
#version 330 // GLSL 3.30
out vec4 Pixel;
// an ultra simple glsl fragment shader
// TODO: Part 3a
struct OBJ_ATTRIBUTES
{
	vec3    Kd; // diffuse reflectivity
	float	    d; // dissolve (transparency) 
	vec3    Ks; // specular reflectivity
	float       Ns; // specular exponent
	vec3    Ka; // ambient reflectivity
	float       sharpness; // local reflection map sharpness
	vec3    Tf; // transmission filter
	float       Ni; // optical density (index of refraction)
	vec3    Ke; // emissive reflectivity
	uint    illum; // illumination model
};

// The following GLSL syntax specefies a uniform buffer
// where the matricies contained within are row major.
layout(row_major) uniform UBO_DATA
{
    vec4 sunDirection, sunColor;
    mat4 viewMatrix, projectionMatrix;
    mat4 world;
    OBJ_ATTRIBUTES material;
    vec4 sunAmbient, camPos;
};
// TODO: Part 4e
// TODO: Part 4b
in vec3 world_nrm;
in vec3 worldPos;
void main() 
{	
	// TODO: Part 3a
	//Pixel = vec4(170/255.0f, 170/255.0f, 44/255.0f, 1); // TODO: Part 1a
    vec4 color = vec4(material.Kd,1);
vec3 nrm = normalize(world_nrm);
float diff = max(dot(nrm, normalize(-sunDirection.xyz)), 0);
	// TODO: Part 4e
	// TODO: Part 4f (half-vector or reflect method)
     vec3 reflectDir = reflect(normalize(sunDirection.xyz), nrm);
vec3 posW = vec3(worldPos.x,worldPos.y,worldPos.z);
vec3 camDir = normalize(camPos.xyz - posW.xyz );
      float spec = pow(max(dot(camDir, reflectDir), 0.0), material.Ns);
vec3 specular = spec * material.Ks;
vec3 diffuse = (sunAmbient.xyz  + (diff* sunColor.xyz) ) * color.xyz + specular;
Pixel = vec4(diffuse,1);
//Pixel = vec4(diffuse,1) * color;
}
)";
// Used to print debug infomation from OpenGL, pulled straight from the official OpenGL wiki.
#ifndef NDEBUG
	void MessageCallback(	GLenum source, GLenum type, GLuint id,
							GLenum severity, GLsizei length,
							const GLchar* message, const void* userParam) {
		fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
		(type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""), type, severity, message);
	}
#endif
// Creation, Rendering & Cleanup
class Renderer
{
	// proxy handles
	GW::SYSTEM::GWindow win;
	GW::GRAPHICS::GOpenGLSurface ogl;
	// what we need at a minimum to draw a triangle
	GLuint vertexArray = 0;
	GLuint vertexBufferObject = 0;
	// TODO: Part 1g
	GLuint indiciesBuffer = 0;
	GLuint vertexShader = 0;
	GLuint fragmentShader = 0;
	GLuint shaderExecutable = 0;
	// TODO: Part 2c
	GLuint uboBuffer = 0;
	// TODO: Part 2a
	GW::MATH::GMatrix matMath;
	GW::MATH::GMATRIXF worldMat = GW::MATH::GIdentityMatrixF;
	GW::MATH::GMATRIXF viewMat = GW::MATH::GIdentityMatrixF;
	GW::MATH::GMATRIXF projMat = GW::MATH::GIdentityMatrixF;
	GW::MATH::GVECTORF lightDir = {-1,-1,-2,0};
	GW::MATH::GVECTORF lightColor = {0.9f,0.9f,1,1};
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
	// TODO: Part 4e
public:
	//Level Data
	LevelData::LevelData lvlData;
	Renderer(GW::SYSTEM::GWindow _win, GW::GRAPHICS::GOpenGLSurface _ogl)
	{
		win = _win;
		ogl = _ogl;
		// TODO: part 2a
		
		 matMath.Create();
		//World
	//	matMath.RotateYLocalF(worldMat, G_DEGREE_TO_RADIAN_F(ftheta), worldMat);
		//Camera
		GW::MATH::GVECTORF cameraPos = { 0.75f,0.25f,1.5f };
		GW::MATH::GVECTORF cameraRot = { 0.15f,0.75f,0.0f};
		GW::MATH::GVECTORF cameraUp = { 0.0f,1.0f,0.0f }; 
	//	matMath.TranslateLocalF(viewMat, cameraPos, viewMat);
		/*matMath.RotateXGlobalF(viewMat,cameraRot.x,viewMat);
		matMath.RotateYLocalF(viewMat, cameraRot.y, viewMat);*/
		
		matMath.LookAtRHF(cameraPos, cameraRot, cameraUp, viewMat);
	    //matMath.InverseF(viewMat, viewMat);
		//Projection
		_ogl.GOpenGLSurface::GetAspectRatio(AR);
		matMath.ProjectionOpenGLRHF(fov, AR, fNear, fFar, projMat);
		
		// TODO: Part 2b
		
		UBO.sunDirection = lightDir;
		UBO.sunColor = lightColor;
		UBO.viewMatrix = viewMat;
		UBO.projectionMatrix = projMat;
		UBO.world = worldMat;
		UBO.material = FSLogo_materials[0].attrib;


		
		// TODO: Part 4e
		UBO.sunAmbient = lightAmbient;
		UBO.camPos = cameraPos;
		// Link Needed OpenGL API functions
		LoadExtensions();
		// In debug mode we link openGL errors to the console
#ifndef NDEBUG
		glEnable(GL_DEBUG_OUTPUT);
		glDebugMessageCallback(MessageCallback, 0);
#endif
		// TODO: Part 1c
		glGenVertexArrays(1, &vertexArray);
		glGenBuffers(1, &vertexBufferObject);
		glBindVertexArray(vertexArray);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
		//glBufferData(GL_ARRAY_BUFFER, sizeof(FSLogo_vertices), FSLogo_vertices, GL_STATIC_DRAW);
		glBufferData(GL_ARRAY_BUFFER, sizeof(lvlData.parsers[0].vertices[0]), (void*)&lvlData.parsers[0].vertices[0], GL_STATIC_DRAW);
		// TODO: Part 1g
		glGenBuffers(1, &indiciesBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indiciesBuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(lvlData.parsers[0].indices[0]), (void*)&lvlData.parsers[0].indices[0], GL_STATIC_DRAW);
		// TODO: Part 2c
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

		// Create Vertex Shader
		vertexShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
		glCompileShader(vertexShader);
		char errors[1024]; GLint result;
		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);
		if (result == false)
		{
			glGetShaderInfoLog(vertexShader, 1024, NULL, errors);
			std::cout << errors << std::endl;
		}
		// Create Fragment Shader
		fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
		glCompileShader(fragmentShader);
		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
		if (result == false)
		{
			glGetShaderInfoLog(fragmentShader, 1024, NULL, errors);
			std::cout << errors << std::endl;
		}
		// Create Executable Shader Program
		shaderExecutable = glCreateProgram();
		glAttachShader(shaderExecutable, vertexShader);
		glAttachShader(shaderExecutable, fragmentShader);
		glLinkProgram(shaderExecutable);
		glGetProgramiv(shaderExecutable, GL_LINK_STATUS, &result);
		if (result == false) 
		{
			glGetProgramInfoLog(shaderExecutable, 1024, NULL, errors);
			std::cout << errors << std::endl;
		}
		
	}
	void Render()
	{
		//Get Delta Time
		std::chrono::steady_clock::time_point currTime = std::chrono::high_resolution_clock::now();
		float deltaTime = std::chrono::duration_cast<std::chrono::microseconds>(currTime - prevTime).count() / 100000.0f;
		ftheta =  deltaTime * 5;
		prevTime = currTime;
		// TODO: Part 2a
		// setup the pipeline
		//World
		GLint locationW = glGetUniformLocation(shaderExecutable, "world");
		glUniformMatrix4fv(locationW, 1, GL_FALSE, (GLfloat*)&worldMat);
		//Camera
		GLint locationV = glGetUniformLocation(shaderExecutable, "viewMatrix");
		glUniformMatrix4fv(locationV, 1, GL_FALSE, (GLfloat*)&viewMat);
		//Projection
		GLint locationP = glGetUniformLocation(shaderExecutable, "projectionMatrix");
		glUniformMatrix4fv(locationP, 1, GL_FALSE, (GLfloat*)&projMat);
	
		// TODO: Part 1e
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
		glUseProgram(shaderExecutable);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)12);
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
		glUseProgram(shaderExecutable);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)24);
		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
		glUseProgram(shaderExecutable);
		// now we can draw
		glBindVertexArray(vertexArray);
		glUseProgram(shaderExecutable);
		// TODO: Part 1d
		// TODO: Part 1h
		
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indiciesBuffer);
		glUseProgram(shaderExecutable);
		// TODO: Part 2e
		unsigned int block_index = glGetUniformBlockIndex(shaderExecutable, "UBO");
		// TODO: Part 2f
		GLuint binding_point_index = 0;
		glBindBufferBase(GL_UNIFORM_BUFFER, binding_point_index, uboBuffer);
		// TODO: Part 2g
		glUniformBlockBinding(shaderExecutable, block_index, 0);
		// TODO: Part 3b
		for (int i = 0; i < lvlData.parsers[0].meshCount; i++)
		{

			// TODO: Part 4d
			if (i == 0)
			{
				UBO.world = GW::MATH::GIdentityMatrixF;
			}
			/*else
			{
				matMath.RotateYLocalF(worldMat, G_DEGREE_TO_RADIAN_F(-ftheta), worldMat);
				UBO.world = worldMat;
			}*/
			// TODO: Part 3c
			
				//UBO.material = FSLogo_materials[i].attrib;
			OBJ_ATTRIBUTES* obj = (OBJ_ATTRIBUTES*)&lvlData.parsers[0].materials[i].attrib;
			UBO.material = *obj;

				glBindBuffer(GL_ARRAY_BUFFER, uboBuffer);
				// get pointer
				void* ptr = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
				// now copy data into memory
				memcpy(ptr, &UBO, sizeof(UBO));
				// make sure to tell OpenGL we're done with the pointer
				glUnmapBuffer(GL_ARRAY_BUFFER);
			
			// TODO: Part 4e
		//glDrawArrays(GL_TRIANGLES, 0, FSLogo_vertexcount);
		//glDrawArrays(GL_INDEX_ARRAY, 0, FSLogo_indexcount);
			
		//glDrawElements(GL_TRIANGLES, FSLogo_batches[i][0], GL_UNSIGNED_INT, (GLvoid*)(sizeof(unsigned int)*FSLogo_batches[i][1]));
				glDrawElements(GL_TRIANGLES, lvlData.parsers[0].batches[i].indexCount, GL_UNSIGNED_INT, (GLvoid*)(sizeof(unsigned int) * lvlData.parsers[0].batches[i].indexOffset));
		}
		// some video cards(cough Intel) need this set back to zero or they won't display
		glBindVertexArray(0);
	}
	~Renderer()
	{
		// free resources
		glDeleteVertexArrays(1, &vertexArray);
		glDeleteBuffers(1, &vertexBufferObject);
		// TODO: Part 1g
		glDeleteBuffers(1, &indiciesBuffer);
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
		glDeleteProgram(shaderExecutable);
		// TODO: Part 2c
		glDeleteBuffers(1, &uboBuffer);
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
