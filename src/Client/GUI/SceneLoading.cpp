#include "common.h"
#include <sstream>
#include <math.h>
#include "common.h"
#include "PseuGUI.h"
#include "PseuWoW.h"
#include "Scene.h"
#include "ShTlTerrainSceneNode.h"
#include "MCamera.h"
#include "MInput.h"
#include "CCursorController.h"
#include "DrawObject.h"
#include "MapTile.h"
#include "World/MapMgr.h"
#include "World/WorldSession.h"
#include "World/World.h"
#include "MemoryInterface.h"
#include <iostream>



 SceneLoading::SceneLoading(PseuGUI *g) : Scene(g)
 {  
	DEBUG(logdebug("SceneLoading: Loading models..."));
	Ldebugmode = false;
	// store some pointers right now to prevent repeated ptr dereferencing later (speeds up code)
    gui = g;
    Lwsession = gui->GetInstance()->GetWSession();
    Lworld = Lwsession->GetWorld();
    Lmapmgr = Lworld->GetMapMgr();
	Finished = false;
	LoadingTime = false;
	uint32 total = 0;
	uint32 loaded = 0;
	uint32 percent = 0;

	dimension2d<u32> scrn = driver->getScreenSize();

	loadingscreen = guienv->addImage(driver->getTexture("data\\misc\\wowloadingscreen.jpg"), core::position2d<s32>(5,5),true,rootgui);
	loadingscreen->setRelativePosition(rect<s32>(0,0,scrn.Width,scrn.Height));
	loadingscreen->setScaleImage(true);

	if (AllDoodads.empty() == true) //fill list
	{
		// TODO: at some later point we will need the geometry for correct collision calculation, etc...
		
		for(uint32 tiley = 0; tiley < 3; tiley++)
		{
			for(uint32 tilex = 0; tilex < 3; tilex++)
			{
				MapTile *maptile = Lworld->GetMapMgr()->GetNearTile(tilex - 1, tiley - 1);
				if(maptile)
				{
					for(uint32 i = 0; i < maptile->GetDoodadCount(); i++)
					{
						Doodad *doo = maptile->GetDoodad(i);
						//load *all* necessary model/skin names to list
						AllDoodads.push_back (doo->MPQpath.c_str());
					
					}
				}
			}
		}
	}
	// only keep doodads that are unique
	AllDoodads.sort();
	AllDoodads.unique();
	total = AllDoodads.size();

 }

void SceneLoading::OnUpdate(s32 timediff)
{
	if (LoadingTime == true)
	{
		// TODO: at some later point we will need the geometry for correct collision calculation, etc...
		
		while (AllDoodads.empty() != true)
		{
			std::string m2model = AllDoodads.back(); //get last elemment from the list
			AllDoodads.pop_back();                   //remove used element from list
			io::IReadFile* modelfile = io::IrrCreateIReadFileBasic(device, m2model.c_str());
			//scene::IAnimatedMesh *mesh = smgr->getMesh(m2model.c_str()); // load m2 with irrlicht
			//gui->_device->getSceneManager()->getMesh(m2model.c_str()); //alternate load m2 with irrlicht
			std::string skinfile = m2model.append(4, 'x');
			NormalizeFilename(skinfile);
			//_FixFileName(skinfile); //use linux '/' instead of windows '\' 
			skinfile.replace(skinfile.end()-7,skinfile.end(),"00.skin");
			//scene::IAnimatedMesh *skin = smgr->getMesh(skinfile.c_str());
			//gui->_device->getSceneManager()->getMesh(skinfile.c_str()); 
			loaded++;
			percent = (loaded*100)/total;
			logdetail("SceneLoading:: loded model", m2model.c_str());
			// when I add a loading bar it must be updated every few load loops so I'll have to load a few files then finish the scene loop and load  more on the next scene cycle 
		}
		Finished = true;
		DEBUG(logdebug("SceneLoading: finished Loading models..."));
	}
	LoadingTime = true;

	if (Finished == true){
		gui->SetSceneState(SCENESTATE_WORLD);
	}
}