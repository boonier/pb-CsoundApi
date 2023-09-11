
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
    _pitch = 0;
    //
    _csCompileResult = -1;
    _ksmpsIndex = 0;
    _cnt = 0;
    
    orc = "sr=48000\n\
    ksmps=64\n\
    nchnls=2\n\
    0dbfs=1\n\
    instr 1\n\
    printk 0.5, 12345\n\
    kmod lfo 12, 3\n\
    aout vco2 0.5, 220+kmod\n\
    outs aout, aout\n\
    endin";
    
    sco = "i1 0 60";
}

CsoundTest::~CsoundTest(){
    //free Csound object
    csound->Cleanup();
    delete csound;
}

bool CsoundTest::init() {
    cout << "CsoundTest init" << endl;
    
    csound = new Csound();
    cout << csound << endl;
    cout << "version:" << csound->GetVersion() << endl;
    cout << "api version:" << csound->GetAPIVersion() << endl;
    csound->CreateMessageBuffer(0);
    
//    compile instance of .csd
//    csCompileResult = csound->Compile(path.c_str());
    
    csound->SetHostImplementedAudioIO(1, 0);
    

    //set CsOptions
    csound->SetOption("-n");
  
    
    //compile orc.
    int resultOrc = csound->CompileOrc(orc.c_str());
    cout << "resultOrc" << resultOrc << endl;

    //compile sco
    int resultSco = csound->ReadScore(sco.c_str());
//    int resultSco = 0;
    cout << "resultSco" << resultSco << endl;

    csound->Start();
    csound->Perform();
    
    
    if (resultOrc == 0 && resultSco == 0) { // compiled OK...
        cout << "Successfully compiled..." << endl;
//        cout << "CSOUND_MESSAGE:" << csound->GetFirstMessage() << endl;
        while (csound->GetMessageCnt() > 0) {
            cout << "CSOUND_MESSAGE:" << csound->GetFirstMessage() << endl;
            csound->PopFirstMessage();
        }
//        cout << "HERE!" << endl;
        _csCompileResult = 0;

        
        //access Csound's input/output buffer
        spout = csound->GetSpout();
        spin  = csound->GetSpin();
        cout << "spin " << spin << endl;
        cout << "spout " << spout << endl;
        
        //start Csound
//        csound->Start();
        
       
        
        cout << "Starting..." << endl;
        return true;
    } else {
        while (csound->GetMessageCnt() > 0) {
           cout << "CSOUND_MESSAGE:" << csound->GetFirstMessage() << endl;
           csound->PopFirstMessage();
        }
        cout << "CSD did not compile:" << _csCompileResult << endl;
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
        while (csound->PerformKsmps() == 0) {
           while (csound->GetMessageCnt() > 0) {
              cout << "CSOUND_MESSAGE:" << csound->GetFirstMessage() << endl;
              csound->PopFirstMessage();
           }
        }
        
        if (_csCompileResult == 0) {
            
            if (_ksmpsIndex == csound->GetKsmps()) {
                _csCompileResult = csound->PerformKsmps();
                if (_csCompileResult == 0)
                    _ksmpsIndex = 0;
            }
           
           
            
            (*s1++) = spout[0 + (_ksmpsIndex * channels)];
            (*s2++) = spout[1 + (_ksmpsIndex * channels)];
            
//            (*s1++) = 0.f;
//            (*s2++) = 0.f;
            
            if (_cnt != 1) {
                cout << "HERE3! " << _ksmpsIndex << " " << csound->GetKsmps() << " " << endl;
                cout << "PerformKsmps() " << csound->PerformKsmps() << endl;
                _cnt++;
            }
            
            _ksmpsIndex++;
        } else {
            (*s1++) = 0.f;
            (*s2++) = 0.f;
        }
        
    }
	

}

