#pragma once

enum ChunkProcess
{
    chunkWrites,
    chunkCopies,
    preChunkLoads,
    postChunkLoads,
    maxProcess
};

 

class ChunkBase
{
public:
    int** clumpBuffer1;
    int** clumpBuffer2;
    int** mainClump;
    Vec2Di mainChunk;
    Vec2Di oldMainChunk;
    Vec2Di curClumpOffset;
    Vec2Di curClumpFarBounds;
    bool chunkNeedsLoading[9];
    bool chunkNeedsSaving[9];

    int* oppositeSize;
    std::deque<Particle>* oppositeParticles;
    int** oppositeClump;

    bool chunkStep[ChunkProcess::maxProcess];
    bool doingChunkOperations;

 //   std::vector<std::future<void>> threads;

    bool currenBuffer;
    ThreadPool chunkBaseThreadPool = ThreadPool();

    Vec2Di upChunk = Vec2Di(0, 1), rightChunk = Vec2Di(1, 0), downChunk = Vec2Di(0, -1), leftChunk = Vec2Di(-1, 0),
        upRightChunk = upChunk + rightChunk,
        downRightChunk = downChunk + rightChunk,
        downLeftChunk = downChunk + leftChunk,
        upLeftChunk = leftChunk + upChunk;

    Vec2Di chunkArray[9] = { Vec2Di(0,0), Vec2Di(0,1)/*upChunk*/, Vec2Di(1,1)/*upRightChunk*/, Vec2Di(1,0)/*rightChunk*/, Vec2Di(1,-1)/*downRightChunk*/, Vec2Di(0,-1)/*downChunk*/, Vec2Di(-1,-1)/*downLeftChunk*/, Vec2Di(-1,0)/*leftChunk*/, Vec2Di(-1, 1)/*upLeftChunk*/ };

    int chunkIndex[3][3] = { {6,7,8}, {5,0,1}, {4,3,2} };

    Vec2Di moveDirection = Vec2Di(0, 0);

    inline int ChunkPosToIndex(Vec2Di pos)
    {
        return chunkIndex[pos.x + 1][pos.y + 1];
    }

    ChunkBase()
    {
        chunkBaseThreadPool.Start(1);
 //       threads.clear();
        currenBuffer = false;
        clumpBuffer1 = { nullptr };
        clumpBuffer2 = { nullptr };
        mainClump = { nullptr };
        mainChunk.Set(1, 1);
        oldMainChunk.Set(1, 1);
        curClumpOffset.Set((mainChunk.x - 1) * I::baseClass->particleResolution[0], (mainChunk.y - 1) * I::baseClass->particleResolution[1]);
        curClumpFarBounds.Set(curClumpOffset.x + I::baseClass->particleResolution3X[0], curClumpOffset.y + I::baseClass->particleResolution3X[1]);
        for (int i = 0; i < 9; i++)
        {
            chunkNeedsLoading[i] = false;
            chunkNeedsSaving[i] = false;
        }

        oppositeSize = nullptr;
        oppositeParticles = nullptr;
        oppositeClump = { nullptr };

        for (int i = 0; i < ChunkProcess::maxProcess; i++)
            chunkStep[i] = 0;

        doingChunkOperations = false;

        clumpBuffer1 = new int* [I::baseClass->particleResolution3X[0]];
        clumpBuffer2 = new int* [I::baseClass->particleResolution3X[0]];


        for (int i = 0; i < I::baseClass->particleResolution3X[0]; ++i)
        {
            clumpBuffer1[i] = new int[I::baseClass->particleResolution3X[1]];
            clumpBuffer2[i] = new int[I::baseClass->particleResolution3X[1]];


            for (int d = 0; d < I::baseClass->particleResolution3X[1]; d++)
            {
                clumpBuffer1[i][d] = 0;
                clumpBuffer2[i][d] = 0;
            }
        }

        mainClump = clumpBuffer1;
    }

    ~ChunkBase()
    {
        chunkBaseThreadPool.Stop();

        for (int i = 0; i < I::baseClass->particleResolution3X[0]; ++i)
        {
            delete[] clumpBuffer1[i];
            delete[] clumpBuffer2[i];
        }

        delete[] clumpBuffer1;
        delete[] clumpBuffer2;
    }

    Vec2Di TranslatePosToChunks(Vec2Di vPos);
    Vec2Di TranslatePosToClumpPos(Vec2Di vPos);
    void WriteChunkData(Vec2Di chunkSection);
    void LoadChunkData(Vec2Di chunkSection);
    void CopyChunkData(Vec2Di fromChunk, Vec2Di toChunk);

    void DoChunkCopies();
    void DoChunkRemovalAndCopy();
    void DealWithChunks();
    void SwitchBuffers();
    void SwitchFormationBuffer();
    void StartChunkOperations();
    void EndChunkOperations();
};