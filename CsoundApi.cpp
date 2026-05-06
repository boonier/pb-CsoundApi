#include "CsoundApi.hpp"

#include <iostream>
#include <limits>
#include <memory>
#include <string>

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

  log("CsoundApi constructor initialising vars");
  // own params
  _blurAmt = 0.25;
  _p1 = _p2 = _p3 = _p4 = _p5 = _p6 = _p7 = _p8 = 0.f;

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
  log("openCsdFile");

  // initialize NFD
  NFD::Guard nfdGuard;
  // auto-freeing memory
  NFD::UniquePath outPath;
  // only allow .csd files
  nfdfilteritem_t filterItem[2] = {{"Csound CSD file", "csd"}};

  // show the dialog (this is a blocking call on the UI thread)
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
  } else {
    std::cout << "Error: " << NFD::GetError() << std::endl;
  }
}

/**
 * @brief Compile a Csound CSD file stored in _savedCsdPath.
 *
 * This function is called when the user selects a new CSD file via the
 * "Open CSD" button. It checks if a valid Csound instance exists, and if
 * not, creates one. It then compiles the CSD file using the Csound
 * instance and stores the result in the current engine state. If the compile is
 * successful, it starts the Csound performance.
 *
 * @return void
 */
void CsoundApi::compileCsdFile() {
  log("calling compileCsdFile");

  if (_savedCsdPath.empty()) {
    std::cerr << "Failed to read the file!" << std::endl;
    return;
  }

  auto newEngine = std::make_shared<EngineState>();
  newEngine->csound = std::make_shared<Csound>();
  log("version:" + to_string(newEngine->csound->GetVersion()));

  newEngine->csound->CreateMessageBuffer(0);
  newEngine->csound->SetHostAudioIO();

  log("compiling " + _savedCsdPath);
  newEngine->csound->SetOption("--env:INCDIR+=/Users/boonier/GIT/Csound/_UDOs");

  newEngine->compileResult = newEngine->csound->Compile(_savedCsdPath.c_str());
  log("compileResult = " + to_string(newEngine->compileResult));

  if (newEngine->compileResult == 0) {
    log("Successful CSD compile, starting...");
    newEngine->csound->Start();
    newEngine->spout = newEngine->csound->GetSpout();
    newEngine->spin = newEngine->csound->GetSpin();
    newEngine->ksmpsIndex = 0;

    std::vector<char> temp(256);
    newEngine->csound->GetStringChannel("cs_params", temp.data());
    log("cs_params = " + string(temp.begin(), temp.begin() + temp.size()));
    std::string str(temp.begin(), temp.begin() + temp.size());
    updateParameter(6, str);

    std::atomic_store(&_engine, newEngine);
  } else {
    while (newEngine->csound->GetMessageCnt() > 0) {
      cout << "CSOUND_MESSAGE:" << newEngine->csound->GetFirstMessage() << endl;
      newEngine->csound->PopFirstMessage();
    }
    log("CSD did not compile:" + to_string(newEngine->compileResult));
  }
}

/**
 * @brief Called when the user triggers the "Recompile CSD" button.
 *
 * This function stops any currently running Csound performance, resets the
 * Csound instance, and then calls compileCsdFile() to recompile the
 * stored CSD path.
 *
 * @return void
 */
void CsoundApi::recompileCsdFile() {
  log("recompileCsdFile called");

  // Don't reset/destroy the current engine in-place while the audio thread
  // may still be using it. Instead, compile a new engine and atomically swap.
  compileCsdFile();
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
  strcpy(pinfos[6].paramInfo.ps.defaultValue, "no p-fields defined");
}

void CsoundApi::getParameterChoices(long id, std::vector<std::string>& vec) {}

void CsoundApi::setDisplayLabel(string& label) {
  size_t lastSlashPos = label.find_last_of("/");
  _displayedCsdPath = label.substr(lastSlashPos + 1);
  updateParameter(4, _displayedCsdPath);
}

void CsoundApi::idle() {
  // Called from the UI thread (per BiduleSDK.h). Do any non time critical work here.
  if (_triggerOpenDialog.exchange(0) == 1) {
    openCsdFile();
  }

  if (_doRecompile.exchange(0) == 1) {
    recompileCsdFile();
  }
}

void CsoundApi::parameterUpdate(long id) {
  if (id == 0) {
    getParameterValue(0, _p1);
  } else if (id == 1) {
    getParameterValue(1, _p2);
  } else if (id == 2) {  // <- BTN - open file
    int triggerOpenDialog = 0;
    getParameterValue(2, triggerOpenDialog);
    _triggerOpenDialog.store(triggerOpenDialog);
  } else if (id == 3) {  // <- TEXTAREA (nogui) - store/recall the csd path
    getParameterValue(3, _savedCsdPath);
    log("parameterUpdate _savedCsdPath = " + _savedCsdPath);
    auto engine = std::atomic_load(&_engine);
    if (_savedCsdPath.length() && (!engine || engine->compileResult == -1)) {
      // update the ez label
      setDisplayLabel(_savedCsdPath);
      log("parameterUpdate _savedCsdPath = " + _savedCsdPath);
      // compile the csd as we have a path
      compileCsdFile();
    }
  } else if (id == 4) {  // <- LABEL - display current csd
    // call re-render here (if there is a valid stored path)

  } else if (id == 5) {  // <- BTN - recompile the existing stored CSD path

    log("do recompile = " + to_string(_doRecompile));
    int doRecompile = 0;
    getParameterValue(5, doRecompile);
    _doRecompile.store(doRecompile);

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

  auto engine = std::atomic_load(&_engine);

  while (--sampleFrames >= 0) {
    // while (_csound->PerformKsmps() == 0) {
    // while (_csound->GetMessageCnt() > 0) {
    //     cout << "CSOUND_MESSAGE:" << _csound->GetFirstMessage() <<
    //     endl; _csound->PopFirstMessage();
    // }
    // }

    if (engine && engine->compileResult == 0 && engine->csound &&
        engine->spin && engine->spout) {
      if (engine->ksmpsIndex == engine->csound->GetKsmps()) {
        engine->compileResult = engine->csound->PerformKsmps();
        if (engine->compileResult == 0) {
          engine->ksmpsIndex = 0;
        }
      }

      engine->csound->SetChannel("p1", _p1);
      engine->csound->SetChannel("p2", _p2);
      engine->csound->SetChannel("p3", _p3);
      engine->csound->SetChannel("p4", _p4);
      engine->csound->SetChannel("p5", _p5);
      engine->csound->SetChannel("p6", _p6);
      engine->csound->SetChannel("p7", _p7);
      engine->csound->SetChannel("p8", _p8);

      engine->spin[0 + (engine->ksmpsIndex * channels)] = *s1in++;
      engine->spin[1 + (engine->ksmpsIndex * channels)] = *s2in++;

      (*s1out++) = engine->spout[0 + (engine->ksmpsIndex * channels)];
      (*s2out++) = engine->spout[1 + (engine->ksmpsIndex * channels)];

      engine->ksmpsIndex++;
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
