
#include "CsoundTest.h"
#include "CsoundLib64/csound.hpp"
#include <iostream>
#include <string>

using namespace plogue::biduleSDK;
using namespace acme;
using namespace std;

CsoundTest::CsoundTest(BiduleHost* host):
BidulePlugin(host){
	//fill in plugin capabilities as a mask
    //  _caps = CAP_SYNCMASTER | CAP_SYNCSLAVE
    _caps = CAP_SYNCSLAVE;

	_numAudioIns=9;
	_numAudioOuts=2;
	_numMIDIIns=0;
	_numMIDIOuts=0;
	_numFreqIns=0;
	_numFreqOuts=0;
	_numMagIns=0;
	_numMagOuts=0;
    _numUIColumns=1;
    
}

CsoundTest::~CsoundTest(){
    //free Csound object
    delete csound;
}

bool CsoundTest::init() {
    cout << "Init CsoundTest" << endl;
    csound = new Csound();
    cout << csound << endl;
    cout << "version:" << csound->GetVersion() << endl;
    cout << "api version:" << csound->GetAPIVersion() << endl;

    //compile instance of csound.
    csCompileResult = csound->Compile("/Users/boonier/Downloads/test8.csd");
    csound->SetHostImplementedAudioIO(1, 0);
    
    if (csCompileResult == 0) { // compiled OK...
        //access Csound's input/output buffer
        spout = csound->GetSpout();
        spin  = csound->GetSpin();

        //start Csound performance
        csound->Start();
        cout << "Successful CSD compile, starting..." << endl;
        return true;
    } else {
        cout << "CSD did not compile:" << csCompileResult << endl;
        return false;
    }

}

void 
CsoundTest::getAudioInNames(std::vector<std::string>& vec){
	vec.push_back("p1");
    vec.push_back("p2");
    vec.push_back("p3");
    vec.push_back("p4");
    vec.push_back("p5");
    vec.push_back("p6");
    vec.push_back("p7");
    vec.push_back("p8");
    vec.push_back("Send Event");
}

void 
CsoundTest::getAudioOutNames(std::vector<std::string>& vec){
	vec.push_back("Csound output 1");
    vec.push_back("Csound output 2");

}

void 
CsoundTest::getMIDIInNames(std::vector<std::string>& vec){
}

void 
CsoundTest::getMIDIOutNames(std::vector<std::string>& vec){
}

void 
CsoundTest::getFreqInNames(std::vector<std::string>& vec){
}

void 
CsoundTest::getFreqOutNames(std::vector<std::string>& vec){
}

void 
CsoundTest::getMagInNames(std::vector<std::string>& vec){
}

void 
CsoundTest::getMagOutNames(std::vector<std::string>& vec){
}

void 
CsoundTest::getParametersInfos(ParameterInfo* pinfos) {
}

void 
CsoundTest::getParameterChoices(long id, std::vector<std::string>& vec) {
}

void 
CsoundTest::parameterUpdate(long id) {
}

void 
CsoundTest::process(Sample** sampleIn, Sample** sampleOut, MIDIEvents* midiIn, MIDIEvents* midiOut, Frequency*** freqIn, Frequency*** freqOut, Magnitude*** magIn, Magnitude*** magOut, SyncInfo* syncIn, SyncInfo* syncOut){
    
    long sampleFrames = _dspInfo.bufferSize;
//    unsigned int position = 0;
    unsigned int channels = 2;
    Sample* s1 = sampleOut[0];
    Sample* s2 = sampleOut[1];
    
    while(--sampleFrames >= 0) {
        
        if (ksmpsIndex == csound->GetKsmps()) {
            csCompileResult = csound->PerformKsmps();
            if (csCompileResult == 0)
                ksmpsIndex = 0;
        }
        
//
//        for (int chans = 0; chans < 2; chans++){
//            position = ksmpsIndex * channels;
//        }
      
        (*s1++) = spout[0 + (ksmpsIndex * channels)];
        (*s2++) = spout[1 + (ksmpsIndex * channels)];
        
        ksmpsIndex++;
//        (*s1++) = _currentSample;
//        (*s2++) = _currentSample;
              
        
        
//        sampleOut[0][i] = spout[ksmpsIndex];
//        sampleOut[1][i] = spout[ksmpsIndex];
    }
	

}

