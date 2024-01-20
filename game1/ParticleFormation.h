#pragma once

enum ParticleFormType
{
    Default,
    Static
};

/*
class CollisionInfo
{
public:
    int targIndex = 0;
    int numOfHits = 0;
    Vec2Df hitPos = { 0,0 };
    bool collidedBecRotation = false;
    bool collidedBecXMove = false;
    bool collidedBecYMove = false;
};

class ParticleFormation // make the particles that are apart of the formation use their xy position as the array index for the submainGrid
{
public:
    int type = 0;
    int x = 0, y = 0;
    float xv = 0, yv = 0;
    double angle = 0;
    double anglev = 0;
    int width = 0, height = 0;
    int startIndex = 0;


    int midX = 0, midY = 0;
    int massX = 0, massY = 0;

    unsigned int** submainGrid;
    std::vector<int> boundryIndexes;

    StringFormation* debugInfo = nullptr;

    ParticleFormation(int xp, int yp, int w, int h, int ind, float nxv = 0.f, float nyv = 0.f, float nav = 0.f, int typee = 0)
    {

        type = typee;
        x = xp;
        y = yp;
        width = w;
        height = h;
        angle = 0;

        xv = nxv;
        yv = nyv;
        anglev = nav;

        submainGrid = new unsigned int* [width];
        for (int i = 0; i < width; ++i)
        {
            submainGrid[i] = new unsigned int[height];
            for (int d = 0; d < height; d++)
                submainGrid[i][d] = 0;
        }


        for (int xst = x; xst < x + width; xst++)
        {
            int nX = xst - x;
            for (int hst = y; hst < y + height; hst++)
            {
                int nY = hst - y;

                int index = *pSize + 1;

                submainGrid[nX][nY] = index;
            //    mainGrid[xst][hst] = index; -------------------------------------------


                Particles[*pSize] = { xst, hst, 0, index, 0, 0, nX, nY };
                *pSize += 1;

                //   if (nX == 0 && nY == 0)
               //        Particles[endIndex - 1].clr.SetColor(static_cast<float>(rand()% 100) / 100.f, static_cast<float>(rand() % 100) / 100.f, static_cast<float>(rand() % 100) / 100.f);

                Particles[index - 1].formationIndex = ind;
            }
        }
    //    calcBoundry();

    //    Vec2Df CentMassPos = getCentMassPos();
    //    debugInfo = CustRender::pushString(CentMassPos.x, CentMassPos.y, "blah", CustRender::getFirstFont(), ColorM(1, 0, 0), .7f, 1);

    }

    void OverridePixelPos(Particle& manip, int x, int y);

    void SetTransform(double ang);

    bool BoundX(int& xp);
    bool BoundY(int& yp);

    void PhysThink();

    Vec2Df getCentMassPos();

    void calcBoundry();
};*/

 

class ParticleObject
{
public:

    int index = 0;
    Vec2Di pos = Vec2Di();
    Vec2Df vel = Vec2Df();
    float angv = 0.f;
    float ang = 0.f;

    int width = 0;
    int height = 0;

    int MassX = 0;
    int MassY = 0;

    b2Vec2 oldCentPos = b2Vec2();

    std::vector<b2Vec2> vertices = std::vector<b2Vec2>();

    b2Body* objectBody = nullptr;

    bool isLoadedNew = false;

    unsigned int** subGrid = nullptr;


    ParticleObject(int iX, int iY, int iWidth, int iHeight ,int iIndex ,float fAngle = 0.f ,float fAngleVelocity = 0.f, Vec2Df vVel = Vec2Df(), bool bSpecial = false)
    {
        isLoadedNew = bSpecial;

        index = iIndex;

        pos = Vec2Di(iX, iY);
        angv = fAngleVelocity;
        ang = fAngle;

        width = iWidth;
        height = iHeight;

        vel = vVel;

        objectBody = nullptr;

        oldCentPos = { 0,0 };

        subGrid = new unsigned int* [iWidth];
        for (int i = 0; i < iWidth; ++i)
        {
            subGrid[i] = new unsigned int[iHeight];
            for (int d = 0; d < iHeight; d++)
                subGrid[i][d] = 0;
        }

        if (!bSpecial)
        {
            for (int xst = iX; xst < iX + iWidth; xst++)
            {
                int nX = xst - iX;
                for (int hst = iY; hst < iY + iHeight; hst++)
                {
                    int nY = hst - iY;


                    int index = *I::particleBase->pSize + 1;

                    subGrid[nX][nY] = index;

                    Vec2Di posToClumpPos = I::chunkBase->TranslatePosToClumpPos(Vec2Di(xst, hst));
                    I::chunkBase->mainClump[posToClumpPos.x][posToClumpPos.y] = index;


                    I::particleBase->Particles->at(*I::particleBase->pSize).Set(xst, hst, 0, index, 0, 0, nX, nY);
                    I::particleBase->Particles->at(*I::particleBase->pSize).formationIndex = iIndex;
                    *I::particleBase->pSize += 1;

                    //   if (nX == 0 && nY == 0)
                   //        Particles[endIndex - 1].clr.SetColor(static_cast<float>(rand()% 100) / 100.f, static_cast<float>(rand() % 100) / 100.f, static_cast<float>(rand() % 100) / 100.f);
                }
            }

            CalcPolygon();
        }

    }

    void OverridePixelPos(Particle& manip, int x, int y);
    void SetTransform(bool bForce = false);
    void CalcPolygon(std::deque<Particle>* custParticles = nullptr);

    bool BoundX(int& xp);
    bool BoundY(int& yp);
};