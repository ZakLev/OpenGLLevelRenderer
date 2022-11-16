// minimalistic code to draw a single triangle, this is not part of the API.
// TODO: Part 1b
 #include "../Assets/FSLogo.h"
#include "LevelData.h"
#include "h2bParser.h"
#include <vector>
#include "Model.h"
#include "FileIO.h"

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
//out vec3 TexCoords;

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
//TexCoords = local_pos;
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
//out vec4 FragColor;

//in vec3 TexCoords;
//in vec3 textureDir; // direction vector representing a 3D texture coordinate
//uniform samplerCube skybox; // cubemap texture sampler

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
 //FragColor = texture(skybox, TexCoords);
}
)";
const char* vertexShaderSkyboxSource = R"(
#version 330 // GLSL 3.30
	layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec3 aNormal;

	out vec3 TexCoords;
    out VS_OUT {
    vec3 normal;
} vs_out;

	uniform mat4 projection;
	uniform mat4 view;

void main()
{
    
	TexCoords = vec3(aPos.xy,-aPos.z);
mat4 unTransView = mat4(mat3(view));

  vec4 pos = projection * unTransView * vec4(aPos, 1.0);
	gl_Position = pos.xyww;
//gl_Position = projection * view * vec4(aPos, 1.0);
}
)";
const char* fragmentShaderSkyboxSource = R"(
#version 330 // GLSL 3.30
	out vec4 FragColor;

	in vec3 TexCoords;

	uniform samplerCube skybox;
void main()
{
	FragColor = texture(skybox, TexCoords);
}
)";
const char* vertexShaderFurSource = R"(
#version 330 // GLSL 3.30
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 uvw;
layout (location = 2) in vec3 aNormal;

out VS_OUT {
    vec3 normal;
} vs_out;

uniform mat4 view;
uniform mat4 model;

void main()
{
    mat3 normalMatrix = mat3(transpose(inverse(view * model)));
    vs_out.normal = vec3(vec4(normalMatrix * aNormal, 0.0));
    gl_Position = view * model * vec4(aPos, 1.0); 
}
)";
const char* fragmentShaderFurSource = R"(
#version 330 // GLSL 3.30
	out vec4 FragColor;

void main()
{
    FragColor = vec4(1.0, 1.0, 0.0, 1.0);
}
)";
const char* geometryShaderFurSource = R"(
#version 330 // GLSL 3.30
layout (triangles) in;
layout (line_strip, max_vertices = 6) out;

in VS_OUT {
    vec3 normal;
} gs_in[];

const float MAGNITUDE = 0.2;

uniform mat4 projection;

void GenerateLine(int index)
{
    gl_Position = projection * gl_in[index].gl_Position;
    EmitVertex();
    gl_Position = projection * (gl_in[index].gl_Position + vec4(gs_in[index].normal, 0.0) * MAGNITUDE);
    EmitVertex();
    EndPrimitive();
}

void main()
{
    GenerateLine(0); // first vertex normal
    GenerateLine(1); // second vertex normal
    GenerateLine(2); // third vertex normal
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
	//LevelData lvlData;

//std::vector<Model*> models = {};
	
	//void getModels(std::vector<Model*>& modelsData)
	//{
	////models.resize(modelsData.size());
	//	/*models.reserve(modelsData.size());
	//	for each (Model* m in modelsData)
	//	{
	//		
	//		models.push_back(m);
	//	}
	//	models.shrink_to_fit();*/
	//	/*for (int i = 0; i < modelsData.size(); i++)
	//	{
	//		models.resize(i + 1);
	//		models[i] = modelsData[i];
	//}*/
	//	//models = modelsData;
	//}
// Creation, Rendering & Cleanup
	class Renderer
	{

		//FileIO readFile;
		//static LevelData lvlData;
		// proxy handles
		GW::SYSTEM::GWindow win;
		GW::GRAPHICS::GOpenGLSurface ogl;
		GW::INPUT::GInput GIn;
		GW::INPUT::GController GCon;
		FileIO readFile;
		std::string filePath = "";
		//std::vector<Model*> models = {};
		std::vector<Model> models = {};
		//// what we need at a minimum to draw a triangle
		//GLuint vertexArray = 0;
		//GLuint vertexBufferObject = 0;
		//// TODO: Part 1g
		//GLuint indiciesBuffer = 0;
		GLuint vertexShader = 0;
		GLuint fragmentShader = 0;
		GLuint shaderExecutable = 0;
		//// TODO: Part 2c
		//GLuint uboBuffer = 0;
		//skybox
		GLuint skyboxVAO = 0;
		GLuint skyboxVBO = 0;
		GLuint cubemapTexture;

		GLuint vertexShaderSkybox = 0;
		GLuint fragmentShaderSkybox = 0;
		GLuint shaderExecutableSkybox = 0;

		GLuint vertexShaderFur = 0;
		GLuint fragmentShaderFur = 0;
		GLuint geometryShaderFur = 0;
		GLuint shaderExecutableFur = 0;
		//// TODO: Part 2a
		GW::MATH::GMatrix matMath;
		//GW::I::GCollisionImplementation colider;
		//GW::MATH::GMATRIXF worldMat = GW::MATH::GIdentityMatrixF;
		GW::MATH::GMATRIXF viewMat = GW::MATH::GIdentityMatrixF;
		GW::MATH::GMATRIXF viewMat2 = GW::MATH::GIdentityMatrixF;
		GW::MATH::GMATRIXF viewMat3 = GW::MATH::GIdentityMatrixF;
		GW::MATH::GMATRIXF projMat = GW::MATH::GIdentityMatrixF;
		//GW::MATH::GVECTORF lightDir = { -1,-1,-2,0 };
		//GW::MATH::GVECTORF lightColor = { 0.9f,0.9f,1,1 };
		//GW::MATH::GVECTORF lightAmbient = { 0.25f,0.25f,0.35f,1 };
		float fov = G_DEGREE_TO_RADIAN_F(65.0f); float fNear = .1f; float fFar = 100.0f; float AR;
		std::chrono::steady_clock::time_point prevTime;
		std::chrono::microseconds FPS;
		bool screenSplit = false;
		bool viewLock = false;
		unsigned int screenHeight;
		unsigned int screenWidth;
		std::vector<int> horseModels;
		//float ftheta = 0;
		//// TODO: Part 2b
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
		float skyboxVertices[108] = {
			// positions          
			-1.0f,  1.0f, -1.0f,
			-1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,

			-1.0f, -1.0f,  1.0f,
			-1.0f, -1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f,  1.0f,
			-1.0f, -1.0f,  1.0f,

			 1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,

			-1.0f, -1.0f,  1.0f,
			-1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f, -1.0f,  1.0f,
			-1.0f, -1.0f,  1.0f,

			-1.0f,  1.0f, -1.0f,
			 1.0f,  1.0f, -1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			-1.0f,  1.0f,  1.0f,
			-1.0f,  1.0f, -1.0f,

			-1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f,  1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f,  1.0f,
			 1.0f, -1.0f,  1.0f

		};
	public:
		//Level Data
		 //LevelData lvlData;


		Renderer(GW::SYSTEM::GWindow _win, GW::GRAPHICS::GOpenGLSurface _ogl, char* fileName = "")
		{
			//int cm = 1;
			///*for (int cm = 0; cm < models.size(); cm++)
			//{*/

				win = _win;
				ogl = _ogl;
				filePath = fileName;
				/*if (filePath.compare(""))
				{
				readFile.ReadGameLevel(models);
				}
				else
				{*/
				readFile.ReadGameLevel(models, filePath.c_str());
				//}

				
			//	// TODO: part 2a

			matMath.Create();
			GIn.Create(win);
			GCon.Create();
		//	colider.Create();
			//	//World
			////	matMath.RotateYLocalF(worldMat, G_DEGREE_TO_RADIAN_F(ftheta), worldMat);
				//Camera
				GW::MATH::GVECTORF cameraPos = { 0.75f,5.25f,1.5f };
				GW::MATH::GVECTORF cameraRot = { 0.15f,0.75f,0.0f };
				GW::MATH::GVECTORF cameraUp = { 0.0f,1.0f,0.0f };
				

				matMath.LookAtRHF(cameraPos, cameraRot, cameraUp, viewMat);

				GW::MATH::GVECTORF cameraPos2 = { 0.75f,25.0f,1.5f };
				GW::MATH::GVECTORF cameraRot2 = { -0.15f,-.75f,0.0f };
				GW::MATH::GVECTORF cameraUp2 = { 0.0f,1.0f,0.0f };
				

				matMath.LookAtRHF(cameraPos2, cameraRot, cameraUp2, viewMat2);
				viewMat3 = viewMat2;
				//Projection
				_ogl.GOpenGLSurface::GetAspectRatio(AR);
				
				matMath.ProjectionOpenGLRHF(fov, AR, fNear, fFar, projMat);

			//	// TODO: Part 2b

			//	UBO.sunDirection = lightDir;
			//	UBO.sunColor = lightColor;
				UBO.viewMatrix = viewMat;
				UBO.projectionMatrix = projMat;
			//	UBO.world = worldMat;
			//	UBO.material = FSLogo_materials[0].attrib;
			//	//UBO.material = (OB)lvlData.parsers[0].materials[0].attrib;


			//	// TODO: Part 4e
			//	UBO.sunAmbient = lightAmbient;
			//	UBO.camPos = cameraPos;
				// Link Needed OpenGL API functions
				LoadExtensions();
				// In debug mode we link openGL errors to the console
#ifndef NDEBUG
				glEnable(GL_DEBUG_OUTPUT);
				glDebugMessageCallback(MessageCallback, 0);
#endif
				// TODO: Part 1c
				//glGenVertexArrays(1, &vertexArray);
				//glGenBuffers(1, &vertexBufferObject);
				//glBindVertexArray(vertexArray);
				//glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
				//glBufferData(GL_ARRAY_BUFFER, sizeof(models[cm].parser.vertices), (void*)&models[cm].parser.vertices, GL_STATIC_DRAW);
				//glBufferData(GL_ARRAY_BUFFER, sizeof(H2B::VERTEX) * models[cm].parser.vertices.size(), models[cm].parser.vertices.data(), GL_STATIC_DRAW);
				//// TODO: Part 1g
				//glGenBuffers(1, &indiciesBuffer);
				//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indiciesBuffer);
				//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(H2B::VERTEX) * (models[cm].parser.indices.size()), models[cm].parser.indices.data(), GL_STATIC_DRAW);
				// TODO: Part 2c
				//models[1].UploadModelDataToGPU(win, ogl);
			  // models[29].UploadModelDataToGPU(win, ogl);
			    for (int i = 0; i < models.size(); i++)
			   {
				   models[i].UploadModelDataToGPU(win,ogl);
				   if (models[i].Name.compare("Statue_Horse") == 0)
					   horseModels.push_back(i);
			   }

				//glGenBuffers(1, &uboBuffer);
				//glBindBuffer(GL_UNIFORM_BUFFER, uboBuffer);
				//glBufferData(GL_UNIFORM_BUFFER, sizeof(UBO), &UBO, GL_DYNAMIC_DRAW);
				//glBindBuffer(GL_UNIFORM_BUFFER, 0);

				////glBindBuffer(GL_UNIFORM_BUFFER, uboBuffer);
				//GLuint binding_point_index = 0;
				//glBindBufferBase(GL_UNIFORM_BUFFER, binding_point_index, uboBuffer);
				//GLvoid* p = glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
				//memcpy(p, &UBO, sizeof(UBO));
				//glUnmapBuffer(GL_UNIFORM_BUFFER);

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

				// Create Vertex Shader Skybox
				vertexShaderSkybox = glCreateShader(GL_VERTEX_SHADER);
				glShaderSource(vertexShaderSkybox, 1, &vertexShaderSkyboxSource, NULL);
				glCompileShader(vertexShaderSkybox);
				char errorsSkybox[1024]; GLint resultSkybox;
				glGetShaderiv(vertexShaderSkybox, GL_COMPILE_STATUS, &resultSkybox);
				if (resultSkybox == false)
				{
					glGetShaderInfoLog(vertexShaderSkybox, 1024, NULL, errors);
					std::cout << errorsSkybox << std::endl;
				}
				// Create Fragment Shader Skybox
				fragmentShaderSkybox = glCreateShader(GL_FRAGMENT_SHADER);
				glShaderSource(fragmentShaderSkybox, 1, &fragmentShaderSkyboxSource, NULL);
				glCompileShader(fragmentShaderSkybox);
				glGetShaderiv(fragmentShaderSkybox, GL_COMPILE_STATUS, &resultSkybox);
				if (resultSkybox == false)
				{
					glGetShaderInfoLog(fragmentShaderSkybox, 1024, NULL, errorsSkybox);
					std::cout << errorsSkybox << std::endl;
				}
				// Create Executable Shader Skybox Program
				shaderExecutableSkybox = glCreateProgram();
				glAttachShader(shaderExecutableSkybox, vertexShaderSkybox);
				glAttachShader(shaderExecutableSkybox, fragmentShaderSkybox);
				glLinkProgram(shaderExecutableSkybox);
				glGetProgramiv(shaderExecutableSkybox, GL_LINK_STATUS, &resultSkybox);
				if (resultSkybox == false)
				{
					glGetProgramInfoLog(shaderExecutableSkybox, 1024, NULL, errorsSkybox);
					std::cout << errorsSkybox << std::endl;
				}
				std::vector<std::string> faces;
				if (filePath.compare("../../Assets/GameLevel.txt") == 0)
				{

				
						std::string right = "../../Assets/skybox/right.jpg";
						std::string left = "../../Assets/skybox/left.jpg";
						std::string top = "../../Assets/skybox/top.jpg";
						std::string bottom = "../../Assets/skybox/bottom.jpg";
						std::string front = "../../Assets/skybox/front.jpg";
						std::string back ="../../Assets/skybox/back.jpg";
						faces.push_back(right);
						faces.push_back(left);
						faces.push_back(top);
						faces.push_back(bottom);
						faces.push_back(front);
						faces.push_back(back);
				
				}
				else if (filePath.compare("../../Assets/GameLevel2.txt") == 0)
				{
					std::string right = "../../Assets/skybox2/right.jpg";
					std::string left = "../../Assets/skybox2/left.jpg";
					std::string top = "../../Assets/skybox2/top.jpg";
					std::string bottom = "../../Assets/skybox2/bottom.jpg";
					std::string front = "../../Assets/skybox2/front.jpg";
					std::string back = "../../Assets/skybox2/back.jpg";
					faces.push_back(right);
					faces.push_back(left);
					faces.push_back(top);
					faces.push_back(bottom);
					faces.push_back(front);
					faces.push_back(back);
				}
				 cubemapTexture = loadCubemap(faces);

				// skybox VAO
				glGenVertexArrays(1, &skyboxVAO);
				glGenBuffers(1, &skyboxVBO);
				glBindVertexArray(skyboxVAO);
				glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
				glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
				glEnableVertexAttribArray(0);
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
				
				glUseProgram(shaderExecutableSkybox);
				glUniform1i(glGetUniformLocation(shaderExecutableSkybox, "shader"), 0);
				

				//Geomtry Fur Shader
				vertexShaderFur = glCreateShader(GL_VERTEX_SHADER);
				glShaderSource(vertexShaderFur, 1, &vertexShaderFurSource, NULL);
				glCompileShader(vertexShaderFur);
				char errorsFur[1024]; GLint resultFur;
				glGetShaderiv(vertexShaderFur, GL_COMPILE_STATUS, &resultFur);
				if (resultFur == false)
				{
					glGetShaderInfoLog(vertexShaderFur, 1024, NULL, errorsFur);
					std::cout << errorsFur << std::endl;
				}
				
				// Create Fragment Shader Fur
				fragmentShaderFur = glCreateShader(GL_FRAGMENT_SHADER);
				glShaderSource(fragmentShaderFur, 1, &fragmentShaderFurSource, NULL);
				glCompileShader(fragmentShaderFur);
				glGetShaderiv(fragmentShaderFur, GL_COMPILE_STATUS, &resultFur);
				if (resultFur == false)
				{
					glGetShaderInfoLog(fragmentShaderFur, 1024, NULL, errorsFur);
					std::cout << errorsFur << std::endl;
				}
				// Create Geomtry Shader Fur
				geometryShaderFur = glCreateShader(GL_GEOMETRY_SHADER);
				glShaderSource(geometryShaderFur, 1, &geometryShaderFurSource, NULL);
				glCompileShader(geometryShaderFur);
				if (resultFur == false)
				{
					glGetShaderInfoLog(geometryShaderFur, 1024, NULL, errorsFur);
					std::cout << errorsFur << std::endl;
				}
				// Create Executable Shader Fur Program
				shaderExecutableFur = glCreateProgram();
				glAttachShader(shaderExecutableFur, vertexShaderFur);
				glAttachShader(shaderExecutableFur, fragmentShaderFur);
				glAttachShader(shaderExecutableFur, geometryShaderFur);
				glLinkProgram(shaderExecutableFur);
				glGetProgramiv(shaderExecutableFur, GL_LINK_STATUS, &resultFur);
				if (resultFur == false)
				{
					glGetProgramInfoLog(shaderExecutableFur, 1024, NULL, errorsFur);
					std::cout << errorsFur << std::endl;
				}
			
				
			//}
			/*	for (int i = 0; i < models.size(); i++)
				{
					if (i != 6 && i != 24 && i != 25 && i != 54 && i != 56 && i != 60 && i != 62 && i != 63 && i != 79)
						models[i]->UploadModelDataToGPU(win,ogl);
				}*/
				/*for each (Model m in models)
				{
					m.UploadModelDataToGPU(win, ogl);
				}*/

//models[0]->UploadModelDataToGPU(win, ogl);

            //  models[1]->UploadModelDataToGPU(win, ogl);
                /* for (int i = 0; i < models.size(); i++)
                 {
					 models[i].UploadModelDataToGPU(win,ogl);
				 }*/

		}
		void Render()
		{

			//models[0]->DrawModel();
			//	//Get Delta Time
			std::chrono::steady_clock::time_point currTime = std::chrono::high_resolution_clock::now();
			float deltaTime = std::chrono::duration_cast<std::chrono::microseconds>(currTime - prevTime).count() / 100000.0f;
			prevTime = currTime;
			glUseProgram(shaderExecutable);
			if (screenSplit)
			{
				glViewport(0, 0, screenWidth / 2, screenHeight);
			}
			//models[1].DrawModel(shaderExecutable, viewMat, projMat, uboBuffer);
			//models[29].DrawModel(shaderExecutable,viewMat,projMat,uboBuffer);
			for (int i = 0; i < models.size(); i++)
			{
				models[i].DrawModel(shaderExecutable, viewMat, projMat);
				
			}

			//models[0].DrawModel(shaderExecutable, viewMat, projMat);

			//Draw Horse with Fur
			for (int j = 0; j < horseModels.size(); j++)
			{

			glUseProgram(shaderExecutableFur);
			models[horseModels[j]].drawFur(shaderExecutableFur, viewMat, projMat);
			//GLint locationVF = glGetUniformLocation(shaderExecutableFur, "view");
			//glUniformMatrix4fv(locationVF, 1, GL_FALSE, (GLfloat*)&viewMat);
			////Projection
			//GLint locationPF = glGetUniformLocation(shaderExecutableFur, "projection");
			//glUniformMatrix4fv(locationPF, 1, GL_FALSE, (GLfloat*)&projMat);
			////Model
			//
			//GLint locationMF = glGetUniformLocation(shaderExecutableFur, "model");
			//glUniformMatrix4fv(locationMF, 1, GL_FALSE, (GLfloat*)&models[horseModels[j]].worldPosition);

			//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(H2B::VERTEX), (void*)0);
			//glEnableVertexAttribArray(0);
			//
			//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(H2B::VERTEX), (void*)12);
			//glEnableVertexAttribArray(1);
			//for (int i = 0; i < models[horseModels[j]].parser.meshes.size(); i++)
			//{
			//glDrawElements(GL_TRIANGLES, models[horseModels[j]].parser.batches[i].indexCount, GL_UNSIGNED_INT, (GLvoid*)(sizeof(unsigned int) * models[horseModels[j]].parser.batches[i].indexOffset));
			//}
			}
			glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content

			glUseProgram(shaderExecutableSkybox);
			
			GLint locationVS = glGetUniformLocation(shaderExecutableSkybox, "view");
			glUniformMatrix4fv(locationVS, 1, GL_FALSE, (GLfloat*)&viewMat);
			//Projection
			GLint locationPS = glGetUniformLocation(shaderExecutableSkybox, "projection");
			glUniformMatrix4fv(locationPS, 1, GL_FALSE, (GLfloat*)&projMat);	
			
			// skybox cube
			glBindVertexArray(skyboxVAO);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
			glUseProgram(shaderExecutableSkybox);
			glDrawArrays(GL_TRIANGLES, 0, 36);
			glBindVertexArray(0);
			glDepthFunc(GL_LESS); // set depth function back to default
			if (screenSplit)
			{
				glViewport(screenWidth / 2, 0, screenWidth / 2, screenHeight);
				glUseProgram(shaderExecutable);
				//models[1].DrawModel(shaderExecutable, viewMat, projMat, uboBuffer);
				//models[29].DrawModel(shaderExecutable,viewMat,projMat,uboBuffer);
				for (int i = 0; i < models.size(); i++)
				{
					models[i].DrawModel(shaderExecutable, viewMat2, projMat);
				}

				//models[0].DrawModel(shaderExecutable, viewMat, projMat);

				glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
				glUseProgram(shaderExecutableSkybox);
				GLint locationVS = glGetUniformLocation(shaderExecutableSkybox, "view");
				glUniformMatrix4fv(locationVS, 1, GL_FALSE, (GLfloat*)&viewMat2);
				//Projection
				GLint locationPS = glGetUniformLocation(shaderExecutableSkybox, "projection");
				glUniformMatrix4fv(locationPS, 1, GL_FALSE, (GLfloat*)&projMat);
				// skybox cube
				glBindVertexArray(skyboxVAO);
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
				glUseProgram(shaderExecutableSkybox);
				glDrawArrays(GL_TRIANGLES, 0, 36);
				glBindVertexArray(0);
				glDepthFunc(GL_LESS); // set depth function back to default
			}
			
		}
		
		void UpdateCamera(bool& changeLevel,char*& fileName, bool& playSound)
		{

			std::chrono::steady_clock::time_point currTime = std::chrono::high_resolution_clock::now();
			FPS = std::chrono::duration_cast<std::chrono::microseconds>(currTime - prevTime);
			float SPSLF = FPS.count() / 100000.0f;
			//float SPSLF = _SPLSLF;
			float levelChange2 = 0;
			float levelChange1 = 0;
			float playAug = 0;
			float fileOpen = 0;
			float changeView = 0;
			float splitScreen = 0;
			float unsplitScreen = 0;
			float lockView = 0;
			float unlockView = 0;
			
			
			GIn.GetState(G_KEY_4, levelChange2);
			if (levelChange2 != 0 && filePath != "../../Assets/GameLevel2.txt")
			{
				levelChange2 = 0;
				fileName = "../../Assets/GameLevel2.txt";
				changeLevel = true;
			}
			GIn.GetState(G_KEY_3, levelChange1);
			if (levelChange1 != 0 && filePath != "../../Assets/GameLevel.txt")
			{
				levelChange1 = 0;
				fileName = "../../Assets/GameLevel.txt";
				changeLevel = true;
				
			}
			GIn.GetState(G_KEY_1, playAug);
			if (playAug != 0 && playSound == false)
			{
				playSound = true;
				playAug = 0;

			}
			GIn.GetState(G_KEY_F1, fileOpen);
			if (fileOpen != 0)
			{
				changeLevel = true;
				
				fileOpen = 0;
				//IFileOpenDialog *fileOpen;

			}
			GIn.GetState(G_KEY_F2, changeView);
			if (changeView != 0)
			{
				viewMat = viewMat3;
				changeView = 0;
				//IFileOpenDialog *fileOpen;

			}
			GIn.GetState(G_KEY_ESCAPE, lockView);
			if (lockView != 0 && viewLock!= true)
			{
				viewLock = true;
				lockView = 0;

			}
			GIn.GetState(G_KEY_TILDE, unlockView);
			if (unlockView != 0 && viewLock != false)
			{
				viewLock = false;
				unlockView = 0;

			}
			win.GetHeight(screenHeight);
			win.GetWidth(screenWidth);
			GIn.GetState(G_KEY_F3, splitScreen);
			if (splitScreen != 0 && screenSplit != true)
			{
					screenSplit = true;
					glViewport(screenWidth / 2, 0, screenWidth / 2, screenHeight);
					splitScreen = 0;
			}
			GIn.GetState(G_KEY_F4, unsplitScreen);
			if (unsplitScreen != 0 && screenSplit != false)
			{
					screenSplit = false;
					glViewport(0, 0, screenWidth, screenHeight);
			}
			// TODO Part 4c
			GW::MATH::GVECTORF cameraPositoin = viewMat.row4;
			if (!viewLock)
			{

			matMath.InverseF(viewMat, viewMat);
			// TODO: Part 4d
			float yChange = 0.0f; float yChangeCon = 0.0f; float yChangeNeg = 0.0f; float yChangeNegCon = 0.0f;
			const float cameraSpeed = 0.3f;
			GIn.GetState(G_KEY_SPACE, yChange);
			//int d = 0;
			//GCon.GetNumConnected(d);
			//printf("%S \n", d);
			GCon.GetState(0, G_RIGHT_TRIGGER_AXIS, yChangeCon);
			GIn.GetState(G_KEY_LEFTSHIFT, yChangeNeg);
			GCon.GetState(0, G_LEFT_TRIGGER_AXIS, yChangeNegCon);
			/*if (yChange > cameraSpeed)
				yChange = 0.3f;*/
			viewMat.row4.y += ((yChange - yChangeNeg) + (yChangeCon - yChangeNegCon)) * cameraSpeed * SPSLF;

			// TODO: Part 4e
			float zChange = 0.0f; float zChangeNeg = 0.0f; float xChange = 0.0f; float xChangeNeg = 0.0f;
			float zChangeCon = 0.0f; float xChangeCon = 0.0f;
			GIn.GetState(G_KEY_D, xChange);
			GIn.GetState(G_KEY_A, xChangeNeg);
			GIn.GetState(G_KEY_S, zChange);
			GIn.GetState(G_KEY_W, zChangeNeg);
			GCon.GetState(0, G_LY_AXIS, zChangeCon);
			GCon.GetState(0, G_LX_AXIS, xChangeCon);

			GW::MATH::GVECTORF translationVec = { ((xChange - xChangeNeg) + xChangeCon) * cameraSpeed * SPSLF, 0.0f, ((zChange - zChangeNeg) + zChangeCon) * cameraSpeed * SPSLF, 0.0f };
			matMath.TranslateLocalF(viewMat, translationVec, viewMat);
			// TODO: Part 4f 
			float speed = 3.14159f * SPSLF * 25.0f; float mouseYChange = 0.0f; float mouseXChange = 0.0f; float conYChange = 0.0f; float conXChange = 0.0f;
			auto res = GIn.GetMouseDelta(mouseXChange, mouseYChange);
			GCon.GetState(0, G_RY_AXIS, yChangeCon);
			GCon.GetState(0, G_RX_AXIS, xChangeCon);
			//GIn.GetMouseDelta(mouseYChange);
			if (screenSplit)
			{
				
				matMath.InverseF(viewMat2, viewMat2);
			
				float yChange2 = 0.0f;  float yChangeNeg2 = 0.0f;
				GIn.GetState(G_KEY_ENTER, yChange2);
			
				GIn.GetState(G_KEY_RIGHTSHIFT, yChangeNeg2);
				
				
				viewMat2.row4.y += ((yChange2 - yChangeNeg2)* cameraSpeed * SPSLF);

				// TODO: Part 4e
				float zChange2 = 0.0f; float zChangeNeg2 = 0.0f; float xChange2 = 0.0f; float xChangeNeg2 = 0.0f;
				
				GIn.GetState(G_KEY_RIGHT, xChange2);
				GIn.GetState(G_KEY_LEFT, xChangeNeg2);
				GIn.GetState(G_KEY_DOWN, zChange2);
				GIn.GetState(G_KEY_UP, zChangeNeg2);
				

				GW::MATH::GVECTORF translationVec2 = { ((xChange2 - xChangeNeg2)) * cameraSpeed * SPSLF, 0.0f, ((zChange2 - zChangeNeg2)) * cameraSpeed * SPSLF, 0.0f };
				matMath.TranslateLocalF(viewMat2, translationVec2, viewMat2);
				// TODO: Part 4f 
				matMath.InverseF(viewMat2, viewMat2);

			}
			if (G_PASS(res) && res != GW::GReturn::REDUNDANT)
			{
				float totalPitch = fov * ((mouseYChange / screenHeight) + conYChange) * -speed;
				GW::MATH::GMATRIXF pitchMatrix = GW::MATH::GIdentityMatrixF;
				//matMath.RotationYawPitchRollF(0.0f, -totalPitch, 0.0f, pitchMatrix);
				GW::MATH::GMATRIXF CameraSave = viewMat;
				matMath.RotateXLocalF(viewMat, totalPitch, viewMat);
				// matMath.MultiplyMatrixF(viewMat4x4,pitchMatrix,viewMat4x4);
				// TODO: Part 4g
				float totalYaw = fov * AR * ((mouseXChange / screenWidth) + conXChange) * (speed);
				GW::MATH::GMATRIXF yawMatrix = GW::MATH::GIdentityMatrixF;
				matMath.RotationYawPitchRollF(-totalYaw, 0.0f, 0.0f, yawMatrix);
				matMath.MultiplyMatrixF(viewMat, yawMatrix, viewMat);
				viewMat.row4 = CameraSave.row4;
			}
			// TODO Part 4c
			matMath.InverseF(viewMat, viewMat);
			//prevTimeInput = currTime;
			}
			/*for (int i =0; i < models.size();i++)
			{
				float dist;
				colider.SqDistancePointToAABBF(viewMat.row4, models[i].BB, dist);
				if (dist == 0)
				{
					viewMat.row4 = cameraPositoin;
					break;
				}
			}*/
		}
		unsigned int loadCubemap(std::vector<std::string> imagePaths) // From Learn OpenGl
		{
			unsigned int tID;
			glGenTextures(1, &tID);
			glBindTexture(GL_TEXTURE_CUBE_MAP, tID);

			int width, height, nrChannels;
			for (unsigned int i = 0; i < imagePaths.size(); i++)
			{
				unsigned char* data = stbi_load(imagePaths[i].c_str(), &width, &height, &nrChannels, 0);
				if (data)
				{
					glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
						0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
					);
					stbi_image_free(data);
				}
				else
				{
					std::cout << "Cubemap tex failed to load at path: " << imagePaths[i] << std::endl;
					stbi_image_free(data);
				}
			}
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		//	glTexParameteri(GL_TEXTURE_CUBE_MAP, WGL_TEXTURE_CUBE_MAP_NEGATIVE_Z_ARB, GL_CLAMP_TO_EDGE);

			return tID;
		}
		void getFile()
		{
			
		}
		~Renderer()
		{
			//// free resources
			//glDeleteVertexArrays(1, &vertexArray);
			//glDeleteBuffers(1, &vertexBufferObject);
			//// TODO: Part 1g
			//glDeleteBuffers(1, &indiciesBuffer);
			glDeleteShader(vertexShader);
			glDeleteShader(fragmentShader);
			glDeleteProgram(shaderExecutable);
		
			//// TODO: Part 2c
			//glDeleteBuffers(1, &uboBuffer);

			//skymap
			glDeleteVertexArrays(1, &skyboxVAO);
			glDeleteBuffers(1, &skyboxVBO);
			glDeleteShader(vertexShaderSkybox);
			glDeleteShader(fragmentShaderSkybox);
			glDeleteProgram(shaderExecutableSkybox);
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
	PFNGLACTIVETEXTUREPROC              glActiveTexture = nullptr;
	PFNGLUNIFORM1IPROC                  glUniform1i = nullptr;
	

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
		ogl.QueryExtensionFunction(nullptr, "glActiveTexture", (void**)&glActiveTexture);
		ogl.QueryExtensionFunction(nullptr, "glUniform1i", (void**)&glUniform1i);
	}
};
