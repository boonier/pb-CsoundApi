
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
    ;printk 0.5, 12345\n\
    kmod lfo 12, 3\n\
    aout vco2 0.5, 220+kmod\n\
    outs aout, aout\n\
    endin";
    
    sco = "i1 0 3600";
    
//    csd = "<CsoundSynthesizer>\n<CsOptions>\n-n\n</CsOptions>\n<CsInstruments>\n; Initialize the global variables.\nksmps = 64\nnchnls = 2\n0dbfs = 1\n\nchn_k \"phsRate\", 1\nchn_k \"pitch\", 1\n\n;instrument will be triggered by keyboard widget\ninstr 1\n    aOutL = oscili:a(scale(oscili:k(1, 0.25, -1, 0), 0.2, 1), 400 * p4 + oscili:k(2, 3))\n    aOutR = oscili:a(scale(oscili:k(1, 0.25, -1, 0.5), 0.2, 1), 200 * p4 + oscili:k(4, 1.5))\n    outs aOutL/8, aOutR/8\nendin\n\ninstr 2\n    aOutL = oscili:a(scale(oscili:k(1, chnget:k(\"phsRate\"), -1, 0), 0.2, 1), chnget:k(\"pitch\") * p4 + oscili:k(2, 3))\n    aOutR = oscili:a(scale(oscili:k(1, chnget:k(\"phsRate\"), -1, 0.5), 0.2, 1), (chnget:k(\"pitch\")/2) * p4 + oscili:k(4, 1.5))\n    outs aOutL/8, aOutR/8\nendin\n\n</CsInstruments>\n<CsScore>\n;causes Csound to run for about 7000 years...\ni2 0 z 0.25\ni2 0 z 0.5\ni2 0 z 0.75\ni2 0 z 1\ni2 0 z 1.25\ni2 0 z 1.5\ni2 0 z 1.75\ni2 0 z 2\n</CsScore>\n</CsoundSynthesizer>";
}

CsoundTest::~CsoundTest(){
    //free Csound stuff
    cout << "cleanup Csound stuff" << endl;
    csound->Cleanup();
    csound->Reset();
    delete csound;
}

bool CsoundTest::init() {
    cout << "CsoundTest init" << endl;
    
    csound = new Csound();
    cout << csound << endl;
    cout << "version: " << csound->GetVersion() << endl;
    cout << "api version: " << csound->GetAPIVersion() << endl;
    csound->CreateMessageBuffer(0);
    
    /*
    // compile instance of .csd
    _csCompileResult = csound->CompileCsdText(csd.c_str());
    cout << "_csCompileResult: " << _csCompileResult << endl;
    */
    
    // disable csound audio handling
    csound->SetHostImplementedAudioIO(1, 0);
    
    //set CsOptions --nosound
    csound->SetOption("-n");
  
    
    // compile orc.
    int _resultOrc = csound->CompileOrc(orc.c_str());
    cout << "resultOrc" << _resultOrc << endl;

    // compile sco
    int _resultSco = csound->ReadScore(sco.c_str());
    cout << "resultSco" << _resultSco << endl;
    
    
    
    
    
    if (_resultOrc == 0 && _resultSco == 0) { // compiled OK...
//        cout << "HERE1"<< endl;
        _csCompileResult = 0;
        csound->Start();
//        cout << "HERE2"<< endl;
//        cout << "Successfully compiled..." << endl;
//
//        // show the Cs logs
//        while (csound->GetMessageCnt() > 0) {
//            cout << "CSOUND_MESSAGE:" << csound->GetFirstMessage() << endl;
//            csound->PopFirstMessage();
//        }
//
//        //access Csound's input/output buffer
        spout = csound->GetSpout();
//        spin = csound->GetSpin();
//        cout << "spin " << spin << endl;
        cout << "spout " << spout << endl;
//
//        csound->Perform();
//        cout << "HERE3" << endl;
//        cout << "Go..." << endl;
        return true;
    } else {
//        while (csound->GetMessageCnt() > 0) {
//           cout << "CSOUND_MESSAGE:" << csound->GetFirstMessage() << endl;
//           csound->PopFirstMessage();
//        }
//        cout << "CSD did not compile:" << _csCompileResult << endl;
        return false;
    }
    
    return true;
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
    
    pinfos[2].id = 2;
    strcpy(pinfos[2].name, "orc code");
    pinfos[2].type = STRINGPARAM;
    pinfos[2].ctrlType = GUICTRL_TEXTAREA;
    pinfos[2].linkable = 0;
    pinfos[2].saveable = 1;
    strcpy(pinfos[4].paramInfo.ps.defaultValue, "test123");
//    pinfos[2].paramInfo.pd.minValue = 60;
//    pinfos[2].paramInfo.pd.maxValue = 800;
//    pinfos[2].paramInfo.pd.precision = 3;
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
        
        if (_csCompileResult == 0) {
            
            if (_ksmpsIndex == csound->GetKsmps()) {
                _csCompileResult = csound->PerformKsmps();
                if (_csCompileResult == 0) {
//                    cout << "reset _ksmpsIndex" << endl;
                    _ksmpsIndex = 0;
                }
                    
            }
           
            csound->SetChannel("phsRate", _blurAmt);
            csound->SetChannel("pitch", _pitch);
            
            (*s1++) = spout[0 + (_ksmpsIndex * channels)];
            (*s2++) = spout[1 + (_ksmpsIndex * channels)];
            _ksmpsIndex++;
        } else {
            (*s1++) = 0.f;
            (*s2++) = 0.f;
        }
        
    }
	

}

