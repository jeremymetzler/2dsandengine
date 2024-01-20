#include "header_stack.h"

using namespace Math;


void ParticleBase::DeleteParticle(int& index)
{
    if (index > 0 && index <= *pSize)// if there is a particle there then enter this scope
    {
        Particle& mainp = Particles->at(index - 1);// gets the particle reference using the index provided

        int formIndex = mainp.formationIndex;// checks to see if the particle is part of a formation
     /*   if (formIndex)
        {
            ParticleFormation& pF = Formations[formIndex - 1];
            pF.submainGrid[mainp.subX][mainp.subY] = 0;// removing this particle from its formation mainGrid
        }*/

        if (index != *pSize)
        {
            size_t lastInd = *pSize - 1; // gets the last particles index in the particle stack
            Particle& lastElm = Particles->at(lastInd); // references the last particle
            Particle cpyP = mainp;// copies the information of the last particle

            mainp = lastElm;// makes the current particle thats being deleted into the last particle in the stack
            mainp.index = index; // updates the last particles index with the new position of the current particle we are getting rid of

            formIndex = mainp.formationIndex;// checks if the replaced particle is in a formation
        /*    if (formIndex)
            {
                ParticleFormation& pF = Formations[formIndex - 1];
                pF.submainGrid[mainp.subX][mainp.subY] = index;// updates the particles index in the formation
            }*/

            lastElm = cpyP;// gives the last particle in the stack the info of the particle we are deleting
            lastElm.index = lastInd + 1;// updates the index of the particle we are deleting

            index = 0;// goodbye old particle index

            Vec2Di posToClumpPos = I::chunkBase->TranslatePosToClumpPos({ mainp.x, mainp.y });

            I::chunkBase->mainClump[posToClumpPos.x][posToClumpPos.y] = mainp.index;// updates the index of the particle in the mainGrid array to the new index of the particle we have just replaced

            lastElm.Clear();
            *pSize -= 1;
        }
        else
        {
            mainp.Clear();
            index = 0;
            *pSize -= 1;
        }

    }
}

void ParticleBase::DeleteParticle2(Particle& mainp)
{
    int index = mainp.index;

    if (index > 0 && index <= *pSize)// if there is a particle there then enter this scope
    {
        Vec2Di posToClumpPos = I::chunkBase->TranslatePosToClumpPos({ mainp.x, mainp.y });
        int& indexPtr = I::chunkBase->mainClump[posToClumpPos.x][posToClumpPos.y];

        int formIndex = mainp.formationIndex;// checks to see if the particle is part of a formation
    /*    if (formIndex)
        {
            ParticleFormation& pF = Formations[formIndex - 1];
            pF.submainGrid[mainp.subX][mainp.subY] = 0;// removing this particle from its formation mainGrid
        }*/

        if (index != *pSize)
        {
            size_t lastInd = *pSize - 1; // gets the last particles index in the particle stack
            Particle& lastElm = Particles->at(lastInd); // references the last particle
            Particle cpyP = mainp;// copies the information of the last particle

            mainp = lastElm;// makes the current particle thats being deleted into the last particle in the stack
            mainp.index = index; // updates the last particles index with the new position of the current particle we are getting rid of

            formIndex = mainp.formationIndex;// checks if the replaced particle is in a formation
        /*    if (formIndex)
            {
                ParticleFormation& pF = Formations[formIndex - 1];
                pF.submainGrid[mainp.subX][mainp.subY] = index;// updates the particles index in the formation
            }*/

            lastElm = cpyP;// gives the last particle in the stack the info of the particle we are deleting
            lastElm.index = lastInd + 1;// updates the index of the particle we are deleting

            indexPtr = 0;// goodbye old particle index

            posToClumpPos = I::chunkBase->TranslatePosToClumpPos({ mainp.x, mainp.y });

            I::chunkBase->mainClump[posToClumpPos.x][posToClumpPos.y] = mainp.index;// updates the index of the particle in the mainGrid array to the new index of the particle we have just replaced

            lastElm.Clear();
            *pSize -= 1;
        }
        else
        {
            mainp.Clear();
            indexPtr = 0;
            *pSize -= 1;
        }

    }
}

void ParticleBase::QueueParticle(Vec2Di pos, int type)
{
    if (pQueueSize < 100'000)
    {
        pParticleQueue[pQueueSize] = Particle(pos.x, pos.y, type, 0);
        pQueueSize += 1;
    }
}

void ParticleBase::QueueParticleEvent(int eventId, int targ, int var1, int var2)
{
    if (eventSize < 200000)
    {
        EventQueue[eventSize] = { eventId, targ, var1, var2 };
        eventSize += 1;
    }
}

void ParticleBase::DoEvent(ParticleEvent& pEvent)
{
    switch (pEvent.id)
    {
    case PEvent::Delete:
    {
        if (*pSize > pEvent.targ)
        {
            Particle& targ = Particles->at(pEvent.targ);
            DeleteParticle2(targ);
        }

        break;
    }
    case PEvent::Swap:
    {
        if (*pSize > pEvent.targ && *pSize > pEvent.var1)
        {
            Particle& manip = Particles->at(pEvent.targ);
            Particle& targ = Particles->at(pEvent.var1);

            manip.Swap(targ);
        }

        break;
    }
    }
}

// the debug brush tool for creating particle formations and normal particles
void ParticleBase::DoBrush()
{
    if (I::inputs->IsKeyPressed(GLFW_KEY_L) && brushSize < 40)// increase brush size
    {
        brushSize += 2;
        halfBrushSize = (brushSize * .5f);
    }
    else if (I::inputs->IsKeyPressed(GLFW_KEY_P) && brushSize > 0)// decrease brush size
    {
        brushSize -= 2;
        halfBrushSize = (brushSize * .5f);
    }

    /*   if (I::inputs->IsKeyHeld(GLFW_KEY_D))// debug redraw of all the particles on screen
       {
           for (int xp = 0; xp < particleResolution[0]; xp++)
               for (int yp = 0; yp < particleResolution[1]; yp++)
               {
                   if (mainGrid[xp][yp])
                       CustRender::drawRect(xp * particleSize, yp * particleSize, particleSize, particleSize, ColorM(0, 0, 1));
               }
       }*/


 //   static StringFormation* doingChunkOpoString = CustRender::pushString(100, 100, " ", CustRender::getFirstFont(), ColorM(1, 1, 1), 2.f, 2);

 //   doingChunkOpoString->SetPos(cameraPos.x - 160.f, cameraPos.y + 40.f);
 //   doingChunkOpoString->SetString(std::to_string(I::chunkBase->doingChunkOperations));

    if (I::inputs->IsKeyHeld(GLFW_KEY_1))// brush type sand
    {
        currentBrushString = "sand";
        currentBrush = SandP;
    }
    else if (I::inputs->IsKeyHeld(GLFW_KEY_2))// brush type water
    {
        currentBrushString = "water";
        currentBrush = WaterP;
    }
    else if (I::inputs->IsKeyHeld(GLFW_KEY_3))// brush type pee
    {
        currentBrushString = "pee";
        currentBrush = PeeP;
    }
    else if (I::inputs->IsKeyHeld(GLFW_KEY_4))// brush type fire
    {
        currentBrushString = "fire";
        currentBrush = FireP;
    }
    else if (I::inputs->IsKeyHeld(GLFW_KEY_5))// brush type wood
    {
        currentBrushString = "wood";
        currentBrush = WoodP;
    }
    else if (I::inputs->IsKeyHeld(GLFW_KEY_6))// brush type gunpowder
    {
        currentBrushString = "gun_powder";
        currentBrush = GunPowderP;
    }
    else if (I::inputs->IsKeyHeld(GLFW_KEY_7))// brush type bomb
    {
        currentBrushString = "bomb";
        currentBrush = BombP;
    }
    else if (I::inputs->IsKeyHeld(GLFW_KEY_9))// brush type white
    {
        currentBrushString = "rock";
        currentBrush = RockGenP;
    }
    else if (I::inputs->IsKeyHeld(GLFW_KEY_0))// brush type white
    {
        currentBrushString = "white";
        currentBrush = 0;
    }

    static Vec2Df pPoint1, pPoint2;
    for (int i = I::baseClass->milliDelta; i >= I::baseClass->msPerThink; i -= I::baseClass->msPerThink) // gets the mouse movement in game time
    {
        pPoint2 = pPoint1;
        //   pPoint1 = I::inputs->particleMousePosition;
    }
    pPoint1 = I::inputs->particleMousePosition;

    Vec2Df mouseVelocity = (pPoint1 - pPoint2);

    if (I::inputs->IsMouseButtonHeld(GLFW_MOUSE_BUTTON_1) /*I::inputs->IsMouseButtonHeld(GLFW_MOUSE_BUTTON_1)*/ && brushSize > 0) // normal particle brush (creates particles when pressing mouse 1)
    {
        for (int x = I::inputs->particleMousePosition.x - halfBrushSize; x < I::inputs->particleMousePosition.x + halfBrushSize; x++)
            for (int y = I::inputs->particleMousePosition.y - halfBrushSize; y < I::inputs->particleMousePosition.y + halfBrushSize; y++)
            {
                if (x >= I::chunkBase->curClumpOffset.x && x < I::chunkBase->curClumpFarBounds.x && I::chunkBase->curClumpOffset.y <= y && y < I::chunkBase->curClumpFarBounds.y)
                {
                    Vec2Di posToClumpPos = I::chunkBase->TranslatePosToClumpPos({ x,y });
                    if (!I::chunkBase->mainClump[posToClumpPos.x][posToClumpPos.y])
                    {
                        int newIndex = *pSize + 1;
                        I::chunkBase->mainClump[posToClumpPos.x][posToClumpPos.y] = newIndex;
                        Particles->at(*pSize).Set(x, y, currentBrush, newIndex, mouseVelocity.x, mouseVelocity.y);
                        *pSize += 1;
                    }
                }
            }
    }

    if (I::inputs->IsKeyPressed(GLFW_KEY_M))//dwadwadwa
    {
        /*       int newIndex = *pSize + 250000;
               ChunkHandle::mainClump[100][100] = newIndex;
               Particles->at(newIndex) = { 100, 100, currentBrush, newIndex, mouseVelocity.x, mouseVelocity.y };
               *pSize += 1;*/
        Particles->shrink_to_fit();
    }

    if (I::inputs->IsKeyPressed(GLFW_KEY_F))//creation of particleformation no spin
    {
        Formations.push_back(ParticleObject(I::inputs->particleMousePosition.x - 10, I::inputs->particleMousePosition.y - 10, 20, 20, Formations.size() + 1));
    }

    /*   if (I::inputs->IsKeyPressed(GLFW_KEY_S))//creation of particleformation with spin
       {
           Formations.push_back(ParticleFormation(I::inputs->particleMousePosition.x - 50, I::inputs->particleMousePosition.y - 50, 100, 100, Formations.size() + 1, mouseVelocity.x, mouseVelocity.y, 4));
       }*/

    if (I::inputs->IsMouseButtonHeld(GLFW_MOUSE_BUTTON_2)) // normal particle and particleformation deletion
    {
        for (int x = I::inputs->particleMousePosition.x - halfBrushSize - ((halfBrushSize == 0) ? 1 : 0); x < I::inputs->particleMousePosition.x + halfBrushSize; x++)
        {
            for (int y = I::inputs->particleMousePosition.y - halfBrushSize - ((halfBrushSize == 0) ? 1 : 0); y < I::inputs->particleMousePosition.y + halfBrushSize; y++)
            {
                if (x >= I::chunkBase->curClumpOffset.x && x < I::chunkBase->curClumpFarBounds.x && y >= I::chunkBase->curClumpOffset.y && y < I::chunkBase->curClumpFarBounds.y) // if the searching point of the loop is in screen bounds then enter this scope
                {
                    Vec2Di posToClumpPos = I::chunkBase->TranslatePosToClumpPos({ x,y });

                    int& index = I::chunkBase->mainClump[posToClumpPos.x][posToClumpPos.y];// gets the reference index of the current pixel being searched
                    DeleteParticle(index);
                }
            }
        }

        /*
           // this is to update all the boundries in the particle formation stack (this can be done for the specific formation that is being manipulated instead of all of them)

           int curPSize = Particles->size();
           int curFormIndex = 0;

           for (ParticleFormation& pF : Formations)
           {
              int pCount = 0;
              for (int sx = 0; sx < pF.width; sx++)
              {
                  for (int sy = 0; sy < pF.height; sy++)
                  {
                      int index = pF.submainGrid[sx][sy];
                      if (index > 0 && index <= curPSize)
                      {
                          Particle& mainp = Particles->at(index - 1);
                          mainGrid[mainp.x][mainp.y] = mainp.index;
                          pCount += 1;
                      }
                  }
              }

              if (pCount == 0)
              {
                  for (int i = 0; i < pF.width; ++i) {
                      delete[] pF.submainGrid[i];
                  }
                  delete[] pF.submainGrid;
                  Formations.erase(Formations.begin() + curFormIndex);
                  continue;
              }

               pF.calcBoundry();

               curFormIndex += 1;
           }*/
    }

    if (I::inputs->IsKeyHeld(GLFW_KEY_R))// deletes all particles
    {
        for (int x = 0; x < I::baseClass->particleResolution3X[0]; x++)
            for (int y = 0; y < I::baseClass->particleResolution3X[1]; y++)
                I::chunkBase->mainClump[x][y] = 0;

        for (int i = 0; i < *pSize; i++)
        {
            Particles->at(i).Clear();
        }

        *pSize = 0;


        for (ParticleObject& pF : Formations)
        {
            SoftDeleteFormation(pF);
        }
        Formations.clear();
    }
}


bool particleMoved = false;

void PThink(int start, int end)
{
    std::shared_lock lock(I::baseClass->queue_mutex);

    for (int i = I::baseClass->milliDelta; i >= I::baseClass->msPerThink; i -= I::baseClass->msPerThink)//think tick every 8 millisec so 125 ticks per sec
    {
        for (int i = start; i < end; i++)
        {
            Particle& mainp = I::particleBase->Particles->at(i);

            if (mainp.formationIndex || mainp.dormantTick > 3)
                continue;


                /*    if (I::inputs->IsKeyHeld(GLFW_KEY_G))
                    {
                        Vec2Df newVel = SimpleProjectPoint({ mainp.pos.x, mainp.pos.y }, { I::inputs->particleMousePosition.x, I::inputs->particleMousePosition.y }, 7);
                        mainp.vel.x += (newVel.x - mainp.pos.x) * gravityPerTick;
                        mainp.vel.y += (newVel.y - mainp.pos.y) * gravityPerTick;
                    }
                    else if (I::inputs->IsKeyHeld(GLFW_KEY_B))
                    {
                        Vec2Df newVel = SimpleProjectPoint({ mainp.pos.x, mainp.pos.y }, { I::inputs->particleMousePosition.x, I::inputs->particleMousePosition.y }, -6);
                        mainp.vel.x += (newVel.x - mainp.pos.x);
                        mainp.vel.y += (newVel.y - mainp.pos.y);
                    }*/

            mainp.Think();
            

            if (!particleMoved && mainp.dormantTick == 0)
                particleMoved = true;
        }
    }

    return;
}

bool ParticleBase::Think()
{


    // no multithreading
    if (*pSize < 100)
    {
        PThink(0, *pSize);
    }
    else // multithreading
    {
        int numThreads = particleBaseThreadPool.numThreads;
        int pSizeSplit = *pSize / numThreads; //numThreads

        if (!particleBaseThreadPool.busy())
        {
            for (int i = 0; i < numThreads; i++)
            {
                int startNum = (i * pSizeSplit);
                int lastNum = startNum + pSizeSplit;
                if (i == numThreads - 1)
                    lastNum = *pSize;

                particleBaseThreadPool.QueueJob([startNum, lastNum] { PThink(startNum, lastNum); });

                //  threads.push_back(std::async(std::launch::async, PThink, (i * pSizeSplit), lastNum)); // TODO: POOL THREADS
            }
        }

        particleBaseThreadPool.FinishJobs();

    /*    for (std::future<void>& threadPointer : threads)
        {
            threadPointer.wait();
        }

        threads.clear();*/
    }

    if (eventSize > 0)
    {
        for (int i = 0; i < eventSize; i++)
        {
            ParticleEvent& pEvent = EventQueue[i];
            DoEvent(pEvent);
        }
        eventSize = 0;
    }


    if (pQueueSize > 0)
    {
        for (int i = 0; i < pQueueSize; i++)
        {
            Particle& pParticle = pParticleQueue[i];
            pParticle.BoundX(pParticle.x);
            pParticle.BoundY(pParticle.y);
            Vec2Di posToClumpPos = I::chunkBase->TranslatePosToClumpPos({ pParticle.x, pParticle.y });
            if (!I::chunkBase->mainClump[posToClumpPos.x][posToClumpPos.y])
            {
                int newIndex = *pSize + 1;
                I::chunkBase->mainClump[posToClumpPos.x][posToClumpPos.y] = newIndex;
                pParticle.index = newIndex;
                Particles->at(*pSize) = pParticle;
                *pSize += 1;
            }
            pParticle.Clear();
        }

        pQueueSize = 0;
    }

    if (particleMoved)
        return true;
    return false;
}

bool ParticleBase::BoundX(int& xp)
{
    if (xp >= I::chunkBase->curClumpFarBounds.x)
    {
        xp = I::chunkBase->curClumpFarBounds.x - 1;
        return false;
    }
    else if (xp < I::chunkBase->curClumpOffset.x)
    {
        xp = I::chunkBase->curClumpOffset.x;
        return false;
    }
    return true;
}

bool ParticleBase::BoundY(int& yp)
{
    if (yp >= I::chunkBase->curClumpFarBounds.y)
    {
        yp = I::chunkBase->curClumpFarBounds.y - 1;
        return false;
    }
    else if (yp < I::chunkBase->curClumpOffset.y)
    {
        yp = I::chunkBase->curClumpOffset.y;
        return false;
    }
    return true;
}

bool ParticleBase::BoundClumpX(int xp)
{
    if (xp >= I::baseClass->particleResolution3X[0])
    {
        return false;
    }
    else if (xp < 0)
    {
        return false;
    }
    return true;
}

bool ParticleBase::BoundClumpY(int yp)
{
    if (yp >= I::baseClass->particleResolution3X[1])
    {
        return false;
    }
    else if (yp < 0)
    {
        return false;
    }
    return true;
}


Vec2Di ParticleBase::TraceRay(int x, int y, int x1, int y1, bool boundOnly)
{
    Vec2Di pos = { x,y };
    Vec2Di targ = { x1, y1 };
    float ang = Math::CalcAngle( pos, targ);

    float fCos = cos(Math::DegToRad(ang));
    float fSin = sin(Math::DegToRad(ang));

    float len = Math::CalcDistance(pos, targ);


    for (float i = 0.f; i <= len; i += 1.f)
    {
        int xCos = static_cast<int>(x + i * fCos);
        int ySin = static_cast<int>(y + i * fSin);

        if (!BoundX(xCos) || !BoundY(ySin))
        {
            BoundY(ySin);
            return { xCos , ySin };
        }

        if (!boundOnly)
        {
            Vec2Di searchPos = { xCos, ySin };

            searchPos = I::chunkBase->TranslatePosToClumpPos(searchPos);

            if (I::chunkBase->mainClump[searchPos.x][searchPos.y] > 0)
            {
                return { static_cast<int>(x + (i - 1) * fCos) , static_cast<int>(y + (i - 1) * fSin) };
            }
        }
    }

    return { x1, y1 };
}

void ParticleBase::SoftDeleteFormation(ParticleObject& form)
{
    if (!form.objectBody)
        return;

    if (form.subGrid)
    {
        for (int i = 0; i < form.height; ++i) {
            if (form.subGrid[i])
            {
                delete[] form.subGrid[i];
                form.subGrid[i] = nullptr;
            }
        }
        delete[] form.subGrid;
        form.subGrid = nullptr;
    }

    form.vertices.clear();

    I::worldBase->physWorld->DestroyBody(form.objectBody);
    form.objectBody = nullptr;
}

void ParticleBase::CleanFormationList(bool loadCheck)
{
    int newListIndex = 0;
    for (int i = 0; i < Formations.size(); i++)
    {
        ParticleObject& form = Formations[i];
        if (!form.objectBody && (loadCheck ? !form.isLoadedNew : true))
        {
            newListIndex += 1;
            Formations.erase(Formations.begin() + i);
        }
        else if (newListIndex != 0)
        {
            form.index -= newListIndex;
            int subIndex = 0;
            for (int sx = 0; sx < form.width; sx++)
            {
                for (int sy = 0; sy < form.height; sy++)
                {
                    subIndex = form.subGrid[sx][sy];
                    if (subIndex > 0)
                    {
                        Particle& pSubParticle = Particles->at(subIndex - 1);
                        pSubParticle.formationIndex -= newListIndex;
                    }
                }
            }
        }
    }
}

void ParticleBase::WakeDormant(int x, int y)
{
    Vec2Di pos = I::chunkBase->TranslatePosToClumpPos(Vec2Di(x, y));

    Vec2Di dirs[4] = { {1, 0}, {0, -1}, {-1, 0}, {0, 1} };
    Vec2Di searchPos;
    int searchIndex;

    for (int i = 0; i < 4; i++)
    {
        searchPos = pos + dirs[i];
        if (BoundClumpX(searchPos.x) && BoundClumpY(searchPos.y))
        {
            searchIndex = I::chunkBase->mainClump[searchPos.x][searchPos.y];
            if (searchIndex > 0)
            {
                Particle& manip = Particles->at(searchIndex - 1);

                manip.dormantTick = 0;
            }
        }
    }


}