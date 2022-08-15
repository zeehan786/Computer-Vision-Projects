#pragma once
#include <fstream>
# include <iostream>
#include <string>
#include <sstream>
#include <cmath>
#include <algorithm>

class ImageCompression
{
public: int numRows, numCols, imgMin, imgMax, newminval = 0, newmaxval = 0, newmin, newmax;

public: int* histarray;
public: int* gaussarray;
public: int** ZFArray;
public: int** SkeletonArray;

public: ImageCompression(int nr, int nc, int min, int  max)
{
	numRows = nr, numCols = nc, imgMax = max, imgMin = min;

	

	ZFArray = new int* [numRows + 2];
	SkeletonArray = new int* [numRows + 2];

	for (int i = 0; i < numRows + 2; i++)
	{
		ZFArray[i] = new int[numCols + 2];
		//ZFArray[i] = 0;
		SkeletonArray[i] = new int[numCols + 2];
	}

	setZero(ZFArray); setZero(SkeletonArray);
}

public:void setZero(int** ary)
{
	for (int i = 0; i < numRows + 2; i++)
	{
		for (int j = 0; j < numCols + 2; j++)
		{
			ary[i][j] = 0; 
		}
	}
}

public: void loadImage(std::ifstream& inFile)
{
	
	for (int i = 1; i < numRows + 1; i++)
	{
		for (int j = 1; j < numCols + 1; j++)
		{
			std::string st;

			inFile >> st;

			std::stringstream str(st);

			
			
			str >> ZFArray[i][j];			 
		}
	}
}

public: void Compute8Distance(std::ofstream& outfile)
{
	
	firstPass8Distance();
	
	reformatPrettyPrint(ZFArray, outfile, "Result of Pass 1:\n\n\n");

	secondPass8Distance();
	
	reformatPrettyPrint(ZFArray, outfile, "Result of Pass 2:\n\n\n");
}

public: void firstPass8Distance()
{
	for (int i = 1; i < numRows + 1; i++)
	{
		for (int j = 1; j < numCols + 1; j++)
		{
			if (ZFArray[i][j] > 0)
			{
				int min1 = std::min(ZFArray[i-1][j-1], ZFArray[i-1][j]), min2 = std::min(ZFArray[i-1][j+1], ZFArray[i][j-1]);
				
				ZFArray[i][j] = std::min(min1, min2) + 1;

			}
		}
	}
}

public: void secondPass8Distance()
{
	for (int i = numRows; i > 0; i--)
	{
		for (int j = numCols; j > 0; j--)
		{
			if (ZFArray[i][j] > 0)
			{
				int min1 = std::min(ZFArray[i][j + 1], ZFArray[i + 1][j - 1]),
					min2 = std::min(ZFArray[i + 1][j], ZFArray[i + 1][j + 1]),
					min3 = std::min(min1, min2) + 1;
				ZFArray[i][j] = std::min(ZFArray[i][j], min3);

			}
		}
	}
}

public: void reformatPrettyPrint(int** ary, std::ofstream& outfile, std::string caption)
{
	outfile << caption+"\n";

	for (int i = 1; i < numRows + 1; i++)
	{
		for (int j = 1; j < numCols + 1; j++)
		{
			if (ary[i][j] > 0) { outfile << ary[i][j]<<" "; }
			else { outfile << "0 "; }
		}
		outfile << "\n";
	}

	outfile << "-----------------------------------------------\n\n";

}

public: void ImageCompresser(std::ofstream& outfile, std::ofstream& skeletonfile)
{
	calLocalMaxima();
	reformatPrettyPrint(SkeletonArray, outfile, "Skeleton Image");
	extractSkeleton(skeletonfile);
	
}

public: void calLocalMaxima()
{
	for (int i = 1; i < numRows + 1; i++)
	{
		for (int j = 1; j < numCols + 1; j++)
		{
			int val = ZFArray[i][j];
			if (val > 0)
			{
				if (val >= ZFArray[i - 1][j - 1] && val >= ZFArray[i - 1][j] && val >= ZFArray[i - 1][j + 1] && val >= ZFArray[i][j - 1] &&
					val >= ZFArray[i][j + 1] && val >= ZFArray[i + 1][j - 1] && val >= ZFArray[i + 1][j] && val >= ZFArray[i + 1][j + 1])
				{
				
					SkeletonArray[i][j] = val;

					if (val > newmaxval)
						newmaxval = val;
					
				}
			}
		}
	}
}

public: void extractSkeleton(std::ofstream& outfile)
{

	outfile << numRows << "  " << numCols << "  " << newminval << "  " << newmaxval << "\n";
	for (int i = 1; i < numRows + 1; i++)
	{
		for (int j = 1; j < numCols + 1; j++)
		{
			if (SkeletonArray[i][j] > 0)
			{						
				outfile << i << "  " << j << "  " << SkeletonArray[i][j] << "\n";

			}
		}
	}
}

public: void loadskeleton(std::ifstream& skfile)
{
	setZero(ZFArray);

	std::string header;
	getline(skfile, header);
	while (!skfile.eof())
	{
		int row = 0, col = 0, val = 0;
		skfile >> row; skfile >> col; skfile >> val;

		ZFArray[row][col] = val;
	}
}

public: void ImageDeCompression(std::ofstream& outfile)
{
	firstpassExpansion();
	reformatPrettyPrint(ZFArray, outfile, "Result of 1st Pass Expansion");
	
	secondPassExpansion();
	reformatPrettyPrint(ZFArray, outfile, "Result of 2nd Pass Expansion");
	
}

public: void firstpassExpansion()
{
	for (int i = 1; i < numRows + 1; i++)
	{
		for (int j = 1; j < numCols + 1; j++)
		{
			
			if (ZFArray[i][j] == 0 && (ZFArray[i - 1][j - 1] > 1 || ZFArray[i - 1][j] > 1 || ZFArray[i - 1][j + 1] > 1 
				|| ZFArray[i][j - 1] > 1 || ZFArray[i][j + 1] > 1 || ZFArray[i + 1][j - 1] > 1 ||  
				ZFArray[i + 1][j] > 1 || ZFArray[i + 1][j + 1] > 1))
			{					
				int max1 = std::max(ZFArray[i - 1][j - 1], ZFArray[i - 1][j]), max2 = std::max(ZFArray[i - 1][j + 1], ZFArray[i][j - 1]),
					max3 = std::max(ZFArray[i][j + 1], ZFArray[i + 1][j - 1]), max4 = std::max(ZFArray[i + 1][j], ZFArray[i + 1][j + 1]);
				int fmax1 = std::max(max1, max2), fmax2 = std::max(max3, max4);
				ZFArray[i][j] = std::max(fmax1, fmax2) - 1;
			}
		}
	}
}

public: void secondPassExpansion()
{
	for (int i = numRows; i > 0; i--)
	{
		for (int j = numCols; j > 0; j--)
		{
			if (ZFArray[i - 1][j - 1] > 1 || ZFArray[i - 1][j] > 1 || ZFArray[i - 1][j + 1] > 1
				|| ZFArray[i][j - 1] > 1 || ZFArray[i][j + 1] > 1 || ZFArray[i + 1][j - 1] > 1 ||
				ZFArray[i + 1][j] > 1 || ZFArray[i + 1][j + 1] > 1)
			{
				int max1 = std::max(ZFArray[i - 1][j - 1], ZFArray[i - 1][j]), max2 = std::max(ZFArray[i - 1][j + 1], ZFArray[i][j - 1]),
					max3 = std::max(ZFArray[i][j + 1], ZFArray[i + 1][j - 1]), max4 = std::max(ZFArray[i + 1][j], ZFArray[i + 1][j + 1]);
				int fmax1 = std::max(max1, max2), fmax2 = std::max(max3, max4);

				if(ZFArray[i][j] < (std::max(fmax1, fmax2) - 1))
				ZFArray[i][j] = std::max(fmax1, fmax2) - 1;
			}
		}
	}
}

public: void threshold(std::ofstream& decomp)
{
	for (int i = 1; i < numRows + 1; i++)
	{
		for (int j = 1; j < numCols + 1; j++)
		{

			if (ZFArray[i][j] >= 1)
				decomp << "1 ";
			else decomp << "0 ";
		}
		decomp << "\n";
	}
}

public:void showArray(int** ary, int nr, int nc)
	  {
		  for (int i = 1; i < nr + 1; i++)
		  {
			  for (int j = 1; j < nc + 1; j++)
			  {
				  std::cout << ary[i][j] << " ";
			  }
			  std::cout << std::endl;
		  }
		  std::cout << "\n\n\n";
	  }


};

