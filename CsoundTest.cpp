#include "CsoundTest.h"
#include <iostream>
#include <string>
#include <math.h>

//#include "nfd.hpp"

#ifdef __APPLE__
//#include <corefoundation/CFBundle.h>
#include <CoreFoundation/CoreFoundation.h>
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
    _numParams = 5;
    
    // own params
    _blurAmt = 0.25;
    
    _csCompileResult = -1;
    _ksmpsIndex = 0;
    _triggerOpenDialog = 0;
}

CsoundTest::~CsoundTest(){
    //free Csound stuff
    _csound->Cleanup();
    _csound->Reset();
//    if (_inputFile.is_open()) {
//        _inputFile.close();
//    }
}

void
CsoundTest::log(const std::string& message) {
    cout << "[LOG] " << message << endl;
}

void
CsoundTest::openCsdFile() {
    if (_triggerOpenDialog == 1) {
        log("openCsdFile" + to_string(_triggerOpenDialog));
        // initialize NFD
        NFD::Guard nfdGuard;
        // auto-freeing memory
        NFD::UniquePath outPath;
        // only allow .csd files
        nfdfilteritem_t filterItem[2] = {{"Csound CSD file", "csd"}};

        // show the dialog
        nfdresult_t result = NFD::OpenDialog(outPath, filterItem, 1);
        if (result == NFD_OKAY) {
            string outPathStr(outPath.get());
            log("outPath result = " + outPathStr);
            // store outPathStr to label
            _savedCsdPath = outPathStr;
            updateParameter(3, _savedCsdPath);
            
            // update the gui
//            size_t lastSlashPos = outPathStr.find_last_of("/");
//            _displayedCsdPath = outPathStr.substr(lastSlashPos + 1);
//            updateParameter(4, _displayedCsdPath);
            setDisplayLabel(outPathStr);
            
        } else if (result == NFD_CANCEL) {
            log("User pressed cancel.");
        } else {
            std::cout << "Error: " << NFD::GetError() << std::endl;
        }
        
        // reset trigger state
        _triggerOpenDialog = 0;
    }

}

bool
CsoundTest::init() {
    log("CsoundTest init");
    
    return true;
    
    
//    _csound = make_unique<Csound>(new Csound());
//    cout << _csound << endl;
//    cout << "version:" << _csound->GetVersion() << endl;
//    cout << "api version:" << _csound->GetAPIVersion() << endl;
//    _csound->CreateMessageBuffer(0);
//    _csound->SetHostImplementedAudioIO(1, 0);
//
//
//    if (_filePath.length()) {
//        _csCompileResult = _csound->Compile(_filePath.c_str());
//        log("_csCompileResult = " + to_string(_csCompileResult));
//
//        if (_csCompileResult == 0) { // compiled OK...
//            cout << "CSOUND_MESSAGE:" << _csound->GetFirstMessage() << endl;
//            //access Csound's input/output buffer
//            spout = _csound->GetSpout();
//            spin  = _csound->GetSpin();
//
//            //start Csound performance
//            _csound->Start();
//            cout << "Successful CSD compile, starting..." << endl;
//            return true;
//        } else {
//            while (_csound->GetMessageCnt() > 0) {
//               cout << "CSOUND_MESSAGE:" << _csound->GetFirstMessage() << endl;
//               _csound->PopFirstMessage();
//            }
//            cout << "CSD did not compile:" << _csCompileResult << endl;
//            return false;
//        }
//    } else {
//        std::cerr << "Failed to read the file!" << std::endl;
//        return false;
//    }
    
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
    strcpy(pinfos[2].name, "Choose CSD file");
    pinfos[2].type = BOOLPARAM;
    pinfos[2].ctrlType = GUICTRL_BUTTON;
    pinfos[2].linkable = 1;
    pinfos[2].saveable = 0;
    pinfos[2].paramInfo.pb.isTrigger = 1;
    pinfos[2].paramInfo.pb.defaultValue = 0;
    
    // state
    pinfos[3].id = 3;
    strcpy(pinfos[3].name, "");
    pinfos[3].type = STRINGPARAM;
    pinfos[3].ctrlType = GUICTRL_TEXTBOX;
    pinfos[3].linkable = 0;
    pinfos[3].saveable = 1;
    strcpy(pinfos[3].paramInfo.ps.defaultValue, "");
    
    pinfos[4].id = 4;
    strcpy(pinfos[4].name, "csd name:");
    pinfos[4].type = STRINGPARAM;
    pinfos[4].ctrlType = GUICTRL_LABEL;
    pinfos[4].linkable = 0;
    pinfos[4].saveable = 0;
    strcpy(pinfos[4].paramInfo.ps.defaultValue, "No path has been set");

}

void 
CsoundTest::getParameterChoices(long id, std::vector<std::string>& vec) {
}

void
CsoundTest::setDisplayLabel(string& label) {
    size_t lastSlashPos = label.find_last_of("/");
    _displayedCsdPath = label.substr(lastSlashPos + 1);
    updateParameter(4, _displayedCsdPath);
}

void
CsoundTest::compileCsdFile(){
    log("calling compileCsdFile");
    // 1. On init, check _savedCsdPath for a stored csd path
    //      a. if nothing found, do nothing
    // 2. If path found, check for existing instance of Csound
    //      a. if Csound found, assign value of nullptr
    // 3. then create new Csound instance
    // 4. call csound->Compile(...), and store result to _csCompileResult
    // 5.
    
    if (_csound.get() != nullptr) {
        _csound = nullptr;
    }
    
    _csound = make_unique<Csound>(new Csound());
    cout << _csound << endl;
    cout << "version:" << _csound->GetVersion() << endl;
    cout << "api version:" << _csound->GetAPIVersion() << endl;
    _csound->CreateMessageBuffer(0);
    _csound->SetHostImplementedAudioIO(1, 0);
    
    
    if (_savedCsdPath.length() > 0) {
        _csCompileResult = _csound->Compile(_savedCsdPath.c_str());
        log("_csCompileResult = " + to_string(_csCompileResult));
                
        if (_csCompileResult == 0) { // compiled OK...
            cout << "CSOUND_MESSAGE:" << _csound->GetFirstMessage() << endl;
            //access Csound's input/output buffer
            spout = _csound->GetSpout();
            spin  = _csound->GetSpin();

            //start Csound performance
            _csound->Start();
            cout << "Successful CSD compile, starting..." << endl;
//            return true;
        } else {
            while (_csound->GetMessageCnt() > 0) {
               cout << "CSOUND_MESSAGE:" << _csound->GetFirstMessage() << endl;
               _csound->PopFirstMessage();
            }
            cout << "CSD did not compile:" << _csCompileResult << endl;
//            return false;
        }
    } else {
        std::cerr << "Failed to read the file!" << std::endl;
//        return false;
    }

}

void 
CsoundTest::parameterUpdate(long id) {
    if(id == 0) {
        getParameterValue(0, _blurAmt);
    } else if (id == 1) {
        getParameterValue(1, _pitch);
    } else if (id == 2) {
        getParameterValue(2, _triggerOpenDialog);
        openCsdFile();
    } else if (id == 3) {
        getParameterValue(3, _savedCsdPath);
        log("parameterUpdate _savedCsdPath = " + _savedCsdPath);
        
        if (_savedCsdPath.length()) {
            // update the ez label
            setDisplayLabel(_savedCsdPath);
            //compile the csd as we have a path
            compileCsdFile();
        }
    }
//        else if (id == 4) {
//
//        log("parameterUpdate _storedCsdPath = " + _savedCsdPath);
//        // call re-render here (if there is a valid stored path)
//    }
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
            if (_ksmpsIndex == _csound->GetKsmps()) {
                _csCompileResult = _csound->PerformKsmps();
                if (_csCompileResult == 0)
                    _ksmpsIndex = 0;
            }
            
            _csound->SetChannel("phsRate", _blurAmt);
            _csound->SetChannel("pitch", _pitch);
            
            (*s1++) = spout[0 + (_ksmpsIndex * channels)];
            (*s2++) = spout[1 + (_ksmpsIndex * channels)];
            
            _ksmpsIndex++;
        } else {
            (*s1++) = 0.f;
            (*s2++) = 0.f;
        }
    }
	
}

