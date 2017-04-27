#include <iostream>
#include "../receiver/FFTreader.hpp"
int main()
{
   FFTreader f("recorded.wav",5);
   vector<vector<FFTreader::byteType>> r_bytes = f.parse(); 
   for(auto& x: r_bytes) {
	for(auto&y : x) cout << y << " ";
	cout << endl;
   }
   return 0;
}
