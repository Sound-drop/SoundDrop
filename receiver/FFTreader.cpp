#include "aquila/global.h"
#include "aquila/source/SignalSource.h"
#include "aquila/transform/FftFactory.h"
#include "aquila/source/WaveFile.h"
#include <algorithm>
#include <functional>
#include <memory>
#include <iostream>
#include <cstdlib>
#include "FFTreader.hpp"
using namespace std;
#define abs_amp 8000 
#define startchirp 201
#define DEBUG_FLAG    (1) 

vector<vector<FFTreader::byteType>> FFTreader::parse(){
   
    int right = findStartingPoint();
    int pre_freq = 0, step = sampleFreq/10; 

    vector<int> peak= freqOfindex(right);

    printStatus( right, peak );
    
    if(peak.size()==1 && pre_freq==0 && peak.back() == startchirp){
        //first startchirp chirp
        pre_freq = startchirp;
        right += step;
    }

    //read pkt length
    peak = freqOfindex(right);
    printStatus( right, peak );
    int pkts = soundTo16bits(peak);
    right += step;
    cout <<"Ready to read pkts:"<< pkts<< endl;
    vector<vector<FFTreader::byteType>> data;

    while(pkts-- >0){

        if(right >= END) break;
        peak = freqOfindex(right);
        printStatus( right, peak );
        int datalen = soundTo16bits(peak);
        right += step;
        cout <<"Ready to read data len :"<< datalen<< endl;
        vector<FFTreader::byteType> pktdata;

        while(datalen > 0){
            if(right >= END) break;
            peak = freqOfindex(right);
            printStatus( right, peak );
            int content = soundTo16bits(peak);
           

            //if odd, right shift padding
            if(datalen==1) content >>= 8;

            int shift = datalen==1 ? 1 : 2;
            const unsigned short _8bitMask  = 0x00FF;
            
            while(shift-- > 0){
                char d = content & _8bitMask;  
                pktdata.push_back(d);  
                content >>= 8;
            }

            right += step;
            //every data block has two bytes
            datalen -= 2;
            
        }

        data.push_back(pktdata);
    }
    return data;
    //return dataToStrings(data);
}
vector<string> FFTreader::dataToStrings(vector<std::vector<int>>& data){

    std::vector<string> ret;
    int pos = 0;

    for(auto& x: data){
        pos++;
        string ret_str = "";
        std::vector<int> ip;
        for(auto tmp :x) {
           if(pos==1){ 
                ip.push_back(tmp);
           }else{ 
                cout << tmp << " ";
                ret_str+=(char) tmp;
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

    return ret;
}
int FFTreader::soundTo16bits(vector<int>& peak){
    int cur = 0;
    for(auto&x : peak){
        int shift = x - (startchirp-16);
        if(x >= startchirp-16 && x <= startchirp-1) cur |= 1 << shift;
    }
    return cur;
}

void FFTreader::printStatus(int right, vector<int>& peak){

#if DEBUG_FLAG
        cout << "@ time " << (double)right/(sampleFreq) << "s"<< endl;
        cout << "Tracked freq (100 Hz): ";
        for(auto&x : peak) cout<< x <<" ";
        cout<<endl;
#endif

}
int FFTreader::findStartingPoint(){

    std::size_t start = 0;
    while(start < END){
        vector<int> peak= freqOfindex(start);

        if (peak.size() ==1 && peak.back() == startchirp){
                    break;
        }
        start += SIZE;
    }
    int left = start - SIZE/2, right = start + SIZE/7;
    while(left < right){

        int mid = (left+right)/2;
        // cout << wav.sample(mid) << endl;
        vector<int> peak= freqOfindex(mid);
        if (peak.size() ==1 && peak.back() == startchirp) right = mid;
        else left = mid+1;
    }
    right += SIZE;
    return right;
}

vector<int> FFTreader::findMax(Aquila::SpectrumType spectrum){
    std::size_t halfLength = spectrum.size() / 2;
    std::vector<double> absSpectrum(halfLength);
    double max = 0;
    int peak_freq  = 0;
    std::vector<int> ret;
    
    //search the band of the freq >= 15000
    int start = 0;
    int highpass = startchirp-20;
    for (std::size_t i = start; i < halfLength; ++i)
    {
        absSpectrum[i] = std::abs(spectrum[i]);
        int round_freq = (int)((i-1)*((double)sampleFreq/halfLength)/2 + 50) /100;
        // if(round_freq > highpass) cout << " amp " <<absSpectrum[i] << endl; 


        if(round_freq > highpass && absSpectrum[i-2] < absSpectrum[i-1] && absSpectrum[i-1] > absSpectrum[i] 
            && absSpectrum[i-1] > abs_amp ){
             

             // cout << "original freq " << (i-1)*(sampleFreq/halfLength)/2 << endl;
             if(ret.size() > 0 && ret.back()==round_freq) round_freq++;
             ret.push_back(round_freq);
             // cout << "round freq: "<<round_freq<<endl;
             // cout << " amp " <<absSpectrum[i-1] << endl;
        
        }
        
        if(absSpectrum[i] > max && round_freq > highpass){ 
            max = absSpectrum[i];
            peak_freq = round_freq;
        }
    }
    // cout << " amp " << max << endl; 
    //cout << "peak freq for input with sample size: "<< halfLength*2 << " which needs to be pow of 2" <<endl;
    //cout <<peak_freq << " Hz max amp:" << max << endl;
    //plot(absSpectrum);
    // if(peak_freq < 190) ret.clear();

    return ret;
}

vector<int> FFTreader::freqOfindex(std::size_t start){

    vector<Aquila::SampleType> chunk;
    for (std::size_t i =start; i< start+SIZE && i < END; ++i)
    {   

        chunk.push_back(wav.sample(i));


    }
    vector<int> ret;
    if(chunk.size()!=SIZE) return ret; 

    Aquila::SignalSource data(chunk, sampleFreq);

    
    auto fft = Aquila::FftFactory::getFft(SIZE);
    // cout << "\n\nSignal spectrum of time index: "<<start<< endl;
    Aquila::SpectrumType spectrum = fft->fft(data.toArray());
 
    //plt.plotSpectrum(spectrum);
    return findMax(spectrum);
    

}