#include "header_stack.h"

using namespace Math;

void Particle::Draw()
{
}

bool Particle::BoundX(int& xp)
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

int Particle::RetBoundX(int xp)
{
    if (xp >= I::baseClass->particleResolution[0])
        xp = I::baseClass->particleResolution[0] - 1;
    else if (xp < 0)
        xp = 0;

    return xp;
}

bool Particle::BoundY(int& yp)
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

int Particle::RetBoundY(int yp)
{
    if (yp >= I::baseClass->particleResolution[1])
        yp = I::baseClass->particleResolution[1] - 1;
    else if (yp < 0)
        yp = 0;

    return yp;
}

void Particle::SetColor(unsigned char cr, unsigned char cg, unsigned char cb)
{
    r = cr;
    g = cg;
    b = cb;
}


void Particle::SetType(int id)
{
    switch (id)
    {
    case AirP:
    {
    //    int Scake = fabs(ValueNoise_2D(x, y)) * 754.0;
      //  int Scake = ValueNoise_2D(x, y) * 254.0;
        SetColor(254, 254, 254);

        break;
    }
    case SandP:
    {
    //    float drkOff = (((rand() % 10) - 5) * .015f);
        int darkOffSet = rand() % 50;
        SetColor(244 - darkOffSet, 244 - darkOffSet, 100 - darkOffSet);
        break;
    }
    case WaterP:
    {
    //    int darkOffSet = x % 20;
        SetColor(50, 50, 230);
        break;
    }
    case PeeP:
    {
        SetColor(230, 230, 40);
        break;
    }
    case FireP:
    {
        int lightOffSet = rand() % 24;
        subX = I::baseClass->curMilli + 500 + rand() % 300;
        SetColor(200 + lightOffSet, 105 + lightOffSet, 40 + lightOffSet);

        break;
    }
    case SmokeP:
    {
        int darkOffSet = rand() % 50;
        subX = I::baseClass->curMilli + 2000 + rand() % 1000;
        SetColor(120 - darkOffSet, 120 - darkOffSet, 120 - darkOffSet);
        break;
    }
    case WoodP:
    {

        int SinX = sin(x) * 19.f;
        int CosY = cos(y + rand() % 5) * 10.f;
        SetColor((120 - SinX) + CosY, (60 - SinX) + CosY, (41 - SinX) + CosY);
        break;
    }
    case GunPowderP:
    {
        int darkOffSet = rand() % 20;
        SetColor(60 - darkOffSet, 60 - darkOffSet, 60 - darkOffSet);
        break;
    }
    case BombP:
    {
        SetColor(253, 0, 0);
        break;
    }
    case RockGenP:
    {
        int SinX = sin(x + rand() % 4) * 19.f;
        int CosY = cos((y % ( 4 - (rand() % 2))* 40)) * 10.f;
        SetColor((70 - SinX) + CosY, (60 - SinX) + CosY, (41 - SinX) + CosY);
        break;
    }
    case UnkP:
    {
        SetColor(253, 253, 253);
        break;
    }
    }

    dormantTick = 0;
    classid = id;
}

/*
TODO: make it so chunkpos is stored in the particle so i dont have translate the position to clump position on every move
*/
bool Particle::SetPos(Vec2Di vPos, bool moveOnly)
{
    if (x == vPos.x && y == vPos.y)
        return false;

 //   BoundX(prevy);
//    BoundY(prevy);

    

    bool OutBoundX = BoundX(vPos.x);
    bool OutBoundY = BoundY(vPos.y);

    if (!OutBoundY)
        return true;

    Vec2Di posToClumpPos = I::chunkBase->TranslatePosToClumpPos(vPos);

    int searchIndex = I::chunkBase->mainClump[posToClumpPos.x][posToClumpPos.y];

    if (!searchIndex)
    {
//        if (!BoundX(prevPos.x) || !BoundY(prevPos.x))
//            return true;

        Vec2Di prevPosToClumpPos = I::chunkBase->TranslatePosToClumpPos({ x,y });

        if (I::chunkBase->mainClump[prevPosToClumpPos.x][prevPosToClumpPos.y] && !moveOnly)
            I::chunkBase->mainClump[prevPosToClumpPos.x][prevPosToClumpPos.y] = 0;

        I::chunkBase->mainClump[posToClumpPos.x][posToClumpPos.y] = index;
         
        if (dormantTick > 0)
        {
            I::particleBase->WakeDormant(x, y);
            dormantTick = 0;
        }

        x = vPos.x;
        y = vPos.y;

        return true;
    }
    else
    {
        Particle& targ = I::particleBase->Particles->at(searchIndex - 1);

        if (CanMove(targ))
        {
            //   if (wasDormant)
            {
                I::particleBase->WakeDormant(x, y);
                targ.dormantTick = 0;
                dormantTick = 0;
                //    isDormant = false;
            //    wasDormant = true;
            }
            I::particleBase->QueueParticleEvent(PEvent::Swap, index - 1, searchIndex - 1);
        }
        else
        {
        //    targ.dormantTick = 0;

           // targ.isDormant = false;
        }
    }

    return false;
}

int Particle::CheckPos(Vec2Di vPos)
{
    if (!BoundX(vPos.x) || !BoundY(vPos.y))
        return 0;

    Vec2Di posToClumpPos = I::chunkBase->TranslatePosToClumpPos(vPos);

 //   I::particleBase->WakeDormant(x, y);

    return I::chunkBase->mainClump[posToClumpPos.x][posToClumpPos.y];
}

void Particle::Swap(Particle& targ)
{
    Particle copy = *this;

    Vec2Di posToClumpPos = I::chunkBase->TranslatePosToClumpPos({ copy.x, copy.y } );

    I::chunkBase->mainClump[posToClumpPos.x][posToClumpPos.y] = targ.index;
    posToClumpPos = I::chunkBase->TranslatePosToClumpPos({ targ.x, targ.y });
    I::chunkBase->mainClump[posToClumpPos.x][posToClumpPos.y] = copy.index;
    x = targ.x;
    y = targ.y;
    targ.x = copy.x;
    targ.y = copy.y;
}

int Particle::GetDensity()
{
//    isDormant = true;

    switch (classid)
    {
    case AirP:
    {
        return 5;
        break;
    }
    case SandP:
    {
        return 5;
        break;
    }
    case WaterP:
    {
        return 3;
        break;
    }
    case PeeP:
    {
        return 3;
        break;
    }
    case FireP:
    {
        return 2;
        break;
    }
    case SmokeP:
    {
        return 1;
        break;
    }
    case WoodP:
    {
        return 6;
        break;
    }
    case GunPowderP:
    {
        return 4;
        break;
    }
    case BombP:
    {
        return 5;
        break;
    }
    case RockGenP:
    {
        return 6;
        break;
    }
    default:
    {
        return 5;
            break;
    }
    }
}

bool Particle::CanMove(Particle& targ)
{
    return GetDensity() > targ.GetDensity();
}

int Particle::AttemptPosition(int xp, int yp)
{
 /*   if (x == xp && y == yp)  TODO: update
        return 0;

    //muahahah do this BECAUSE
  //  xp = static_cast<float>(xp) + .5f;
  //  yp = static_cast<float>(yp) + .5f;


    Vec2Df destP = { xp, yp };

    float xMoves = destP.x - x;
    float yMoves = destP.y - y;

    int moveSum = abs(xMoves) + abs(yMoves);

    float xStep = xMoves / static_cast<float>(moveSum);
    float yStep = yMoves / static_cast<float>(moveSum);

    destP = { x, y };

    for (int i = 1; i <= moveSum; i++)
    {
        int xSim = static_cast<float>(x) + RoundToInt(xStep * static_cast<float>(i));
        int ySim = static_cast<float>(y) + RoundToInt(yStep * static_cast<float>(i));

        bool OutBoundX = BoundX(xSim);
        bool OutBoundY = BoundY(ySim);

        int retIndex = mainGrid[xSim][ySim];
        if (retIndex && retIndex != index)
        {
            Particle& HitP = Particles->at(retIndex - 1);

            if (destP.x != xSim)
            {
                double oldxv = vx;

                vx = HitP.vx * 0.9f;
                HitP.vx = oldxv;
            }

            if (destP.y != ySim)
            {
                double oldyv = vy;

                vy = HitP.vy * 0.9f;
                HitP.vy = oldyv;
            }

            return SetPos(destP.x, destP.y);
        }
        else if (!OutBoundX || !OutBoundY)
        {
            if (!OutBoundX)
                vx = 0;
            if (!OutBoundY)
                vy = 0;
            return SetPos(destP.x, destP.y);
        }

        destP = { xSim, ySim };
    }

    if (!BoundX(xp))
        vx = 0;
    if (!BoundY(yp))
        vy = 0;

    return SetPos(xp, yp);*/
    return 0;
}

void Particle::SandThink()
{
/*    int retInd = mainGrid[RetBoundX(x)][RetBoundX(y - 1)];
    if (!retInd && y - 1 > 0)
        vy -= gravityPerTick; // gravity

*/
    if (!SetPos({ x, y - 1 })/*y > 0 && !AttemptPosition(RoundToInt(x + (vx)), RoundToInt(y + (vy)))*/)
    {
        /*   if (retInd)
           {
               Particle& pPointr = Particles->at(retInd - 1);
               if (pPointr.classid == WaterP)
                   Swap(pPointr);
           }// simple density swap*/

        int newChX = 0;
        int newX = 0;
        int newChY = 0;
        int newY = 0;

        if (lastrnd == -1)
            lastrnd = (rand() % 2);

        int rndDir = lastrnd ? 1 : -1;

        /*   if (fabs(vx) > fabs(vy))
           {
               if (vx < 0)
               {
                   newChX = prevx - 1;
                   newX = x;

                   newChY = y + rndDir;
                   newY = newChY;
               }
               else
               {
                   newChX = prevx + 1;
                   newX = x;

                   newChY = y + rndDir;
                   newY = newChY;
               }
           }
           else
           {
               if (vy < 0)
               {*/
        newY = y;

        newChX = x + rndDir;
        newX = newChX;
        /*    }
            else
            {
                newChY = prevy + 1;
                newY = y;

                newChX = x + rndDir;
                newX = newChX;
            }
        }*/

        if (!CheckPos({ newChX, y - 1 }))
        {
            if (SetPos({ newX, newY }))
            {
                lastrnd = -1;
            }
            else
            {
                if (lastrnd == 1)
                    lastrnd = 0;
                else
                    lastrnd = 1;
            }
        }
        else
        {
            subX += 1;
            if (lastrnd == 1)
                lastrnd = 0;
            else
                lastrnd = 1;
        }

        if (subX < 2)
            dormantTick = 0;
    }
    else
        subX = 0;
}

void Particle::WaterThink()
{
    if (!SetPos({ x, y - 1 }))
    {
        if (lastrnd == -1)
            lastrnd = (rand() % 2);

        int rndDir = lastrnd ? 1 : -1;

        if (!SetPos({ x + rndDir, y }))
        {
            if (lastrnd == 1)
                lastrnd = 0;
            else
                lastrnd = 1;              
        }
    }
}

Vec2Di directions[4] = { Vec2Di(1,0),Vec2Di(0,-1),Vec2Di(-1,0), Vec2Di(0,1) };

void Particle::FireThink()
{
    if (subX < I::baseClass->curMilli)
    {
        SetType(SmokeP);
        return;
    }

    int retIndex = CheckPos(Vec2Di(x,y) + directions[rand() % 4]);
    if (retIndex)
    {
        Particle& pPointr = I::particleBase->Particles->at(retIndex - 1);

        switch (pPointr.classid)
        {
        case AirP:
        {
            break;
        }
        case SandP:
        case WaterP:
        case PeeP:
        {
            pPointr.SetType(SmokeP);
            break;
        }
        case WoodP:
        {
            if (pPointr.subX == 0)
                pPointr.subX = I::baseClass->curMilli + 2000 + rand() % 1000;
            break;
        }
        case GunPowderP:
        {
            pPointr.SetType(FireP);
            break;
        }
        case BombP:
        {
            if (pPointr.subX == 0)
                pPointr.subX = 1;
            break;
        }
        default:
        {
            break;
        }
        }
    }

    if (!SetPos({ x  + ((rand() % 3) - 1), y + 1}))
    {
        if (lastrnd == -1)
            lastrnd = (rand() % 2);

        int rndDir = lastrnd ? 1 : -1;

        if (!SetPos({ x + rndDir, y }))
        {
            if (lastrnd == 1)
                lastrnd = 0;
            else
                lastrnd = 1;
        }
    }
}

void Particle::SmokeThink()
{
    if (subX < I::baseClass->curMilli)
    {
        I::particleBase->QueueParticleEvent(PEvent::Delete, index - 1 );

        return;
    }

    if (!SetPos({ x + ((rand() % 2) == 1 ? ((rand() % 3) - 1) : 0), y + (rand() % 2) }))
    {
        if (lastrnd == -1)
            lastrnd = (rand() % 2);

        int rndDir = lastrnd ? 1 : -1;

        if (!SetPos({ x + rndDir, y }))
        {
            if (lastrnd == 1)
                lastrnd = 0;
            else
                lastrnd = 1;
        }
    }
}

void Particle::WoodThink()
{
    if (subX != 0)
    {
        if (subX < I::baseClass->curMilli)
        {
            SetType(FireP);
            return;
        }
        else if (subY <= 0)
        {
            Vec2Di theCheckPos = Vec2Di(x, y) + directions[3];
            int retIndex = CheckPos(theCheckPos);
            if (!retIndex) // dosnt work right need to make a particle creation queue since this is multithreaded and stuff getting criss crossed apple sauce and what not
            {
                I::particleBase->QueueParticle(theCheckPos, FireP);
            }

            subY = 1;
        }
        else
            subY -= 1;
    }
}

void Particle::GunPowderThink()
{
    if (!SetPos({ x, y - 1 })/*y > 0 && !AttemptPosition(RoundToInt(x + (vx)), RoundToInt(y + (vy)))*/)
    {
        /*   if (retInd)
           {
               Particle& pPointr = Particles->at(retInd - 1);
               if (pPointr.classid == WaterP)
                   Swap(pPointr);
           }// simple density swap*/

        int newChX = 0;
        int newX = 0;

        int newY = 0;

        if (lastrnd == -1)
            lastrnd = (rand() % 2);

        int rndDir = lastrnd ? 1 : -1;

        /*   if (fabs(vx) > fabs(vy))
           {
               if (vx < 0)
               {
                   newChX = prevx - 1;
                   newX = x;

                   newChY = y + rndDir;
                   newY = newChY;
               }
               else
               {
                   newChX = prevx + 1;
                   newX = x;

                   newChY = y + rndDir;
                   newY = newChY;
               }
           }
           else
           {
               if (vy < 0)
               {*/

        newY = y;

        newChX = x + rndDir;
        newX = newChX;
        /*    }
            else
            {
                newChY = prevy + 1;
                newY = y;

                newChX = x + rndDir;
                newX = newChX;
            }
        }*/

        if (!CheckPos({ newChX, y - 1 }))
        {
            if (SetPos({ newX, newY }))
            {
                lastrnd = -1;
            }
            else
            {
                if (lastrnd == 1)
                    lastrnd = 0;
                else
                    lastrnd = 1;
            }
        }
        else
        {
            if (lastrnd == 1)
                lastrnd = 0;
            else
                lastrnd = 1;
        }
    }
}

void Particle::BombThink()
{
    if (subX != 0)
    {
        float radius = 50.f;
        float radiusSqr = radius * radius;
        float radiusHalf = radius / 2.f;

        int xStart = x - radius;
        int yStart = y - radius;

        for (int x = -radius; x < radius; x++)
        {
            for (int y = -radius; y < radius; y++)
            {
                Vec2Di theCheckPos = Vec2Di(xStart, yStart);

                if (std::pow(x, 2) + std::pow(y, 2) <= radiusSqr)
                {
                    int retIndex = CheckPos(theCheckPos);
                    if (!retIndex)
                    {
                        I::particleBase->QueueParticle(theCheckPos, FireP);
                    }
                    else
                    {
                        I::particleBase->Particles->at(retIndex - 1).SetType(FireP);
                    }
                }

                yStart += 1;
            }

            yStart = y - radius;
            xStart += 1;
        }

        I::audio->PlaySoundO(SoundID::Explosion, x, y);
    }
    else
    {
        SetPos({ x, y - 1 });
    }
}

void Particle::Think()
{
    switch (classid)
    {
    case AirP:
    {
        dormantTick++;
        break;
    }
    case SandP:
    {
        dormantTick++;
        SandThink();
        break;
    }
    case WaterP:
    {
        dormantTick++;
        WaterThink();
        break;
    }
    case PeeP:
    {
    //    dormantTick = 0;
        WaterThink();
        break;
    }
    case FireP:
    {
    //    dormantTick = 0;
        FireThink();
        break;
    }
    case SmokeP:
    {
    //    dormantTick = 0;
        SmokeThink();
        break;
    }
    case WoodP:
    {
        dormantTick++;
        WoodThink();
        break;
    }
    case GunPowderP:
    {
        dormantTick++;
        GunPowderThink();
        break;
    }
    case BombP:
    {
        dormantTick++;
        BombThink();
        break;
    }
    case RockGenP:
    {      
        dormantTick++;
        break;
    }
    }

    //wasDormant = isDormant;
//    convx = x * particleSize;
 //   convy = y * particleSize;
}

void Particle::Clear()
{
    x = 0;
    y = 0;
    r = 0;
    g = 0;
    b = 0;
    index = 0;
    classid = 0;
    lastrnd = -1;
    formationIndex = 0;
    subX = 0;
    subY = 0;
    dormantTick = 0;
}