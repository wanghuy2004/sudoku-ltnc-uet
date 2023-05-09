#include<bits/stdc++.h>

int main()
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

void getElement(const int row, const int col) const
{
	return mGrid[row * 9 + col];
}