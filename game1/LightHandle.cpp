#include "header_stack.h"


Vec2Di LightBase::PosToPixel(Vec2Df pos)
{
    pos.x = pos.x - I::baseClass->screenLeft;
    pos.y = pos.y - I::baseClass->screenBottom;
    if (pos.x < 0 || pos.x >= I::baseClass->particleResolution[0] || pos.y < 0 || pos.y >= I::baseClass->particleResolution[1])
        return { -9999, 9999 };

    return { pos.x , pos.y };
}


void LightBase::DoLighting(int xP, int yP)
{
    int searchIndex = 0;
    Vec2Df searchPos = { xP, yP };

    float inShadow = 0;
    while (searchPos.x >= 0 && searchPos.x < I::baseClass->particleResolution3X[0] && searchPos.y >= 0 && searchPos.y < I::baseClass->particleResolution3X[1])
    {
        searchIndex = I::chunkBase->mainClump[static_cast<int>(searchPos.x)][static_cast<int>(searchPos.y)];
        if (searchIndex > 0 && inShadow <= shadowFadeLen)
        {

            float curScale = scale * inShadow;
            Particle& p = I::particleBase->Particles->at(searchIndex - 1);
            if (p.x < XBoundsMax && p.x > XBoundsMin && p.y < YBoundsMax && p.y > YBoundsMin)
            {
                switch (p.classid)
                {
                case WaterP:
                case PeeP:
                case FireP:
                case SmokeP:
                {
                    Vec2Di pixPos = PosToPixel({ p.x, p.y });
                    //optimiz
                    if (pixPos.x != -9999)
                    {
                        int& bufIndex = lightBuffer[pixPos.x][pixPos.y];
                        if (bufIndex == 0)
                        {
                            int off = (offStep + p.x + p.y) % 10;
                            int whitr = (off > 5 && (p.y + whiteStep) % 4 == 0 && p.classid == WaterP) ? 100 : 0;

                            inShadow += 0.5f;
                            if (inShadow > shadowFadeLen)
                                inShadow = shadowFadeLen;
                            ColorM darkamnt = ColorM(max(p.r - curScale, 0), max(p.g - curScale, 0), max(p.b - curScale, 0));


                            oppositeDrawStack[*oppositeDrawSize] = { p.x, p.y, static_cast<unsigned char>(darkamnt.r + whitr), static_cast<unsigned char>(darkamnt.g + whitr), static_cast<unsigned char>(darkamnt.b) };
                            *oppositeDrawSize += 1;
                            bufIndex = *oppositeDrawSize;
                        }
                    }
                    break;
                }
                default:
                {
                    inShadow += 2.5f;
                    if (inShadow > shadowFadeLen)
                        inShadow = shadowFadeLen;
                    ColorM darkamnt = ColorM(max(p.r - curScale, 0), max(p.g - curScale, 0), max(p.b - curScale, 0));
                    Vec2Di pixPos = PosToPixel({ p.x, p.y });
                    if (pixPos.x != -9999)
                    {
                        int& bufIndex = lightBuffer[pixPos.x][pixPos.y];
                        if (bufIndex == 0)
                        {
                            oppositeDrawStack[*oppositeDrawSize] = { p.x, p.y, static_cast<unsigned char>(darkamnt.r), static_cast<unsigned char>(darkamnt.g), static_cast<unsigned char>(darkamnt.b) };
                            *oppositeDrawSize += 1;

                            bufIndex = *oppositeDrawSize;
                        }
                    }
                    break;
                }
                }
            }
            else
                inShadow += 2.5f;

            if (inShadow > shadowFadeLen)
                inShadow = shadowFadeLen;

        }
        else if (inShadow > 0.f)
        {
            inShadow -= 0.25f;

            float curScale = scale * inShadow;
            Vec2Di pSearchPos = { static_cast<int>(searchPos.x), static_cast<int>(searchPos.y) };
            pSearchPos.x += I::chunkBase->curClumpOffset.x;
            pSearchPos.y += I::chunkBase->curClumpOffset.y;
            Vec2Di pixPos = PosToPixel({ pSearchPos.x, pSearchPos.y });
            if (pixPos.x != -9999 && pSearchPos.x < XBoundsMax && pSearchPos.x > XBoundsMin && pSearchPos.y < YBoundsMax && pSearchPos.y > YBoundsMin)
            {
                int& bufIndex = lightBuffer[pixPos.x][pixPos.y];
                if (bufIndex == 0)
                {
                    ColorM backClr = { 63.f,167.f,202.f };

                    ColorM darkamnt = ColorM(max(backClr.r - curScale, 0), max(backClr.g - curScale, 0), max(backClr.b - curScale, 0));

                    oppositeDrawStack[*oppositeDrawSize] = { pSearchPos.x, pSearchPos.y, static_cast<unsigned char>(darkamnt.r), static_cast<unsigned char>(darkamnt.g), static_cast<unsigned char>(darkamnt.b) };
                    *oppositeDrawSize += 1;

                    bufIndex = *oppositeDrawSize;
                }
            }
            //  else
            //      break;

        }
        else if (inShadow < 0.f)
        {
            break;
            Vec2Di pSearchPos = { static_cast<int>(searchPos.x), static_cast<int>(searchPos.y) };
            pSearchPos.x += I::chunkBase->curClumpOffset.x;
            pSearchPos.y += I::chunkBase->curClumpOffset.y;
            Vec2Di pixPos = PosToPixel({ pSearchPos.x, pSearchPos.y });
            if (pixPos.x != -9999 && pSearchPos.x < XBoundsMax && pSearchPos.x > XBoundsMin && pSearchPos.y < YBoundsMax && pSearchPos.y > YBoundsMin)
            {
                lightBuffer[pixPos.x][pixPos.y] = 0;
            }
            /*    float curScale = scale * inShadow;
                Vec2Di pSearchPos = { static_cast<int>(searchPos.x), static_cast<int>(searchPos.y) };
                ColorM backClr = { 63.f,167.f,202.f };

                ColorM darkamnt = ColorM(max(backClr.r - curScale, 0), max(backClr.g - curScale, 0), max(backClr.b - curScale, 0));
                glColor3ub(darkamnt.r, darkamnt.g, darkamnt.b);
                glVertex2d(pSearchPos.x, pSearchPos.y);*/
        }
        searchPos.x += xCos;
        searchPos.y += ySin;

    }

}

void LightBase::DoSpotLights()
{
    for (SpotLight& lgt : spotLights)
    {
        int xCpy = lgt.x;
        int yCpy = lgt.y;
        if (!I::particleBase->BoundX(xCpy) || !I::particleBase->BoundY(yCpy))
            continue;
        float vertextCount = (lgt.radius * 2.f) * PI;
        shadowFadeLen = lgt.radius;
        scale = 254.f / shadowFadeLen;
        float stepSize = 360.f / vertextCount;
        float curBlend = 0.f;
        float curScale = 0.f;
        float brightOffset = 1.f - lgt.brightness;

        Vec2Df cPos = { lgt.x, lgt.y };
        for (float i = 0; i < 180.f; i += stepSize)
        {
            float normAng = Math::NormalizeAngle(i);
            float rxCos = cos(Math::DegToRad(normAng));
            float rySin = sin(Math::DegToRad(normAng));
            normAng = Math::NormalizeAngle(normAng + 180.f);
            float rRxCos = cos(Math::DegToRad(normAng));
            float rRySin = sin(Math::DegToRad(normAng));

            float shadowAmnt[2] = { 0,0 };



            float curXCos[2] = { rxCos ,rRxCos };
            float curYSin[2] = { rySin ,rRySin };
            for (float r = 0; r <= lgt.radius; r += 1.f)
            {

                Vec2Di vPos[2] = { cPos + Vec2Df(rxCos * r, rySin * r), cPos + Vec2Df(rRxCos * r, rRySin * r) };

                for (int pt = 0; pt < 2; pt++)
                {
                    if (I::particleBase->BoundX(vPos[pt].x) && I::particleBase->BoundY(vPos[pt].y))
                    {
                        Vec2Di posToClump = I::chunkBase->TranslatePosToClumpPos(vPos[pt]);
                        int searchIndex = I::chunkBase->mainClump[posToClump.x][posToClump.y];
                        if (searchIndex > 0 && shadowAmnt[pt] <= shadowFadeLen)
                        {


                            Particle& p = I::particleBase->Particles->at(searchIndex - 1);
                            if (p.x < XBoundsMax && p.x > XBoundsMin && p.y < YBoundsMax && p.y > YBoundsMin)
                            {
                                switch (p.classid)
                                {
                                case WaterP:
                                {
                                    //optimiz
                                    int off = (offStep + p.x + p.y) % 10;
                                    int whitr = (off > 5 && (p.y + whiteStep) % 4 == 0) ? 100 : 0;

                                    shadowAmnt[pt] += 1.f;//0.5f;
                                    if (shadowFadeLen < shadowAmnt[pt])
                                    {
                                        shadowAmnt[pt] = shadowFadeLen;

                                    }
                                    curScale = scale * shadowAmnt[pt];
                                    curBlend = shadowAmnt[pt] / shadowFadeLen;
                                    curBlend += brightOffset;
                                    if (curBlend > 1.f)
                                        curBlend = 1;

                                    ColorM darkamnt = ColorM(max(p.r - curScale, 0), max(p.g - curScale, 0), max(p.b - curScale, 0));
                                    Vec2Di pixPos = PosToPixel({ p.x, p.y });
                                    if (pixPos.x != -9999)
                                    {
                                        int checkLight = lightBuffer[pixPos.x][pixPos.y];
                                        if (checkLight > 0 && checkLight <= *oppositeDrawSize)
                                        {
                                            PixelInfo& targ = oppositeDrawStack[checkLight - 1];
                                            //    darkamnt = { static_cast<float>(targ.r), static_cast<float>(targ.g), static_cast<float>(targ.b) };
                                            
                                            ColorM ogClr = ColorM(static_cast<float>(targ.r), static_cast<float>(targ.g), static_cast<float>(targ.b));

                                             //   ColorM othDrkAmnt = ColorM(max(lgt.clr.r - curScale, 0), max(lgt.clr.g - curScale, 0), max(lgt.clr.b - curScale, 0));
                                            ColorM newClr = Math::BlendColor2(/*darkamnt*/ogClr, lgt.clr, curBlend);

                                            //   if (static_cast<int>(targ.r) + static_cast<int>(targ.g) + static_cast<int>(targ.b) < darkamnt.r + darkamnt.g + darkamnt.b)
                                            {
                                                oppositeDrawStack[checkLight - 1] = { p.x, p.y, static_cast<unsigned char>(newClr.r + whitr), static_cast<unsigned char>(newClr.g + whitr), static_cast<unsigned char>(newClr.b) };
                                             //   *drawSize += 1;
                                             //   LightHandle::lightBuffer[pixPos.x][pixPos.y] = *drawSize;
                                            }
                                        }
                                    }

                                    break;
                                }
                                default:
                                {
                                    shadowAmnt[pt] += 2.0f;
                                    if (shadowFadeLen < shadowAmnt[pt])
                                    {
                                        shadowAmnt[pt] = shadowFadeLen;
                                    }

                                    curScale = scale * shadowAmnt[pt];
                                    curBlend = shadowAmnt[pt] / shadowFadeLen;
                                    curBlend += brightOffset;
                                    if (curBlend > 1.f)
                                        curBlend = 1;


                                    ColorM darkamnt = ColorM(max(p.r, 0), max(p.g, 0), max(p.b, 0));

                                    Vec2Di pixPos = PosToPixel({ p.x, p.y });
                                    if (pixPos.x != -9999)
                                    {
                                        int checkLight = lightBuffer[pixPos.x][pixPos.y];
                                        if (checkLight > 0 && checkLight <= *oppositeDrawSize && pixPos.x != -9999)
                                        {
                                            PixelInfo& targ = oppositeDrawStack[checkLight - 1];
                                            //   darkamnt = { static_cast<float>(targ.r), static_cast<float>(targ.g), static_cast<float>(targ.b) };
                                           //    ColorM othDrkAmnt = ColorM(max(lgt.clr.r - curScale, 0), max(lgt.clr.g - curScale, 0), max(lgt.clr.b - curScale, 0));
                                            ColorM newClr1 = Math::BlendColor2(darkamnt, lgt.clr, curBlend);
                                            darkamnt = ColorM(targ.r, targ.g, targ.b);
                                            ColorM newClr = Math::BlendColor2(darkamnt, newClr1, curBlend);

                                            //  if (static_cast<int>(targ.r) + static_cast<int>(targ.g) + static_cast<int>(targ.b) < newClr.r + newClr.g + newClr.b)
                                            {
                                                oppositeDrawStack[checkLight - 1] = { p.x, p.y, static_cast<unsigned char>(newClr.r), static_cast<unsigned char>(newClr.g), static_cast<unsigned char>(newClr.b) };
                                            //    *drawSize += 1;
                                             //   LightHandle::lightBuffer[pixPos.x][pixPos.y] = *drawSize;
                                            }
                                        }
                                    }

                                    break;
                                }
                                }
                            }
                            else
                                shadowAmnt[pt] += 1.0f;

                            if (shadowFadeLen < shadowAmnt[pt])
                            {
                                shadowAmnt[pt] = shadowFadeLen;
                            }

                            if (curBlend == 1.f)
                                continue;
                        }
                        else
                        {
                            shadowAmnt[pt] += 1.0f;

                            if (shadowFadeLen < shadowAmnt[pt])
                            {
                                shadowAmnt[pt] = shadowFadeLen;
                            }

                            curScale = scale * shadowAmnt[pt];
                            curBlend = shadowAmnt[pt] / shadowFadeLen;
                            curBlend += brightOffset;
                            if (curBlend > 1.f)
                                curBlend = 1;

                            Vec2Di pSearchPos = { static_cast<int>(vPos[pt].x), static_cast<int>(vPos[pt].y) };

                            if (pSearchPos.x < XBoundsMax && pSearchPos.x > XBoundsMin && pSearchPos.y < YBoundsMax && pSearchPos.y > YBoundsMin)
                            {
                                ColorM backClr = { 63.f,167.f,202.f };

                                ColorM darkamnt = ColorM(max(backClr.r - curScale, 0), max(backClr.g - curScale, 0), max(backClr.b - curScale, 0));
                                ColorM newClr;
                                ColorM othDrkAmnt = ColorM(max(lgt.clr.r - curScale, 0), max(lgt.clr.g - curScale, 0), max(lgt.clr.b - curScale, 0));
                                //    ColorM newClr = Math::BlendColor2(darkamnt, lgt.clr, shadowAmnt[pt] / shadowFadeLen);

                                Vec2Di pixPos = PosToPixel({ pSearchPos.x, pSearchPos.y });
                                if (pixPos.x != -9999)
                                {
                                    int& bufIndex = lightBuffer[pixPos.x][pixPos.y];

                                    if (bufIndex > 0 && bufIndex <= *drawSize)
                                    {
                                        PixelInfo& targ = oppositeDrawStack[bufIndex - 1];
                                        //   newClr = { static_cast<float>(targ.r), static_cast<float>(targ.g), static_cast<float>(targ.b) };
                                        newClr = Math::BlendColor2(backClr, lgt.clr, curBlend);
                                        othDrkAmnt = { static_cast<float>(targ.r), static_cast<float>(targ.g), static_cast<float>(targ.b) };
                                        ColorM newClr1 = Math::BlendColor2(othDrkAmnt, newClr, curBlend);
                                        //ColorM newClr = Math::BlendColor2(darkamnt, newClr1, curBlend);

                                      //  newClr = ColorM(max(newClr.r - curScale, 0), max(newClr.g - curScale, 0), max(newClr.b - curScale, 0));
                                   //    if (static_cast<int>(targ.r) + static_cast<int>(targ.g) + static_cast<int>(targ.b) < newClr.r + newClr.g + newClr.b)
                                        {
                                            oppositeDrawStack[bufIndex - 1] = { pSearchPos.x, pSearchPos.y, static_cast<unsigned char>(newClr1.r), static_cast<unsigned char>(newClr1.g), static_cast<unsigned char>(newClr1.b) };
                                         //   *drawSize += 1;
                                         //   LightHandle::lightBuffer[pixPos.x][pixPos.y] = *drawSize;
                                        }
                                    }
                                    else if (*oppositeDrawSize < maxBufferSize)
                                    {

                                        newClr = Math::BlendColor2(backClr, lgt.clr, curBlend);

                                        /// crash v
                                        oppositeDrawStack[*oppositeDrawSize] = { pSearchPos.x, pSearchPos.y, static_cast<unsigned char>(newClr.r), static_cast<unsigned char>(newClr.g), static_cast<unsigned char>(newClr.b) };
                                        *oppositeDrawSize += 1;
                                        bufIndex = *oppositeDrawSize;
                                    }
                                }
                            }

                            if (curBlend == 1.f)
                                continue;
                            //  else
                            //      break;

                        }

                        //   vPos[pt] += Vec2Df(xCos * 1.f, ySin * 1.f);
                    }
                }

            }

        }
    }

    renderQueue[4] = 0;
}

void TopLighting(LightBase* parent)
{
    int yPos = I::baseClass->particleResolution3X[1] - 1;

    for (int xPos = 0; xPos < I::baseClass->particleResolution3X[0]; xPos++)
    {
        parent->DoLighting(xPos, yPos);
    }

    parent->renderQueue[0] = 0;
}

void RightLighting(LightBase* parent)
{
    int xPos = I::baseClass->particleResolution3X[0] - 1;

    for (int yPos = 0; yPos < I::baseClass->particleResolution3X[1]; yPos++)
    {
        parent->DoLighting(xPos, yPos);
    }
    parent->renderQueue[1] = 0;
}

void BottomLighting(LightBase* parent)
{
    int yPos = 0;

    for (int xPos = 0; xPos < I::baseClass->particleResolution3X[0]; xPos++)
    {
        parent->DoLighting(xPos, yPos);
    }
    parent->renderQueue[2] = 0;
}

void LeftLighting(LightBase* parent)
{
    int xPos = 0;
    for (int yPos = 0; yPos < I::baseClass->particleResolution3X[1]; yPos++)
    {
        parent->DoLighting(xPos, yPos);
    }
    parent->renderQueue[3] = 0;
}

bool FirstBufferSwitch = false;
void LightBase::Render(bool Update)
{
    if (*I::particleBase->pSize > 0)
    {

        // integer size will become a problem with this method
    //    glColor3f(1, 1, 1);
     //   glColor3b
        if (Update && renderQueue[4] == -1)
        {
            renderQueue[0] = 0;
            renderQueue[1] = 0;
            renderQueue[2] = 0;
            renderQueue[3] = 0;
            renderQueue[4] = 3;


            for (int x = 0; x < I::baseClass->particleResolution[0]; x++)
            {
                for (int y = 0; y < I::baseClass->particleResolution[1]; y++)
                {
                    lightBuffer[x][y] = 0;
                }
            }

            if (!bufferSwitch)
            {
                oppositeDrawStack = pixelBuffer2;
                oppositeDrawSize = &bufferSize2;
            }
            else
            {
                oppositeDrawStack = pixelBuffer1;
                oppositeDrawSize = &bufferSize1;
            }

            *oppositeDrawSize = 0;
            
            XBoundsMax = I::baseClass->screenRight;//cameraPos.x + (I::baseClass->particleResolutionHalf[0] - 5);
            XBoundsMin = I::baseClass->screenLeft;//cameraPos.x - (I::baseClass->particleResolutionHalf[0] - 5);
            YBoundsMax = I::baseClass->screenTop;//cameraPos.y + (I::baseClass->particleResolutionHalf[1] - 5);
            YBoundsMin = I::baseClass->screenBottom;//cameraPos.y - (I::baseClass->particleResolutionHalf[1] - 5);

            Vec2Df topBoundPos;
            Vec2Df rightBoundPos;
            Vec2Df bottomBoundPos;
            Vec2Df leftBoundPos;

            topBoundPos.Set(I::chunkBase->curClumpOffset.x + static_cast<float>(I::baseClass->particleResolution3X[0]) * 0.5f, I::chunkBase->curClumpFarBounds.y);
            rightBoundPos.Set(I::chunkBase->curClumpOffset.x + static_cast<float>(I::baseClass->particleResolution3X[0]), I::chunkBase->curClumpOffset.y + static_cast<float>(I::baseClass->particleResolution3X[1]) * 0.5f);
            bottomBoundPos.Set(I::chunkBase->curClumpOffset.x + static_cast<float>(I::baseClass->particleResolution3X[0]) * 0.5f, I::chunkBase->curClumpOffset.y);
            leftBoundPos.Set(I::chunkBase->curClumpOffset.x, I::chunkBase->curClumpOffset.y + static_cast<float>(I::baseClass->particleResolution3X[1]) * 0.5f);

            xCos = cos(Math::DegToRad(sunAngle));
            ySin = sin(Math::DegToRad(sunAngle));

            Vec2Df sunPos = Math::ProjectPoint(Vec2Df(I::chunkBase->curClumpOffset.x + static_cast<float>(I::baseClass->particleResolution3X[0]) * 0.5f, I::chunkBase->curClumpOffset.y + static_cast<float>(I::baseClass->particleResolution3X[1]) * 0.5f), Math::NormalizeAngle(sunAngle + 180.f), static_cast<float>(I::baseClass->particleResolution3X[0]) * 3.0f);

            offStep = static_cast<int>(sin(static_cast<float>(I::baseClass->curMilli) / 1000) * 25.f);
            whiteStep = static_cast<int>(sin(static_cast<float>(I::baseClass->curMilli) / 1500) * 15.f);

            shadowFadeLen = 100.f;
            scale = 254.f / shadowFadeLen;

            float leftDelta = 999;
            int leftPosIndex = 0; // relative to the sun

            float rightDelta = -999;
            int rightPosIndex = 0; // relative to the sun

            Vec2Df topRightCamPos;
            Vec2Df bottomRightCamPos;
            Vec2Df bottomLeftCamPos;
            Vec2Df topLeftCamPos;

            topRightCamPos.Set(XBoundsMax, YBoundsMax);
            bottomRightCamPos.Set(XBoundsMax, YBoundsMin);
            bottomLeftCamPos.Set(XBoundsMin, YBoundsMin);
            topLeftCamPos.Set(XBoundsMin, YBoundsMax);

            Vec2Df posArray[4] = { topRightCamPos, bottomRightCamPos, bottomLeftCamPos, topLeftCamPos };

            for (int i = 0; i < 4; i++)
            {
                float curAng = Math::CalcAngle(sunPos, posArray[i]);
                float curDelta = Math::NormalizeAngle(sunAngle - curAng);
                if (curDelta < leftDelta)
                {
                    leftPosIndex = i;
                    leftDelta = curDelta;
                }
                if (curDelta > rightDelta)
                {
                    rightPosIndex = i;
                    rightDelta = curDelta;
                }
            }

         //   CustRender::drawPoint(posArray[leftPosIndex].x, posArray[leftPosIndex].y, 100, ColorM(1, 0, 0));
         //   CustRender::drawPoint(posArray[rightPosIndex].x, posArray[rightPosIndex].y, 100, ColorM(0, 1, 0));

            int curBoundPos = 0; // 0 = bottom, 1 = left, 2 = top, 3 = right

            Vec2Di rightBackwardTrace = Math::ProjectPoint(posArray[rightPosIndex], Math::NormalizeAngle(sunAngle + 180.f), 2000.f);
            Vec2Di rightBackwardTraceBoundPos = I::particleBase->TraceRay(posArray[rightPosIndex].x, posArray[rightPosIndex].y, rightBackwardTrace.x, rightBackwardTrace.y, true);
            Vec2Di leftBackwardTrace = Math::ProjectPoint(posArray[leftPosIndex], Math::NormalizeAngle(sunAngle + 180.f), 2000.f);
            Vec2Di leftBackwardTraceBoundPos = I::particleBase->TraceRay(posArray[leftPosIndex].x, posArray[leftPosIndex].y, leftBackwardTrace.x, leftBackwardTrace.y, true);

            Vec2Di direction[4] = { {0, -1}/*bottom*/, {-1, 0}/*left*/,  {0, 1}/*top*/,  {1, 0}/*right*/ };
            // find initial direction
            for (int i = 0; i < 4; i++)
            {
                Vec2Di searchPos = rightBackwardTraceBoundPos + direction[i];

                if (!I::particleBase->BoundX(searchPos.x) || !I::particleBase->BoundY(searchPos.y))
                {
                    curBoundPos = i;
                }
            }

            Vec2Di clumpPos = I::chunkBase->TranslatePosToClumpPos(rightBackwardTraceBoundPos);
            DoLighting(clumpPos.x, clumpPos.y);

            curBoundPos += 1;

            if (curBoundPos > 3)
                curBoundPos = 0;

            Vec2Di curSearchPos = rightBackwardTraceBoundPos;
            while (curSearchPos != leftBackwardTraceBoundPos)
            {
                Vec2Di newSearchPos = curSearchPos + direction[curBoundPos];
                if (!I::particleBase->BoundX(newSearchPos.x) || !I::particleBase->BoundY(newSearchPos.y))
                {
                    curBoundPos += 1;

                    if (curBoundPos > 3)
                        curBoundPos = 0;

                    continue;
                }

                clumpPos = I::chunkBase->TranslatePosToClumpPos(newSearchPos);
                DoLighting(clumpPos.x, clumpPos.y);

                curSearchPos = newSearchPos;
            }

            //   glPointSize(particleSize);
            //   glBegin(GL_POINTS);

            //   threads.clear();

  /*          float topAng = Math::CalcAngle(topBoundPos, sunPos);
            bool useTopArray = false;
            if (topAng >= 0.f && topAng <= 180.f)
            {
                renderQueue[0] = 1;
                useTopArray = true;

                TopLighting(this);
                //  threads.push_back(std::async(std::launch::async, TopLighting, this));
               //   threads.push_back(std::async(std::launch::async, TopLighting, 1));
              //   threads.push_back(std::async(std::launch::async, TopLighting, 2));

            }

            float rightAng = Math::CalcAngle(rightBoundPos, sunPos);
            bool useRightArray = false;
            if (rightAng <= 90.f && rightAng >= -90.f)
            {
                renderQueue[1] = 1;
                useRightArray = true;
                RightLighting(this);
                //  threads.push_back(std::async(std::launch::async, RightLighting, this));
               //   threads.push_back(std::async(std::launch::async, RightLighting, 1));
               //   threads.push_back(std::async(std::launch::async, RightLighting, 2));
            }

            float bottomAng = Math::CalcAngle(bottomBoundPos, sunPos);
            bool useBottomArray = false;
            if (bottomAng <= 0.f && bottomAng >= -180.f)
            {
                renderQueue[2] = 1;
                useBottomArray = true;
                BottomLighting(this);
                //   threads.push_back(std::async(std::launch::async, BottomLighting, this));
               //    threads.push_back(std::async(std::launch::async, BottomLighting, 1));
               //    threads.push_back(std::async(std::launch::async, BottomLighting, 2));
            }

            float leftAng = Math::CalcAngle(leftBoundPos, sunPos);
            bool useLeftArray = false;
            if ((leftAng >= 90.f && leftAng <= 180.f) || (leftAng <= -90.f && leftAng >= -180.f))
            {
                renderQueue[3] = 1;
                useLeftArray = true;
                LeftLighting(this);
                //  threads.push_back(std::async(std::launch::async, LeftLighting, this));
               //   threads.push_back(std::async(std::launch::async, LeftLighting, 1));
               //   threads.push_back(std::async(std::launch::async, LeftLighting, 2));

            }*/

            renderQueue[4] = 2;


            //TEMP
            renderQueue[4] = 1;
            DoSpotLights();
            //TEMP

            //    threads.push_back(std::async(std::launch::async, doSpotLights));
        }
        else if (renderQueue[4] == 2)
        {
            if (renderQueue[0] == 0 && renderQueue[1] == 0 && renderQueue[2] == 0 && renderQueue[3] == 0)
            {
                /*   for (std::future<void>& threadPointer : threads)
                   {
                       threadPointer.wait();
                   }
                   threads.clear();*/
               // renderQueue[4] = 0;
                    renderQueue[4] = 1;
                    DoSpotLights();
                  //  threads.push_back(std::async(std::launch::async, DoSpotLights, this));
            }
        }
        else if (renderQueue[4] == 0)
        {
            //    *drawSize = 0;

            drawStack = oppositeDrawStack;
            drawSize = oppositeDrawSize;

            oppositeDrawStack = nullptr;
            oppositeDrawSize = nullptr;

            bufferSwitch = !bufferSwitch;

            renderQueue[4] = -1;

            /*    for (std::future<void>& threadPointer : threads)
                {
                    threadPointer.wait();
                }

                threads.clear();*/

            FirstBufferSwitch = true;
        }

        //   std::cout << renderQueue[0] << " " << renderQueue[1] << " " << renderQueue[2] <<" "<< renderQueue[3] << " " << renderQueue[4] << std::endl;

        if (FirstBufferSwitch && drawSize && *drawSize > 0)
        {
            glEnableClientState(GL_VERTEX_ARRAY);
            glEnableClientState(GL_COLOR_ARRAY);
            glVertexPointer(2, GL_INT, sizeof(PixelInfo), &drawStack[0].x);
            glColorPointer(3, GL_UNSIGNED_BYTE, sizeof(PixelInfo), &drawStack[0].r);
            glPointSize(I::baseClass->particleSize);
            glDrawArrays(GL_POINTS, 0, *drawSize);            
            glDisableClientState(GL_COLOR_ARRAY);
            glDisableClientState(GL_VERTEX_ARRAY); // !
            glEnd();

        }


        /*    for (Particle& p : *Particles)
            {
                if (p.x < XBoundsMax && p.x > XBoundsMin && p.y < YBoundsMax && p.y > YBoundsMin)
                {
                    switch (p.classid)
                    {
                    case WaterP:
                    {
                        //optimiz
                        int off = (offStep + p.x + p.y) % 10;
                        int whitr = (off > 5 && (p.y + whiteStep) % 4 == 0) ? 100 : 0;

                        glColor3ub(p.r + whitr, p.g + whitr, p.b);
                        glVertex2d(p.x, p.y);
                        break;
                    }
                    default:
                    {
                        glColor3ub(p.r, p.g, p.b);
                        glVertex2d(p.x, p.y);
                        break;
                    }
                    }
                }
            }*/

            //    glEnd();



                /*    int index = -1;
                    if (oldPSize != *pSize)
                    {
                        for (Particle& p : *Particles)
                        {
                            index += 1;
                            TESTBB& ddd = drawStack[index];
                            ddd.x = p.x;
                            ddd.y = p.y;
                            ddd.r = p.r;
                            ddd.g = p.g;
                            ddd.b = p.b;
                        }
                    }
                    else
                    {
                        for (Particle& p : *Particles)
                        {
                            index += 1;
                            if (p.isDormant)
                                continue;
                            TESTBB& ddd = drawStack[index];
                            ddd.x = p.x;
                            ddd.y = p.y;
                            ddd.r = p.r;
                            ddd.g = p.g;
                            ddd.b = p.b;
                        }
                    }

                    glEnableClientState(GL_VERTEX_ARRAY);
                    glEnableClientState(GL_COLOR_ARRAY);
                    glVertexPointer(2, GL_INT, sizeof(TESTBB), &drawStack[0].x);
                    glColorPointer(3, GL_UNSIGNED_BYTE, sizeof(TESTBB), &drawStack[0].r);
                    glPointSize(particleSize);
                    glDrawArrays(GL_POINTS, 0, *pSize);
                    glDisableClientState(GL_VERTEX_ARRAY);
                    glDisableClientState(GL_COLOR_ARRAY);
                    glEnd();*/

                    /*   if (debugDraw.size() > 0)
                       {
                           glEnableClientState(GL_VERTEX_ARRAY);
                           glColor3f(0, 0, 1);
                           glVertexPointer(2, GL_FLOAT, sizeof(Vec2Df), &debugDraw[0]);
                           glPointSize(particleSize);
                           glDrawArrays(GL_POINTS, 0, debugDraw.size());
                           glDisableClientState(GL_VERTEX_ARRAY);
                           glEnd();
                       }*/
        
                       /*    if (I::worldBase->curGroupSize > 0) // debug drawing world collision
                            {
                                float sclaeGrpClr = 1.f / static_cast<float>(I::worldBase->curGroupSize);

                                for (int xp = 0; xp < I::baseClass->particleResolution3X[0]; xp++)
                                    for (int yp = 0; yp < I::baseClass->particleResolution3X[1]; yp++)
                                    {
                                        int curGroup = I::worldBase->particleGroups[xp][yp];
                                        if (curGroup > 0)
                                        {
                                            CustRender::drawRect(xp, yp, 1, 1, ColorM().FromHSV(sclaeGrpClr * static_cast<float>(curGroup), 1.f, 1.f));
                                        }
                                    }
                            }*/

                            //    gay.clear();
    }
}