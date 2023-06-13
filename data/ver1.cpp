#include "Sudoku.h"



//----------------------------------Constructor and Destructor---------------------------------//
//----------------------------------Tạo và Huỷ---------------------------------//
Sudoku::Sudoku::Sudoku()
	: mWindowHeight(800), mWindowWidth(720), //tạo kích thước màn hình
	  mGridHeight(800), mGridWidth(720), //chiều cao và rộng của lưới đều là 720
	  mGridRows(9), mGridCols(9), //số hàng và cột trong lưới
	  mWindow(nullptr), mRenderer(nullptr), //con trỏ đến cửa sổ và con trỏ đến renderer được mặc định là không có gì
	  mTotalTextures(14), mTextureCache{ nullptr }, //tổng số texture của trò chơi, con trỏ chứa các texture của trò chơi
	  mFont(nullptr), mFontSize(mGridHeight/9), //Con trỏ đến font chữ và font chữ được đọ dài của lưới chia 9
	  mTotalCells(81), //tổng các ô là 81
	  mClearColour({ 0, 0, 0, SDL_ALPHA_OPAQUE }) //màu sắc của màn hình khi xoá
{

}

Sudoku::Sudoku::~Sudoku()
{
	freeTextures(); //giải phóng bộ nhớ cho các texture của trò chơi
	close(); //đóng game
}

//--------------------------------------Private----------------------------------------//
bool Sudoku::Sudoku::initialiseSDL()
{
	//tạo giá trị khởi tạo thành công = true
	bool success = true;

	//Khởi tạo SDL video subsystem 
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		std::cout << "SDL could not intialise! Error: " << SDL_GetError() << std::endl;
		success = false;
	}

	//Khởi tạo font chữ
	if (TTF_Init() == -1)
	{
		std::cout << "SDL_ttf could not initialise! Error: " << TTF_GetError() << std::endl;
		success = false;
	}

	//Tạo cửa sổ ra window
	mWindow = SDL_CreateWindow("Game Sudoku", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, mWindowWidth, mWindowHeight, SDL_WINDOW_SHOWN);
	if (mWindow == nullptr)
	{
		std::cout << "SDL could not create window! Error: " << SDL_GetError() << std::endl;
		success = false;
	}

	// Tạo các kết xuất(render)
	mRenderer = SDL_CreateRenderer(mWindow, -1, 0);
	if (mRenderer == nullptr)
	{
		std::cout << "SDL could not create renderer! Error: " << SDL_GetError() << std::endl;
		success = false;
	}

	//load font chữ
	mFont = TTF_OpenFont("assets/font.ttf", mFontSize);
	if (mFont == nullptr)
	{
		std::cout << "Failed to load lazy font! Error: " << TTF_GetError() << std::endl;
		success = false;
	}

	return success; //trả về thành công, còn không thì trả về các lỗi tương ứng
}

inline int Sudoku::Sudoku::getIndex(int row, int col) const //tính toán chỉ số của một ô trong mảng mCells
{
	return row * mGridRows + col; //công thức tính 
/* mGridRows là số hàng của bảng Sudoku (thường là 9).
Ví dụ, nếu ta muốn tính chỉ số của ô ở hàng 3, cột 4 thì ta sẽ sử dụng công thức getIndex(3, 4) = 3 * 9 + 4 = 31.
Chỉ số này tương ứng với phần tử thứ 31 trong mảng 1 chiều, và là ô ở hàng 3, cột 4 trong bảng Sudoku.*/
}

void Sudoku::Sudoku::loadTexture(SDL_Texture*& texture, const char* text, SDL_Color& fontColour)
{
	//Tạo bề mặt 
	SDL_Surface* textSurface = TTF_RenderText_Solid(mFont, text, fontColour);
	if (textSurface == nullptr)
	{
		std::cout << "Could not create TTF SDL_Surface! Error: " << TTF_GetError() << std::endl;
	}
	else
	{
		//Tạo cấu trức từ trên từng pixel của surface
		texture = SDL_CreateTextureFromSurface(mRenderer, textSurface);
		if (texture == nullptr)
		{
			std::cout << "Could not create texture from surface! Error: " << SDL_GetError() << std::endl;
		}
		SDL_FreeSurface(textSurface);
	}
}

void Sudoku::Sudoku::preloadTextures() //tải các texture lên bộ nhớ trước khi hiển thị chúng trên màn hình
{
	//chọn màu cho font
	SDL_Color fontColour = { 0, 0, 0, SDL_ALPHA_OPAQUE }; // black

	//load các cấu trúc cho các khoảng trắng
	loadTexture(mTextureCache[0], " ", fontColour);

	//load texture cho các số từ 1 tới 9
	for (int num = 1; num < 10; num++)
	{
	const char temp[] = { static_cast<char>('0' + num), '\0' };  //ép kiểu int sang char
	loadTexture(mTextureCache[num], temp, fontColour);
	}

	//load cấu trúc cho các nút "Kiem tra" và "Moi"
	loadTexture(mTextureCache[10], "Kiem Tra", fontColour);
	loadTexture(mTextureCache[11], "Moi", fontColour);
	loadTexture(mTextureCache[12], "Sai!", fontColour);
	loadTexture(mTextureCache[13], "Dung!", fontColour);
}

void Sudoku::Sudoku::createInterfaceLayout()
{
	const int thinBorder = 2;
	const int thickBorder = thinBorder + 5;

	/*----------------------------------------Hiển thị chữ "SUDOKU"------------------------------------------*/
	//Làm phần chữ không thể bấm được gì trong đó
	int buttonStartRow = 0;
	int buttonWidth = mGridWidth - 2 * thickBorder;
	// mWindowHeight = buttonHeight + 6 * thinBorder + 6 * thickBorder (tính theo phương trình này)
	int buttonHeight = (mWindowHeight - 6 * thinBorder - 6 * thickBorder) / 11; //chiều cao của chữ "SUDOKU" trong màn hình

	buttonStartRow += thickBorder;
	int buttonStartCol = 0;
	buttonStartCol += thickBorder;
	//set vị trí và kích thước của button
	SDL_Rect buttonRect = { buttonStartCol, buttonStartRow, buttonWidth, buttonHeight };
	mTimer.setButtonRect(buttonRect);
	/*----------------------------------Tạo button cho từng pixel-------------------------------------*/
	//Định nghĩa kích thước của ô
	// mGridWidth = 6 * thinBorder + 4 * thickBorder + 9 * buttonWidth (dựa trên phương trình này)
	buttonWidth = (mGridWidth - 6 * thinBorder - 4 * thickBorder) / mGridCols; //độ rộng của từng nút trong màn hình

	//tiếp tục từ hàng trước đó
	buttonStartRow += buttonHeight;

	//set vị trí và kích thước của button cho từng cell
	for (int gridRow = 0; gridRow < mGridRows; gridRow++)
	{
		//thêm vào hàng đầu tien
		if (gridRow == 0) buttonStartRow += thickBorder;
		else if (gridRow % 3 == 0) buttonStartRow += buttonHeight + thickBorder;
		else buttonStartRow += buttonHeight + thinBorder;

		//reset lại cột đầu tiên
		int buttonStartCol = 0;

		for (int gridCol = 0; gridCol < mGridCols; gridCol++)
		{
			//thêm vào cột đầu tiên
			if (gridCol == 0) buttonStartCol += thickBorder;
			else if (gridCol % 3 == 0) buttonStartCol += buttonWidth + thickBorder;
			else buttonStartCol += buttonWidth + thinBorder;

			//set vị trí và kích thước của button
			SDL_Rect buttonRect = { buttonStartCol, buttonStartRow, buttonWidth, buttonHeight };
			int index = getIndex(gridRow, gridCol);
			mGrid[index].setButtonRect(buttonRect);
		}
	}

	/*----------------------------------Tạo nút "Kiem Tra" và "Moi"-------------------------------------*/
	const int numberOfOtherButtons = 2;
	mCheckButton.setTexture(mTextureCache[10]);
	mNewButton.setTexture(mTextureCache[11]);
	Button* otherButtons[numberOfOtherButtons] = { &mCheckButton, &mNewButton };

	//tái định nghĩa lại độ rộng
	// mGridWidth = 3 * thickBorder + 9 * numberOfOtherButtons (dựa trên phương trình này)
	buttonWidth = (mGridWidth - 3 * thickBorder) / numberOfOtherButtons; //độ trộng của button

	//tiếp tục từ hàng đầu tiên
	buttonStartRow += buttonHeight + thickBorder;

	//reset lại cột đầu tiên
	int borderWidthTotal = 0;

	//set ra nút "Kiem Tra" và "Moi"
	for (int button = 0; button < numberOfOtherButtons; button++) // colBlock là mỗi 3 cột của cell
	{
		//thêm chiều rộng của đường viền 
		if (button == 0) borderWidthTotal += thickBorder;
		else borderWidthTotal += thickBorder;
		int buttonStartCol = button * buttonWidth + borderWidthTotal;

		//set vị trí và kích thước của button
		SDL_Rect buttonRect = { buttonStartCol, buttonStartRow, buttonWidth, buttonHeight };
		otherButtons[button]->setButtonRect(buttonRect);
	}

}


void Sudoku::Sudoku::generateSudoku()
{
	//tạo một lưới trống 81 ô để chứa các con số của game
	int generatedGrid[81] = { };

	//tạo một lưới trống 81 ô để chứa các giải pháp của game
	int solution[81] = { };

	//tạo ra một trò chơi Sudoku với lưới trống
	Generator G;
	G.generate(generatedGrid, solution);

	for (int i = 0; i < 81; i++)
	{
		//set số và giải pháp cho từng ô
		mGrid[i].setNumber(generatedGrid[i]);
		mGrid[i].setSolution(solution[i]); 

		//set khả năng chỉnh sửa
		if (generatedGrid[i] == 0)
		{
			//nếu là 1 ô trống thì được đánh dấu là có khả năng chỉnh sửa
			mGrid[i].setEditable(true);
		}
		else
		{
			//nếu không thì, ô đó được dánh dấu là không thể chỉnh sửa
			mGrid[i].setEditable(false);
		}

		// thiết lập texture cho từng ô với các số từ 1->9
		mGrid[i].setTexture(mTextureCache[generatedGrid[i]]);

		//căn giữa texture trên button
		mGrid[i].centerTextureRect();

	}
}

//giải phóng các texture
void Sudoku::Sudoku::freeTextures()
{
	for (int i = 0; i < mTotalTextures; i++)
	{
		//nếu bộ nhớ có chứa các texture thì giải phóng nó
		if (mTextureCache[i] != nullptr)
		{
			SDL_DestroyTexture(mTextureCache[i]);
			mTextureCache[i] = nullptr;
		}
	}
}

//--------------------------------------Public----------------------------------------//


//hàm chính của game//
void Sudoku::Sudoku::play()
{
	//khởi tạo sdl
	if (!initialiseSDL())
	{
		close(); //nếu khởi tạo không thành công thì đóng game
	}

	//tải trước các texture cần thiết cho lưới sudoku và các nút
	preloadTextures();

	//tạo giao diện người dùng cho game
	createInterfaceLayout();

	//tạo ra một trò chơi Sudoku mới và thiết lập số và giải pháp cho từng ô
	//và cũng tạo thêm khả năng chỉnh sửa cho mỗi ô
	generateSudoku();

	//thiết lập ô đầu tiên được chọn là ô đầu tiên có thể chỉnh sửa
	//lặp lại từng ô trong lưới để tìm ô đầu tiên có thể chỉnh sửa và đặt ô đó là ô được chọn
	Cell* currentCellSelected = &mGrid[0];
	for (int cell = 0; cell < mTotalCells; cell++)
	{
		if (mGrid[cell].isEditable())
		{
			currentCellSelected = &mGrid[cell];
			currentCellSelected->setSelected(true);
			break;
		}
	}

	//cho phép người chơi nhập văn bản
	SDL_StartTextInput();

	//vòng lặp chính để xử lý các sự kiện và hiển thị trò chơi trên màn hình
	SDL_Event event;
	bool stop = false;
	bool completed = false;
	bool generateNewSudoku = false;
	bool checkSolution = false;

	//đo thời gian cho nút "Kiem Tra"
	bool measureTimeForCheckButton = false;
	time_t startTimeForCheckButton;

	//thời gian
	time_t startTimer = time(NULL);
	time(&startTimer);

	//Game loop
	while (!stop)
	{
		//Xử lý các sự kiện trên hàng đợi
		while (SDL_PollEvent(&event) != 0)
		{
			//Xử lý thoát và hoàn thành
			if (event.type == SDL_QUIT)
			{
				//tạo flag stop
				stop = true;
			}
			//Xử lý event cho nút "Kiem Tra"
			if (mCheckButton.getMouseEvent(&event) == ButtonState::BUTTON_MOUSE_DOWN)
			{
				// tạo flag cho nút Kiem Tra = true
				checkSolution = true;
			}
			//Xử lý event cho nút "Moi"
			if (mNewButton.getMouseEvent(&event) == ButtonState::BUTTON_MOUSE_DOWN)
			{
				//tạo flag cho game sudoku mới = true
				generateNewSudoku = true;
			}
			//Xử lý các event của chuột cho từng cell
			for (int cell = 0; cell < mTotalCells; cell++)
			{
				//Nếu nó có thể chỉnh sửa
				if (mGrid[cell].isEditable())
				{
					//Đặt trạng thái nút và trả về nếu nhấn chuột trên ô
					if (mGrid[cell].getMouseEvent(&event) == ButtonState::BUTTON_MOUSE_DOWN)
					{
						//nếu chọn ô mới thì chỉnh ô được chọn trước đó thành false
						currentCellSelected->setSelected(false);
						
						//ô được chọn hiện tại chỉnh thành true
						currentCellSelected = &mGrid[cell];
						currentCellSelected->setSelected(true);
					}
				}
			}
			//Xử lý các event từ bàn phím cho cell đang được chọn
			currentCellSelected->handleKeyboardEvent(&event, mTextureCache);
		}
		//nếu "Moi" được click vào
		if (generateNewSudoku)
		{
			//Tạo game mới
			generateSudoku();

			//đặt các cell hiện tại thành false hết
			currentCellSelected->setSelected(false);

			//tìm các ô khởi đầu mới
			for (int cell = 0; cell < mTotalCells; cell++)
			{
				if (mGrid[cell].isEditable())
				{
					currentCellSelected = &mGrid[cell];
					currentCellSelected->setSelected(true);
					break;
				}
			}

			//reset lại các flag
			generateNewSudoku = false;
			completed = false;

			//reset lại thời gian
			time(&startTimer);
		}
		//nếu nút "Kiem Tra" được bấm
		if (checkSolution)
		{
			//nếu người chơi hoàn thành và bấm "Kiem Tra"
			for (int cell = 0; cell < mTotalCells; cell++)
			{
				if (!mGrid[cell].isCorrect())
				{
					completed = false;
					break;
				}
				completed = true;
			}

			//Đặt cờ đo thời gian và thời gian bắt đầu
			measureTimeForCheckButton = true;
			time(&startTimeForCheckButton);

			//reset lại flag
			checkSolution = false;
		}
		//Đếm thời gian cho nút "Kiem Tra"
		if (measureTimeForCheckButton)
		{
			int seconds = 2;
			//nếu hoàn thành
			if (difftime(time(NULL), startTimeForCheckButton) < seconds && completed)
			{
				//set màu thành màu xanh
				SDL_Color colour = { 91, 191, 116, SDL_ALPHA_OPAQUE };

				//set màu của render thành màu xanh
				SDL_SetRenderDrawColor(mRenderer, colour.r, colour.g, colour.b, SDL_ALPHA_OPAQUE);

				//cài texture thành "Dung"
				mCheckButton.setTexture(mTextureCache[13]);

				//chỉnh sửa chuột bấm xuống thành màu xanh
				mCheckButton.setMouseDownColour(colour);
			}
			//nếu mà sai
			else if (difftime(time(NULL), startTimeForCheckButton) < seconds && !completed)
			{
				//chỉnh màu thành red
				SDL_Color colour = { 200, 73, 46, SDL_ALPHA_OPAQUE };

				//chỉnh sửa màu của render thành đỏ
				SDL_SetRenderDrawColor(mRenderer, colour.r, colour.g, colour.b, SDL_ALPHA_OPAQUE);

				//tạo cấu trúc thành "Sai!"
				mCheckButton.setTexture(mTextureCache[12]);

				//tạo khi chuột bấm xuống thành màu đỏ
				mCheckButton.setMouseDownColour(colour);
			}
			else
			{
				//reset lại flag của thời gian đo
				measureTimeForCheckButton = false;
			}
		}
		else
		{
			//tạo texture cho "Kiem Tra"
			mCheckButton.setTexture(mTextureCache[10]);

			//tạo render thành cờ màu đen
			SDL_SetRenderDrawColor(mRenderer, mClearColour.r, mClearColour.g, mClearColour.b, mClearColour.a);
		}

		//clear màn hình với màu render
		SDL_RenderClear(mRenderer);

		// Render buttons và texture của từng cell thành bộ đệm nền
		for (int cell = 0; cell < mTotalCells; cell++)
		{
			// Render button
			mGrid[cell].renderButton(mRenderer);

			//Căn giữa lại vì các số khác nhau có texture có kích thước khác nhau

			mGrid[cell].centerTextureRect();

			// Render texture
			mGrid[cell].renderTexture(mRenderer);
		}

		// Render button của nút "Kiem Tra"
		mCheckButton.renderButton(mRenderer);
		mCheckButton.centerTextureRect();
		mCheckButton.renderTexture(mRenderer);

		// Render button của nút "Moi"
		mNewButton.renderButton(mRenderer);
		mNewButton.centerTextureRect();
		mNewButton.renderTexture(mRenderer);

		// hiển thị chữ sudoku trên màn hình
		char sudokuText[] = "SUDOKU";

		// Hiển thị chữ "SUDOKU" trên màn hình
		SDL_Texture* textTexture = nullptr;
		SDL_Color fontColour = { 0, 0, 0, SDL_ALPHA_OPAQUE }; // black
		loadTexture(textTexture, sudokuText, fontColour);
		mTimer.setTexture(textTexture);
		mTimer.renderButton(mRenderer);
		mTimer.centerTextureRect();
		mTimer.renderTexture(mRenderer);
		SDL_DestroyTexture(textTexture);
		textTexture = nullptr;

		//update màn hình từ bộ đệm và xoá bộ đệm đi
		SDL_RenderPresent(mRenderer);

		//làm trò chơi chạy chậm vì không cần phải nhanh
		SDL_Delay(10);
	}

	//vô hiệu hoá người chơi nhập text
	SDL_StopTextInput();

	//giải phóng texture
	freeTextures();

	//phá huỷ và thoát
	close();
}

//hàm đóng game
void Sudoku::Sudoku::close()
{
	//phá huỷ
	SDL_DestroyRenderer(mRenderer);
	SDL_DestroyWindow(mWindow);
	mRenderer = nullptr;
	mWindow = nullptr;

	//giải phóng font
	TTF_CloseFont(mFont);
	mFont = nullptr;

	//thoát
	SDL_Quit();
	TTF_Quit();
}



