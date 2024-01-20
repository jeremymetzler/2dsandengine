#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <box2d/box2d.h>
#include "al.h"
#include "alc.h"
#include "alut.h"
#include <vector>
#include <string>
#include <thread>
#include <windows.h>
#include <iostream>
#include <fstream>
#include <sys/timeb.h>
#include <math.h>
#include <future>
#include <cmath>
#include <deque>
#include <random>
#include <numeric>
#include <filesystem>
#include <queue>
#include <shared_mutex>



#include "MiscStuff.h"


#include "SoundHandle.h"
#include "PerlinNoise.h"
#include "Renderer.h"

class InputBase;
class BaseClass;
class ChunkBase;
class LightBase;
class ParticleObject;
class ParticleBase;
class PhysWorldBase;
class Player;
class C_BaseMenu;

namespace I
{
	extern PerlinNoise* noise;
	extern SoundBase* audio;
	extern BaseClass* baseClass;
	extern InputBase* inputs;
	extern ParticleBase* particleBase;
	extern ChunkBase* chunkBase;
	extern PhysWorldBase* worldBase;
	extern LightBase* lightBase;
	extern Player* mainPlayer;
}

#include "BaseClass.h"
#include "InputHandle.h"
#include "JemScript.h"
#include "UIHandle.h"
#include "LightHandle.h"
#include "Particles.h"
#include "ChunkHandle.h"
#include "ParticleHandle.h"
#include "PhysicsWorld.h"
#include "ParticleFormation.h"
#include "Player.h"