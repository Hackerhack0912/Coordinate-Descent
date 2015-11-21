#include <iostream>
#include <fstream>
using namespace std;

int main()
{
  //double fnum[4] = {9.5, -3.4, 1.0, 2.1};
  double *fnum = new double[4];
  double *fnum1 = new double[4];
  double *write = new double[1];
  for(int j = 0; j < 4; j ++)
  {
  	fnum[j] = j+1;
  }
    
  for(int k = 0; k < 4; k ++)
  {
      fnum1[k] = 2*k;
  }
  int i;

  ofstream out("numbers", ios::out | ios::app | ios::binary);
  if(!out) {
    cout << "Cannot open file.";
    return 1;
   }

  //out.write((char *) fnum, 2*(sizeof (double)));
  //out.write((char *) fnum, 1*(sizeof (double)));
  for(int i = 0; i < 4; i ++)
  {
  	write[0] = fnum[i];
	out.write((char *) write, (sizeof (double)));
  }

  out.close();
    
    
    ofstream out1("numbers1", ios::out | ios::app | ios::binary);
    if(!out1) {
        cout << "Cannot open file.";
        return 1;
    }
    
    //out.write((char *) fnum, 2*(sizeof (double)));
    //out.write((char *) fnum, 1*(sizeof (double)));
   
    write[0] = fnum[i];
    out1.write((char *) fnum1, 4*(sizeof (double)));
    
    
    out1.close();


  //for(i=0; i<4; i++) // clear array
    //fnum[i] = 0.0;
   
  ifstream in("numbers1", ios::in | ios::binary);
  in.read((char *) fnum, 4*(sizeof (double)));
  
cout<<4*(sizeof (double))<<endl;
  // see how many bytes have been read
  cout << in.gcount() << " bytes read\n";

  for(i=0; i<4; i++) // show values read from file
  cout << fnum[i] << " "<<endl;
  
  delete [] fnum;
delete [] fnum1;
  delete [] write;

  in.close();

  return 0;
}
