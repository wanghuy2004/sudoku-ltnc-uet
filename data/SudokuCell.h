#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <stdlib.h> // for atoi

#include "Button.h"

namespace Sudoku
{
	class Cell : public Button
	{
	private:
		// định nghĩa khả năng chỉnh sửa (có thể nhập số vào ô)
		bool mEditable;

		// Xác định số hiện đang hiển thị và số của solution
		char mCharNumber;
		char mCharSolution;

	public:
		// kiến trúc
		Cell();

		// set và lấy số
		void setNumber(const int number);
		char getNumber() const;

		// Set solution
		void setSolution(const int solution);

		// set và lấy khả năng chỉnh sửa (thiết lập khả năng chỉnh sửa thay đổi map màu cho button)
		void setEditable(const bool editable);
		bool isEditable() const;

		// xử lý các event
		void handleKeyboardEvent(const SDL_Event* event, SDL_Texture* textureCache[]);

		// so sánh số với solution
		bool isCorrect() const;

	};

};