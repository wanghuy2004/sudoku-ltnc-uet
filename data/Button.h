/* A button class with colours and a texture*/
#pragma once
#include <SDL2/SDL.h>

enum class ButtonState
{
	BUTTON_MOUSE_OUT = 0,
	BUTTON_MOUSE_OVER_MOTION = 1,
	BUTTON_MOUSE_DOWN = 2,
	BUTTON_MOUSE_UP = 3,
};

class Button
{

private:
	//Trạng thái hiện tại của nút
	ButtonState mCurrentState;

	//Texture được sử dụng để hiển thị hình ảnh trên nút
	SDL_Texture* mTexture;

	//tạo vị trí và kích thước của button
	SDL_Rect mButtonRect; //toạ độ và kích thước của nút
	SDL_Rect mTextureRect; //Tọa độ và kích thước của texture được hiển thị trên nút

	//Biến đánh dấu xem nút đã được chọn bằng chuột hay chưa
	bool mSelected;

protected:
	//khởi tạo màu của button
	SDL_Color mMouseOutColour;
	SDL_Color mMouseOverMotionColour;
	SDL_Color mMouseDownColour;
	SDL_Color mMouseUpColour;

public:
	//Cấu trúc
	Button();

	//Tạo cấu trúc cho nút
	void setTexture(SDL_Texture* texture);

	//thiết lập vị trí và kích thước của nút
	void setButtonRect(const SDL_Rect& rect);
	void setTextureRect(const SDL_Rect& rect);

	//chỉnh cấu trúc của button để nằm ở giữa
	void centerTextureRect();

	//nếu có gì đượ chọn
	void setSelected(const bool selected);

	//nếu trỏ chuột bên trong button
	bool isMouseInside(const int x, const int y);
	
	//xử lý các event của chuột
	ButtonState getMouseEvent(const SDL_Event* event);

	//khi mà chuột trỏ xuống đâu thì hiện màu ở đó
	void setMouseDownColour(const SDL_Color& colour);

	//render ra button và texture
	void renderButton(SDL_Renderer* renderer);
	void renderTexture(SDL_Renderer* renderer);

};