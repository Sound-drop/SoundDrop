#include <iostream>
#include <chrono>
#include <thread>
#include "../receiver/FFTreader.hpp"
void f(vector<vector<FFTreader::byteType>>& data) { 
   std::this_thread::sleep_for(std::chrono::milliseconds(500));
	//sender
	for(auto& x: data) {
		for(auto&y : x) cout << y << " ";
	 	cout << endl;
   	}
}
using namespace std;
int main()
{
   vector<vector<FFTreader::byteType>> data;
   for(int i = 0; i < 2; i++){
   		vector<FFTreader::byteType> chunk;
   		if(i==0){
   			for(int j=1; j<10;j++) chunk.push_back((unsigned char)j);
   		}else{
   			string s = "random things";
   			for(int j = 0 ; j < s.size(); j++)chunk.push_back(s[j]);
   		}
   		data.push_back(chunk);
   }
   thread sender(f, ref(data));
   FFTreader rv("recorded.wav",5);
   vector<vector<FFTreader::byteType>> r_bytes = rv.parse(); 
   for(auto& x: r_bytes) {
	for(auto&y : x) cout << y << " ";
	cout << endl;
   }
   sender.join();
   return 0;
}
