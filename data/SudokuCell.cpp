#include "SudokuCell.h"

Sudoku::Cell::Cell()
	: mEditable(false), 
	  mCharNumber(' '), 
	  mCharSolution(' ')
{

}

void Sudoku::Cell::setNumber(const int number)
{
	if (number == 0)
	{
		mCharNumber = ' ';
	}
	else
	{
		mCharNumber = '0' + number;
	}
}

char Sudoku::Cell::getNumber() const
{
	return mCharNumber;
}

void Sudoku::Cell::setSolution(const int solution)
{
	if (solution == 0)
	{
		mCharSolution = ' ';
	}
	else
	{
		mCharSolution = '0' + solution;
	}
}

void Sudoku::Cell::setEditable(const bool editable)
{
	mEditable = editable;
	if (mEditable)
	{
		mMouseOutColour = { 255, 255, 255, SDL_ALPHA_OPAQUE }; // white
		mMouseOverMotionColour = { 95, 89, 191, SDL_ALPHA_OPAQUE }; // blue
		mMouseDownColour = { 91, 191, 116, SDL_ALPHA_OPAQUE }; // green
		mMouseUpColour = { 95, 89, 191, SDL_ALPHA_OPAQUE }; // blue
	}
	else
	{
		mMouseOutColour = { 230, 230, 250, SDL_ALPHA_OPAQUE }; // lavender
		mMouseOverMotionColour = { 230, 230, 250, SDL_ALPHA_OPAQUE }; // lavender
		mMouseDownColour = { 230, 230, 250, SDL_ALPHA_OPAQUE }; // lavender
		mMouseUpColour = { 230, 230, 250, SDL_ALPHA_OPAQUE }; // lavender
	}
}

bool Sudoku::Cell::isEditable() const
{
	return mEditable;
}

void Sudoku::Cell::handleKeyboardEvent(const SDL_Event* event, SDL_Texture* textureCache[])
{
	// Xử lý nút backspace
	if (event->key.keysym.sym == SDLK_BACKSPACE && mCharNumber != ' ')
	{
		//  Xóa ký tự
		mCharNumber = ' ';

		// Set texture trống
		setTexture(textureCache[0]);
	}
	// Xử lý đầu vào văn bản
	else if (event->type == SDL_TEXTINPUT)
	{
		// Kiểm tra nếu là số nguyên dương
		if (atoi(event->text.text))
		{
			// Thay thế ký tự
			mCharNumber = *(event->text.text);

			// Đặt ký tự trên texture dựa trên số nhập vào
			setTexture(textureCache[atoi(event->text.text)]);

		}
	}
}


bool Sudoku::Cell::isCorrect() const
{
	return mCharNumber == mCharSolution;
}

