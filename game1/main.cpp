#include "header_stack.h"

PerlinNoise* I::noise = nullptr;
SoundBase* I::audio = nullptr;
BaseClass* I::baseClass = nullptr;
InputBase* I::inputs = nullptr;
ParticleBase* I::particleBase = nullptr;
ChunkBase* I::chunkBase = nullptr;
PhysWorldBase* I::worldBase = nullptr;
LightBase* I::lightBase = nullptr;
Player* I::mainPlayer = nullptr;

using namespace std;
using namespace Math;

int main()
{
    I::noise = new PerlinNoise();
    I::audio = new SoundBase();
    I::baseClass = new BaseClass();

#pragma region JUST_TESTING

    CustRender::CompileFonts();

    I::baseClass->menuBase  = new C_BaseMenu(10, 10, 100, 50);
    C_BaseControlGroup* Box00 = new C_BaseControlGroup(10, 10, 10, 20, UIFlags::H_FromParentBottomSide | UIFlags::W_FromParentRightSide | UIFlags::X_SequentialChildPlacement, I::baseClass->menuBase);
    new C_CheckBox(Box00, "Erm");
    new C_CheckBox(Box00, "DErm");
    new C_CheckBox(Box00, "BBErm");
    new C_CheckBox(Box00, "SSSErm");

#pragma endregion

    I::inputs = new InputBase();
    I::particleBase = new ParticleBase();
    I::chunkBase = new ChunkBase();
    I::worldBase = new PhysWorldBase();
    I::lightBase = new LightBase();
    I::mainPlayer = new Player(100, 100);


    std::cout << "movement: WASD LSHIFT\nchange brush type: 0-9\nincrease brush size: L\ndecrease brush size: P\nadd particles: MOUSE1\ndelete particles: MOUSE2\n";

    I::lightBase->spotLights.push_back({ static_cast<int>(I::inputs->particleMousePosition.x), static_cast<int>(I::inputs->particleMousePosition.y), 130 , {254, 0,0}, .7f });

    I::worldBase->ground->SetTransform(b2Vec2(I::chunkBase->curClumpOffset.x + static_cast<float>(I::baseClass->particleResolution[0]) * 1.5f, 0), I::chunkBase->curClumpOffset.y);

    /* Loop until the user closes the window */
    while (!I::baseClass->ShouldClose())
    {
        
        I::baseClass->NewFrame();

        //I::audio->MoveSound(SoundID::DefaultSong, I::inputs->particleMousePosition.x, I::inputs->particleMousePosition.y, 100.f);

        I::baseClass->Run();

        I::baseClass->RenderUI();

        //    CustRender::DebugPrint(100, 100, "hello world 0123456789", ColorM(255, 255, 255));

     //   static StringFormation* woo = CustRender::pushString(100, 100, "soon tm.", CustRender::getFirstFont(), ColorM(1, 1, 1), 2.f, 2);


       
        for (StringFormation* ddd : CustRender::StringStack)
        {
            if (!ddd->manualRender)
                ddd->Render();
        }

        if (I::inputs->IsKeyPressed(GLFW_KEY_ESCAPE))
            break;

        I::inputs->Reset();
        
        I::baseClass->EndFrame();
    }

    delete I::mainPlayer;
    delete I::lightBase;
    delete I::worldBase;
    delete I::chunkBase;
    delete I::particleBase;
    delete I::inputs;
    delete I::baseClass;
    delete I::audio;
    delete I::noise;

    glfwTerminate();
    return 0;
}