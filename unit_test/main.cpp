#include <iostream>
#include <chrono>
#include <thread>
#include "../sound_drop.h"
#include "../receiver/FFTreader.hpp"
void f(vector<vector<FFTreader::byteType>>& data) { 
   std::this_thread::sleep_for(std::chrono::milliseconds(500));

   /* Sender */
   SoundDrop sd;
   sd.load(data);
   sd.send();
}
using namespace std;
int main()
{
   vector<vector<FFTreader::byteType>> data;
   for(int i = 0; i < 2; i++){
   		vector<FFTreader::byteType> chunk;
   		if(i==0){
   			for(int j=1; j<10;j++) chunk.push_back(j);
   		}else{
   			string s = "random things";
   			for(int j = 0 ; j < s.size(); j++)chunk.push_back(s[j]);
   		}
   		data.push_back(chunk);
   }
   thread sender(f, ref(data));
   FFTreader rv("recorded.wav",5);
   vector<vector<FFTreader::byteType>> r_bytes = rv.parse(); 
   sender.join();

   cout <<"\n\nResult:\n";
   if(data.size()!=r_bytes.size()){
      cout<<"unit test packet size failed" <<endl; 
      return 0;
   }
   for(int i=0; i < data.size(); i++) {
      if(data[i].size()!=r_bytes[i].size()){
         cout<< i <<"unit test data length failed" <<endl; 
         return 0;
      }
      // if(!succeed) break;
      for(int j=0 ; j < data[i].size(); j++){
         if(data[i][j] != r_bytes[i][j]){
            cout<<i<<","<<j<< " : "<< (int)data[i][j] <<","<< (int)r_bytes[i][j] <<"  :"<<"unit test data failed" <<endl; 
            return 0;
         }
      }    
   }
   cout << "unit test succeed" << endl;
   return 0;
}
