#include <iostream>
#include <fstream>
using namespace std;

int main()
{
  //double fnum[4] = {9.5, -3.4, 1.0, 2.1};
  double *fnum = new double[4];
  for(int j = 0; j < 4; j ++)
  {
  	fnum[j] = j;
  }
  int i;

  ofstream out("numbers", ios::out | ios::binary);
  if(!out) {
    cout << "Cannot open file.";
    return 1;
   }

  out.write((char *) fnum, 4*(sizeof (double)));
  //out.write((char *) *fnum, sizeof *fnum);

  out.close();

  for(i=0; i<4; i++) // clear array
    fnum[i] = 0.0;

  ifstream in("numbers", ios::in | ios::binary);
  in.read((char *) fnum, 4*(sizeof (double)));
  cout<<4*(sizeof (double))<<endl;
  // see how many bytes have been read
  cout << in.gcount() << " bytes read\n";

  for(i=0; i<4; i++) // show values read from file
  cout << fnum[i] << " "<<endl;
  
  delete [] fnum;
  in.close();

  return 0;
}
