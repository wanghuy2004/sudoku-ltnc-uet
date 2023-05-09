#include "SudokuGenerator.h"

Sudoku::Generator::Generator()
	: mGrid(nullptr), mGridSolution(nullptr)
{

}

//--------------------------------------Private methods----------------------------------------//
inline void Sudoku::Generator::setElement(const int row, const int col, const int num)
{
	mGrid[row * 9 + col] = num;
}

inline int Sudoku::Generator::getElement(const int row, const int col) const
{
	return mGrid[row * 9 + col];
}

void Sudoku::Generator::swapNumbers(const int index1, const int index2)
{
	// chỉ hoạt động khi mà chúng chỉ số [index] 
	mGrid[index1] = mGrid[index1] ^ mGrid[index2];
	mGrid[index2] = mGrid[index1] ^ mGrid[index2];
	mGrid[index1] = mGrid[index1] ^ mGrid[index2];
}

void Sudoku::Generator::swapRows(const int row1, const int row2)
{
	for (int col = 0; col < 9; col++)
	{
		swapNumbers(row1 * 9 + col, row2 * 9 + col);
	}
}

void Sudoku::Generator::swapCols(const int col1, const int col2)
{
	for (int row = 0; row < 9; row++)
	{
		swapNumbers(row * 9 + col1, row * 9 + col2);
	}
}

void Sudoku::Generator::swapRowBlocks(const int rowBlock1, const int rowBlock2)
{
	int startRow1 = rowBlock1 * 3;
	int startRow2 = rowBlock2 * 3;

	for (int i = 0; i < 3; i++)
	{
		swapRows(startRow1 + i, startRow2 + i);
	}
}

void Sudoku::Generator::swapColBlocks(const int colBlock1, const int colBlock2)
{
	int startCol1 = colBlock1 * 3;
	int startCol2 = colBlock2 * 3;

	for (int i = 0; i < 3; i++)
	{
		swapCols(startCol1 + i, startCol2 + i);
	}
}

void Sudoku::Generator::fillNextRow(const int previousRow, const int nextRow, const int shifts)
{
	for (int col = 0; col < (9 - shifts); col++)
	{
		setElement(nextRow, col, getElement(previousRow, col + shifts));
	}

	for (int col = (9 - shifts); col < 9; col++)
	{
		setElement(nextRow, col, getElement(previousRow, col - 9 + shifts));
	}
}

void Sudoku::Generator::copyGrid(int* grid) const
{
	for (int i = 0; i < 81; i++)
	{
		grid[i] = mGrid[i];
	}
}

void Sudoku::Generator::createCompletedSudoku()
{
	// Tạo ra các giá trị ngẫu nhiên khác nhau mỗi khi chạy chương trình
	srand((unsigned int)time(NULL));

	// 1. Điền số từ 1 đến 9 vào hàng đầu tiên
	for (int i = 0; i < 9; i++)
	{
		mGrid[i] = i + 1;
	}

	// 2. Xáo trộn hàng đầu tiên 
	int swaps = 50;
	for (int i = 0; i < swaps; i++)
	{
		int randIndex1 = rand() % 9;
		int randIndex2 = rand() % 9;
		if (randIndex1 != randIndex2)
		{
			swapNumbers(randIndex1, randIndex2);
		}
	}

	// 3. Điền số vào hàng thứ 2 và 3 bằng cách dịch chuyển hàng trước đó 3 vị trí
	fillNextRow(0, 1, 3);
	fillNextRow(1, 2, 3);

	// 4. Điền số vào hàng thứ 4 bằng cách dịch chuyển hàng trước đó 1 vị trí
	fillNextRow(2, 3, 1);

	// 5. Điền số vào hàng thứ 5 và 6 bằng cách dịch chuyển hàng trước đó 3 vị trí
	fillNextRow(3, 4, 3);
	fillNextRow(4, 5, 3);

	// 6. Điền số vào hàng thứ 7 bằng cách dịch chuyển hàng trước đó 1 vị trí
	fillNextRow(5, 6, 1);

	// 7. Điền số vào hàng thứ 8 và 9 bằng cách dịch chuyển hàng trước đó 3 vị trí
	fillNextRow(6, 7, 3);
	fillNextRow(7, 8, 3);

	// 8. Xáo trộn các hàng trong mỗi block của hàng
	int shuffles = 10;
	for (int rowBlock = 0; rowBlock < 3; rowBlock++)
	{
		for (int shuffle = 0; shuffle < shuffles; shuffle++)
		{
			int randRow1 = rowBlock * 3 + (rand() % 3);
			int randRow2 = rowBlock * 3 + (rand() % 3);
			if (randRow1 != randRow2)
			{
				swapRows(randRow1, randRow2);
			}
		}
	}

	// 9. Xáo trộn các cột trong mỗi block cột
	for (int colBlock = 0; colBlock < 3; colBlock++)
	{
		for (int shuffle = 0; shuffle < shuffles; shuffle++)
		{
			int randCol1 = colBlock * 3 + (rand() % 3);
			int randCol2 = colBlock * 3 + (rand() % 3);
			if (randCol1 != randCol2)
			{
				swapCols(randCol1, randCol2);
			}
		}
	}

	// 10. Xáo trộn các block của hàng
	for (int shuffle = 0; shuffle < shuffles; shuffle++)
	{
		int randRowBlock1 = rand() % 3;
		int randRowBlock2 = rand() % 3;
		if (randRowBlock1 != randRowBlock2)
		{
			swapRowBlocks(randRowBlock1, randRowBlock2);
		}
	}

	// 11. Xáo trộn các block của cột
	for (int shuffle = 0; shuffle < shuffles; shuffle++)
	{
		int randColBlock1 = rand() % 3;
		int randColBlock2 = rand() % 3;
		if (randColBlock1 != randColBlock2)
		{
			swapColBlocks(randColBlock1, randColBlock2);
		}
	}

	// 12. lưu solution và lưới của solution
	copyGrid(mGridSolution);

}

//--------------------------------------Public methods----------------------------------------//
void Sudoku::Generator::generate(int* grid, int* solutionGrid)
{
	// set lưới của sudoku và lưới của solution
	mGrid = grid;
	mGridSolution = solutionGrid;

	// tạo game sudoku hoàn chỉnh
	createCompletedSudoku();

	// tạo giá trị ngẫu nhiên môi khi chạy game
	srand((unsigned int)time(NULL));

	// tạo đối tượng Solver của sudoku
	Solver SS;

	// thiết lập quyền của đối tượng Solver có khả năng chỉnh sửa trong generator
	SS.setGenModifier(true);

	// Tạo lưới với kiểu bool để theo dõi nếu các phần tử bị xoá khỏi trong lưới chính của game
	bool removed[81] = { };

	// tạo 1 lưới trùng lặp tạm thời
	int duplicateGrid[81];

	// Xác định số hiện tại cần được loại bỏ
	int removingNumber = 0;

	// số lượng phần tử cần được loại bỏ
	int toRemove = 50;

	while (toRemove)
	{
		// 1. Chọn số ngẫu nhiên mà bạn chưa từng thử xoá trước đây
		int randRow = rand() % 9;
		int randCol = rand() % 9;
		if (!removed[randRow * 9 + randCol])
		{
			// 2. Xoá số đó, sau đó chạy solver mà không cần xác định số bị xoá
			removingNumber = getElement(randRow, randCol);
			copyGrid(duplicateGrid);
			duplicateGrid[randRow * 9 + randCol] = 0;
			Ignore numToIgnore = { removingNumber, randRow, randCol };
			SS.setGrid(duplicateGrid, numToIgnore);

			// 3. Nếu mà solver không thể tìm ra solution, thì xoá số đó
			/*nếu Solve tìm ra được lời giải thì bảng Sudoku sẽ có thể có nhiều cách giải khác nhau. 
			Việc xoá các phần tử của bảng Sudoku gốc sẽ làm cho bảng Sudoku mới có thể có một số lượng lời giải ít hơn so với bảng gốc.*/
			if (!SS.solve())
			{
				setElement(randRow, randCol, 0);
				removed[randRow * 9 + randCol] = true;
				toRemove--;
			}
		}
		// 4.Lặp lại cho tới khi đủ số lượng 50 số bị xoá
	}

}

// hiển thị bảng Sudoku ra màn hình console
void Sudoku::Generator::display() const
{
	for (int row = 0; row < 9; row++)
	{
		for (int col = 0; col < 9; col++)
		{
			std::cout << getElement(row, col) << ", ";
		}
		std::cout << std::endl;
	}
}
