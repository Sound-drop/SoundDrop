#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <functional>
#include <memory>
#include <iostream>
#include <cstdlib>
#include "FFTreader.hpp"


int main()
{
    FFTreader f("recorded.wav",5);
    vector<vector<FFTreader::byteType>> r_bytes = f.parse();

    //In this example, sender will send (ip, user_name, file_path).
    //We should collect 3 strings into ret.
    vector<string> ret;

    for (int pos = 0; pos < r_bytes.size(); pos++){
        string ret_str = "";
        vector<unsigned char> ip;
        for(auto tmp :r_bytes[pos]) {
               if(pos==0){ 
                    ip.push_back((unsigned char)tmp);
               }else{ 
                    cout << (int)tmp << " ";
                    ret_str+=tmp;
               }
        }
        if(ip.size()>0){
            for(int xx=0; xx< ip.size(); xx++){ 
                if(xx == ip.size()-1) ret_str += std::to_string(ip[xx]);
                else ret_str+= std::to_string(ip[xx])+'.';
            }
        cout<< "ascii codes";
        }
        cout << endl;
        ret.push_back(ret_str);
    }

    //double check the output
    for (auto& x : ret) cout << x <<endl;

    if(ret.size() == 3){
        //Current string data is cstr, which contains '\0' at end. Parsed it to std::string.
        //Otherwise, the command will not concatenate correctly. 
        string command = "scp -i ~/.ssh/sounddrop " + string(ret[1].c_str())+"@"+ret[0] + 
        ":" + string(ret[2].c_str()) + " . ";
        cout << command.c_str() << endl;
        system(command.c_str());
    }  
    return 0;
}

