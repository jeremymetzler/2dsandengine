#include "header_stack.h"

using namespace Math;

/*
* TODO: increase max vertex count because its only 8 atm
*/

void ParticleObject::OverridePixelPos(Particle& manip, int x, int y)
{
    if (manip.x == x && manip.y == y)
        return;

    manip.BoundX(x);
    manip.BoundY(y);

    Vec2Di posToClumpPos = I::chunkBase->TranslatePosToClumpPos(Vec2Di(x, y));
    int index = I::chunkBase->mainClump[posToClumpPos.x][posToClumpPos.y];

    if (index == 0 || ((index != 0) ? I::particleBase->Particles->at(index - 1).formationIndex : false))
    {
    //    manip.prevPos.x = manip.pos.x;
    //    manip.prevPos.y = manip.pos.y;

        if (!isLoadedNew)
        {
            if (manip.BoundX(manip.x) && manip.BoundY(manip.y))
            {
                Vec2Di prevPosToClumpPos = I::chunkBase->TranslatePosToClumpPos(Vec2Di(manip.x, manip.y));

                if (I::chunkBase->mainClump[prevPosToClumpPos.x][prevPosToClumpPos.y] == manip.index)
                    I::chunkBase->mainClump[prevPosToClumpPos.x][prevPosToClumpPos.y] = 0;
            }
        }

        manip.x = x;
        manip.y = y;

        I::chunkBase->mainClump[posToClumpPos.x][posToClumpPos.y] = manip.index;
    }
    else if (index != 0)
    {
        Particle& targ = I::particleBase->Particles->at(index - 1);

        if (manip.CanMove(targ))
        {
            I::particleBase->QueueParticleEvent(PEvent::Swap, manip.index - 1, index - 1);
        }
    }
}

void ParticleObject::SetTransform(bool bForce)
{

    b2Transform TransF = objectBody->GetTransform();
    vel = objectBody->GetLinearVelocity();
    angv = objectBody->GetAngularVelocity();
    float curDeg = NormalizeAngle(RadToDeg(TransF.q.GetAngle()));

    if (!subGrid[0][0])
        return;
    if (!bForce && curDeg == ang && pos == TransF.p )
    {
        return;
    }
    else
    {
    //    objectBody->SetType(b2_dynamicBody);

    }

    ang = curDeg;
    b2Vec2 NewCentMassPos = objectBody->GetWorldCenter();
    if (static_cast<int>(oldCentPos.x) != static_cast<int>(NewCentMassPos.x))
    {
        pos.x += static_cast<int>(NewCentMassPos.x) - static_cast<int>(oldCentPos.x);
        oldCentPos.x = static_cast<int>(NewCentMassPos.x);
    }
    if (static_cast<int>(oldCentPos.y) != static_cast<int>(NewCentMassPos.y))
    {
        pos.y += static_cast<int>(NewCentMassPos.y) - static_cast<int>(oldCentPos.y);
        oldCentPos.y = static_cast<int>(NewCentMassPos.y);
    }
    
//    pos = objectBody->GetWorldCenter();
    // int midX = width / 2;
    // int midY = height / 2;

    Vec2Df midPoint = { pos.x + MassX, pos.y + MassY };

    Vec2Df midLeft = ProjectPoint(midPoint, ang + 180.0, MassX);
    Vec2Df bottomLeft = ProjectPoint(midLeft, ang - 90.0, MassY);
    //   Vec2Df topLeft = ProjectPoint(midLeft, ang + 90.0, midY);
    //   Vec2Df bottomRight = ProjectPoint(bottomLeft, ang, width);

    float cosX = cos(DegToRad(ang)), sinX = sin(DegToRad(ang));
    float nY = NormalizeAngle(ang + 90.f);
    float cosY = cos(DegToRad(nY)), sinY = sin(DegToRad(nY));
    int lastY = -1;

    int curPSize = *I::particleBase->pSize;

    for (int hp = 0; hp < height; hp++)
    {
        Vec2Df newY = { bottomLeft.x + cosY * hp, bottomLeft.y + sinY * hp };
        int oldX = 0, oldY = 0;
        int oldIndex = 0;
        for (int xp = 0; xp < width; xp++)
        {
            int index = subGrid[xp][hp];
            if (index > 0 && index <= curPSize)
            {
                Vec2Df newX = { newY.x + cosX * xp, newY.y + sinX * xp };

                if (lastY != hp || !oldIndex)
                {
                    oldX = newX.x;
                    oldY = newX.y;
                    lastY = hp;
                }

                Particle& manip = I::particleBase->Particles->at(index - 1);

                int newXDelta = static_cast<int>(newX.x) - oldX;
                int newYDelta = static_cast<int>(newX.y) - oldY;


                //   if (xp != 0 && abs(newXDelta) > 0 && abs(newYDelta) > 0) // this fixes the dithering, not very efficient but it works
                //   {
                   //    CustRender::drawRect(static_cast<int>(newX.x) * particleSize, static_cast<int>(newX.y - newYDelta) * particleSize, particleSize, particleSize, manip.clr);
                //   }

                   //    CustRender::drawRect(static_cast<int>(newX.x) * particleSize, static_cast<int>(newX.y) * particleSize, particleSize, particleSize, manip.clr);
                OverridePixelPos(manip, newX.x, newX.y);
                //    CustRender::drawRect(static_cast<int>(manip.x) * particleSize, static_cast<int>(manip.y) * particleSize, particleSize, particleSize, manip.clr);
                oldX = newX.x;
                oldY = newX.y;
            }
            oldIndex = index;
        }
    }

 /*   Vec2Df newY = {bottomLeft.x + cosY * MassX, bottomLeft.y + sinY * massY};
    Vec2Df newX = { newY.x + cosX * massX, newY.y + sinX * massX };

    CustRender::drawRect(static_cast<int>(newX.x) * particleSize, static_cast<int>(newX.y) * particleSize, particleSize, particleSize, ColorM(1, 0, 0));

    debugInfo->SetString("xv:" + std::to_string(xv) + " yv:" + std::to_string(yv) + " angv:" + std::to_string(anglev));
    debugInfo->SetPos(newX.x * particleSize, newX.y * particleSize);*/
    /*
        float xMoves1 = bottomRight.x - bottomLeft.x;
        float yMoves1 = bottomRight.y - bottomLeft.y;

        int moveSumX = abs(xMoves1) + abs(yMoves1);

        float xStep1 = xMoves1 / static_cast<float>(moveSumX);
        float yStep1 = yMoves1 / static_cast<float>(moveSumX);

        float xSubIndexScale = static_cast<float>(width) / static_cast<float>(moveSumX);

        float xMoves2 = topLeft.x - bottomLeft.x;
        float yMoves2 = topLeft.y - bottomLeft.y;

        int moveSumY = abs(xMoves2) + abs(yMoves2);

        float xStep2 = xMoves2 / static_cast<float>(moveSumY);
        float yStep2 = yMoves2 / static_cast<float>(moveSumY);

        float ySubIndexScale = static_cast<float>(height) / static_cast<float>(moveSumY);

        int prevX = 0;
        int prevY = 0;

        for (int xp = 0; xp < moveSumX; xp++)
        {
            int curXIndex = static_cast<float>(xp) * xSubIndexScale;

            int curXorgx = bottomLeft.x + (xStep1 * static_cast<float>(xp));
            int curXorgy = bottomLeft.y + (yStep1 * static_cast<float>(xp));

            int oldX = 0, oldY = 0;
            for (int hp = 0; hp < moveSumY; hp++)
            {
                int curYIndex = static_cast<float>(hp) * ySubIndexScale;

                int index = submainGrid[curXIndex][curYIndex];
                if (index)
                {
                    int curYorgx = curXorgx + ((xStep2 * static_cast<float>(hp)) + .5f);
                    int curYorgy = curXorgy + ((yStep2 * static_cast<float>(hp)) + .5f);

                    Particle& manip = Particles[index - 1];

                    int newXDelta = curYorgx - oldX;
                    int newYDelta = curYorgy - oldY;

                    if (abs(newXDelta) > 0 && abs(newYDelta) > 0)
                    {
                        CustRender::drawRect((curYorgx ) * particleSize, (curYorgy - newYDelta) * particleSize, particleSize, particleSize, ColorM(1.f, 1.f, 1.f));
                    }

                    OverridePixelPos(manip, curYorgx, curYorgy);

                    CustRender::drawRect(curYorgx * particleSize, curYorgy * particleSize, particleSize, particleSize, ColorM(1.f, 1.f, 1.f));
                    oldX = curYorgx;
                    oldY = curYorgy;
                }

            }
        }
        */
    isLoadedNew = false;
}

static Vec2Df direction[8] = { {0, -1}/*bottom*/, {-1, -1}/*btmL*/, {-1, 0}/*left*/, {-1, 1}/*topLeft*/, {0, 1}/*top*/, {1, 1}/*topRight*/, {1, 0}/*right*/, {1, -1}/*btmRight*/ };
void ParticleObject::CalcPolygon(std::deque<Particle>* custParticles)
{
    int startIndex = 0;
    int startX = 0;
    int startY = 0;
    bool breakDa = false;
    int curPSize = 0;

    bool bSpecial = true;

    if (!custParticles)
    {
        bSpecial = false;
        custParticles = I::particleBase->Particles;
    }
        
    curPSize = custParticles->size();

    Particle* curManip = nullptr;
    for (int xp = 0; xp < width; xp++)
    {
        for (int hp = 0; hp < height; hp++)
        {
            int index = subGrid[xp][hp];
            if (index > 0 && index <= curPSize)
            {
                curManip = &custParticles->at(index - 1);

                curManip->SetColor(0, 254, 0);
                startX = xp;
                startY = hp;
                startIndex = index;
                breakDa = true;
                break;
            }
        }
        if (breakDa)
            break;
    }

    int lastDir = 0;
    int lastMoveDir = 0;
    int area = (width * height) * 4;
    int step = 0;

    vertices.clear();

    int smallestX = 999999;
    int largestX = -999999;

    int smallestY = 999999;
    int largestY = -999999;

    while (step < area)
    {
        step += 1;
        int newX = startX + static_cast<int>(direction[lastDir].x);
        int newY = startY + static_cast<int>(direction[lastDir].y);

        if (!BoundX(newX) || !BoundY(newY))
        {
            lastDir += 1;

            if (lastDir > 7)
                lastDir = 0;
            continue;
        }

        int index = subGrid[newX][newY];
        if (index > 0 && index <= curPSize)
        {
            if (index == startIndex)
                break;

            curManip->SetColor(0, 254, 0);

            if (lastMoveDir != lastDir )
            {
                if (bSpecial)
                {
                    vertices.push_back(b2Vec2(curManip->subX, curManip->subY));
                }
                else
                {
                    vertices.push_back(b2Vec2((curManip->x - pos.x), (curManip->y - pos.y)));
                }

            }

            curManip = &custParticles->at(index - 1);

            lastMoveDir = lastDir;

            lastDir -= 2;

            startX = newX;
            startY = newY;


            if (smallestX > newX)
                smallestX = newX;

            if (largestX < newX)
                largestX = newX;

            if (smallestY > newY)
                smallestY = newY;

            if (largestY < newY)
                largestY = newY;

            if (lastDir < 0)
                lastDir += 7;
        }
        else
            lastDir += 1;

        if (lastDir > 7)
            lastDir = 0;
    }

    MassX = smallestX + ((largestX - smallestX) / 2);
    MassY = smallestY + ((largestY - smallestY) / 2);

    int totalX = 0, totalY = 0;

    int totalP = 0;

    for (int xp = 0; xp < width; xp++)
    {
        for (int hp = 0; hp < height; hp++)
        {
            int index = subGrid[xp][hp];
            if (index)
            {
                totalX += xp - MassX;
                totalY += hp - MassY;
                totalP += 1;
            }
            else
                continue;
        }
    }

    MassY += totalY / totalP;
    MassX += totalX / totalP;

    b2BodyDef bd;
    if (bSpecial)
    {
        bd.enabled = false;
        bd.type = b2_staticBody;
    }
    else
        bd.type = b2_dynamicBody;

    bd.position.Set(pos.x, pos.y);

    objectBody = I::worldBase->physWorld->CreateBody(&bd);

    b2PolygonShape Shape;
    b2Vec2 verts[99];

    int i = 0;
    for (b2Vec2& vert : vertices)
    {
        verts[i] = vert;
        i += 1;
    }

    Shape.Set(verts, i);

    b2FixtureDef fd;
    fd.density = 1.0f;

    fd.friction = 0.2f;
    fd.restitution = 0.15f;

    fd.shape = &Shape;
    objectBody->CreateFixture(&fd);

//    MassY = objectBody->GetMassData().center.y;
//    MassX = objectBody->GetMassData().center.x;

    BoundY(MassY);
    BoundX(MassX);

    oldCentPos = objectBody->GetWorldCenter();

    objectBody->SetTransform({ static_cast<float>(pos.x), static_cast<float>(pos.y) }, Math::DegToRad(ang));
    objectBody->SetAngularVelocity(angv);
    objectBody->SetLinearVelocity({ vel.x, vel.y });

    if (bSpecial)
    {
        pos = objectBody->GetPosition() - b2Vec2(MassX, MassY);
    }
    else
    {
        pos = objectBody->GetPosition();
    }


 //   isLoadedNew = false;
//    b2Vec2 CentPos = objectBody->GetLocalCenter();
//    MassY = CentPos.y;
//    MassX = CentPos.x;

 //   objectBody->SetLinearVelocity(b2Vec2(0.5f, 0.0f));

}

bool ParticleObject::BoundX(int& xp)
{
    if (xp >= width)
    {
        xp = width - 1;
        return false;
    }
    else if (xp < 0)
    {
        xp = 0;
        return false;
    }
    return true;
}

bool ParticleObject::BoundY(int& yp)
{
    if (yp >= height)
    {
        yp = height - 1;
        return false;
    }
    else if (yp < 0)
    {
        yp = 0;
        return false;
    }
    return true;
}


/*
void ParticleFormation::OverridePixelPos(Particle& manip, int x, int y)
{
    if (manip.x == x && manip.y == y)
        return;

    manip.BoundX(x);
    manip.BoundY(y);

    int index = mainGrid[x][y];
    
    if (!index || ((index != 0) ? Particles->at(index - 1).formationIndex : false))
    {
        manip.prevx = manip.x;
        manip.prevy = manip.y;

        if (mainGrid[manip.prevx][manip.prevy] == manip.index)
            mainGrid[manip.prevx][manip.prevy] = 0;

        manip.x = x;
        manip.y = y;

        mainGrid[x][y] = manip.index;
    }
}

void ParticleFormation::SetTransform(double ang)
{
    angle = NormalizeAngle(ang);
    ang = angle;
    // int midX = width / 2;
    // int midY = height / 2;

    Vec2Df midPoint = { x + massX, y + massY };

    Vec2Df midLeft = ProjectPoint(midPoint, ang + 180.0, massX);
    Vec2Df bottomLeft = ProjectPoint(midLeft, ang - 90.0, massY);
    //   Vec2Df topLeft = ProjectPoint(midLeft, ang + 90.0, midY);
    //   Vec2Df bottomRight = ProjectPoint(bottomLeft, ang, width);

    float cosX = cos(DegToRad(ang)), sinX = sin(DegToRad(ang));
    float nY = NormalizeAngle(ang + 90.f);
    float cosY = cos(DegToRad(nY)), sinY = sin(DegToRad(nY));
    int lastY = -1;

    int curPSize = Particles->size();

    for (int hp = 0; hp < height; hp++)
    {
        Vec2Df newY = { bottomLeft.x + cosY * hp, bottomLeft.y + sinY * hp };
        int oldX = 0, oldY = 0;
        int oldIndex = 0;
        for (int xp = 0; xp < width; xp++)
        {
            int index = submainGrid[xp][hp];
            if (index > 0 && index <= curPSize)
            {
                Vec2Df newX = { newY.x + cosX * xp, newY.y + sinX * xp };

                if (lastY != hp || !oldIndex)
                {
                    oldX = newX.x;
                    oldY = newX.y;
                    lastY = hp;
                }

                Particle& manip = Particles->at(index - 1);

                int newXDelta = static_cast<int>(newX.x) - oldX;
                int newYDelta = static_cast<int>(newX.y) - oldY;


             //   if (xp != 0 && abs(newXDelta) > 0 && abs(newYDelta) > 0) // this fixes the dithering, not very efficient but it works
             //   {
                //    CustRender::drawRect(static_cast<int>(newX.x) * particleSize, static_cast<int>(newX.y - newYDelta) * particleSize, particleSize, particleSize, manip.clr);
             //   }

                //    CustRender::drawRect(static_cast<int>(newX.x) * particleSize, static_cast<int>(newX.y) * particleSize, particleSize, particleSize, manip.clr);
                OverridePixelPos(manip, newX.x, newX.y);
            //    CustRender::drawRect(static_cast<int>(manip.x) * particleSize, static_cast<int>(manip.y) * particleSize, particleSize, particleSize, manip.clr);
                oldX = newX.x;
                oldY = newX.y;
            }
            oldIndex = index;
        }
    }

    Vec2Df newY = { bottomLeft.x + cosY * massY, bottomLeft.y + sinY * massY };
    Vec2Df newX = { newY.x + cosX * massX, newY.y + sinX * massX };

    CustRender::drawRect(static_cast<int>(newX.x) * particleSize, static_cast<int>(newX.y) * particleSize, particleSize, particleSize, ColorM(1, 0, 0));

    debugInfo->SetString("xv:"+std::to_string(xv) + " yv:" + std::to_string(yv) + " angv:" + std::to_string(anglev));
    debugInfo->SetPos(newX.x * particleSize, newX.y * particleSize);
    /*
        float xMoves1 = bottomRight.x - bottomLeft.x;
        float yMoves1 = bottomRight.y - bottomLeft.y;

        int moveSumX = abs(xMoves1) + abs(yMoves1);

        float xStep1 = xMoves1 / static_cast<float>(moveSumX);
        float yStep1 = yMoves1 / static_cast<float>(moveSumX);

        float xSubIndexScale = static_cast<float>(width) / static_cast<float>(moveSumX);

        float xMoves2 = topLeft.x - bottomLeft.x;
        float yMoves2 = topLeft.y - bottomLeft.y;

        int moveSumY = abs(xMoves2) + abs(yMoves2);

        float xStep2 = xMoves2 / static_cast<float>(moveSumY);
        float yStep2 = yMoves2 / static_cast<float>(moveSumY);

        float ySubIndexScale = static_cast<float>(height) / static_cast<float>(moveSumY);

        int prevX = 0;
        int prevY = 0;

        for (int xp = 0; xp < moveSumX; xp++)
        {
            int curXIndex = static_cast<float>(xp) * xSubIndexScale;

            int curXorgx = bottomLeft.x + (xStep1 * static_cast<float>(xp));
            int curXorgy = bottomLeft.y + (yStep1 * static_cast<float>(xp));

            int oldX = 0, oldY = 0;
            for (int hp = 0; hp < moveSumY; hp++)
            {
                int curYIndex = static_cast<float>(hp) * ySubIndexScale;

                int index = submainGrid[curXIndex][curYIndex];
                if (index)
                {
                    int curYorgx = curXorgx + ((xStep2 * static_cast<float>(hp)) + .5f);
                    int curYorgy = curXorgy + ((yStep2 * static_cast<float>(hp)) + .5f);

                    Particle& manip = Particles[index - 1];

                    int newXDelta = curYorgx - oldX;
                    int newYDelta = curYorgy - oldY;

                    if (abs(newXDelta) > 0 && abs(newYDelta) > 0)
                    {
                        CustRender::drawRect((curYorgx ) * particleSize, (curYorgy - newYDelta) * particleSize, particleSize, particleSize, ColorM(1.f, 1.f, 1.f));
                    }

                    OverridePixelPos(manip, curYorgx, curYorgy);

                    CustRender::drawRect(curYorgx * particleSize, curYorgy * particleSize, particleSize, particleSize, ColorM(1.f, 1.f, 1.f));
                    oldX = curYorgx;
                    oldY = curYorgy;
                }

            }
        }
        */
/*
}

bool ParticleFormation::BoundX(int& xp)
{
    if (xp >= width)
    {
        xp = width - 1;
        return false;
    }
    else if (xp < 0)
    {
        xp = 0;
        return false;
    }
    return true;
}

bool ParticleFormation::BoundY(int& yp)
{
    if (yp >= height)
    {
        yp = height - 1;
        return false;
    }
    else if (yp < 0)
    {
        yp = 0;
        return false;
    }
    return true;
}

static Vec2Df direction[4] = { {0, -1}, {-1, 0}, {0, 1}, {1, 0} };
void ParticleFormation::calcBoundry()
{
    int startIndex = 0;
    int startX = 0;
    int startY = 0;
    bool breakDa = false;
    int curPSize = Particles->size();
    for (int xp = 0; xp < width; xp++)
    {
        for (int hp = 0; hp < height; hp++)
        {
            int index = submainGrid[xp][hp];
            if (index > 0 && index <= curPSize)
            {
                Particle& manip = Particles->at(index - 1);

                manip.clr.SetColor(0.f, 1.f, 0.f);
                startX = xp;
                startY = hp;
                startIndex = index;
                breakDa = true;
                break;
            }
        }
        if (breakDa)
            break;
    }

    int lastDir = 0;
    int area = (width * height) * 4;
    int step = 0;

    boundryIndexes.clear();

    int smallestX = 999999;
    int largestX = -999999;

    int smallestY = 999999;
    int largestY = -999999;

    while (step < area)
    {
        step += 1;
        int newX = startX + static_cast<int>(direction[lastDir].x);
        int newY = startY + static_cast<int>(direction[lastDir].y);

        if (!BoundX(newX) || !BoundY(newY))
        {
            lastDir += 1;

            if (lastDir > 3)
                lastDir = 0;
            continue;
        }

        int index = submainGrid[newX][newY];
        if (index > 0 && index <= curPSize)
        {
            if (index == startIndex)
                break;

            Particle& manip = Particles->at(index - 1);

            manip.clr.SetColor(0.f, 1.f, 0.f);

            lastDir -= 1;

            startX = newX;
            startY = newY;


            if (smallestX > newX)
                smallestX = newX;

            if (largestX < newX)
                largestX = newX;

            if (smallestY > newY)
                smallestY = newY;

            if (largestY < newY)
                largestY = newY;

            if (lastDir < 0)
                lastDir = 3;

            boundryIndexes.push_back(index);
        }
        else
            lastDir += 1;

        if (lastDir > 3)
            lastDir = 0;
    }

    midX = smallestX + ((largestX - smallestX) / 2);
    midY = smallestY + ((largestY - smallestY) / 2);

    massX = midX;
    massY = midY;

    if (submainGrid[midX][midY])
    {
        Particle& manip = Particles->at(submainGrid[midX][midY] - 1);
        manip.clr.SetColor(0, 1, 0);
    }

    int totalX = 0, totalY = 0;

    int totalP = 0;

    for (int xp = 0; xp < width; xp++)
    {
        for (int hp = 0; hp < height; hp++)
        {
            int index = submainGrid[xp][hp];
            if (index)
            {
                totalX += xp - midX;
                totalY += hp - midY;
                totalP += 1;
            }
            else
                continue;
        }
    }

    massY = midY + (totalY / totalP);
    massX = midX + (totalX / totalP);

    BoundX(massX);
    BoundY(massY);

    if (submainGrid[massX][massY])
    {
        Particle& manip = Particles->at(submainGrid[massX][massY] - 1);
        manip.clr.SetColor(0, 0, 1);
    }
}

void ParticleFormation::PhysThink()
{
    int xp = RoundToInt(x + xv);
    int yp = RoundToInt(y + yv);
    double ang = NormalizeAngle(angle + anglev);
    if (x == xp && y == yp && ang == angle)
        return;

    //   BoundX(xp);
    //    BoundY(yp);

    //    int midX = width / 2;
    //    int midY = height / 2;
    Vec2Df midPoint = { x + massX, y + massY };
    Vec2Df destP = { xp + massX, yp + massY };

    float xMoves = destP.x - midPoint.x;
    float yMoves = destP.y - midPoint.y;

    float angMoves = NormalizeAngle(ang - angle);

    int moveSum = abs(xMoves) + abs(yMoves) + abs(angMoves);

//    if (moveSum > 30)
//        moveSum = 30;

    //   if (moveSum < 10)
    //       moveSum = 10;

    float angSim = ang;
    float angStep = angMoves / static_cast<float>(moveSum);

    float clrStep = 1.f / static_cast<float>(moveSum);

    float xStep = xMoves / static_cast<float>(moveSum);
    float yStep = yMoves / static_cast<float>(moveSum);

    destP = { x, y };

    int oldX = 0;
    int oldY = 0;
    float oldAng = angle;

    float oldCosX = 0.f, oldCosY = 0.f, oldSinX = 0.f, oldSinY = 0.f;

    int oldPosX = 0, oldPosY = 0;

    std::vector<Vec2Df> OldPos;
    OldPos.clear();
    OldPos.resize(boundryIndexes.size());

    int curPSize = Particles->size();

    for (int i = 0; i < boundryIndexes.size(); i++)
    {
        int index = boundryIndexes[i];
        Particle& manip = Particles->at(index - 1);
        OldPos[i] = { manip.x, manip.y };
    }

    Vec2Df oldCentMass = getCentMassPos();

    for (int i = 1; i <= moveSum; i++)// steps through each simulated position for each move tick
    {
        int moveX = (xStep * static_cast<float>(i)); // the next simulated position for the x axis
        int moveY = (yStep * static_cast<float>(i)); // the next simulated position for the y axis
        angSim = NormalizeAngle(angle + (angStep * static_cast<float>(i))); // the next simulated angle

        float oldmidpoinX = midPoint.x;
        float oldmidpointY = midPoint.y;

        midPoint.x = x + massX + moveX;
        midPoint.y = y + massY + moveY;

        Vec2Df midLeft = ProjectPoint(midPoint, angSim + 180.0, massX); // the left side of the object
        Vec2Df bottomLeft = ProjectPoint(midLeft, angSim - 90.0, massY); // the bottom left of the object

        float cosX = cos(DegToRad(angSim)), sinX = sin(DegToRad(angSim));
        float nY = NormalizeAngle(angSim + 90.f);
        float cosY = cos(DegToRad(nY)), sinY = sin(DegToRad(nY));

        Vec2Df centMassY = { bottomLeft.x + cosY * massY, bottomLeft.y + sinY * massY };
        Vec2Df centMassPos = { centMassY.x + cosX * massX, centMassY.y + sinX * massX };

        int collided = 0;
        //    std::vector<Vec2Df> oldPoss(); 
        //    oldPoss().clear();

        /*
        get the average position of all the collisions
        then calculate the collision physics
        */
/* -------------------------------------------------------------------------------------------------------------------------------------------
        std::vector<CollisionInfo> collisions;
        collisions.clear();

        int prevTargForm = -1;
        int currentCollisionIndex = -1;

        for (int ii = 0; ii < boundryIndexes.size(); ii++)
        {
            int index = boundryIndexes[ii];
            Particle& manip = Particles->at(index - 1);

            Vec2Df newY = { bottomLeft.x + cosY * manip.subY, bottomLeft.y + sinY * manip.subY };
            Vec2Df simBoundryPos = { newY.x + cosX * manip.subX, newY.y + sinX * manip.subX }; // the simulated position of where this specific boundry particle will be

            debugDraw.push_back(simBoundryPos);

            int targIndex = mainGrid[manip.RetBoundX(static_cast<int>(simBoundryPos.x))][manip.RetBoundY(static_cast<int>(simBoundryPos.y))];

            int formIndex = 0;

            if (targIndex > 0 && targIndex <= curPSize)
            {
                Particle& targManip = Particles->at(targIndex - 1);

                formIndex = targManip.formationIndex;

                if (formIndex != 0 && formIndex != manip.formationIndex) // collided with another particle formation
                {
                    manip.clr.SetColor(1.f, 0.f, 0.f);

                    collided = 1;

                    if (prevTargForm != formIndex)
                    {
                        currentCollisionIndex += 1;
                        collisions.push_back({ formIndex, 1,  OldPos[ii] });

                        if (moveX == oldX && oldY == moveY)
                            collisions[currentCollisionIndex].collidedBecRotation = true;
                        else if (moveX == oldX)
                            collisions[currentCollisionIndex].collidedBecYMove = true;
                        else if (moveY == oldY)
                            collisions[currentCollisionIndex].collidedBecXMove = true;
                    }
                    else
                    {
                        collisions[currentCollisionIndex].hitPos += OldPos[ii];
                        collisions[currentCollisionIndex].numOfHits += 1;
                    }

                    /* Vec2Df midLeft2 = ProjectPoint({oldmidpoinX, oldmidpointY}, angSim + 180.0, manip.subX);
                       Vec2Df bottomLeft2 = ProjectPoint(midLeft2, angSim - 90.0, manip.subY);
                       Vec2Df curpxY = { bottomLeft2.x + oldCosY * manip.subY, bottomLeft2.y + oldSinY * manip.subY };
                       Vec2Df CurPX = { centMassY.x + oldCosX * manip.subX, centMassY.y + oldSinX * manip.subX };*/
                       /* -------------------------------------------------------------------------------------------------------------------------------------------
                 /*   int particleDirX = xv;//simBoundryPos.x - OldPos[ii].x;
                        int particleDirY = yv;//simBoundryPos.y - OldPos[ii].y;

                        int targParticleDirX = targForm.xv;//targManip.x - targManip.prevx;
                        int targParticleDirY = targForm.yv;//targManip.y - targManip.prevy;

                    Vec2Df targCentMass = targForm.getCentMassPos();

                    Vec2Df targPPos(targManip.x, targManip.y);

                    float targMassToTargpAng = CalcAngle(targCentMass, targPPos);

                    Vec2Df predParticlePos(targPPos.x + targParticleDirX, targPPos.y + targParticleDirY);

                    float targMassToPredParticle = CalcAngle(targCentMass, predParticlePos);

                    float angDelta = NormalizeAngle(targMassToPredParticle - targMassToTargpAng);

                    if (angDelta > 10)
                        angDelta = 10;
                    if (angDelta < -10)
                        angDelta = -10;

                    targForm.anglev -= angDelta;// * .7f;

                    predParticlePos = { simBoundryPos.x + particleDirX, simBoundryPos.y + particleDirY };

                    float distDelta = CalcDistance(targCentMass, predParticlePos) - CalcDistance(targCentMass, targPPos);

                    Vec2Df newVel = ProjectPoint(Vec2Df(0, 0), targMassToTargpAng, distDelta);

                    targForm.xv += newVel.x;// * .7f;
                    targForm.yv += newVel.y;// * .7f;


                    targPPos = { OldPos[ii].x, OldPos[ii].y };

                    targMassToTargpAng = CalcAngle(centMassPos, targPPos);

                    predParticlePos = { simBoundryPos.x + particleDirX, simBoundryPos.y + particleDirY };

                    targMassToPredParticle = CalcAngle(centMassPos, simBoundryPos);

                    angDelta = NormalizeAngle(targMassToPredParticle - targMassToTargpAng);

                    if (angDelta > 10)
                        angDelta = 10;
                    if (angDelta < -10)
                        angDelta = -10;

                    anglev = -angDelta;// *.7f;

                    predParticlePos = { targPPos.x + targParticleDirX, targPPos.y + targParticleDirY };

                    distDelta = CalcDistance(centMassPos, predParticlePos) - CalcDistance(centMassPos, targPPos);

                    newVel = ProjectPoint(Vec2Df(0, 0), targMassToTargpAng, distDelta);

                    xv = newVel.x;// * .7f;
                    yv = newVel.y;// * .7f;

                    // the scalar can be replaced with the particles elasticity

                 //   x += oldX;
                 //   y += oldY;
                  

                    float hitParticleToCentMassAng = CalcAngle(OldPos[ii], centMassPos);
                    Vec2Df centMassPlusVelocity = centMassPos + Vec2Df(xv, yv);
                    float hitParticleToPredCentMassAng = CalcAngle(OldPos[ii], centMassPlusVelocity);
                    
                    float centMassDist = fabs(CalcDistance(simBoundryPos, centMassPos));

                    Vec2Df newSimCentMassPos = ProjectPoint(OldPos[ii], hitParticleToPredCentMassAng, centMassDist) + Vec2Df(targForm.xv, targForm.yv);

                    Vec2Df deltaDistanceBetweenSimCentMassAndCurrentCentMass = newSimCentMassPos - centMassPlusVelocity;

                //    Vec2Df newVelocitySim

                    xv += deltaDistanceBetweenSimCentMassAndCurrentCentMass.x ;// * .7f;
                    yv += deltaDistanceBetweenSimCentMassAndCurrentCentMass.y ;// * .7f;

                //    targForm.xv -= deltaDistanceBetweenSimCentMassAndCurrentCentMass.x;
                 //   targForm.yv -= deltaDistanceBetweenSimCentMassAndCurrentCentMass.y;

                    float newCentMassPosAngDelta = NormalizeAngle(hitParticleToPredCentMassAng - hitParticleToCentMassAng);
                    anglev = newCentMassPosAngDelta;

                    // sets the position to 1 simulation tick before this one


                    if (oldX == 0 && oldY == 0)
                        break;
                    else
                    {
                        x += oldX;
                        y += oldY;
                    }

                    SetTransform(oldAng);
                    collided = 1;
                    return; *//* -------------------------------------------------------------------------------------------------------------------------------------------
                }
           /*    else if (formIndex == 0)// hit a normal pixel
                {

                    Vec2Df centMassY = { bottomLeft.x + cosY * massY, bottomLeft.y + sinY * massY };
                    Vec2Df centMassPos = { centMassY.x + cosX * massX, centMassY.y + sinX * massX };

                    Vec2Df targPPos = { manip.x, manip.y };

                    float targMassToTargpAng = CalcAngle(centMassPos, targPPos);

                    int particleDirX = manip.x - manip.prevx;
                    int particleDirY = manip.y - manip.prevy;

                 //   targManip.vx += particleDirX;
                  //  targManip.vy += particleDirY;

                    Vec2Df predParticlePos = { targPPos.x - particleDirX, targPPos.y - particleDirY };

                    float targMassToPredParticle = CalcAngle(centMassPos, predParticlePos);

                    float angDelta = NormalizeAngle(targMassToPredParticle - targMassToTargpAng);

                    anglev = angDelta; //* .75f;

                    float distDelta = CalcDistance(centMassPos, predParticlePos) - CalcDistance(centMassPos, targPPos);

                    Vec2Df newVel = ProjectPoint(Vec2Df(0, 0), targMassToTargpAng, distDelta);

                    xv = newVel.x * .75f;
                    yv = newVel.y * .75f;


                    x += oldX;
                    y += oldY;

                    SetTransform(oldAng);

                    collided = 2;
                    return;
                }*//* -------------------------------------------------------------------------------------------------------------------------------------------

                //   oldPoss().push_back(newX);
            }


            prevTargForm = formIndex;
            OldPos[ii] = Vec2Df(manip.RetBoundX(static_cast<int>(simBoundryPos.x)), manip.RetBoundY(static_cast<int>(simBoundryPos.y)));
            //    CustRender::drawRect(static_cast<int>(newX.x) * particleSize, static_cast<int>(newX.y) * particleSize, particleSize, particleSize, ColorM().FromHSV(static_cast<float>(i) * clrStep, 1.f, 1.f));
        }

        /*
        i need to make a way for the object to use its rotation momentum to push it in a direction it gets stuck when it keeps trying to rotate
        itself into a wall

       
        *//* -------------------------------------------------------------------------------------------------------------------------------------------


        for (CollisionInfo& info : collisions)
        {
            info.hitPos /= static_cast<float>(info.numOfHits);
            ParticleFormation& targForm = Formations[info.targIndex - 1];

            float hitParticleToCentMassAng = CalcAngle(info.hitPos, oldCentMass);
            Vec2Df centMassPlusVelocity = oldCentMass + Vec2Df(xv, yv);
            float hitParticleToPredCentMassAng = CalcAngle(info.hitPos, centMassPlusVelocity);

            float centMassDist = fabs(CalcDistance(info.hitPos, oldCentMass));

            Vec2Df newSimCentMassPos = ProjectPoint(info.hitPos, hitParticleToPredCentMassAng, centMassDist) + Vec2Df(targForm.xv, targForm.yv);

            Vec2Df deltaDistanceBetweenSimCentMassAndCurrentCentMass = newSimCentMassPos - centMassPlusVelocity;

            //    Vec2Df newVelocitySim

            xv += deltaDistanceBetweenSimCentMassAndCurrentCentMass.x;// * .7f;
            yv += deltaDistanceBetweenSimCentMassAndCurrentCentMass.y;// * .7f;

            targForm.xv -= deltaDistanceBetweenSimCentMassAndCurrentCentMass.x;
            targForm.yv -= deltaDistanceBetweenSimCentMassAndCurrentCentMass.y;

            float newCentMassPosAngDelta = NormalizeAngle(hitParticleToPredCentMassAng - hitParticleToCentMassAng);

            anglev = newCentMassPosAngDelta;

            // just for testing
            if (collisions[currentCollisionIndex].collidedBecYMove)
            {
                oldY = -oldY;
            //    yv = 0;
            }
                
            if (collisions[currentCollisionIndex].collidedBecXMove)
            {
                oldX = -oldX;
            //    xv = 0;
            }
                
            if (collisions[currentCollisionIndex].collidedBecRotation)
            {
                oldAng = angle;
                anglev = 0;
            }
              

        //    if (info.collidedBecRotation)
        //        anglev -= newCentMassPosAngDelta;
        //    else

            /*  targIndex = mainGrid[manip.RetBoundX(RoundToInt(simBoundryPos.x + xv))][manip.RetBoundY(RoundToInt(simBoundryPos.y + yv))];

               if (targIndex == 0)
                   continue;
               else
               {
                   Particle& newTargManip = Particles->at(targIndex - 1);

                   formIndex = newTargManip.formationIndex;
                   if (formIndex == manip.formationIndex)
                   {
                       continue;
                   }

               }*//* -------------------------------------------------------------------------------------------------------------------------------------------

               // sets the position to 1 simulation tick before this one

        //    targForm.PhysThink();
        }

        oldCosX = cosX;
        oldCosY = cosY;

        oldSinX = sinX;
        oldSinY = sinY;

    //    if (collided != 0)
    //        break;

        if (i == moveSum && collided == 0)
        {
            x += moveX;
            y += moveY;
            SetTransform(angSim);
            return;
        }
        else if (collided != 0)
        {
            x += oldX;
            y += oldY;
            SetTransform(oldAng);
            return;
        }

        oldCentMass = centMassPos;

        oldX = moveX;
        oldY = moveY;

        oldAng = angSim;
        //     destP = { xSim, ySim };
    }

}

Vec2Df ParticleFormation::getCentMassPos()
{
    Vec2Df midPoint = { x + massX, y + massY };
    Vec2Df midLeft = ProjectPoint(midPoint, angle + 180.0, massX);
    Vec2Df bottomLeft = ProjectPoint(midLeft, angle - 90.0, massY);

    float cosX = cos(DegToRad(angle)), sinX = sin(DegToRad(angle));
    float nY = NormalizeAngle(angle + 90.f);
    float cosY = cos(DegToRad(nY)), sinY = sin(DegToRad(nY));

    Vec2Df newY = { bottomLeft.x + cosY * massY, bottomLeft.y + sinY * massY };
    Vec2Df newX = { newY.x + cosX * massX, newY.y + sinX * massX };

    return newX;
}*/