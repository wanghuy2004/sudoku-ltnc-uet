#include "Button.h"

//hàm khởi tạo (constructor) của lớp, nó thiết lập trạng thái ban đầu của nút, kích thước và màu sắc của nó
Button::Button()
	: mCurrentState(ButtonState::BUTTON_MOUSE_OUT),
	  mTexture(nullptr),
	  mButtonRect({ 0, 0, 0, 0 }),
	  mTextureRect({0, 0, 0, 0}),
	  mMouseOutColour({ 255, 255, 255, SDL_ALPHA_OPAQUE }), // lavender
	  mMouseOverMotionColour({ 95, 89, 191, SDL_ALPHA_OPAQUE }),// blue
	  mMouseDownColour({ 91, 191, 116, SDL_ALPHA_OPAQUE }), // green
	  mMouseUpColour({ 95, 89, 191, SDL_ALPHA_OPAQUE }), // blue
	  mSelected(false)
{

}

//đặt cấu tạo(texture) cho nút. Đây là hình ảnh được hiển thị trên nút
void Button::setTexture(SDL_Texture* texture)
{
	mTexture = texture;
}

//đặt kích thước và vị trí của nút trong không gian màn hình
void Button::setButtonRect(const SDL_Rect& rect)
{
	mButtonRect = rect;
}

//đặt kích thước và vị trí của texture trên nút
void Button::setTextureRect(const SDL_Rect& rect)
{
	mTextureRect = rect;
}

//đặt vị trí của cấu tạo(texture) ở giữa nút
void Button::centerTextureRect()
{
	int textureWidth;
	int textureHeight;
	SDL_QueryTexture(mTexture, NULL, NULL, &textureWidth, &textureHeight);

	const int textureStartRow = mButtonRect.y + 0.5 * (mButtonRect.h - textureHeight);
	const int textureStartCol = mButtonRect.x + 0.5 * (mButtonRect.w - textureWidth);

	mTextureRect = { textureStartCol, textureStartRow, textureWidth, textureHeight };
}

//đặt trạng thái được chọn của button
void Button::setSelected(const bool selected)
{
	mSelected = selected;
}

//kiểm tra chuột có nằm trong vùng button hay không
bool Button::isMouseInside(const int x, const int y)
{
	//kiểm tra nếu chuột trong nút
	bool inside = true;

	//nếu chuột ở bên trái button
	if (x < mButtonRect.x)
	{
		inside = false;
	}
	//nếu chuột ở bên phải button
	else if (x > mButtonRect.x + mButtonRect.w)
	{
		inside = false;
	}
	//nếu chuột ở bên trên button
	else if (y < mButtonRect.y)
	{
		inside = false;
	}
	//nếu chuột ở bên dưới button
	else if (y > mButtonRect.y + mButtonRect.h)
	{
		inside = false;
	}

	return inside;
}

/*Hàm getMouseEvent() sẽ được gọi mỗi khi một sự kiện chuột được phát sinh và từ đó, trạng thái của nút được cập nhật.*/
//xác định trạng thái của nút dựa trên event của chuột (SDL_Event)
ButtonState Button::getMouseEvent(const SDL_Event* event)
{
	//nếu event của mouse xảy ra
	if (event->type == SDL_MOUSEMOTION || event->type == SDL_MOUSEBUTTONDOWN || event->type == SDL_MOUSEBUTTONUP)
	{
		//lấy vị trí chuột
		int x, y;
		SDL_GetMouseState(&x, &y);

		//kiểm tra xem chuột đang ở ngoài
		if (!isMouseInside(x, y))
		{
			mCurrentState = ButtonState::BUTTON_MOUSE_OUT;
		}
		//nếu chuột ở bên trong
		else
		{
			//cập nhật trạng thái của chuột
			switch (event->type)
			{
			case SDL_MOUSEMOTION:
				mCurrentState = ButtonState::BUTTON_MOUSE_OVER_MOTION;
				break;

			case SDL_MOUSEBUTTONDOWN:
				mCurrentState = ButtonState::BUTTON_MOUSE_DOWN;
				break;

			case SDL_MOUSEBUTTONUP:
				mCurrentState = ButtonState::BUTTON_MOUSE_UP;
				break;

			}
		}
	}
	return mCurrentState;
}

//khi mà chuột trỏ tới đâu thì hiện màu tới đó
void Button::setMouseDownColour(const SDL_Color& colour)
{
	mMouseDownColour = colour;
}

//hiển thị render chuột
void Button::renderButton(SDL_Renderer* renderer)
{
	//nếu button được chọn thì in ra màu 
	if (mSelected)
	{
		SDL_SetRenderDrawColor(renderer, mMouseDownColour.r, mMouseDownColour.g, mMouseDownColour.b, mMouseDownColour.a);
	}
	else
	{
		//đang ở vị trí nào thì in ra màu ở vị trí đó
		switch (mCurrentState)
		{
		case ButtonState::BUTTON_MOUSE_OUT:
			SDL_SetRenderDrawColor(renderer, mMouseOutColour.r, mMouseOutColour.g, mMouseOutColour.b, mMouseOutColour.a);
			break;
		case ButtonState::BUTTON_MOUSE_OVER_MOTION:
			SDL_SetRenderDrawColor(renderer, mMouseOverMotionColour.r, mMouseOverMotionColour.g, mMouseOverMotionColour.b, mMouseOverMotionColour.a);
			break;
		case ButtonState::BUTTON_MOUSE_DOWN:
			SDL_SetRenderDrawColor(renderer, mMouseDownColour.r, mMouseDownColour.g, mMouseDownColour.b, mMouseDownColour.a);
			break;
		case ButtonState::BUTTON_MOUSE_UP:
			SDL_SetRenderDrawColor(renderer, mMouseUpColour.r, mMouseUpColour.g, mMouseUpColour.b, mMouseUpColour.a);
			break;

		}
	}

//vẽ hình chữ nhật đặc tại vị trí và kích thước đã được định nghĩa trước đó cho nút bấm (mButtonRect)
//Màu sắc của hình chữ nhật sẽ phụ thuộc vào trạng thái hiện tại của nút bấm
	SDL_RenderFillRect(renderer, &mButtonRect);
}

//in ra các cấu trúc trên nút
void Button::renderTexture(SDL_Renderer* renderer)
{
	// render ra khoảng cách
	SDL_RenderCopy(renderer, mTexture, nullptr, &mTextureRect);
}
