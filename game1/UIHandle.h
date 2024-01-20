#pragma once

#define Clamp(x,l,h) (x > h ? x = h : (x < l ? x = l : x))

class C_BaseUIComponent;

extern float testClosestDistance;
extern C_BaseUIComponent* testClosestComp;

inline bool PointInRectangle(int x, int y, int x1, int y1, int width1, int height1, Vec2Df* new_offset = nullptr, float* distance = nullptr)
{
	if (x >= x1 && x <= x1 + width1 && y >= y1 && y <= y1 + height1)
	{
		if (new_offset) // gets the closest point outside the box's perimeter
		{
			float xLDelta = x - x1;
			float xRDelta = (x1 + width1) - x;

			float yTDelta = y - y1;
			float yBDelta = (y1 + height1) - y;

			bool closerToLeft = xLDelta < xRDelta;
			bool closerToTop = yTDelta < yBDelta;

			if (closerToLeft)
			{
				if (closerToTop)
				{
					closerToTop = xLDelta > yTDelta;

					if (closerToTop)
					{
						new_offset->y -= yTDelta + 1;
						if (distance)
							*distance = fabs(yTDelta + 1);
					}
					else
					{
						new_offset->x -= xLDelta + 1;
						if (distance)
							*distance = fabs(xLDelta + 1);
					}
				}
				else
				{
					bool closerToBottom = yBDelta < xLDelta;

					if (closerToBottom)
					{
						new_offset->y += yBDelta + 1;
						if (distance)
							*distance = fabs(yBDelta + 1);
					}
					else
					{
						new_offset->x -= xLDelta + 1;
						if (distance)
							*distance = fabs(xLDelta + 1);
					}
				}
			}
			else
			{
				if (closerToTop)
				{
					closerToTop = xRDelta > yTDelta;

					if (closerToTop)
					{
						new_offset->y -= yTDelta + 1;
						if (distance)
							*distance = fabs(yTDelta + 1);
					}
					else
					{
						new_offset->x += xRDelta + 1;
						if (distance)
							*distance = fabs(xRDelta + 1);
					}
				}
				else
				{
					bool closerToBottom = yBDelta < xRDelta;

					if (closerToBottom)
					{
						new_offset->y += yBDelta + 1;
						if (distance)
							*distance = fabs(yBDelta + 1);
					}
					else
					{
						new_offset->x += xRDelta + 1;
						if (distance)
							*distance = fabs(xRDelta + 1);
					}
				}
			}
		}

		return true;
	}
	return false;
}

inline bool Clamped(float& val, float l, float h)
{
	float valcpy = val;
	Clamp(val, l, h);
	return valcpy != val;
}

inline bool PointOutRectangle(int x, int y, int x1, int y1, int width1, int height1, Vec2Df* new_offset = nullptr, float* distance = nullptr)
{
	if (!(x >= x1 && x <= x1 + width1 && y >= y1 && y <= y1 + height1))
	{
		if (new_offset) // gets the closest point inside the box's perimeter
		{
			/*	float xLDelta = fabs(x - x1);
				float xRDelta = fabs((x1 + width1) - x);

				float yTDelta = fabs(y - y1);
				float yBDelta = fabs((y1 + height1) - y);

				bool closerToLeft = xLDelta < xRDelta;
				bool closerToTop = yTDelta < yBDelta;

				if (closerToLeft)
					new_offset->x += xLDelta + 1;
				else
					new_offset->x -= xRDelta + 1;

				if (closerToTop)
					new_offset->y += yTDelta + 1;
				else
					new_offset->y -= yBDelta + 1;*/

			float xLDelta = x - x1;
			float xRDelta = (x1 + width1) - x;

			float yTDelta = y - y1;
			float yBDelta = (y1 + height1) - y;

			bool closerToLeft = xLDelta < xRDelta;
			bool closerToTop = yTDelta < yBDelta;

			if (closerToLeft)
			{
				if (closerToTop)
				{
					closerToTop = xLDelta > yTDelta;

					if (closerToTop)
					{
						new_offset->y -= yTDelta - 1;

						float xClamped = x;
						if (Clamped(xClamped, x1, x1 + width1))
						{
							new_offset->x += xClamped - x;

							if (distance)
							{
								Vec2Df og = { x, y };
								*distance = og.Distance(*new_offset);
							}
						}
						else if (distance)
						{
							*distance = fabs(yTDelta - 1);
						}
					}
					else
					{
						new_offset->x -= xLDelta - 1;

						float yClamped = y;
						if (Clamped(yClamped, y1, y1 + height1))
						{
							new_offset->y += yClamped - y;

							if (distance)
							{
								Vec2Df og = { x, y };
								*distance = og.Distance(*new_offset);
							}
						}
						else if (distance)
						{
							*distance = fabs(xLDelta - 1);
						}
					}
				}
				else
				{
					bool closerToBottom = yBDelta < xLDelta;

					if (closerToBottom)
					{
						new_offset->y += yBDelta - 1;

						float xClamped = x;
						if (Clamped(xClamped, x1, x1 + width1))
						{
							new_offset->x += xClamped - x;

							if (distance)
							{
								Vec2Df og = { x, y };
								*distance = og.Distance(*new_offset);
							}
						}
						else if (distance)
						{
							*distance = fabs(yBDelta - 1);
						}
					}
					else
					{
						new_offset->x -= xLDelta - 1;

						float yClamped = y;
						if (Clamped(yClamped, y1, y1 + height1))
						{
							new_offset->y += yClamped - y;

							if (distance)
							{
								Vec2Df og = { x, y };
								*distance = og.Distance(*new_offset);
							}
						}
						else if (distance)
						{
							*distance = fabs(xLDelta - 1);
						}
					}
				}
			}
			else
			{
				if (closerToTop)
				{
					closerToTop = xRDelta > yTDelta;

					if (closerToTop)
					{
						new_offset->y -= yTDelta - 1;

						float xClamped = x;
						if (Clamped(xClamped, x1, x1 + width1))
						{
							new_offset->x += xClamped - x;

							if (distance)
							{
								Vec2Df og = { x, y };
								*distance = og.Distance(*new_offset);
							}
						}
						else if (distance)
						{
							*distance = fabs(yTDelta - 1);
						}
					}
					else
					{
						new_offset->x += xRDelta - 1;

						float yClamped = y;
						if (Clamped(yClamped, y1, y1 + height1))
						{
							new_offset->y += yClamped - y;

							if (distance)
							{
								Vec2Df og = { x, y };
								*distance = og.Distance(*new_offset);
							}
						}
						else if (distance)
						{
							*distance = fabs(xRDelta - 1);
						}
					}
				}
				else
				{
					bool closerToBottom = yBDelta < xRDelta;

					if (closerToBottom)
					{
						new_offset->y += yBDelta - 1;

						float xClamped = x;
						if (Clamped(xClamped, x1, x1 + width1))
						{
							new_offset->x += xClamped - x;

							if (distance)
							{
								Vec2Df og = { x, y };
								*distance = og.Distance(*new_offset);
							}
						}
						else if (distance)
						{
							*distance = fabs(yBDelta - 1);
						}
					}
					else
					{
						new_offset->x += xRDelta - 1;

						float yClamped = y;
						if (Clamped(yClamped, y1, y1 + height1))
						{
							new_offset->y += yClamped - y;

							if (distance)
							{
								Vec2Df og = { x, y };
								*distance = og.Distance(*new_offset);
							}
						}
						else if (distance)
						{
							*distance = fabs(xRDelta - 1);
						}
					}
				}
			}
		}

		return true;
	}
	return false;
}

enum UIFlags
{
	FNone = 0,
	X_RelativeToParent = (1 << 0), // adds the x position to its parent then uses the sum
	Y_RelativeToParent = (1 << 1), // adds the y position to its parent then uses the sum
	X_CenterSelf = (1 << 2), // centers the object by its width
	Y_CenterSelf = (1 << 3), // centers the object by its height
	X_ScaledByParentWidth = (1 << 4), // sets the x position as a percentage of the parents width !!!()!!! !!!(dont use this flag if the direct parent's size is auto expanding)!!!
	Y_ScaledByParentHeight = (1 << 5), // sets the y position as a percentage of the parents height !!!(dont use this flag if the direct parent's size is auto expanding)!!!
	W_Reverse = (1 << 6), // reverses the width direction
	H_Reverse = (1 << 7), // reverses the height direction
	W_ScaledByParentWidth = (1 << 8), // sets the width based on a percentage of it's parent !!!(dont use this flag if the direct parent's size is auto expanding)!!!
	H_ScaledByParentHeight = (1 << 9), // sets the height based on a percentage of it's parent !!!(dont use this flag if the direct parent's size is auto expanding)!!!
	W_AutoExpand = (1 << 10), // expands the width to the size of it's children
	H_AutoExpand = (1 << 11), // expands the height to the size of it's children
	X_SequentialChildPlacement = (1 << 12), // places the children sequencialy in the X direction using their X_OFF as padding from the previous control !!!(the child must use the RelativeToParent flags)!!!
	Y_SequentialChildPlacement = (1 << 13), // places the children sequencialy in the Y direction using their Y_OFF as padding from the previous control !!!(the child must use the RelativeToParent flags)!!!
	X_ScaledByParentHeight = (1 << 14), // sets the X position as a percentage of the parents Height !!!(DONT USE THIS FLAG IF X_ScaledByParentWidth IS ACTIVE)!!! !!!(DONT USE THIS FLAG IF THE DIRECT PARENT IS H_AutoExpand)!!!
	Y_ScaledByParentWidth = (1 << 15), // sets the Y position as a percentage of the parents Width !!!(DONT USE THIS FLAG IF Y_ScaledByParentHeight IS ACTIVE)!!! !!!(DONT USE THIS FLAG IF THE DIRECT PARENT IS W_AutoExpand)!!!
	W_ScaledByParentHeight = (1 << 16),
	H_ScaledByParentWidth = (1 << 17),
	W_FromParentRightSide = (1 << 18), // the W_OFF will be used as an offset amount from the right side of its parent going inwards
	H_FromParentBottomSide = (1 << 19), // the H_OFF will be used as an offset amount from the top side of its parent going inwards
};

enum UIRefreshFlags
{
	UFNone = 0,
	Full = (1 << 0), // Refresh all dynamic parents in child's branch
	Post = (1 << 1),  // trigger uicomponent PostRefresh
	FullAndPost = (Full | Post), // Post and Full update flags
	Normal = (1 << 2), // normally refresh self and children (only nessesary if youre not passing any other flag)
	SkipSelf = (1 << 3) // only refresh children
	//!!!!!!!!!IF YOU ADD MORE CHANGE THE REFRESH FLAG TYPE ON ALL FUNCTIONS!!!!!!!!!
};

enum UICorners
{
	TopLeft,
	TopRight,
	BottomRight,
	BottomLeft
};

enum UIClassID
{
	CBaseUIComponent = (1 << 0),
	CBaseUIControl = (1 << 1),
	CText = (1 << 2),
	CSwitchButton = (1 << 3),
	CCheckBox = (1 << 4),
	CBaseControlGroup = (1 << 5),
	CMenuTab = (1 << 6),
	CBaseMenu = (1 << 7)
};

#define CompareBits(flag, bits) ((flag & (bits)) == (bits))

class C_BaseUIComponent
{
public:
	float x = 0; // base x
	float x_off = 0; // offset x position (for extra flags)/ rendering position
	float y = 0; // base y
	float y_off = 0; // offset y position (for extra flags)/ rendering position
	float w = 0; // base w
	float w_off = 0; // offset w size (for extra flags)/ rendering size
	float h = 0; // base h
	float h_off = 0; // offset h size (for extra flags)/ rendering size

	float child_x_padding = 0;
	float child_y_padding = 0;

	bool renderThis = false;
	bool focusThis = false;

	C_BaseUIComponent* parent = nullptr;

	size_t childCount = 0;
	C_BaseUIComponent** children = nullptr;

	unsigned long long flags = 0;

	CustRender::DefaultThemeFN theme = nullptr;

	unsigned int classID = 0;
	unsigned int childFocusedClassIDs = 0;

	inline void Reset()
	{
		x = 0;
		x_off = 0;
		y = 0;
		y_off = 0;
		w = 0;
		w_off = 0;
		h = 0;
		h_off = 0;
		child_x_padding = 0;
		child_y_padding = 0;
		parent = nullptr;
		renderThis = true;
		children = nullptr;
		childCount = 0;
		flags = 0;
		theme = nullptr;
		classID = UIClassID::CBaseUIComponent;
	}

	C_BaseUIComponent() {}

	C_BaseUIComponent(int x_, int y_, int w_, int h_)
	{
		DefaultConstructor(x_, y_, w_, h_);
	}

	virtual void XChanged() {};
	inline void SetX(float x_ = 0, BYTE refresh_flags = UIRefreshFlags::FullAndPost)
	{
		x_off = x_;

		{
			if (parent)
			{
				if (flags & X_ScaledByParentWidth)
					x_off = x_ * parent->w_off;
				else if (flags & X_ScaledByParentHeight)
					x_off = x_ * parent->h_off;

				if (flags & X_RelativeToParent)
					x_off += parent->x_off;
			}
		}

		x = x_; XChanged();

		if (refresh_flags > 0) // either or
			Refresh(refresh_flags);
	}

	virtual void YChanged() {};
	inline void SetY(float y_ = 0, BYTE refresh_flags = UIRefreshFlags::FullAndPost)
	{
		y_off = y_;

		{
			if (parent)
			{
				if (flags & Y_ScaledByParentHeight)
					y_off = y_ * parent->h_off;
				else if (flags & Y_ScaledByParentWidth)
					y_off = y_ * parent->w_off;

				if (flags & Y_RelativeToParent)
					y_off += parent->y_off;
			}
		}

		y = y_; YChanged();

		if (refresh_flags > 0) // either or
			Refresh(refresh_flags);
	}

	virtual void WChanged() {};
	inline void SetW(float w_ = 0, BYTE refresh_flags = UIRefreshFlags::FullAndPost)
	{
		w_off = w_;

		{
			if (parent)
			{
				if (flags & W_ScaledByParentWidth)
					w_off = w_ * parent->w_off;
				else if (flags & W_ScaledByParentHeight)
					w_off = w_ * parent->h_off;

				if (flags & W_FromParentRightSide)
				{
					float parentRightSide = parent->x_off + parent->w_off;
					float desiredPosition = parentRightSide - w_off;
					w_off = desiredPosition - x_off;
				}
			}

			if (flags & X_CenterSelf)
				x_off -= w_off * .5f;
			else if (flags & W_Reverse)
				x_off -= w_off;
		}

		w = w_; WChanged();

		if (refresh_flags > 0) // either or
			Refresh(refresh_flags);
	}

	virtual void HChanged() {};
	inline void SetH(float h_ = 0, BYTE refresh_flags = UIRefreshFlags::FullAndPost)
	{
		h_off = h_;

		{
			if (parent)
			{
				if (flags & H_ScaledByParentHeight)
					h_off = h_ * parent->h_off;
				else if (flags & H_ScaledByParentWidth)
					h_off = h_ * parent->w_off;

				if (flags & H_FromParentBottomSide)
				{
					float parentBottomSide = parent->y_off + parent->h_off;
					float desiredPosition = parentBottomSide - h_off;
					h_off = desiredPosition - y_off;
				}
			}

			if (flags & Y_CenterSelf)
				y_off -= h_off * .5f;
			else if (flags & H_Reverse)
				y_off -= h_off;
		}

		h = h_; HChanged();

		if (refresh_flags > 0) // either or
			Refresh(refresh_flags);
	}

	virtual void ParentChanged() {};
	inline void SetParent(C_BaseUIComponent* p_, BYTE refresh_flags = UIRefreshFlags::Post)
	{
		if (!p_ || p_ == parent)
			return;

		if (parent)
			parent->RemoveChild(this);

		p_->AddChild(this); ParentChanged();

		if ((refresh_flags & UIRefreshFlags::FullAndPost) && !p_->IsDynamic())
			Refresh(refresh_flags);
	}

	virtual void FlagsChanged() {};
	inline void SetFlags(unsigned long long f_, BYTE refresh_flags = UIRefreshFlags::FullAndPost)
	{
		if (flags == f_)
			return;

		flags = f_; FlagsChanged();

		if (refresh_flags > 0) // either or
			Refresh(refresh_flags);
	}

	virtual void Refresh(BYTE flags = UIRefreshFlags::UFNone);
	virtual void PostRefresh() {}; // !!!!(dont use the UIRefreshFlags::Post flag for any functions in PostRefresh)!!!!

	inline void DefaultConstructor(float x_, float y_, float w_, float h_, unsigned long long f_ = 0, C_BaseUIComponent* p_ = nullptr)
	{
		Reset();
		SetFlags(f_, UIRefreshFlags::Post);
		SetX(x_, UIRefreshFlags::Post);
		SetY(y_, UIRefreshFlags::Post);
		SetW(w_, UIRefreshFlags::Post);
		SetH(h_, UIRefreshFlags::Post);
		SetParent(p_, UIRefreshFlags::Post);

		theme = CustRender::DefaultTheme;
	}

	C_BaseUIComponent(float x_, float y_, float w_, float h_, unsigned long long f_ = 0, C_BaseUIComponent* parent_ = nullptr)
	{
		DefaultConstructor(x_, y_, w_, h_, f_, parent_);
	}

	C_BaseUIComponent(C_BaseUIComponent* parent_, int x_, int y_, int w_, int h_)
	{
		DefaultConstructor(x_, y_, w_, h_, UIFlags::X_RelativeToParent | UIFlags::Y_RelativeToParent, parent_);
	}

	C_BaseUIComponent(C_BaseUIComponent* parent_, float x_percent, float y_percent, float w_percent, float h_percent)
	{
		DefaultConstructor(x_percent, y_percent, w_percent, h_percent, UIFlags::X_RelativeToParent | UIFlags::Y_RelativeToParent |
			UIFlags::X_ScaledByParentWidth | UIFlags::Y_ScaledByParentHeight |
			UIFlags::W_ScaledByParentWidth | UIFlags::H_ScaledByParentHeight, parent_);
	}

	virtual void Render();

	inline bool AddChild(C_BaseUIComponent* child)
	{
		if (!child || child->parent == this)
			return false;

		child->parent = this;

		size_t newCount = childCount + 1;

		C_BaseUIComponent** newArray = new C_BaseUIComponent * [newCount];

		bool alreadyChild = false;

		for (size_t i = 0; i < childCount; i++)
		{
			if (child == children[i])
			{
				alreadyChild = true;
				break;
			}
			newArray[i] = children[i];
		}

		if (alreadyChild)
		{
			delete[] newArray;
			return false;
		}

		newArray[childCount] = child;
		childCount = newCount;

		delete[] children;
		children = newArray;

		if (IsDynamic())
			Refresh(UIRefreshFlags::FullAndPost);

		return true;
	}

	inline bool RemoveChild(C_BaseUIComponent* child)
	{
		if (!child || child->parent != this)
			return false;

		child->parent = nullptr;

		size_t newCount = childCount - 1;

		C_BaseUIComponent** newArray = new C_BaseUIComponent * [newCount];

		size_t shiftedCount = 0;

		for (size_t i = 0; i < childCount; i++)
		{
			if (child == children[i])
			{
				shiftedCount += 1;
				continue;
			}
			newArray[i - shiftedCount] = children[i];
		}

		if (shiftedCount == 0)
		{
			delete[] newArray;
			return false;
		}

		childCount = newCount;
		delete[] children;
		children = newArray;

		if (IsDynamic())
			Refresh(UIRefreshFlags::FullAndPost);

		return true;
	}

	inline bool RemoveChild(int childIndex)
	{
		if (childIndex >= childCount || childIndex < 0)
			return false;

		return RemoveChild(children[childIndex]);
	}

	inline bool DeleteChild(C_BaseUIComponent* child)
	{
		if (!child)
			return false;

		if (RemoveChild(child))
		{
			delete[] child;
			return true;
		}

		return false;
	}

	inline bool DeleteChild(int childIndex)
	{
		if (childIndex >= childCount || childIndex < 0)
			return false;

		return DeleteChild(children[childIndex]);
	}

	inline bool LeaveParent()
	{
		if (!parent)
			return false;

		return parent->RemoveChild(this);
	}

	inline void OffsetPosition(float x_, float y_)
	{
		x_off += x_;
		y_off += y_;

		for (int i = 0; i < childCount; i++)
		{
			children[i]->OffsetPosition(x_, y_);
		}
	}

	inline void Move(float x_, float y_, bool full_update = true)
	{
		SetX(x_, full_update);
		SetY(y_, full_update);
	}

	inline bool IsDynamic()
	{
		return (flags & (W_AutoExpand | H_AutoExpand | X_SequentialChildPlacement | Y_SequentialChildPlacement));
	}

	inline bool HasChildPadding()
	{
		return child_x_padding != 0.f || child_y_padding != 0.f;
	}

	inline Vec2Df GetCornerPosition(short corner_)
	{
		switch (corner_)
		{
		case 0:
		{
			return { x_off, y_off + h };
			break;
			
		}
		case 1:
		{
			return { x_off + w, y_off + h };
			break;
		}
		case 2:
		{	
			return { x_off + w, y_off };
			break;
		}
		case 3:
		{
			return { x_off, y_off };
			break;
		}
		}
	}

	inline void RenderActive(bool state)
	{
		if (state == renderThis)
			return;

		renderThis = state;

		Refresh(UIRefreshFlags::Full);
	}
};

class C_BaseUIControl : public C_BaseUIComponent
{
public:

	C_BaseUIControl() {}
};

class C_Text : public C_BaseUIComponent
{
public:
	std::string txt = "";
	CDText* textObj = nullptr;
	C_Text(float x_, float y_, std::string txt_, unsigned long long flags_ = 0, C_BaseUIComponent* parent_ = nullptr)
	{
		textObj = new CDText(x_, y_, txt_, CustRender::getFirstFont(), ColorM(1, 1, 1, 1), 2, 2, 0);
		txt = txt_;

		DefaultConstructor(x_, y_, textObj->GetWidth(), textObj->GetHeight(), flags_, parent_);

		classID = UIClassID::CText;

		theme = nullptr;
	}

	inline void SetText(std::string txt_, int refresh_flags = UIRefreshFlags::FullAndPost)
	{
		txt = txt_;
		textObj->ChangeText(txt_);
		SetW(textObj->GetWidth(), refresh_flags);
		SetH(textObj->GetHeight(), refresh_flags);
	}

	virtual void Render();
};

class C_SwitchButton : public C_BaseUIComponent
{
public:
	bool* active = nullptr;
	C_SwitchButton(float x_, float y_, float w_, float h_, unsigned long long f_ = 0, C_BaseUIComponent* parent_ = nullptr)
	{
		DefaultConstructor(x_, y_, w_, h_, f_, parent_);

		active = new bool;
		*active = false;

		classID = UIClassID::CSwitchButton;
	}

	virtual void Render();

	inline void ChangeButtonPtr(bool* ptr, bool delete_old = true)
	{
		if (delete_old && active)
			delete[] active;

		active = ptr;
	}
};

class C_CheckBox : public C_BaseUIControl
{
public:
	C_Text* text = nullptr;
	C_SwitchButton* button = nullptr;

	C_CheckBox(C_BaseUIComponent* parent_, std::string txt_, bool* var = nullptr)
	{
		DefaultConstructor(.05f, .05f, 15.f, 7.f, UIFlags::X_RelativeToParent | UIFlags::Y_RelativeToParent | UIFlags::X_ScaledByParentWidth | UIFlags::Y_ScaledByParentHeight, parent_);
		//	theme = nullptr;

		text = new C_Text(0.f, .5f, txt_, UIFlags::X_RelativeToParent | UIFlags::Y_RelativeToParent | UIFlags::X_ScaledByParentWidth | UIFlags::Y_ScaledByParentHeight | UIFlags::Y_CenterSelf, this);
		button = new C_SwitchButton(1.f, .5f, 0.f, 1.f, UIFlags::X_RelativeToParent | UIFlags::Y_RelativeToParent |
			UIFlags::Y_CenterSelf | UIFlags::X_ScaledByParentWidth | UIFlags::Y_ScaledByParentHeight | UIFlags::W_Reverse | UIFlags::H_ScaledByParentHeight, this);

		if (var)
			button->ChangeButtonPtr(var);

		button->SetW(button->h_off);
		SetW(text->w_off + button->w_off + 5);

		classID = UIClassID::CCheckBox;

		theme = nullptr;
	}

	virtual void PostRefresh()
	{
		if (!button)
			return;

		//	button->SetW(button->h_off, UIRefreshFlags::Normal);
		//	SetW(text->w_off + button->w_off + 5, UIRefreshFlags::Normal);
	}

	/*	virtual void Render()
		{
			for (int i = 0; i < childCount; i++)
			{
				children[i]->Render();
			}
		}*/
};

class C_BaseControlGroup : public C_BaseUIComponent
{
public:
	float scrollBarPosition = 0.f;
	float scrollBarExtraLength = 0.f;

	C_BaseControlGroup(float x_, float y_, float w_, float h_, unsigned long long f_ = 0, C_BaseUIComponent* parent_ = nullptr)
	{
		DefaultConstructor(x_, y_, w_, h_, f_ | UIFlags::X_RelativeToParent | UIFlags::Y_RelativeToParent, parent_);

		classID = UIClassID::CBaseControlGroup;
	}
};

class C_BaseMenu;
class C_MenuTab : public C_BaseUIComponent
{
public:
	C_BaseMenu* menuParent = nullptr;
	C_BaseUIComponent* child = nullptr;
	//std::string name = "";
	C_MenuTab(float x_, float y_, float w_, float h_, unsigned long long f_ = 0, C_BaseUIComponent* child_ = nullptr, std::string name_ = "", C_BaseUIComponent* parent_ = nullptr)
	{
		DefaultConstructor(x_, y_, w_, h_, f_ | UIFlags::X_RelativeToParent | UIFlags::Y_RelativeToParent | UIFlags::X_ScaledByParentWidth | UIFlags::Y_ScaledByParentHeight | UIFlags::W_ScaledByParentWidth | UIFlags::H_ScaledByParentHeight, parent_);

		child = child_;
		active = false;
		new C_Text(.5f, .5f, name_, UIFlags::X_RelativeToParent | UIFlags::Y_RelativeToParent | UIFlags::X_ScaledByParentWidth | UIFlags::Y_ScaledByParentHeight | UIFlags::Y_CenterSelf | UIFlags::X_CenterSelf, this);

		classID = UIClassID::CMenuTab;
	}

	bool active = false;

	virtual void Render();
};

class C_BaseMenu : public C_BaseUIComponent
{
public:
	int tabCount = 0;
	C_MenuTab** tabs = nullptr;

	Vec2Df ogMousePos = {};
	bool dragging = false;
	bool resizing = false;


	C_BaseMenu(int x_, int y_, int w_, int h_)
	{
		DefaultConstructor(x_, y_, w_, h_);

		classID = UIClassID::CBaseMenu;
		ogMousePos.Clear();
		dragging = false;
		resizing = false;
	}

	inline bool AddTab(C_MenuTab* child)
	{
		if (!child)
			return false;

		size_t newCount = tabCount + 1;

		C_MenuTab** newArray = new C_MenuTab * [newCount];

		bool alreadyTab = false;

		for (size_t i = 0; i < tabCount; i++)
		{
			if (child == tabs[i])
			{
				alreadyTab = true;
				break;
			}
			newArray[i] = tabs[i];
		}

		if (alreadyTab)
		{
			delete[] newArray;
			return false;
		}

		newArray[tabCount] = child;
		tabCount = newCount;

		delete[] tabs;
		tabs = newArray;

		child->menuParent = this;

		if (tabCount != 1)
		{
			child->active = false;
			child->child->RenderActive(false);
		}

		return true;
	}

	inline bool RemoveTab(C_MenuTab* child)
	{
		if (!child)
			return false;

		size_t newCount = tabCount - 1;

		C_MenuTab** newArray = new C_MenuTab * [newCount];

		size_t shiftedCount = 0;

		for (size_t i = 0; i < tabCount; i++)
		{
			if (child == tabs[i])
			{
				shiftedCount += 1;
				continue;
			}
			newArray[i - shiftedCount] = tabs[i];
		}

		if (shiftedCount == 0)
		{
			delete[] newArray;
			return false;
		}

		tabCount = newCount;
		delete[] tabs;
		tabs = newArray;

		child->menuParent = nullptr;

		return true;
	}

	virtual void Render();
};