#include "header_stack.h"

#include <filesystem>
namespace fs = std::filesystem;


void ThreadPool::Start(int threads_num) {

    I::baseClass->curThreads += threads_num;

    if (I::baseClass->curThreads > I::baseClass->maxThreads)
    {
        glfwTerminate();
        throw std::invalid_argument("MAX THREADS EXCEEDED");
        return;
    }

    numThreads = threads_num;//std::thread::hardware_concurrency(); // Max # of threads the system supports
    threads.reserve(numThreads);
    for (uint32_t i = 0; i < numThreads; i++) {
        threads.push_back(std::thread(&ThreadPool::ThreadLoop, this));
    }
}

void ThreadPool::QueueJob(const std::function<void()>& job) {
    {
        std::unique_lock lock(I::baseClass->queue_mutex);
        jobs.push(job);
    }
    mutex_condition.notify_one();
}

void ThreadPool::FinishJobs()
{
    while (busy())
    {
        
    }
}

bool ThreadPool::busy() {
    bool poolbusy;
    {
        std::unique_lock lock(I::baseClass->queue_mutex);
        poolbusy = !jobs.empty();
    }
    return poolbusy;
}

void ThreadPool::Stop() {
    {
        std::unique_lock lock(I::baseClass->queue_mutex);
        should_terminate = true;
    }
    mutex_condition.notify_all();
    for (std::thread& active_thread : threads) {
        active_thread.join();
    }
    threads.clear();
}

void ThreadPool::ThreadLoop() {
    while (true) {
        std::function<void()> job;
        {
            std::unique_lock lock(I::baseClass->queue_mutex);
            mutex_condition.wait(lock, [this] {
                return !jobs.empty() || should_terminate;
                });
            if (should_terminate) {
                return;
            }
            job = jobs.front();
            jobs.pop();
        }
        job();
    }
}

BaseClass::BaseClass()
{
    short maxThreadCount = std::thread::hardware_concurrency();
    std::cout << "Max Threads " << std::thread::hardware_concurrency() << std::endl;

    maxThreads = maxThreadCount;
    curThreads = 0;

    tickCount = 125.f;
    fixedDeltaTime = 1.f / tickCount;
    msPerThink = (1000.0f / tickCount) + 0.5f;

    lastMilli = glfwGetTime() * 1000.0f;//getMilliCount()
    curMilli = glfwGetTime() * 1000.0f;//getMilliCount()
    milliDelta = 0;

    gravityFeetPerSec = 9.8f * 3.281f; // x3.281 is the aprox conversion of meters to feet (changing this will change the scale of the physics movement)
    gravityPerTick = pow(fixedDeltaTime * gravityFeetPerSec, 2);

    screenResolution[0] = 1280;
    screenResolution[1] = 720;

    //size of 1 chunk    
    particleResolution[0] = 320;
    particleResolution[1] = 180;

    particleResolutionHalf[0] = static_cast<float>(particleResolution[0]) * 0.5f;
    particleResolutionHalf[1] = static_cast<float>(particleResolution[1]) * 0.5f;

    halfXParticleRes = particleResolutionHalf[0];
    halfYParticleRes = particleResolutionHalf[1];

    cameraPos.Set(halfXParticleRes, halfYParticleRes);
    oldCameraPos = cameraPos;

    cameraPos.x += particleResolution[0];
    cameraPos.y += particleResolution[1];
  
    particleResolution3X[0] = particleResolution[0] * 3;
    particleResolution3X[1] = particleResolution[1] * 3;//size of 1 clump

    particleSize = (static_cast<float>(screenResolution[0]) / static_cast<float>(particleResolution[0]));


    /* Initialize the library */
    if (glfwInit() == GLFW_FALSE)
        return;

    /*
    aloud resolutions
    640x360
    960x540
    1280x720
    1600x900
    1920x1080
    2560x1440
    3840x2160
    5120x2880
    7680x4320
    */

    const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

    screenResolution[0] = mode->width;
    screenResolution[1] = mode->height;

    int resolutionXPresets[9][2] = { {640,360}, {960,540}, {1280,720}, {1600,900}, {1920,1080}, {2560,1440}, {3840,2160}, {5120,2880}, {7680,4320} };

    for (int i = 0; i < 9; i++)
    {
        if (i == 8)
        {
            screenResolution[0] = resolutionXPresets[i][0];
            screenResolution[1] = resolutionXPresets[i][1];
        }
        else if (resolutionXPresets[i][0] <= screenResolution[0] && resolutionXPresets[i + 1][0] > screenResolution[0])
        {
            screenResolution[0] = resolutionXPresets[i][0];
            screenResolution[1] = resolutionXPresets[i][1];
            break;
        }
    }


    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(screenResolution[0], screenResolution[1], "game v0.1.0", NULL, NULL);

    cfgInfo = new Cfg();

    particleSize = (static_cast<float>(screenResolution[0]) / static_cast<float>(particleResolution[0]));


    //    glfwSetWindowMonitor(window, glfwGetPrimaryMonitor(), 0, 0, screenResolution[0], screenResolution[1], mode->refreshRate); // fullscreen

    glfwSetWindowAttrib(window, GLFW_RESIZABLE, GL_FALSE);

    if (!window)
    {
        glfwTerminate();
        return;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    // Set color used when clearing the window to black
    glClearColor(63.f / 254.f, 167.f / 254.f, 202.f / 254.f, 1);

    // Set up orthographic projection
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, particleResolution[0], 0.0, particleResolution[1], -1.0, 1.0);
    glViewport(0, 0, screenResolution[0], screenResolution[1]);

    fs::create_directories("TestMap");
}

void BaseClass::CalcTime()
{
    curMilli = glfwGetTime() * 1000.0f;
    if (lastMilli != curMilli)
    {
        milliDelta += curMilli - lastMilli;
        lastMilli = curMilli;
    }
}

void BaseClass::TimeKetchup() // catches time up after all the time used in doing everything but leaves remainders so when enough error adds up it will account for the lost time
{
    while (milliDelta >= msPerThink)
    {
        //        if (MainPlayer)
        //            MainPlayer->Update();
        //        else
        //            MainPlayer = new Player(100, 100);

        milliDelta -= msPerThink;
    }
}

bool BaseClass::ShouldClose()
{
    return glfwWindowShouldClose(window);
}

void BaseClass::NewFrame()
{
    /* Render here */
//   if (RenderFrame)
    glClear(GL_COLOR_BUFFER_BIT);
    //    glFlush();
}

void BaseClass::EndFrame()
{
    /* Swap front and back buffers */
    glfwSwapBuffers(window);

    /* Poll for and process events */
    glfwPollEvents();
}

void BaseClass::Run()
{
    float curtime = glfwGetTime();

    CalcTime();

    static float deltaTimeHalf = fixedDeltaTime * 3.f;

    bool newTick = false;

    // around 52425 is the max chunk x
    for (int i = milliDelta; i >= msPerThink; i -= msPerThink)
    {
    /*    if (I::inputs->IsKeyHeld(GLFW_KEY_A))
        {
            cameraPos.x -= 1280;//particleResolution[0];
        }
        if (I::inputs->IsKeyHeld(GLFW_KEY_D))
        {
            cameraPos.x += 1280;//particleResolution[0];
        }
        if (I::inputs->IsKeyHeld(GLFW_KEY_W))
        {
            cameraPos.y += 3;//particleResolution[1];
        }
        if (I::inputs->IsKeyHeld(GLFW_KEY_S))
        {
            cameraPos.y -= 3;//particleResolution[1];
        }*/

        if (I::worldBase->physWorld && I::worldBase->physWorld->GetWarmStarting())
            I::worldBase->physWorld->Step(deltaTimeHalf, 8, 3); // physic sim steps 8, 3

        I::particleBase->CleanFormationList();
        for (ParticleObject& Objects : I::particleBase->Formations)
        {
            if (Objects.objectBody && (I::chunkBase->doingChunkOperations ? !Objects.isLoadedNew : true))
                Objects.SetTransform(Objects.isLoadedNew);
        }

     //   lightBase->sunAngle += 1.f;

        if (I::lightBase->sunAngle > 0.f)
            I::lightBase->sunAngle = -180.f;

        newTick = true;
    }

    if (I::inputs->IsKeyPressed(GLFW_KEY_G))
        I::mainPlayer->Teleport(40000, 25000);

    I::mainPlayer->Update();

    //    int oldPSize = *pSize;
    I::particleBase->DoBrush();

    CalcTime();

    /*   if (I::inputs->IsKeyPressed(GLFW_KEY_H))//test of particleformation
        {
            Formations.push_back(ParticleFormation(1, 30, 250, 30, Formations.size() + 1, 0, 0, 0, ParticleFormType::Static));
            Formations.push_back(ParticleFormation(50, 60, 125, 30, Formations.size() + 1, 0, 0, 0, ParticleFormType::Static));
            Formations.push_back(ParticleFormation(100, 90, 50, 30, Formations.size() + 1, 0, 0, 0, ParticleFormType::Static));
        }*/

        /*   if (I::inputs->IsKeyHeld(GLFW_KEY_T))//test of particleformation
           {

               for (int i = milliDelta; i >= msPerThink; i -= msPerThink)//think tick every 8 millisec so 125 ticks per sec
               {
                   debugDraw.clear();
                   for (ParticleFormation& pF : Formations)
                   {
                       if (pF.type == ParticleFormType::Static)
                       {
                           pF.yv = gravityPerTick;
                           pF.xv = 0;
                           pF.anglev = 0;
                           continue;
                       }

                       if (I::inputs->IsKeyHeld(GLFW_KEY_G))
                       {
                           Vec2Df CentMasPos = pF.getCentMassPos();
                           Vec2Df newVel = SimpleProjectPoint(CentMasPos, {I::inputs->particleMousePosition.x, I::inputs->particleMousePosition.y}, 7);
                           pF.xv += (newVel.x - CentMasPos.x) * gravityPerTick;
                           pF.yv += (newVel.y - CentMasPos.y) * gravityPerTick;
                       }

                       pF.yv -= gravityPerTick;
                       pF.PhysThink();
                   }
               }
           }

           ParticleHandle::CalcTime(); */


    static bool particleMoved = false;
    if (I::particleBase->Think())
    {
        particleMoved = true;
    }

    TimeKetchup();

    int curtimeInt = curtime;
    static int lastTime = curtime;
    static int fps = 0;
    static int frameCount = 0;
    frameCount += 1;
    if (lastTime != curtimeInt)
    {
        fps = frameCount;
        frameCount = 0;
        lastTime = curtimeInt;

    }

    static float lastMili = curtime + 0.125f;
    static int lastSize = 0;

    if (lastMili < curtime)
    {
        //  if (WorldFuncs::threads.size() == 0)
         //     WorldFuncs::threads.push_back(std::async( std::launch::async, WorldFuncs::BuildWorldCollision ));
        if (particleMoved || lastSize != *I::particleBase->pSize)
        {
            I::worldBase->BuildWorldCollision();
            lastSize = *I::particleBase->pSize;
            particleMoved = false;
        }
        lastMili = curtime + 0.250f;

    }


    if (I::inputs->IsKeyPressed(GLFW_KEY_I))
    {
        //   alSourcei(I::audio->Index[SoundID::Explosion], AL_LOOPING, AL_FALSE);
           //play
       //    alSourcePlay(I::audio->Index[SoundID::Explosion]);

        I::audio->PlaySoundO(SoundID::Explosion, I::inputs->particleMousePosition.x, I::inputs->particleMousePosition.y);
        I::audio->MoveSound(SoundID::Explosion, I::inputs->particleMousePosition.x, I::inputs->particleMousePosition.y, 10000.f);
        I::audio->EndLoop(SoundID::DefaultSong);
    }

    if (I::inputs->IsKeyHeld(GLFW_KEY_O))
    {
        int mainGridPTotal = 0;
        for (int xp = 0; xp < particleResolution3X[0]; xp++)
            for (int yp = 0; yp < particleResolution3X[1]; yp++)
            {
                if (I::chunkBase->mainClump[xp][yp])
                    mainGridPTotal++;
            }

        std::cout << "particlecount: " << *I::particleBase->pSize << "  particlesonscreen: " << mainGridPTotal << "  milliseconds between frames:" << milliDelta << "  fps: " << fps << std::endl;
    }

    //    ParticleHandle::TimeKetchup();





/*       float xMoves = destP.x - originalP.x;
       float yMoves = destP.y - originalP.y;

       int moveSum = abs(xMoves) + abs(yMoves);

       float xStep = xMoves / static_cast<float>(moveSum);
       float yStep = yMoves / static_cast<float>(moveSum);

       destP = originalP;

       for (int i = 1; i <= moveSum; i++)
       {
           int xSim = originalP.x + (xStep * static_cast<float>(i)) + 0.5f;
           int ySim = originalP.y + (yStep * static_cast<float>(i)) + 0.5f;

        //   if (RenderFrame)
               CustRender::drawRect(xSim, ySim, 1, 1, ColorM(0.f, 1.f, 0.f));
       }*/


    if (cameraPos != oldCameraPos)
    {
        Vec2Dd cameraMoveDelta = oldCameraPos - cameraPos; // translate is backward for whatever reason
        I::inputs->particleMousePosition -= cameraMoveDelta;

        glTranslated(cameraMoveDelta.x, cameraMoveDelta.y, 0.0);

        oldCameraPos = cameraPos;

        screenLeft = cameraPos.x - particleResolutionHalf[0];
        screenBottom = cameraPos.y - particleResolutionHalf[1];
        screenRight = cameraPos.x + particleResolutionHalf[0];
        screenTop = cameraPos.y + particleResolutionHalf[1];

        I::chunkBase->StartChunkOperations();

    }

    I::chunkBase->EndChunkOperations();

    I::lightBase->spotLights[0].x = I::inputs->particleMousePosition.x;
    I::lightBase->spotLights[0].y = I::inputs->particleMousePosition.y;

    I::lightBase->spotLights[0].clr.FromHSV(static_cast<float>(curMilli % 2000) / 2000.f, 1, 1);


    CustRender::drawProjectedLine(I::chunkBase->curClumpOffset.x + particleResolution[0], I::chunkBase->curClumpOffset.y, 90.f, particleResolution[1], { 1,1,1 });
    CustRender::drawProjectedLine(I::chunkBase->curClumpOffset.x + particleResolution[0] + particleResolution[0], I::chunkBase->curClumpOffset.y, 90.f, particleResolution[1], { 1,1,1 });


    I::lightBase->Render(newTick);// crashing from passing some buffer 

    I::worldBase->DebugRender();


    CustRender::drawRect(I::inputs->particleMousePosition.x - (I::particleBase->brushSize * .5f), I::inputs->particleMousePosition.y - (I::particleBase->brushSize * .5f), I::particleBase->brushSize, I::particleBase->brushSize, ColorM(1, 1, 1));
  
    Vec2Df pnt1 = { particleResolution[0] * .5f, particleResolution[1] * .5f };
    CustRender::drawRect(std::clamp<float>(cameraPos.x - pnt1.x, 0.f, FLT_MAX), -100, particleResolution[0], 100, ColorM(0.5f, 0.5f, 0.5f));// the floor


    I::mainPlayer->Render();
}

void BaseClass::RenderUI()
{


    static StringFormation* particleCountString = CustRender::pushString(100, 100, " ", CustRender::getFirstFont(), ColorM(1, 1, 1), 2.f, 2);

    particleCountString->SetPos(screenLeft, screenTop - 10.f);
    particleCountString->SetString(std::to_string(*I::particleBase->pSize));

    static StringFormation* particleBrushString = CustRender::pushString(100, 100, "sand", CustRender::getFirstFont(), ColorM(1, 1, 1), 2.f, 2);

    particleBrushString->SetPos(screenLeft, screenTop - 20.f);
    particleBrushString->SetString(I::particleBase->currentBrushString);

    static StringFormation* formationCountString = CustRender::pushString(100, 100, " ", CustRender::getFirstFont(), ColorM(1, 1, 1), 2.f, 2);

    formationCountString->SetPos(screenLeft, screenTop - 30.f);
    formationCountString->SetString(std::to_string(I::particleBase->Formations.size()));

    if (cfgInfo->bMenuOpen)
    {
        I::inputs->EnableInputs();

        if (!I::inputs->IsKeyPressed(GLFW_KEY_Q))
        {
        //    CustRender::drawRect(screenLeft, screenBottom, particleResolution[0], particleResolution[1], ColorM(.5, .5, .5));

            menuBase->Render();

            I::inputs->DisableInputs();
        }
        else
            cfgInfo->bMenuOpen = false;
    }
    else if (I::inputs->IsKeyPressed(GLFW_KEY_Q))
    {
        cfgInfo->bMenuOpen = true;
    }
}