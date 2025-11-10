#include "CsoundApi.hpp"

#include <math.h>

#include <iostream>
#include <limits>
#include <memory>
#include <string>
#include <thread>

// #include "csound.hpp"

// #include "nfd.hpp"

#ifdef __APPLE__
#include <CoreFoundation/CoreFoundation.h>
#endif

using namespace plogue::biduleSDK;
using namespace acme;
using namespace std;

CsoundApi::CsoundApi(BiduleHost* host) : BidulePlugin(host) {
  // fill in plugin capabilities as a mask
  //  _caps = CAP_SYNCMASTER | CAP_SYNCSLAVE
  _caps = CAP_SYNCSLAVE;

  _numAudioIns = 2;
  _numAudioOuts = 2;
  _numMIDIIns = 1;
  _numMIDIOuts = 1;
  _numFreqIns = 0;
  _numFreqOuts = 0;
  _numMagIns = 0;
  _numMagOuts = 0;
  _numUIColumns = 1;
  _numParams = 7;

  // own params
  _blurAmt = 0.25;
  _p1 = _p2 = _p3 = _p4 = _p5 = _p6 = _p7 = _p8 = 0.f;

  _csCompileResult = -1;
  _ksmpsIndex = 0;
  _triggerOpenDialog = 0;
  _doRecompile = 0;

  _displayedParams = "";
  _tempDisplayedParams = unique_ptr<char[]>(new char[4096]);

  _isDone = false;
}

CsoundApi::~CsoundApi() { log("CsoundApi destructor"); }

void CsoundApi::log(string_view message) {
  cout << "[LOG] " << message << endl;
}

/**
 * @brief Opens a file dialog for selecting a Csound CSD file. The chosen file
 * path is stored in _savedCsdPath and used to compile the Csound engine.
 *
 * This function is called by the GUI when the user clicks on the "Open CSD"
 * button.
 *
 * @return void
 */
void CsoundApi::openCsdFile() {
  if (_triggerOpenDialog == 1) {
    log("openCsdFile" + to_string(_triggerOpenDialog));

    // Pause audio processing
    _isProcessing = false;

    // Give audio thread time to finish current processing
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

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
      setDisplayLabel(outPathStr);

      // compile the new csd path
      compileCsdFile();
    } else if (result == NFD_CANCEL) {
      log("User pressed cancel.");
      _isProcessing = true;
    } else {
      std::cout << "Error: " << NFD::GetError() << std::endl;
      _isProcessing = true;
    }

    // reset trigger state
    _triggerOpenDialog = 0;
  }
}

/**
 * @brief Compile a Csound CSD file stored in _savedCsdPath.
 *
 * This function is called when the user selects a new CSD file via the
 * "Open CSD" button. It checks if a valid Csound instance exists, and if
 * not, creates one. It then compiles the CSD file using the Csound
 * instance and stores the result in _csCompileResult. If the compile is
 * successful, it starts the Csound performance.
 *
 * @return void
 */
void CsoundApi::compileCsdFile() {
  log("calling compileCsdFile");

  // re-create the csound instance
  _csound = make_unique<Csound>();
  // check it's instantiated
  log("version:" + to_string(_csound->GetVersion()));

  _csound->CreateMessageBuffer(0);
  _csound->SetHostAudioIO();

  if (_savedCsdPath.length() > 0 && _csound != nullptr) {
    log("compiling " + _savedCsdPath);
    _csCompileResult = _csound->Compile(_savedCsdPath.c_str());
    log("_csCompileResult = " + to_string(_csCompileResult));

    if (_csCompileResult == 0) {  // compiled OK...
      log("Successful CSD compile, starting...");
      _csound->Start();
      spout = _csound->GetSpout();
      spin = _csound->GetSpin();
      _isProcessing = true;

      //////
      std::string logMsg = "Process status: ";
      logMsg +=
          "_isProcessing=" + std::string(_isProcessing ? "true" : "false");
      logMsg += ", _csCompileResult=" + std::to_string(_csCompileResult);
      logMsg += ", _csound=" + std::string(_csound ? "valid" : "nullptr");
      logMsg += ", spin=" + std::string(spin ? "valid" : "nullptr");
      logMsg += ", spout=" + std::string(spout ? "valid" : "nullptr");
      log(logMsg);
      /////

      std::vector<char> temp(256);  // allocate a buffer of size 256
      _csound->GetStringChannel("cs_params", temp.data());
      log("cs_params = " + string(temp.begin(), temp.begin() + temp.size()));
      std::string str(temp.begin(), temp.begin() + temp.size());
      updateParameter(6, str);

    } else {
      while (_csound->GetMessageCnt() > 0) {
        cout << "CSOUND_MESSAGE:" << _csound->GetFirstMessage() << endl;
        _csound->PopFirstMessage();
      }
      log("CSD did not compile:" + to_string(_csCompileResult));
      //            return false;
    }
  } else {
    std::cerr << "Failed to read the file!" << std::endl;
    //        return false;
  }
}

void CsoundApi::recompileCsdFile() {
  log("recompileCsdFile called");

  _isProcessing = false;

  std::this_thread::sleep_for(std::chrono::milliseconds(10));

  if (_doRecompile == 1) {
    if (_csound) {
      _csound->Reset();
    }
    spin = nullptr;
    spout = nullptr;
    _csCompileResult = -1;
    _doRecompile = 0;

    // call compileCsdFile and join all the stuff up again
    compileCsdFile();
  }
}

bool CsoundApi::init() {
  log("CsoundApi init");
  return true;
}

void CsoundApi::getAudioInNames(std::vector<std::string>& vec) {
  for (int i = 0; i < 16; i++) {
    std::string name = "p" + std::to_string(i + 1);
    vec.push_back(name);
  }
  vec.push_back("Send Event");
}

void CsoundApi::getAudioOutNames(std::vector<std::string>& vec) {
  vec.push_back("Csound output 1");
  vec.push_back("Csound output 2");
}

void CsoundApi::getMIDIInNames(std::vector<std::string>& vec) {}

void CsoundApi::getMIDIOutNames(std::vector<std::string>& vec) {}

void CsoundApi::getFreqInNames(std::vector<std::string>& vec) {}

void CsoundApi::getFreqOutNames(std::vector<std::string>& vec) {}

void CsoundApi::getMagInNames(std::vector<std::string>& vec) {}

void CsoundApi::getMagOutNames(std::vector<std::string>& vec) {}

void CsoundApi::getParametersInfos(ParameterInfo* pinfos) {
  pinfos[0].id = 0;
  strcpy(pinfos[0].name, "param 1");
  pinfos[0].type = DOUBLEPARAM;
  pinfos[0].ctrlType = GUICTRL_SLIDER;
  pinfos[0].linkable = 1;
  pinfos[0].saveable = 1;
  pinfos[0].paramInfo.pd.defaultValue = 0.f;
  pinfos[0].paramInfo.pd.minValue = -std::numeric_limits<float>::max();
  pinfos[0].paramInfo.pd.maxValue = std::numeric_limits<float>::max();
  pinfos[0].paramInfo.pd.precision = 6;

  pinfos[1].id = 1;
  strcpy(pinfos[1].name, "param 2");
  pinfos[1].type = DOUBLEPARAM;
  pinfos[1].ctrlType = GUICTRL_SLIDER;
  pinfos[1].linkable = 1;
  pinfos[1].saveable = 1;
  pinfos[1].paramInfo.pd.defaultValue = 0.f;
  pinfos[1].paramInfo.pd.minValue = -1.f;
  pinfos[1].paramInfo.pd.maxValue = 1.f;
  pinfos[1].paramInfo.pd.precision = 6;

  pinfos[2].id = 2;
  strcpy(pinfos[2].name, "choose CSD file");
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
  pinfos[3].ctrlType = GUICTRL_NOGUI;
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

  pinfos[5].id = 5;
  strcpy(pinfos[5].name, "recompile");
  pinfos[5].type = BOOLPARAM;
  pinfos[5].ctrlType = GUICTRL_BUTTON;
  pinfos[5].linkable = 1;
  pinfos[5].saveable = 0;
  pinfos[5].paramInfo.pb.isTrigger = 1;
  pinfos[5].paramInfo.pb.defaultValue = 0;

  // parameters
  pinfos[6].id = 6;
  strcpy(pinfos[6].name, "info");
  pinfos[6].type = STRINGPARAM;
  pinfos[6].ctrlType = GUICTRL_TEXTAREA;
  pinfos[6].linkable = 0;
  pinfos[6].saveable = 0;
  // strcpy(pinfos[6].paramInfo.ps.defaultValue, "no p-fields");
  strcpy(pinfos[6].paramInfo.ps.defaultValue, "no p-fields");
}

void CsoundApi::getParameterChoices(long id, std::vector<std::string>& vec) {}

void CsoundApi::setDisplayLabel(string& label) {
  size_t lastSlashPos = label.find_last_of("/");
  _displayedCsdPath = label.substr(lastSlashPos + 1);
  updateParameter(4, _displayedCsdPath);
}

void CsoundApi::parameterUpdate(long id) {
  if (id == 0) {
    getParameterValue(0, _p1);
  } else if (id == 1) {
    getParameterValue(1, _p2);
  } else if (id == 2) {  // <- BTN - open file
    getParameterValue(2, _triggerOpenDialog);
    openCsdFile();
  } else if (id == 3) {  // <- TEXTAREA (nogui) - store/recall the csd path
    getParameterValue(3, _savedCsdPath);
    log("parameterUpdate _savedCsdPath = " + _savedCsdPath);
    if (_savedCsdPath.length() && _csCompileResult == -1) {
      // update the ez label
      setDisplayLabel(_savedCsdPath);
      // compile the csd as we have a path
      compileCsdFile();
    }
  } else if (id ==
             4) {  // <- LABEL - display current csd
                   // call re-render here (if there is a valid stored path)
  } else if (id == 5) {  // <- BTN - recompile the existing stored CSD path
    log("do recompile");
    // set the _doRecompile flag

    getParameterValue(5, _doRecompile);
    // call the recompile fn
    recompileCsdFile();
  } else if (id == 6) {
    log("p-fields textarea");
  }
}
void CsoundApi::process(Sample** sampleIn, Sample** sampleOut,
                        MIDIEvents* midiIn, MIDIEvents* midiOut,
                        Frequency*** freqIn, Frequency*** freqOut,
                        Magnitude*** magIn, Magnitude*** magOut,
                        SyncInfo* syncIn, SyncInfo* syncOut) {
  // Ensure we have valid pointers before processing
  if (!sampleIn || !sampleOut) return;

  long sampleFrames = _dspInfo.bufferSize;
  unsigned int channels = 2;

  Sample* s1in = sampleIn[0];
  Sample* s2in = sampleIn[1];
  Sample* s1out = sampleOut[0];
  Sample* s2out = sampleOut[1];

  while (--sampleFrames >= 0) {
    // while (_csound->PerformKsmps() == 0) {
    // while (_csound->GetMessageCnt() > 0) {
    //     cout << "CSOUND_MESSAGE:" << _csound->GetFirstMessage() <<
    //     endl; _csound->PopFirstMessage();
    // }
    // }

    // Only process if we have everything we need
    if (_isProcessing && _csCompileResult == 0 && _csound != nullptr &&
        spin != nullptr && spout != nullptr) {
      if (_ksmpsIndex == _csound->GetKsmps()) {
        // _isRunning = syncIn->playing;
        // log("isRunning = " + to_string(_isRunning));
        _csCompileResult = _csound->PerformKsmps();

        if (_csCompileResult == 0) {
          _ksmpsIndex = 0;
        }
      }

      // use the plugin ui slider
      _csound->SetChannel("p1", _p1);
      _csound->SetChannel("p2", _p2);
      _csound->SetChannel("p3", _p3);
      _csound->SetChannel("p4", _p4);
      _csound->SetChannel("p5", _p5);
      _csound->SetChannel("p6", _p6);
      _csound->SetChannel("p7", _p7);
      _csound->SetChannel("p8", _p8);

      // send the input to csound
      spin[0 + (_ksmpsIndex * channels)] = *s1in++;
      spin[1 + (_ksmpsIndex * channels)] = *s2in++;

      // get the output from csound
      (*s1out++) = spout[0 + (_ksmpsIndex * channels)];
      (*s2out++) = spout[1 + (_ksmpsIndex * channels)];

      _ksmpsIndex++;
    } else {
      (*s1out++) = 0.f;
      (*s2out++) = 0.f;
    }

    if (!_isDone) {
      log("processing...");
      _isDone = true;
    }
  }
}
