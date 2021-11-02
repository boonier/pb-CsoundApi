
#include "CsoundTest.h"
#include "CsoundLib64/csound.hpp"
#include <iostream>
#include <string>
#include <array>
#include <math.h>

#ifdef __APPLE__
#include <corefoundation/CFBundle.h>    
#endif

using namespace plogue::biduleSDK;
using namespace acme;
using namespace std;

CsoundTest::CsoundTest(BiduleHost* host):
BidulePlugin(host){
	//fill in plugin capabilities as a mask
    // _caps = CAP_SYNCMASTER | CAP_SYNCSLAVE
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
    
//    for (int i = 0; i < 256; i++) {
//        sinTable[i] = (float)i/256;
//        cout << sinTable[i] << endl;
//    }
//    cout << "sinTable size: " << sinTable.size() << endl;
//    cout << M_PI << endl;
 
}

CsoundTest::~CsoundTest(){
    //free Csound object
    delete csound;
}

bool CsoundTest::init() {

    // get the local bundle CSD from Resources...
    CFBundleRef mainBundle = CFBundleGetBundleWithIdentifier(CFSTR("com.boonier.CsoundTest"));
    CFURLRef csdURL = CFBundleCopyResourceURL(mainBundle, CFSTR("test8.csd"), NULL, NULL);
    string path = CFStringGetCStringPtr(CFURLGetString(csdURL), kCFStringEncodingMacRoman );
    string sub = "file://";
    string::size_type i = path.find(sub);
    if (i != string::npos)
        path.erase(i, sub.length());
    
    cout << "Init CsoundTest" << endl;
    csound = new Csound();
    cout << csound << endl;
    cout << "version:" << csound->GetVersion() << endl;
    cout << "api version:" << csound->GetAPIVersion() << endl;
    csound->CreateMessageBuffer(0);
   
    //compile instance of csound.
    csCompileResult = csound->Compile(path.c_str());
    csound->SetHostImplementedAudioIO(1, 0);
    
    if (csCompileResult == 0) { // compiled OK...
        cout << "CSOUND_MESSAGE:" << csound->GetFirstMessage() << endl;
        //access Csound's input/output buffer
        spout = csound->GetSpout();
        spin  = csound->GetSpin();
        
        //start Csound performance
        csound->Start();
        cout << "Successful CSD compile, starting..." << endl;
        return true;
    } else {
        while (csound->GetMessageCnt() > 0) {
           cout << "CSOUND_MESSAGE:" << csound->GetFirstMessage() << endl;
           csound->PopFirstMessage();
        }
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
//        while (csound->PerformKsmps() == 0) {
//           while (csound->GetMessageCnt() > 0) {
//              cout << "CSOUND_MESSAGE:" << csound->GetFirstMessage() << endl;
//              csound->PopFirstMessage();
//           }
//        }
        
        if (csCompileResult == 0) {
            if (ksmpsIndex == csound->GetKsmps()) {
                csCompileResult = csound->PerformKsmps();
                if (csCompileResult == 0)
                    ksmpsIndex = 0;
            }
            
            (*s1++) = spout[0 + (ksmpsIndex * channels)];
            (*s2++) = spout[1 + (ksmpsIndex * channels)];
            ksmpsIndex++;
        } else {
            (*s1++) = 0.f;
            (*s2++) = 0.f;
        }
        
//        (*s1++) = sinTable[position];
//        (*s2++) = sinTable[position];
//        position = position < sinTable.size() ? position+1 : 0;
    }
	

}

