#include "header_stack.h"

/*
!!!!!!!!!!!!!!!!!!

this code was originally from a different cordinate system so it dosnt function properly at the moment

*/

float testClosestDistance = 999999.f;
C_BaseUIComponent* testClosestComp = nullptr;

#pragma region C_BaseUIComponent
void C_BaseUIComponent::Render()
{
	if (!renderThis)
		return;

	if (theme)
		theme(this);

	Vec2Df off = I::inputs->particleMousePosition;
	float distance = 0.f;
	bool mouseInBounds = PointInRectangle(I::inputs->particleMousePosition.x, I::inputs->particleMousePosition.y, x_off, y_off, w_off, h_off, &off, &distance);

	BYTE parentHasFocus = (parent ? parent->focusThis : 2);

	if (parentHasFocus == 1 && mouseInBounds)
		parent->childFocusedClassIDs |= classID;

	focusThis = mouseInBounds && parentHasFocus;

	if (parentHasFocus && (mouseInBounds || PointOutRectangle(I::inputs->particleMousePosition.x, I::inputs->particleMousePosition.y, x_off, y_off, w_off, h_off, &off, &distance)))
	{
		if (testClosestDistance > distance)
		{
			testClosestDistance = distance;
			testClosestComp = this;
		}

		//	Draw::FilledRect(off.x - 1, off.y - 1, 2, 2, Color(127, 255, 255, 255));
		//	g_pVGuiSurface->DrawSetColor(127, 255, 255, 255);
		//	g_pVGuiSurface->DrawOutlinedCircle(InputVars::mousePos.x, InputVars::mousePos.y, distance, 20);
	}

	childFocusedClassIDs = 0;

	/*	if (IsDynamic()) need to finish clipping and scrolling
	{
		Draw::PushClip(x_off + 1, y_off + 1, w_off - 2, h_off - 2);

		if (HasChildPadding())
		{
			for (int i = 0; i < childCount; i++)
			{
				children[i]->OffsetPosition(child_x_padding, child_y_padding);
				children[i]->Render();
				children[i]->OffsetPosition(-child_x_padding, -child_y_padding);
			}
		}
		else
			for (int i = 0; i < childCount; i++)
				children[i]->Render();

		if (focusThis && !(childFocusedClassIDs & UIClassID::CBaseControlGroup) && (classID & UIClassID::CBaseControlGroup) && InputVars::scrollAmount != 0)// scrollbar move calc
		{
			C_BaseControlGroup* thisGroup = (C_BaseControlGroup*)this;

			if (thisGroup->scrollBarExtraLength > 0.f)
			{
				thisGroup->scrollBarPosition += InputVars::scrollAmount * .2f;
				Clamp(thisGroup->scrollBarPosition, 0.f, 1.f);

				child_y_padding = -thisGroup->scrollBarPosition * thisGroup->scrollBarExtraLength;
			}
			else if (thisGroup->scrollBarExtraLength < 0.f)
			{
				thisGroup->scrollBarPosition += InputVars::scrollAmount * .2f;
				Clamp(thisGroup->scrollBarPosition, 0.f, 1.f);

				child_x_padding = thisGroup->scrollBarPosition * thisGroup->scrollBarExtraLength;
			}
		}

		Draw::PopClip();
	}
	else*/
	{
		if (HasChildPadding())
		{
			for (int i = 0; i < childCount; i++)
			{
				children[i]->OffsetPosition(child_x_padding, child_y_padding);
				children[i]->Render();
				children[i]->OffsetPosition(-child_x_padding, -child_y_padding);
			}
		}
		else
			for (int i = 0; i < childCount; i++)
				children[i]->Render();
	}
}

void C_BaseUIComponent::Refresh(BYTE refresh_flags)
{
	if (refresh_flags & UIRefreshFlags::Full)
	{
		if (parent && parent->IsDynamic())
		{
			//	Refresh(refresh_flags & ~UIRefreshFlags::Full); // double pass if we go to parent
			parent->Refresh(refresh_flags); // upstream
			return;
		}
		else // ready to post update once it gets to the last child
		{
			refresh_flags &= ~UIRefreshFlags::Full;
			//	post_update = true;
		}
	}

	if (!renderThis)
		return;

	if (!(refresh_flags & UIRefreshFlags::SkipSelf))
	{
		SetX(x, UIRefreshFlags::UFNone);
		SetY(y, UIRefreshFlags::UFNone);
		SetW(w, UIRefreshFlags::UFNone);
		SetH(h, UIRefreshFlags::UFNone);
	}
	else
		refresh_flags &= ~UIRefreshFlags::SkipSelf;

	for (size_t i = 0; i < childCount; i++)
	{
		children[i]->Refresh(refresh_flags);
	}

	if (IsDynamic() && childCount > 0)
	{
		int heighestX = 0;
		int lowestY = 0;

		if (flags & UIFlags::X_SequentialChildPlacement)
		{
			int currentYOffset = 0;
			for (size_t i = 0; i < childCount; i++)
			{
				C_BaseUIComponent* child = children[i];

				if (!child->renderThis)
					continue;

				if (CompareBits(child->flags, UIFlags::X_RelativeToParent | UIFlags::Y_RelativeToParent))
				{
					float xPadding = child->x_off - x_off; // x padding from the parent x_off
					float xDelta = heighestX == 0 ? 0 : (heighestX + xPadding) - child->x_off; // how much this child needs to move to be to the right of the last control + its padding to the parent

					heighestX = child->x_off + child->w_off + xDelta;// new furthest x position in this colum
					if (heighestX > x_off + w_off) // child's right side passes the end of the parent
					{
						heighestX = child->x_off + child->w_off;
						xDelta = 0;
						currentYOffset = lowestY;
					}

					float yPadding = child->y_off - y_off;
					float yDelta = currentYOffset == 0 ? 0 : (currentYOffset + yPadding) - child->y_off;

					if (child->flags & H_FromParentBottomSide)
					{
						float childH_OffResim = child->h;

						if (child->flags & H_ScaledByParentHeight)
							childH_OffResim = childH_OffResim * h_off;
						else if (child->flags & H_ScaledByParentWidth)
							childH_OffResim = childH_OffResim * w_off;

						//	if (child->flags & H_FromParentBottomSide)
						{
							float parentBottomSide = y_off + h_off;
							float desiredPosition = parentBottomSide - childH_OffResim;
							childH_OffResim = desiredPosition - (yDelta + child->y_off);

							child->h_off = childH_OffResim;

							child->Refresh(UIRefreshFlags::SkipSelf | UIRefreshFlags::Post);
						}
					}

					child->OffsetPosition(xDelta, yDelta);

					float height = child->y_off + child->h_off;
					if (lowestY < height)
						lowestY = height;

					if (classID & UIClassID::CBaseControlGroup)// scrollbar bound calc
					{
						C_BaseControlGroup* thisGroup = (C_BaseControlGroup*)this;

						float extraYDelta = lowestY - (y_off + h_off);
						if (!Clamped(extraYDelta, 0.f, 99999.f))
							thisGroup->scrollBarExtraLength = extraYDelta + yPadding; // add padding for the bottom
						else
						{
							thisGroup->scrollBarExtraLength = 0.f;
							thisGroup->scrollBarPosition = 0.f;
							child_y_padding = 0.f;
						}
					}
				}
			}
		}
		else if (flags & UIFlags::Y_SequentialChildPlacement)
		{
			int currentXOffset = 0;
			for (size_t i = 0; i < childCount; i++)
			{
				C_BaseUIComponent* child = children[i];

				if (!child->renderThis)
					continue;

				if (CompareBits(child->flags, UIFlags::X_RelativeToParent | UIFlags::Y_RelativeToParent))
				{
					float yPadding = child->y_off - y_off;
					float yDelta = lowestY == 0 ? 0 : (lowestY + yPadding) - child->y_off;

					if (child->flags & H_FromParentBottomSide)
					{
						float childH_OffResim = child->h;

						if (child->flags & H_ScaledByParentHeight)
							childH_OffResim = childH_OffResim * h_off;
						else if (child->flags & H_ScaledByParentWidth)
							childH_OffResim = childH_OffResim * w_off;

						//	if (child->flags & H_FromParentBottomSide)
						{
							float parentBottomSide = y_off + h_off;
							float desiredPosition = parentBottomSide - childH_OffResim;
							childH_OffResim = desiredPosition - (yDelta + child->y_off);

							child->h_off = childH_OffResim;

							child->Refresh(UIRefreshFlags::SkipSelf | UIRefreshFlags::Post);
						}
					}

					lowestY = child->y_off + child->h_off + yDelta;// new furthest x position in this colum
					if (lowestY > y_off + h_off) // child's right side passes the end of the parent
					{
						lowestY = child->y_off + child->h_off;
						yDelta = 0;
						currentXOffset = heighestX;
					}

					float xPadding = child->x_off - x_off; // x padding from the parent x_off
					float xDelta = currentXOffset == 0 ? 0 : (currentXOffset + xPadding) - child->x_off; // how much this child needs to move to be to the right of the last control + its padding to the parent


					child->OffsetPosition(xDelta, yDelta);

					float width = child->x_off + child->w_off;
					if (heighestX < width)
						heighestX = width;

					if (classID & UIClassID::CBaseControlGroup)// scrollbar bound calc
					{
						C_BaseControlGroup* thisGroup = (C_BaseControlGroup*)this;

						float extraXDelta = heighestX - (x_off + w_off);
						if (!Clamped(extraXDelta, 0.f, 99999.f))
							thisGroup->scrollBarExtraLength = -extraXDelta - xPadding; // negative to signify x axis
						else
						{
							thisGroup->scrollBarExtraLength = 0.f;
							thisGroup->scrollBarPosition = 0.f;
							child_x_padding = 0.f;
						}
					}
				}
			}
		}
	}

	if (refresh_flags & UIRefreshFlags::Post)
		PostRefresh();
}
#pragma endregion C_BaseUIComponent

void C_Text::Render()
{
	if (!renderThis)
		return;

	C_BaseUIComponent::Render();

	textObj->x = x_off;
	textObj->y = y_off;
	textObj->Render();
}

void C_SwitchButton::Render()
{
	if (!renderThis)
		return;

	C_BaseUIComponent::Render();

	if (!active)
		return;

	if (focusThis)
	{
		if (I::inputs->IsMouseButtonPressed(GLFW_MOUSE_BUTTON_1) && PointInRectangle(I::inputs->particleMousePosition.x, I::inputs->particleMousePosition.y, x_off, y_off, w_off, h_off))
			*active = !*active;
	}

	if (*active)
	{
		CustRender::FilledRect(x_off, y_off, w_off, h_off, ColorM(1, 1, 1, 1));
	}
}


void C_MenuTab::Render()
{
	if (!renderThis)
		return;

	C_BaseUIComponent::Render();

	if (focusThis)
	{
		if (I::inputs->IsMouseButtonPressed(GLFW_MOUSE_BUTTON_1) && PointInRectangle(I::inputs->particleMousePosition.x, I::inputs->particleMousePosition.y, x_off, y_off, w_off, h_off))
		{
			if (!active)
			{
				for (int i = 0; i < menuParent->tabCount; i++)
				{
					C_MenuTab* tab = menuParent->tabs[i];
					tab->child->RenderActive(false);
					tab->active = false;
				}

				active = true;
				child->RenderActive(true);
			}
		}
	}
}


void C_BaseMenu::Render()
{
	if (!renderThis)
		return;

	C_BaseUIComponent::Render();

	if (I::inputs->IsMouseButtonHeld(GLFW_MOUSE_BUTTON_1))
	{

		if (!dragging && !resizing && focusThis && !childFocusedClassIDs)
		{
			ogMousePos = I::inputs->particleMousePosition;
			dragging = true;
		}
		else if (!dragging && !resizing && I::inputs->particleMousePosition.Distance(GetCornerPosition(BottomRight)) < 20.f)
		{
			resizing = true;
		}

		if (dragging)
		{
			if (ogMousePos != I::inputs->particleMousePosition)
			{
				ogMousePos = I::inputs->particleMousePosition - ogMousePos;
				OffsetPosition(ogMousePos.x, ogMousePos.y);

				ogMousePos = I::inputs->particleMousePosition;
			}
		}
		else if (resizing)
		{
			Vec2Df delta = I::inputs->particleMousePosition - GetCornerPosition(TopLeft);

			int lastX_Off = x_off;
			int lastY_Off = y_off;

			SetW(delta.x);
			SetH(delta.y);

			OffsetPosition(lastX_Off - x_off, lastY_Off - y_off);
		}
	}
	else
	{
		dragging = false;
		resizing = false;
	}
}