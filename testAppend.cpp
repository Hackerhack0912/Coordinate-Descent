#include <iostream>
#include <fstream>

using namespace std;

int main()
{
	ofstream outFile("output", ios::binary );
	
	if(outFile.fail())
	{
		cerr<< "Unable to open file for writing."<<endl;
		exit(1);
	}
	
	outFile.close();

	ofstream appendFile("output",ios::app | ios::binary );

	if(appendFile.fail())
	{
		cerr<<"Unable to open file for writing."<<endl;
		exit(1);
	}

	appendFile<<"0.002"<<endl;

	appendFile.close();
	
	
}
