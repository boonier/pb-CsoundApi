
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
    _numParams = 2;
    
    _blurAmt = 0.25;
    
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
    cout << "CsoundTest init" << endl;
    
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
    
    //setup performance thread
//    CsoundPerformanceThread* perfThread = new CsoundPerformanceThread(csound);
    
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
    pinfos[0].id = 0;
    strcpy(pinfos[0].name, "phase rate");
    pinfos[0].type = DOUBLEPARAM;
    pinfos[0].ctrlType = GUICTRL_SLIDER;
    pinfos[0].linkable = 1;
    pinfos[0].saveable = 1;
    pinfos[0].paramInfo.pd.defaultValue = 0.25;
    pinfos[0].paramInfo.pd.minValue = 0;
    pinfos[0].paramInfo.pd.maxValue = 20;
    pinfos[0].paramInfo.pd.precision = 3;
    //    strcpy(pinfos[0].paramInfo.pd.units, "dollar");
//    pinfos[0].paramInfo.pd.scaling = SCALING_LINEAR;
    
    pinfos[1].id = 1;
    strcpy(pinfos[1].name, "pitch");
    pinfos[1].type = DOUBLEPARAM;
    pinfos[1].ctrlType = GUICTRL_SLIDER;
    pinfos[1].linkable = 1;
    pinfos[1].saveable = 1;
    pinfos[1].paramInfo.pd.defaultValue = 400;
    pinfos[1].paramInfo.pd.minValue = 60;
    pinfos[1].paramInfo.pd.maxValue = 800;
    pinfos[1].paramInfo.pd.precision = 3;
}

void 
CsoundTest::getParameterChoices(long id, std::vector<std::string>& vec) {
}

void 
CsoundTest::parameterUpdate(long id) {
    if(id == 0) {
        getParameterValue(0, _blurAmt);
    } else if (id == 1) {
        getParameterValue(1, _pitch);
    }
}
void 
CsoundTest::process(Sample** sampleIn, Sample** sampleOut, MIDIEvents* midiIn, MIDIEvents* midiOut, Frequency*** freqIn, Frequency*** freqOut, Magnitude*** magIn, Magnitude*** magOut, SyncInfo* syncIn, SyncInfo* syncOut){
    
    long sampleFrames = _dspInfo.bufferSize;
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
            
            csound->SetChannel("phsRate", _blurAmt);
            csound->SetChannel("pitch", _pitch);
            
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

