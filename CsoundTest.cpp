
#include "CsoundTest.h"
#include "CsoundLib64/csound.hpp"
#include <iostream>
#include <string>
#include <array>
#include <math.h>

using namespace plogue::biduleSDK;
using namespace acme;
using namespace std;

CsoundTest::CsoundTest(BiduleHost* host):
BidulePlugin(host){
    log("constructor");
    
	//fill in plugin capabilities as a mask
    // _caps = CAP_SYNCMASTER | CAP_SYNCSLAVE
    _caps = CAP_SYNCSLAVE;

	_numAudioIns=9;
	_numAudioOuts=2;
	_numMIDIIns=1;
	_numMIDIOuts=1;
	_numFreqIns=0;
	_numFreqOuts=0;
	_numMagIns=0;
	_numMagOuts=0;
    _numUIColumns=1;
    _numParams = 5;

    _blurAmt = 0.25;
    _pitch = 0;
    _amp = 0;
    //
    _csCompileResult = -1;
    _ksmpsIndex = 0;
    _cnt = 0;
    _recompileTrig = 0;
    
    _orcHdr = "sr=48000\n\
    ksmps=64\n\
    nchnls=2\n\
    0dbfs=1\n\
    chn_k \"phsRate\", 1\n\
    chn_k \"pitch\", 1\n\
    chn_k \"amp\", 1\n\";
    
    _orc1 = "instr 1\n\
    aOutL = oscili:a(scale(oscili:k(1, chnget:k(\"phsRate\"), -1, 0), 0.2, 1), chnget:k(\"pitch\") * p4 + oscili:k(2, 3))\n\
    aOutR = oscili:a(scale(oscili:k(1, chnget:k(\"phsRate\"), -1, 0.5), 0.2, 1), (chnget:k(\"pitch\")/2) * p4 + oscili:k(4, 1.5))\n\
    outs powershape(aOutL/8, lfo(.3, 0.1, 1), 0.3) * 0.2, powershape(aOutR/8, lfo(.3, 0.1, 1), .3) * 0.2\n\
    endin";

    
    _orc2 = "instr 1\n\
    ares = poscil3(1, p4)\n\
    outs ares,ares\n\
    endin";
    
//    sco = "i1 0 z 0.25\n\
//    i1 0 z 0.5\n\
//    i1 0 z 0.75\n\
//    i1 0 z 1\n\
//    i1 0 z 1.25\n\
//    i1 0 z 1.5\n\
//    i1 0 z 1.75\n\
//    i1 0 z 2";
        
    _sco = "i1 0 4 2";
    
    


}

CsoundTest::~CsoundTest(){
    //free Csound stuff
    cout << "cleanup Csound stuff" << endl;
    csound->Cleanup();
    csound->Reset();
    delete csound;
}

bool CsoundTest::init() {
    
    log("CsoundTest init");
    
    csound = new Csound();
    cout << csound << endl;
    cout << "version: " << csound->GetVersion() << endl;
    cout << "api version: " << csound->GetAPIVersion() << endl;
    csound->CreateMessageBuffer(0);
    
    // disable csound audio handling
    csound->SetHostImplementedAudioIO(1, 0);
    
    //set CsOptions --nosound
    csound->SetOption("-n");
    
    // compile orc.
    int _resultOrc = csound->CompileOrc(_orc1.c_str());
    cout << "resultOrc" << _resultOrc << endl;

    // compile sco
    int _resultSco = csound->ReadScore(_sco.c_str());
    cout << "resultSco" << _resultSco << endl;
 
    if (_resultOrc == 0 && _resultSco == 0) { // compiled OK...
//        cout << "HERE1"<< endl;
        _csCompileResult = 0;
        csound->Start();

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
        return true;
    } else {
        while (csound->GetMessageCnt() > 0) {
           cout << "CSOUND_MESSAGE:" << csound->GetFirstMessage() << endl;
           csound->PopFirstMessage();
        }
        cout << "CSD did not compile:" << _csCompileResult << endl;
        return false;
    }
    
    return true;
}

void
CsoundTest::recompileOrc() {
    
    
    log("recompileOrc");
    log("_recompileTrig ini = " + std::to_string(_recompileTrig));
    
    if (_recompileTrig == 1) {
        csound->Reset();
        cout << "Do re-compile" << endl;
        log("templateOrc = " + _orc2);
        _csCompileResult = csound->CompileOrc(_orc2.c_str());
        _sco = "i1 0 4 220";
        log("_csCompileResult = " + std::to_string(_csCompileResult));
        
        if (_csCompileResult == 0) {
            csound->ReadScore(_sco.c_str());
        }
        
//        csound->Start();
        _recompileTrig = 0;
    }
    
    log("_recomTrig = " + std::to_string(_recompileTrig));

}


void
CsoundTest::log(const std::string& message) {
    cout << "[LOG] " << message << endl;
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
    strcpy(pinfos[2].name, "orc");
    pinfos[2].type = STRINGPARAM;
    pinfos[2].ctrlType = GUICTRL_TEXTAREA;
    pinfos[2].linkable = 1;
    pinfos[2].saveable = 1;
    strcpy(pinfos[2].paramInfo.ps.defaultValue, _orc2.c_str());
    
    pinfos[3].id = 3;
    strcpy(pinfos[3].name, "sco");
    pinfos[3].type = STRINGPARAM;
    pinfos[3].ctrlType = GUICTRL_TEXTAREA;
    pinfos[3].linkable = 1;
    pinfos[3].saveable = 1;
    strcpy(pinfos[3].paramInfo.ps.defaultValue, ";i1 0 1");
    
    pinfos[4].id = 4;
    strcpy(pinfos[4].name, "re-compile");
    pinfos[4].type = BOOLPARAM;
    pinfos[4].ctrlType = GUICTRL_BUTTON;
    pinfos[4].linkable = 1;
    pinfos[4].saveable = 0;
    pinfos[4].paramInfo.pb.isTrigger = 1;
    pinfos[4].paramInfo.pb.defaultValue = 0;
}

void 
CsoundTest::getParameterChoices(long id, std::vector<std::string>& vec) {
}

void 
CsoundTest::parameterUpdate(long id) {

    log("parameterUpdate called: " + to_string(id));

    if(id == 0) {
        getParameterValue(0, _blurAmt);
    } else if (id == 1) {
        getParameterValue(1, _pitch);
    }
    else if (id == 4) {
        log("_recompileTrig (before getParameterValue) = " + std::to_string(_recompileTrig));
        getParameterValue(4, _recompileTrig);
        log("_recompileTrig (after getParameterValue) = " + std::to_string(_recompileTrig));
        recompileOrc();
    }
}

void 
CsoundTest::process(Sample** sampleIn, Sample** sampleOut, MIDIEvents* midiIn, MIDIEvents* midiOut, Frequency*** freqIn, Frequency*** freqOut, Magnitude*** magIn, Magnitude*** magOut, SyncInfo* syncIn, SyncInfo* syncOut){
    
    long sampleFrames = _dspInfo.bufferSize;
    unsigned int channels = 2;
    
    // inputs
    Sample* p4in = sampleIn[3];
    Sample* p5in = sampleIn[4];
    Sample* p6in = sampleIn[5];
    Sample* p7in = sampleIn[6];
    Sample* p8in = sampleIn[7];
    
    // outputs
    Sample* s1out = sampleOut[0];
    Sample* s2out = sampleOut[1];
    


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
            csound->SetChannel("amp", _amp);
            
            (*s1out++) = spout[0 + (_ksmpsIndex * channels)];
            (*s2out++) = spout[1 + (_ksmpsIndex * channels)];
            _ksmpsIndex++;
            
        } else {
            (*s1out++) = 0.f;
            (*s2out++) = 0.f;
        }
        
    }
	

}

