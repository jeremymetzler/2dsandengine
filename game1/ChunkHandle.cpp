#include "header_stack.h"

Vec2Di ChunkBase::TranslatePosToChunks(Vec2Di vPos) // translates the vPos to the current chunk its in
{
    int currentChunksToTheRight = vPos.x % I::baseClass->particleResolution[0];
    currentChunksToTheRight = (vPos.x - currentChunksToTheRight) / I::baseClass->particleResolution[0];

    int currentChunksToTheUp = vPos.y % I::baseClass->particleResolution[1];
    currentChunksToTheUp = (vPos.y - currentChunksToTheUp) / I::baseClass->particleResolution[1];

    return { currentChunksToTheRight, currentChunksToTheUp };
}

Vec2Di ChunkBase::TranslatePosToClumpPos(Vec2Di vPos) // translates the vPos to the current position in the clump
{
    return vPos - curClumpOffset;
}

void ChunkBase::WriteChunkData(Vec2Di chunkSection)// writes chunk data to its respective file
{
    std::string currentLineData = "";
    std::ofstream myfile = std::ofstream();

    Vec2Di chunkPosition = chunkSection + oldMainChunk;

    int startingYIndex = I::baseClass->particleResolution[1] * (1 + chunkSection.y);
    int endingYIndex = I::baseClass->particleResolution[1] * (2 + chunkSection.y);

    int startingXIndex = I::baseClass->particleResolution[0] * (1 + chunkSection.x);
    int endingXIndex = I::baseClass->particleResolution[0] * (2 + chunkSection.x);

    std::string fileName = "TestMap/" + std::to_string(chunkPosition.x) + '-' + std::to_string(chunkPosition.y) + ".txt";
    
    myfile.open(fileName, std::ofstream::out | std::ofstream::trunc);

//    while (!myfile.is_open()) {}
     
    // some of the particles arent making it to the deletion stage in their chunk for whatever reason 
    /*
    i should probably make some system where you queue the particles you want deleted and then
    at the end of the main while loop delete the particles all at once (somehow)
    */

 //   int loadSize = 0;

    int lastFormIndex = 0;
    for (int y = startingYIndex; y < endingYIndex; y++)
    {
        currentLineData = "";
        for (int x = startingXIndex; x < endingXIndex; x++)
        {
            int& index = mainClump[x][y];
            if (index != 0 && index <= *I::particleBase->pSize)
            {
                Particle& pParticle = I::particleBase->Particles->at(index - 1);
                if (pParticle.formationIndex == 0)
                {
                    currentLineData += ", " + std::to_string(pParticle.x) + ' ' + std::to_string(pParticle.y) + ' ' +
                        std::to_string(pParticle.r) + ' ' + std::to_string(pParticle.g) + ' ' + std::to_string(pParticle.b) + ' ' +
                        std::to_string(pParticle.classid) + ' ';
                }
                else
                {
                    ParticleObject& form = I::particleBase->Formations[pParticle.formationIndex - 1];
                    if (form.objectBody != nullptr)
                    {
                        lastFormIndex = pParticle.formationIndex;

                        currentLineData += "? " + std::to_string(form.pos.x) + ' ' + std::to_string(form.pos.y) + ' ' +
                            std::to_string(form.vel.x) + ' ' + std::to_string(form.vel.y) + ' ' + std::to_string(form.angv) + ' ' + std::to_string(form.ang) + ' ' +
                            std::to_string(form.width) + ' ' + std::to_string(form.height) + ' ';

                        int subIndex = 0;
                        for (int sx = 0; sx < form.width; sx++)
                        {
                            for (int sy = 0; sy < form.height; sy++)
                            {
                                if (form.subGrid[sx] != nullptr)
                                {
                                    subIndex = form.subGrid[sx][sy];
                                    if (subIndex > 0)
                                    {
                                        Particle& pSubParticle = I::particleBase->Particles->at(subIndex - 1);
                                        currentLineData += std::to_string(sx) + ' ' + std::to_string(sy) + ' ' +
                                            std::to_string(pSubParticle.r) + ' ' + std::to_string(pSubParticle.g) + ' ' + std::to_string(pSubParticle.b) + ' ' +
                                            std::to_string(pSubParticle.classid) + ' ';
                                    }
                                }
                            }
                        }
                        myfile << currentLineData;
                        currentLineData = "";
                     //   form.isLoadedNew = true;
                        I::particleBase->SoftDeleteFormation(form);
                    }
                    
                   // currentLineData +=
                }
            //    loadSize += 1;
                //                ParticleHandle::DeleteParticle(index);
            }
            /*    else
                {
                    currentLineData += "0 ";
                }*/

                //    if (x == particleResolution[0] - 1)
                //        currentLineData += "; ";
        }

        myfile << currentLineData;
    }

    I::particleBase->CleanFormationList(false);

/*    std::cout << fileName << " saved " << loadSize << " particles\n";

    if (loadSize == 0)
    {
        myfile << "1";
    }*/
    
    myfile.close();
}

void ChunkBase::LoadChunkData(Vec2Di chunkSection)// loads chunk data from its respective file
{
    std::string currentLineData = "";
    std::ifstream myfile = std::ifstream();
    
    Vec2Di chunkPosition = chunkSection + mainChunk;

    /*
        oldMainChunk = ChunkHandle::mainChunk;
    curClumpOffset = { (ChunkHandle::mainChunk.x - 1) * particleResolution[0], (ChunkHandle::mainChunk.y - 1) * particleResolution[1] };
    curClumpFarBounds = { ChunkHandle::curClumpOffset.x + particleResolution3X[0],  ChunkHandle::curClumpOffset.y + particleResolution3X[1] };

    */

//    int startingYIndex = particleResolution[1] * (1 + chunkSection.y);

//    int startingXIndex = particleResolution[0] * (1 + chunkSection.x);

    Vec2Di newClumpOffset = { (mainChunk.x - 1) * I::baseClass->particleResolution[0], (mainChunk.y - 1) * I::baseClass->particleResolution[1] };

    std::string fileName = "TestMap/" + std::to_string(chunkPosition.x) + '-' + std::to_string(chunkPosition.y) + ".txt";
    bool fileExists = std::filesystem::exists(fileName);

    if (!fileExists && I::baseClass->cfgInfo->bRandomGen)
    {
        int startingYIndex = I::baseClass->particleResolution[1] * (1 + chunkSection.y);
        int endingYIndex = I::baseClass->particleResolution[1] * (2 + chunkSection.y);

        int startingXIndex = I::baseClass->particleResolution[0] * (1 + chunkSection.x);
        int endingXIndex = I::baseClass->particleResolution[0] * (2 + chunkSection.x);


        int yOffset = I::baseClass->particleResolution[1] * (1 + chunkSection.y);

        int xOffset = I::baseClass->particleResolution[0] * (1 + chunkSection.x);

        Vec2Di newCurClumpOffset = { (mainChunk.x - 1) * I::baseClass->particleResolution[0], (mainChunk.y - 1) * I::baseClass->particleResolution[1] };
        Vec2Di newCurClumpFarBounds = { newCurClumpOffset.x + I::baseClass->particleResolution3X[0],  newCurClumpOffset.y + I::baseClass->particleResolution3X[1] };

        int seaLevel = 24500;

        int yStep = 0;
        for (int y = startingYIndex; y < endingYIndex; y++)
        {
        //    int yTo = yOffset + yStep;

            int xStep = 0;
            for (int x = startingXIndex; x < endingXIndex; x++)
            {
            //    int xTo = xOffset + xStep;


                Vec2Di searchPos = Vec2Di(x, y);
                Vec2Di realPos = newClumpOffset + searchPos;

                
                int flux = seaLevel + std::sin(Math::DegToRad(realPos.x % 360)) * 200;

                bool belowSea = flux > realPos.y;

                if (belowSea && I::noise->ValueNoise(realPos.x, realPos.y) > .001 )
                {
                    int newSize = *oppositeSize + 1;
                    Particle& pParticle = oppositeParticles->at(*oppositeSize);
                    pParticle.x = realPos.x;
                    pParticle.y = realPos.y;
                    //   pParticle.r = clrR;
                    //   pParticle.g = clrG;
                    //   pParticle.b = clrB;
                    pParticle.index = newSize;
                    pParticle.SetType(RockGenP);
                    //    pParticle.classid = classID;

                    oppositeClump[searchPos.x][searchPos.y] = newSize;
                    *oppositeSize += 1;
                }

                xStep += 1;
            }
            yStep += 1;
        }
        return;
    }

    myfile.open(fileName);


    std::string output = "";

//    int xOffset = 0;
//    int yOffset = 0;

    while (myfile >> output)
    {
/*        int xIndex = startingXIndex + xOffset;
        int yIndex = startingYIndex + yOffset;

        if (output == ";")
        {
            xOffset = 0;
            yOffset += 1;
        }
        else if (output == "0")
        {
            xOffset += 1;
        }
        else*/ if (output == ",")
        {
        particleRead:
            myfile >> output;
            int xPos = std::stoi(output);
            myfile >> output;
            int yPos = std::stoi(output);
            myfile >> output;
            unsigned char clrR = std::stoi(output);
            myfile >> output;
            unsigned char clrG = std::stoi(output);
            myfile >> output;
            unsigned char clrB = std::stoi(output);
            myfile >> output;
            unsigned char classID = std::stoi(output);

            int newSize = *oppositeSize + 1;

         //   std::cout << "adding particle " << newSize << "\n";

            Vec2Di clumpPos = Vec2Di( xPos, yPos ) - newClumpOffset;
            Particle& pParticle = oppositeParticles->at(*oppositeSize);
            pParticle.x = xPos;
            pParticle.y = yPos;
            pParticle.r = clrR;
            pParticle.g = clrG;
            pParticle.b = clrB;
            pParticle.index = newSize;
            pParticle.classid = classID;

            oppositeClump[clumpPos.x][clumpPos.y] = newSize;
            *oppositeSize += 1;

//            xOffset += 1;
        }    
        else if (output == "?")
        {
            formationRead:
            myfile >> output;
            int xPos = std::stoi(output);
            myfile >> output;
            int yPos = std::stoi(output);
            myfile >> output;
            float vx = std::stof(output);
            myfile >> output;
            float vy = std::stof(output);
            myfile >> output;
            float angv = std::stof(output);
            myfile >> output;
            float ang = std::stof(output);
            myfile >> output;
            int width = std::stoi(output);
            myfile >> output;
            int height = std::stoi(output);

            size_t newFormSize = I::particleBase->Formations.size() + 1;
            I::particleBase->Formations.push_back(ParticleObject(xPos, yPos, width, height, newFormSize, ang, angv, {vx, vy}, true));
            ParticleObject& newForm = I::particleBase->Formations[newFormSize - 1];

            while (myfile >> output)
            {
                if (output == ",")
                {
                    newForm.CalcPolygon(oppositeParticles);
                    goto particleRead;
                }
                else if (output == "?")
                {
                    newForm.CalcPolygon(oppositeParticles);
                    goto formationRead;
                }

 //               myfile >> output;
                int xSubPos = std::stoi(output);
                myfile >> output;
                int ySubPos = std::stoi(output);
                myfile >> output;
                unsigned char clrR = std::stoi(output);
                myfile >> output;
                unsigned char clrG = std::stoi(output);
                myfile >> output;
                unsigned char clrB = std::stoi(output);
                myfile >> output;
                unsigned char classID = std::stoi(output);

                int newSize = *oppositeSize + 1;

                Particle& pParticle = oppositeParticles->at(*oppositeSize);
                pParticle.subX = xSubPos;
                pParticle.subY = ySubPos;
                pParticle.r = clrR;
                pParticle.g = clrG;
                pParticle.b = clrB;
                pParticle.index = newSize;
                pParticle.classid = classID;
                pParticle.formationIndex = newFormSize;

                newForm.subGrid[xSubPos][ySubPos] = newSize;
                *oppositeSize += 1;
            }

            newForm.CalcPolygon(oppositeParticles);
            break;
        }
    }

    myfile.close();
}

void ChunkBase::CopyChunkData(Vec2Di fromChunk,  Vec2Di toChunk)
{
    int startingYIndex = I::baseClass->particleResolution[1] * (1 + fromChunk.y);
    int endingYIndex = I::baseClass->particleResolution[1] * (2 + fromChunk.y);

    int startingXIndex = I::baseClass->particleResolution[0] * (1 + fromChunk.x);
    int endingXIndex = I::baseClass->particleResolution[0] * (2 + fromChunk.x);

 
    int yOffset = I::baseClass->particleResolution[1] * (1 + toChunk.y);

    int xOffset = I::baseClass->particleResolution[0] * (1 + toChunk.x);


    int yStep = 0;
    for (int y = startingYIndex; y < endingYIndex; y++)
    {
        int yTo = yOffset + yStep;

        int xStep = 0;
        for (int x = startingXIndex; x < endingXIndex; x++)
        {
            int xTo = xOffset + xStep;
            int& index = mainClump[x][y];
            if (index)
            {
                Particle& pParticle = I::particleBase->Particles->at(index - 1);
                if (pParticle.formationIndex <= 0)
                {
                    Particle& pOpoParticle = oppositeParticles->at(*oppositeSize);
                    pOpoParticle = pParticle;
                    *oppositeSize += 1;
                    /*   int formIndex = pOpoParticle.formationIndex;
                       if (pOpoParticle.formationIndex != 0)
                       {
                           Formations[formIndex - 1].subGrid[pOpoParticle.subX][pOpoParticle.subY] = *oppositeSize;
                       }*/
                    pOpoParticle.index = *oppositeSize;
                    oppositeClump[xTo][yTo] = *oppositeSize;

                    //    index = 0;
                }
                else
                    oppositeClump[xTo][yTo] = 0;
            }
            else
                oppositeClump[xTo][yTo] = 0;

            xStep += 1;
        }
        yStep += 1;
    }
}

void DoChunkWrites(ChunkBase* parent)
{
    std::shared_lock lock(I::baseClass->queue_mutex);
    
    for (int i = 0; i < 9; i++)
    {
        if (parent->chunkNeedsSaving[i])
        {
            parent->WriteChunkData(parent->chunkArray[i]);
        }
    }
    parent->chunkStep[ChunkProcess::chunkWrites] = true;
}

void ChunkBase::DoChunkCopies()
{
    for (int i = 0; i < 9; i++)
        chunkNeedsLoading[i] = true;

    for (int i = 0; i < 9; i++)
    {
        if (!chunkNeedsSaving[i])
        {
            int chunkToReplace = ChunkPosToIndex(chunkArray[i] - moveDirection);

            CopyChunkData(chunkArray[i], chunkArray[chunkToReplace]);

            chunkNeedsLoading[chunkToReplace] = false;
        }
    }
    chunkStep[ChunkProcess::chunkCopies] = true;
}

void ChunkBase::DoChunkRemovalAndCopy()
{

    for (int i = 0; i < 9; i++)
    {
        chunkNeedsSaving[i] = false;
        Vec2Di chunkSearch = oldMainChunk + chunkArray[i];
        Vec2Di searchDirection = mainChunk - chunkSearch;
        if (abs(searchDirection.x) > 1 || abs(searchDirection.y) > 1)
        {
            chunkNeedsSaving[i] = true;
        }
    }
//    DoChunkWrites(chunkHandled); 
    DoChunkCopies(); // TODO: i think since the particle info is changing it cant perform operations on it so it crashes in threaded mode

    chunkBaseThreadPool.QueueJob([this] { DoChunkWrites(this); });
//    threads.push_back(std::async(std::launch::async, DoChunkWrites, this));
//    threads.push_back(std::async(std::launch::async, DoChunkCopies, chunkHandled));
}

void DoChunkLoads(ChunkBase* parent)
{
    std::shared_lock lock(I::baseClass->queue_mutex);

    for (int i = 0; i < 9; i++)
    {
        if (parent->chunkNeedsLoading[i])
            parent->LoadChunkData(parent->chunkArray[i]);
    }

    parent->chunkStep[ChunkProcess::postChunkLoads] = true;
}

void ChunkBase::DealWithChunks()
{
    doingChunkOperations = true;

    moveDirection = mainChunk - oldMainChunk;

//    threads.clear();

    for (int i = 0; i < ChunkProcess::maxProcess; i++)
    {
        chunkStep[i] = false;
    }

    if (currenBuffer)
    {
        oppositeParticles = I::particleBase->pBuffer1;
        oppositeSize = &I::particleBase->pSize1;
        oppositeClump = clumpBuffer1;
    }
    else
    {
        oppositeParticles = I::particleBase->pBuffer2;
        oppositeSize = &I::particleBase->pSize2;
        oppositeClump = clumpBuffer2;
    }

    for (int x = 0; x < I::baseClass->particleResolution3X[0]; x++)
        for (int y = 0; y < I::baseClass->particleResolution3X[1]; y++)
            oppositeClump[x][y] = 0;

    for (int i = 0; i < *oppositeSize; i++)
    {
        oppositeParticles->at(i).Clear();
    }

    *oppositeSize = 0;

    DoChunkRemovalAndCopy();
}

void ChunkBase::SwitchFormationBuffer()
{
    for (ParticleObject& form : I::particleBase->Formations)
    {
        if (form.isLoadedNew || !form.subGrid)
        {
         //   form.isLoadedNew = false;
            continue;
        }
        for (int x = 0; x < form.width; x++)
        {
            for (int y = 0; y < form.height; y++)
            {
                if (form.subGrid[x][y] > 0)
                {
                    Particle& opoEquiv = oppositeParticles->at(*oppositeSize);

                    int newIndex = *oppositeSize + 1;
                    opoEquiv = I::particleBase->Particles->at(form.subGrid[x][y] - 1);
                    opoEquiv.index = newIndex;
                    form.subGrid[x][y] = newIndex;

                    I::particleBase->BoundX(opoEquiv.x);
                    I::particleBase->BoundY(opoEquiv.y);

                    Vec2Di posToClump = TranslatePosToClumpPos({ opoEquiv.x, opoEquiv.y });
                    oppositeClump[posToClump.x][posToClump.y] = newIndex;

                    *oppositeSize += 1;
                }
            }
        }
    }
}

void ChunkBase::SwitchBuffers()
{

    oldMainChunk = mainChunk;
    curClumpOffset = Vec2Di( (mainChunk.x - 1) * I::baseClass->particleResolution[0], (mainChunk.y - 1) * I::baseClass->particleResolution[1] );
    curClumpFarBounds = Vec2Di(curClumpOffset.x + I::baseClass->particleResolution3X[0],  curClumpOffset.y + I::baseClass->particleResolution3X[1] );


    chunkBaseThreadPool.FinishJobs();
/*    for (std::future<void>& threadPointer : threads)
    {
        threadPointer.wait();
    }*/

 //   threads.clear();

    doingChunkOperations = false;

    SwitchFormationBuffer();

    I::particleBase->Particles = oppositeParticles;
    I::particleBase->pSize = oppositeSize;
    mainClump = oppositeClump;

    oppositeParticles = nullptr;
    oppositeSize = nullptr;
    oppositeClump = nullptr;
/*    if (currenBuffer)
    {
        Particles = pBuffer1;
        pSize = &pSize1;
        mainClump = clumpBuffer1;
    }
    else
    {
        Particles = pBuffer2;
        pSize = &pSize2;
        mainClump = clumpBuffer2;
    }*/

    currenBuffer = !currenBuffer;
}

void ChunkBase::StartChunkOperations()
{
    if (!doingChunkOperations)
    {
        mainChunk = TranslatePosToChunks(I::baseClass->cameraPos);

        if (mainChunk.x <= 0)
            mainChunk.x = 1;
        if (mainChunk.y <= 0)
            mainChunk.y = 1;

        if (mainChunk != oldMainChunk)
        {
            //   std::future<void>(std::async(std::launch::async, ChunkHandle::DealWithChunks));
            DealWithChunks();

          //w  std::cout << "chunk " << mainChunk.x << " " << mainChunk.y << std::endl;
        }
    }
}

void ChunkBase::EndChunkOperations()
{
    if (doingChunkOperations)
    {
        if (chunkStep[ChunkProcess::chunkWrites] && chunkStep[ChunkProcess::chunkCopies] && !chunkStep[ChunkProcess::preChunkLoads])
        {
            //    ChunkHandle::DoChunkLoads();
            chunkStep[ChunkProcess::preChunkLoads] = true;
        /*    for (std::future<void>& threadPointer : threads)
            {
                threadPointer.wait();
            }
            threads.push_back(std::async(std::launch::async, DoChunkLoads, this));*/

            chunkBaseThreadPool.QueueJob([this] { DoChunkLoads(this); });
        }
        else if (chunkStep[ChunkProcess::postChunkLoads])
        {
            for (ParticleObject& Objects : I::particleBase->Formations)
            {
                if (Objects.objectBody && Objects.objectBody->GetType() != b2_dynamicBody)
                {
                    Objects.objectBody->SetEnabled(true);
                    Objects.objectBody->SetType(b2_dynamicBody);
                }
            }
            SwitchBuffers();

            I::worldBase->ground->SetTransform(b2Vec2(curClumpOffset.x + static_cast<float>(I::baseClass->particleResolution[0]) * 1.5f, 0), 0.f);
        }
    }
}