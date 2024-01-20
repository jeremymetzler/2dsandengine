#include "header_stack.h"

bool PhysWorldBase::BoundX(int& x)
{
    if (x >= I::baseClass->particleResolution3X[0])
    {
        x = I::baseClass->particleResolution3X[0] - 1;
        return false;
    }
    else if (x < 0)
    {
        x = 0;
        return false;
    }

    return true;
}

bool PhysWorldBase::BoundY(int& y)
{
    if (y >= I::baseClass->particleResolution3X[1])
    {
        y = I::baseClass->particleResolution3X[1] - 1;
        return false;
    }
    else if (y < 0)
    {
        y = 0;
        return false;
    }

    return true;
}

void PhysWorldBase::DebugRender()
{
    b2Body* PhysWorldBodies = physWorld->GetBodyList();

    //    b2Body* nexBody = PhysWorldBodies->GetNext();
    int ChainCount = 0;
    ColorM DebugClr = ColorM();
    while (PhysWorldBodies) // debug drawing for physics objects
    {
        //   CustRender::drawRect(nexBody->GetPosition().x, nexBody->GetPosition().y, 10, 10, ColorM(1, 1, 0));
        const b2Transform& xf = PhysWorldBodies->GetTransform();
        for (b2Fixture* f = PhysWorldBodies->GetFixtureList(); f; f = f->GetNext())
        {
            switch (f->GetType())
            {
            case 0: // circle
            {
                b2CircleShape* circle = (b2CircleShape*)f->GetShape();

                int vertextCount = (circle->m_radius /* * 2.f  not including this for slight performace increase*/) * PI;

                int stepSize = 360.f / vertextCount;

                glColor3f(1, 1, 1);
                glBegin(GL_POLYGON);

                Vec2Df cPos = { PhysWorldBodies->GetPosition().x + circle->m_p.x, PhysWorldBodies->GetPosition().y + circle->m_p.y };
                for (int i = 0; i < 359; i += stepSize)
                {
                    Vec2Df vPos = Math::ProjectPoint(cPos, Math::NormalizeAngle(i), circle->m_radius);
                    glVertex2d(vPos.x, vPos.y);
                }

                glEnd();


                glColor3f(0, 0, 0);
                glBegin(GL_LINES);
                glVertex2d(cPos.x, cPos.y);
                Vec2Df vPos = Math::ProjectPoint(cPos, Math::NormalizeAngle(Math::RadToDeg(PhysWorldBodies->GetAngle())), circle->m_radius);
                glVertex2d(vPos.x, vPos.y);
                glEnd();

                break;
            }
            case 1: // edge
            {
                b2EdgeShape* edge = (b2EdgeShape*)f->GetShape();

                glColor3f(1, 1, 1);
                glBegin(GL_LINES);

                b2Vec2 vec0 = b2Mul(xf, edge->m_vertex1);
                glVertex2d(vec0.x, vec0.y);
                b2Vec2 vec1 = b2Mul(xf, edge->m_vertex2);
                glVertex2d(vec1.x, vec1.y);

                glEnd();

                break;
            }
            case 2: // polygon 
            {
                b2PolygonShape* poly = (b2PolygonShape*)f->GetShape();
                int32 vertexCount = poly->m_count;
                //    b2Vec2 vertices[b2_maxPolygonVertices];


                glColor3f(1, 1, 1);
                glBegin(GL_POLYGON);

                for (int32 i = 0; i < vertexCount; ++i)
                {
                    b2Vec2 vPos = b2Mul(xf, poly->m_vertices[i]);
                    //    vPos *= 2; 
                    glVertex2d(vPos.x, vPos.y);
                }

                glEnd();

                break;
            }
            case 3: // chain
            {
                b2ChainShape* chain = (b2ChainShape*)f->GetShape();
                int32 vertexCount = chain->m_count;
                //    b2Vec2 vertices[b2_maxPolygonVertices];
                ChainCount += 1;
                DebugClr.FromHSV(static_cast<float>(ChainCount % 10) / 10.f, 1, 1);

                glColor3f(DebugClr.r, DebugClr.g, DebugClr.b);
                glBegin(GL_LINE_STRIP);

                for (int32 i = 0; i < vertexCount; ++i)
                {
                    b2Vec2 vPos = b2Mul(xf, chain->m_vertices[i]);
                    //    vPos *= 2; 
                    glVertex2d(vPos.x, vPos.y);
                }

                glEnd();

                break;
            }
            }

        }

        PhysWorldBodies = PhysWorldBodies->GetNext();
    }
}


static Vec2Df direction[8] = { {0, -1}/*bottom*/, {-1, -1}/*btmL*/, {-1, 0}/*left*/, {-1, 1}/*topLeft*/, {0, 1}/*top*/, {1, 1}/*topRight*/, {1, 0}/*right*/, {1, -1}/*btmRight*/ };
void PhysWorldBase::BuildWorldGroups()
{
    curGroupSize = 0;

    for (int x = 0; x < I::baseClass->particleResolution3X[0]; x++)
        for (int y = 0; y < I::baseClass->particleResolution3X[1]; y++)
            particleGroups[x][y] = 0;

    for (int i = 0; i < 4000; i++)
    {
        checkPosBuffer1[i].Clear();
        checkPosBuffer2[i].Clear();
    }

    Vec2Di* currPosBuffer = checkPosBuffer1;
    int* currPosSize = &posSize1;
    Vec2Di* oppoPosBuffer = checkPosBuffer2;
    int* currOpoSize = &posSize2;

    auto CheckSurrounding = [&](int x, int y, int currGroup)
    {
        Vec2Di checkPos;

        int& mainGroupIndex = particleGroups[x][y];

        bool hasOpen = false;

        for (int i = 0; i < 8; i += 2)
        {
            checkPos.x = x;
            checkPos.y = y;

            checkPos += direction[i];

            bool BX = BoundX(checkPos.x);
            bool BY = BoundY(checkPos.y);

            int index = I::chunkBase->mainClump[checkPos.x][checkPos.y];
            int& groupIndex = particleGroups[checkPos.x][checkPos.y];
            if (index != 0 && BX && BY)
            {
                if (abs(groupIndex) != currGroup)
                {
                    oppoPosBuffer[*currOpoSize] = checkPos;
                    *currOpoSize += 1;
                    groupIndex = currGroup;
                }
            }
            else
                hasOpen = true;
        }

        if (hasOpen)
            mainGroupIndex *= -1;
    };
    
    auto SwitchBuffer = [&]()
    {
        if (curBuffer)
        {
            currPosBuffer = checkPosBuffer1;
            currPosSize = &posSize1;
            oppoPosBuffer = checkPosBuffer2;
            currOpoSize = &posSize2;

            *currOpoSize = 0;
            for (int i = 0; i < 2000; i++)
            {
                oppoPosBuffer[i].Clear();
            }

            curBuffer = false;
        }
        else
        {
            currPosBuffer = checkPosBuffer2;
            currPosSize = &posSize2;
            oppoPosBuffer = checkPosBuffer1;
            currOpoSize = &posSize1;

            *currOpoSize = 0;
            for (int i = 0; i < 2000; i++)
            {
                oppoPosBuffer[i].Clear();
            }

            curBuffer = true;
        }
    };

findNextGroup:
    bool breakDa = false;
    for (int xp = 0; xp < I::baseClass->particleResolution3X[0]; xp++)
    {
        for (int hp = 0; hp < I::baseClass->particleResolution3X[1]; hp++)
        {
            int curGroupIndex = particleGroups[xp][hp];
            if (curGroupIndex == 0)
            {
                int index = I::chunkBase->mainClump[xp][hp];
                if (index != 0)
                {
                    curGroupSize += 1;
                    particleGroups[xp][hp] = curGroupSize;

                    CheckSurrounding(xp, hp, curGroupSize);
                    SwitchBuffer();

                    breakDa = true;
                    break;
                }
            }

        }
        if (breakDa)
            break;
    }

    if (!breakDa)
        return;

findNext:
    for (int i = 0; i < *currPosSize; i++)
    {
        Vec2Di& pos = currPosBuffer[i];
        CheckSurrounding(pos.x, pos.y, curGroupSize);
    }

    SwitchBuffer();

    if (*currPosSize != 0)
        goto findNext;
    else
        goto findNextGroup;
}


void PhysWorldBase::BuildWorldCollision()
{
    for (int i = 0; i < terrainBodySize; i++)
    {
        if (!terrainBodies[i])
            continue;

        physWorld->DestroyBody(terrainBodies[i]);
        terrainBodies[i] = nullptr;
    }
    terrainBodySize = 0;

    curGroupSize = 0;

    for (int x = 0; x < I::baseClass->particleResolution3X[0]; x++)
        for (int y = 0; y < I::baseClass->particleResolution3X[1]; y++)
            particleGroups[x][y] = 0;

    int lastX = 0;
    int lastY = 0;


findNext:
    for (int i = 0; i < maxCheckBufferSize; i++)
    {
        checkPosBuffer1[i].Clear();
        //       checkPosBuffer2[i].Clear();
    }
    posSize1 = 0;

    int startIndex = 0;
    int startX = 0;
    int startY = 0;

    int curPSize = *I::particleBase->pSize;

    bool breakDa = false;
    Vec2Di checkPos;
    checkPos.Clear();

    Particle* curManip = nullptr;

    int lastDir = 0;
    int cycleBack = 2;
    int lastMoveDir = 2;
    int area = 999999999;
    int step = 0;

    for (int xp = lastX; xp < I::baseClass->particleResolution3X[0]; xp++)
    {
        for (int hp = 0; hp < I::baseClass->particleResolution3X[1]; hp++)
        {
            if (lastY != 0)
            {
                hp += lastY + 1;
                lastY = 0;

                if (hp >= I::baseClass->particleResolution3X[1])
                    continue;       
            }

            int& curGroupIndex = particleGroups[xp][hp];
            if (curGroupIndex == 0)
            {
                int index = I::chunkBase->mainClump[xp][hp];

                if (index != 0)
                {
                    curManip = &I::particleBase->Particles->at(index - 1);

                    if (/*curManip->dormantTick > 2 &&*/ curManip->formationIndex == 0 && curManip->classid != WaterP && curManip->classid != SmokeP && curManip->classid != FireP)
                    {
                        char hasOpen = 0;
                        for (int i = 0; i < 8; i += 2)
                        {
                            checkPos.x = xp;
                            checkPos.y = hp;

                            checkPos += direction[i];

                            bool BX = BoundX(checkPos.x);
                            bool BY = BoundY(checkPos.y);

                            if (!BX || !BY || I::chunkBase->mainClump[checkPos.x][checkPos.y] == 0)
                            {
                                hasOpen += 1;
                                lastDir = i;
                                lastMoveDir = i;
                               
                                if (hasOpen > 2)
                                {
                                    hasOpen = 0;
                                    break;
                                }
                            }
                        }

                        if (hasOpen > 0)
                        {
                            curGroupSize += 1;
                        //    curGroupIndex = curGroupSize;

                            //   checkPosBuffer1[posSize1] = Vec2Di( curManip->x, curManip->y);
                            //   posSize1 += 1;

                            startX = xp;
                            startY = hp;

                            lastX = xp;
                            lastY = hp;

                            startIndex = index;

                            breakDa = true;
                            break;
                        }
                        else
                            curGroupIndex = -1;
                    }
                    else
                        curGroupIndex = -1;
                }
            }

        }
        if (breakDa)
            break;
    }

    if (!breakDa)
    {
        //   threads.clear();
        return;
    }

    bool looping = false;

    bool start = true;

    while (step < area)
    {
        step += 1;

        if (step == area)
        {
            std::cout << "HUUUGE LOOP\n";
            std::cout << "lastMoveDir " << lastMoveDir << " lastDir " << lastDir << " posSize1 " << posSize1 << std::endl;

            if (posSize1 < 50)
            {
                for (int i = 0; i < posSize1; i++)
                {
                    std::cout << checkPosBuffer1[i].x << " " << checkPosBuffer1[i].y << std::endl;
                }
            }

            looping = true;
            break;
        }

        int& iParticleGroup = particleGroups[startX][startY];

        int newX = startX + static_cast<int>(direction[lastDir].x);
        int newY = startY + static_cast<int>(direction[lastDir].y);


        if (!BoundX(newX) || !BoundY(newY))
        {
            lastDir += 1;

            if (lastDir > 7)
                lastDir = 0;

            int lastMoveBack = (lastMoveDir - cycleBack);

            if (lastMoveBack > 7)
                lastMoveBack -= 8;

            if (lastMoveBack < 0)
                lastMoveBack += 8;

            if (lastMoveBack == lastDir) // looping
            {
                looping = true;
                iParticleGroup = -1;
                break;
            }

            continue;
        }

        int& iSearchParticleGroup = particleGroups[newX][newY];
        int index = I::chunkBase->mainClump[newX][newY];
        bool foundStart = index == startIndex;
        if (index > 0 && index <= curPSize && (iSearchParticleGroup == curGroupSize  || iSearchParticleGroup == 0 || foundStart))
        {

            if (!foundStart)
            {
                Particle& manip = I::particleBase->Particles->at(index - 1);
                if (/*manip.dormantTick < 3 ||*/ manip.formationIndex != 0 || manip.classid == WaterP || manip.classid == SmokeP || manip.classid == FireP)
                {
                    lastDir += 1;

                    if (lastDir > 7)
                        lastDir = 0;

                    int lastMoveBack = (lastMoveDir - cycleBack);

                    if (lastMoveBack > 7)
                        lastMoveBack -= 8;

                    if (lastMoveBack < 0)
                        lastMoveBack += 8;

                    if (lastMoveBack == lastDir) // looping
                    {
                        iParticleGroup = -1;
                        looping = true;
                        break;
                    }

                    continue;
                }
            }

            if (iParticleGroup == 0)
                iParticleGroup = curGroupSize;

            if ((lastMoveDir != lastDir && (iSearchParticleGroup == 0 || foundStart)) || start)
            {         
                if (posSize1 + 1 >= maxCheckBufferSize)
                {
                    looping = true;
                    iParticleGroup = -1;
                    break;
                }

                checkPosBuffer1[posSize1] = Vec2Di(curManip->x, curManip->y);
                posSize1 += 1;

                start = false;
            }

            if (foundStart)
                break;

            curManip = &I::particleBase->Particles->at(index - 1);

            lastMoveDir = lastDir;

            lastDir -= cycleBack;

            startX = newX;
            startY = newY;

            if (lastDir < 0)
                lastDir += 8;
        }
        else
        {
            lastDir += 1;

            if (lastDir > 7)
                lastDir = 0;

            int lastMoveBack = (lastMoveDir - cycleBack);

            if (lastMoveBack > 7)
                lastMoveBack -= 8;

            if (lastMoveBack < 0)
                lastMoveBack += 8;

            if (lastMoveBack == lastDir) // looping
            {
                looping = true;
                iParticleGroup = -1;
                break;
            }

            if (iSearchParticleGroup > 0 && iSearchParticleGroup != curGroupSize) // this search collided with a previous boundary
            {
                looping = true;
                iParticleGroup = -1;
                break;
            }
        }

        if (lastDir > 7)
            lastDir = 0;
    }

    if (posSize1 < 3 || looping)
    {
        curGroupSize -= 1;
        goto findNext;
    }

    if (terrainBodySize >= 100)
        return;

    b2BodyDef bd = b2BodyDef();
    terrainBodies[terrainBodySize] = physWorld->CreateBody(&bd);
    int revi = 0;
    for (int i = posSize1 - 1; i >= 0; i--)
    {
        Vec2Di& pos = checkPosBuffer1[i];
        vertices[revi].Set(pos.x, pos.y);
        revi += 1;
    }

    b2ChainShape shape = b2ChainShape();
    shape.CreateLoop(vertices, posSize1);

    b2FixtureDef fd = b2FixtureDef();
    fd.shape = &shape;
    fd.density = 0.0f;
    fd.friction = 1.f;
    fd.userData.pointer = 2;
    terrainBodies[terrainBodySize]->CreateFixture(&fd);
    terrainBodySize += 1;

    goto findNext;
 /*   MassX = smallestX + ((largestX - smallestX) / 2);
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

    BoundX(MassY);
    BoundY(MassX);


    b2BodyDef bd;
    bd.type = b2_dynamicBody;
    bd.position.Set(pos.x, pos.y);

    objectBody = physWorld->CreateBody(&bd);

    int vertSize = vertices.size();
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

    oldCentPos = objectBody->GetWorldCenter();*/

}



/*
void PhysWorldBase::BuildWorldCollision()
{

    for (int i = 0; i < terrainBodySize; i++)
    {
        physWorld->DestroyBody(terrainBodies[i]);
        terrainBodies[i] = nullptr;
    }
    terrainBodySize = 0;

    curGroupSize = 0;

    for (int x = 0; x < I::baseClass->particleResolution3X[0]; x++)
        for (int y = 0; y < I::baseClass->particleResolution3X[1]; y++)
            particleGroups[x][y] = 0;

    int lastX = 0;
    int lastY = 0;

findNext:
    for (int i = 0; i < 4000; i++)
    {
        checkPosBuffer1[i].Clear();
        //       checkPosBuffer2[i].Clear();
    }
    posSize1 = 0;

    int startIndex = 0;
    int startX = 0;
    int startY = 0;

    int curPSize = *I::particleBase->pSize;

    bool breakDa = false;
    Vec2Di checkPos;
    checkPos.Clear();

    Particle* curManip = nullptr;

    for (int xp = lastX; xp < I::baseClass->particleResolution3X[0]; xp++)
    {
        for (int hp = 0; hp < I::baseClass->particleResolution3X[1]; hp++)
        {
            if (lastY != 0)
            {
                hp += lastY;
                lastY = 0;
            }

            int& curGroupIndex = particleGroups[xp][hp];
            if (curGroupIndex == 0)
            {
                int index = I::chunkBase->mainClump[xp][hp];

                bool hasOpen = false;
                if (index != 0)
                {
                    curManip = &I::particleBase->Particles->at(index - 1);

                    if (curManip->isDormant && curManip->formationIndex == 0 && curManip->classid != WaterP && curManip->classid != SmokeP && curManip->classid != FireP)
                    {
                        for (int i = 0; i < 8; i += 2)
                        {
                            checkPos.x = xp;
                            checkPos.y = hp;

                            checkPos += direction[i];

                            bool BX = BoundX(checkPos.x);
                            bool BY = BoundY(checkPos.y);

                            if (!BX || !BY || I::chunkBase->mainClump[checkPos.x][checkPos.y] == 0)
                            {
                                hasOpen = true;
                                break;
                            }
                        }

                        if (hasOpen)
                        {
                            curGroupSize += 1;
                            curGroupIndex = curGroupSize;

                            //   checkPosBuffer1[posSize1] = Vec2Di( curManip->x, curManip->y);
                            //   posSize1 += 1;

                            startX = xp;
                            startY = hp;

                            lastX = xp;
                            lastY = hp;

                            startIndex = index;

                            breakDa = true;
                            break;
                        }
                        else
                            curGroupIndex = -1;
                    }
                    else
                        curGroupIndex = -1;
                }
            }

        }
        if (breakDa)
            break;
    }

    if (!breakDa)
    {
        //   threads.clear();
        return;
    }


    int lastDir = 0;
    int cycleBack = 2;
    int lastMoveDir = cycleBack;
    int area = 999999999;
    int step = 0;

    bool looping = false;

    while (step < area)
    {
        step += 1;

        if (step == area)
        {
            
            << "HUUUGE LOOP\n";
            std::cout << "lastMoveDir " << lastMoveDir << " lastDir " << lastDir << " posSize1 " << posSize1 << std::endl;

            if (posSize1 < 50)
            {
                for (int i = 0; i < posSize1; i++)
                {
                    std::cout << checkPosBuffer1[i].x << " " << checkPosBuffer1[i].y << std::endl;
                }
            }

            looping = true;
            break;
        }

        int& iParticleGroup = particleGroups[startX][startY];

        int newX = startX + static_cast<int>(direction[lastDir].x);
        int newY = startY + static_cast<int>(direction[lastDir].y);


        if (!BoundX(newX) || !BoundY(newY))
        {
            lastDir += 1;

            if (lastDir > 7)
                lastDir = 0;

            int lastMoveBack = (lastMoveDir - cycleBack);

            if (lastMoveBack > 7)
                lastMoveBack -= 8;

            if (lastMoveBack < 0)
                lastMoveBack += 8;

            if (lastMoveBack == lastDir) // looping
            {
                looping = true;
                iParticleGroup = -1;
                break;
            }

            continue;
        }

        int& iSearchParticleGroup = particleGroups[newX][newY];
        int index = I::chunkBase->mainClump[newX][newY];
        bool foundStart = index == startIndex;
        if (index > 0 && index <= curPSize && (iSearchParticleGroup == curGroupSize || iSearchParticleGroup == 0 || foundStart))
        {

            if (!foundStart)
            {
                Particle& manip = I::particleBase->Particles->at(index - 1);
                if (!manip.isDormant || manip.formationIndex != 0 || manip.classid == WaterP || manip.classid == SmokeP || manip.classid == FireP)
                {
                    lastDir += 1;

                    if (lastDir > 7)
                        lastDir = 0;

                    int lastMoveBack = (lastMoveDir - cycleBack);

                    if (lastMoveBack > 7)
                        lastMoveBack -= 8;

                    if (lastMoveBack < 0)
                        lastMoveBack += 8;

                    if (lastMoveBack == lastDir) // looping
                    {
                        iParticleGroup = -1;
                        looping = true;
                        break;
                    }

                    continue;
                }
            }

            if (iParticleGroup == 0)
                iParticleGroup = curGroupSize;

            if (lastMoveDir != lastDir && (iSearchParticleGroup == 0 || foundStart))
            {
                checkPosBuffer1[posSize1] = Vec2Di(curManip->x, curManip->y);
                posSize1 += 1;
            }

            if (foundStart)
                break;

            curManip = &I::particleBase->Particles->at(index - 1);

            lastMoveDir = lastDir;

            lastDir -= cycleBack;

            startX = newX;
            startY = newY;

            if (lastDir < 0)
                lastDir += 8;
        }
        else
        {
            lastDir += 1;

            if (lastDir > 7)
                lastDir = 0;

            int lastMoveBack = (lastMoveDir - cycleBack);

            if (lastMoveBack > 7)
                lastMoveBack -= 8;

            if (lastMoveBack < 0)
                lastMoveBack += 8;

            if (lastMoveBack == lastDir) // looping
            {
                looping = true;
                iParticleGroup = -1;
                break;
            }
        }

        if (lastDir > 7)
            lastDir = 0;
    }

    if (posSize1 < 3 || looping)
    {
        curGroupSize -= 1;
        goto findNext;
    }

    b2BodyDef bd;
    terrainBodies[terrainBodySize] = physWorld->CreateBody(&bd);
    static b2Vec2* vertices = new b2Vec2[4000];
    int revi = 0;
    for (int i = posSize1 - 1; i >= 0; i--)
    {
        Vec2Di& pos = checkPosBuffer1[i];
        vertices[revi].Set(pos.x, pos.y);
        revi += 1;
    }

    b2ChainShape shape;
    shape.CreateLoop(vertices, posSize1);

    b2FixtureDef fd;
    fd.shape = &shape;
    fd.density = 0.0f;
    fd.friction = 1.f;
    fd.userData.pointer = 2;
    terrainBodies[terrainBodySize]->CreateFixture(&fd);
    terrainBodySize += 1;

    goto findNext;

}*/