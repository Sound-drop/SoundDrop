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
#define abs_amp 500000 
#define startchirp 201
#define DEBUG_FLAG     (1) 

vector<string> FFTreader::parse(){
    std::size_t start = 0;


    while(start < END){
        vector<int> peak= freqOfindex(start);

        if (peak.size() ==1 && peak.back() == startchirp){
         break;
        }
        start += SIZE;
    }
    int left = start - SIZE,right = start;
    while(left < right){
        int mid = (left+right)/2;
        vector<int> peak= freqOfindex(mid);
        if (peak.size() ==1 && peak.back() == startchirp) right = mid;
        else left = mid+ SIZE/8;
    }
    std::vector<std::vector<int>> data;
    int pre_freq = 0, pkts = 0, step = sampleFreq/10; 

        vector<int> peak= freqOfindex(right);

#if DEBUG_FLAG
        cout << "@ time " << (double)right/(sampleFreq) << "s"<< endl;

        cout << "Tracked freq (100 Hz): ";
        for(auto&x : peak) cout<< x <<" ";
        cout<<endl;
#endif
        
        if(peak.size()==1 && pre_freq==0 && peak.back() == startchirp){
            //first startchirp chirp
            pre_freq = startchirp;
            right += step;
        }
        peak= freqOfindex(right);

        if (peak.size()==1 && pre_freq==startchirp && peak.back() ==startchirp){
            //second startchirp chirp
            right += step;
        }

        peak = freqOfindex(right);

        //read pkt lengt
        int cur = 0;
        for(auto&x : peak){
            int shift = x - (startchirp-16);
            if(x>=(startchirp-16) && x<=(startchirp-1)) cur |= 1 << shift;
        }

#if DEBUG_FLAG
        cout << "@ time " << (double)right/(sampleFreq) << "s"<< endl;

        cout << "Tracked freq (100 Hz): ";
        for(auto&x : peak) cout<< x <<" ";
        cout<<endl;
#endif
        pkts = cur;
        right += step;
        cout <<"Ready to read pkts:"<< pkts<< endl;
        while(pkts-- >0){
            peak = freqOfindex(right);

#if DEBUG_FLAG
        cout << "@ time " << (double)right/(sampleFreq) << "s"<< endl;

        cout << "Tracked freq (100 Hz): ";
        for(auto&x : peak) cout<< x <<" ";
        cout<<endl;
#endif
            int datalen = 0;
            for(auto&x : peak){
                int shift = x - (startchirp-16);
                if(x>=(startchirp-16) && x<=(startchirp-1)) datalen |= 1 << shift;
            }
            right += step;
            vector<int> pktdata;
            cout <<"Ready to read data len :"<< datalen<< endl;
            while(datalen > 0){
                peak = freqOfindex(right);
#if DEBUG_FLAG
        cout << "@ time " << (double)right/(sampleFreq) << "s"<< endl;

        cout << "Tracked freq (100 Hz): ";
        for(auto&x : peak) cout<< x <<" ";
        cout<<endl;
#endif
       
                int content = 0;
                for(auto&x : peak){
                    int shift = x - (startchirp-16);
                    if(shift >=0) content |= 1 << shift;
                }

                //if odd, right shift padding
                if(datalen==1) content >>= 8;
                pktdata.push_back(content);
                right += step;
                //every data has two bytes
                datalen -= 2;
                if(right >= END) break;
            }
            data.push_back(pktdata);
            if(right >= END) break;
        }
        
        const unsigned short _8bitMask  = 0x00FF;
        std::vector<string> ret;
        int pos = 0;
        for(auto& x: data){
            pos++;
            string ret_str = "";
            std::vector<int> ip;
            for(auto tmp :x) {

                while(tmp > 0){
                    int d = tmp & _8bitMask;    

                   if(pos==1){ 
                        ip.push_back(d);
                   }else{ 
                        ret_str+=(char) d;
                   }

                    tmp >>= 8;
                }
                
                


            }
            if(ip.size()>0){
                    for(int xx=0; xx< ip.size(); xx++){ 
                        if(xx == ip.size()-1) ret_str += std::to_string(ip[xx]);
                        else ret_str+= std::to_string(ip[xx])+'.';
                    }
            }
            ret.push_back(ret_str);
            cout<<endl;
        }

        //ready to return vector<string>
        // for(auto& x: ret) cout<< x<< endl;

        return ret;
}

vector<int> FFTreader::findMax(Aquila::SpectrumType spectrum){
        std::size_t halfLength = spectrum.size() / 2;
        std::vector<double> absSpectrum(halfLength);
        double max = 0;
        int peak_freq  = 0;
        std::vector<int> ret;
        
        //search the band of the freq >= 15000
        int start = 0;
        int highpass = startchirp-16;
        for (std::size_t i = start; i < halfLength; ++i)
        {
            absSpectrum[i] = std::abs(spectrum[i]);
            int round_freq = (int)((i-1)*(sampleFreq/halfLength)/2 + 50) /100;

            //if(round_freq > highpass) cout << round_freq<< " amp " << absSpectrum[i-1] << endl;
            if(round_freq > highpass && absSpectrum[i-2] < absSpectrum[i-1] && absSpectrum[i-1] > absSpectrum[i] 
                && absSpectrum[i-1] > abs_amp ){
                 
                 ret.push_back(round_freq);
                 // cout << round_freq<< " amp " <<absSpectrum[i-1] << endl;
            
            }
            
            if(absSpectrum[i] > max){ 
                max = absSpectrum[i];
                peak_freq = round_freq;
            }
        }
        //cout << "peak freq for input with sample size: "<< halfLength*2 << " which needs to be pow of 2" <<endl;
        //cout <<peak_freq << " Hz max amp:" << max << endl;
        //plot(absSpectrum);
        // if(peak_freq < 190) ret.clear();

        return ret;
    }

vector<int> FFTreader::freqOfindex(std::size_t start){

    vector<Aquila::SampleType> chunk;
    for (std::size_t i =start; i< start+SIZE; ++i)
    {   

        chunk.push_back(wav.sample(i));

    }
    Aquila::SignalSource data(chunk, sampleFreq);

   

 
    
    auto fft = Aquila::FftFactory::getFft(SIZE);
    // cout << "\n\nSignal spectrum of time index: "<<start<< endl;
    Aquila::SpectrumType spectrum = fft->fft(data.toArray());
 
    //plt.plotSpectrum(spectrum);
    return findMax(spectrum);
    

}