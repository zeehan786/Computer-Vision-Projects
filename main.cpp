# include<fstream>
# include <iostream>
#include <string>
#include "ImageCompression.cpp"

using namespace std;

int main(int argc, char** argv)
{
	if (argc != 3)
	{
		std::cout << "Wrong number of arguments\n";
		std::cout << "argc = " << argc << std::endl;
		exit(0);
	}
	int numRows = 0, numCols = 0, minval = 0, maxval = 0;
	
	ifstream inFile, skfile;

	ofstream outFile(argv[2]);

	std::string subname = "_skelelton.txt";
	std::string subname2 = "_decompressed.txt";

	ofstream outFile2(argv[1] + subname), outFile3(argv[1] + subname2);

	inFile.open(argv[1]);

	if (inFile.fail()) cout << "Error in opening " << argv[1] << "\n";

	

	inFile >> numRows; inFile >> numCols; inFile >> minval; inFile >> maxval;

	std::string header = to_string(numRows) + "  " + to_string(numCols) + "  " + to_string(minval) + "  " + to_string(maxval);
	
	ImageCompression obj(numRows, numCols, minval, maxval);

	obj.loadImage(inFile);

	obj.Compute8Distance(outFile);

	obj.ImageCompresser(outFile, outFile2); outFile2.close(); skfile.open(argv[1] + subname);

	obj.loadskeleton(skfile);

	obj.ImageDeCompression(outFile);

	if (obj.newmaxval != 0) obj.newmaxval = 1;

	outFile3 << numRows << " " << numCols << " " << obj.newminval << " " << obj.newmaxval << "\n";

	obj.threshold(outFile3);

	inFile.close();
	outFile.close();
	skfile.close();
	outFile3.close();
	

	return 0;
}