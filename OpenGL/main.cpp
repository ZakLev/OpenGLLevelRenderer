// Simple basecode showing how to create a window and attatch a openglsurface
#define GATEWARE_ENABLE_CORE // All libraries need this
#define GATEWARE_ENABLE_SYSTEM // Graphics libs require system level libraries
#define GATEWARE_ENABLE_GRAPHICS // Enables all Graphics Libraries
#define GATEWARE_ENABLE_MATH  // Math Library
#define GATEWARE_ENABLE_INPUT
#define GATEWARE_ENABLE_AUDIO
#define GATEWARE_ENABLE_COLLISION
// TODO: Part 2a
// Ignore some GRAPHICS libraries we aren't going to use
#define GATEWARE_DISABLE_GDIRECTX11SURFACE // we have another template for this
#define GATEWARE_DISABLE_GDIRECTX12SURFACE // we have another template for this
#define GATEWARE_DISABLE_GVULKANSURFACE // we have another template for this
#define GATEWARE_DISABLE_GRASTERSURFACE // we have another template for this

// With what we want & what we don't defined we can include the API
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "../Gateware/Gateware.h"
#include "Model.h"
#include "renderer.h" // example rendering code (not Gateware code!)
#include "FileIO.h"
#include "LevelData.h"
#include "h2bParser.h"
// open some namespaces to compact the code a bit
using namespace GW;
using namespace CORE;
using namespace SYSTEM;
using namespace GRAPHICS;
// lets pop a window and use OpenGL to render to a window
int main()
{
	GWindow win;
	GEventResponder msgs;
	GOpenGLSurface ogl;
	bool changeLevel = false;
	bool playSound = false;
	bool soundOver = false;
	bool openDialogue = false;
	bool PlayMusic = true;
	int level = 0;
	char* filePath = "../../Assets/GameLevel.txt";
	std::string file = "";
	char* musicPath = "../../Assets/Music/OOOF.wav";
	char* soundPath = "../../Assets/Sound/AUG.wav";
	GW::AUDIO::GMusic song;
	GW::AUDIO::GAudio aud;
	GW::AUDIO::GSound sou;
	
	if (+win.Create(0, 0, 800, 600, GWindowStyle::WINDOWEDBORDERED))
	{
		

		float clr[] = { 48 / 255.0f, 20 / 255.0f, 150 / 255.0f, 1 }; // TODO: Part 1a
		win.SetWindowName("Zak Levine LevelRenderer - OpenGL");
		msgs.Create([&](const GW::GEvent& e) {
			GW::SYSTEM::GWindow::Events q;
			if (+e.Read(q) && q == GWindow::Events::RESIZE)
				clr[2] += 0.01f; // shift color as they resize
		});
		win.Register(msgs);
		
		
		do
		{
			changeLevel = false;
		if (+ogl.Create(win, GW::GRAPHICS::DEPTH_BUFFER_SUPPORT))
		{
			bool isPlay;
			song.isPlaying(isPlay);
			if (isPlay)
			{
				aud.Create();
				song.Create(musicPath, aud, 1.0f);
				sou.Create(soundPath,aud,1.0f);
				song.Play(true);
				aud.PlayMusic();
			}
			if (file.compare(""))
			{
				filePath = (char*)file.c_str();
			}
			Renderer renderer(win, ogl, filePath);
			file = "";
		
			
			while (+win.ProcessWindowEvents())
			{
				glClearColor(clr[0], clr[1], clr[2], clr[3]);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				renderer.UpdateCamera(changeLevel, filePath, playSound,PlayMusic,file);
				if (changeLevel)
					break;
				if (playSound && !soundOver)
				{
					aud.PlaySounds();
					song.Pause();
				}
				sou.isPlaying(soundOver);
				if (!soundOver && playSound)
				{
					if (PlayMusic)
					{
					song.Resume();
					}
					playSound = false;
				}
				bool songPlaying;
				song.isPlaying(songPlaying);
				if (PlayMusic && songPlaying == false)
				{
					song.Resume();
				}
				else if (songPlaying && PlayMusic == false)
				{
					song.Pause();
				}
				
				renderer.Render();
				ogl.UniversalSwapBuffers();
			}
		}
		} while (changeLevel);
	}
	return 0; // that's all folks
}