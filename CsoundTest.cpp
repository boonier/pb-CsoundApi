#include "CsoundTest.h"
#include <iostream>
#include <string>
#include <math.h>
#include <memory>

// #include "nfd.hpp"

#ifdef __APPLE__
#include <CoreFoundation/CoreFoundation.h>
#endif

using namespace plogue::biduleSDK;
using namespace acme;
using namespace std;

CsoundTest::CsoundTest(BiduleHost *host) : BidulePlugin(host)
{
    // fill in plugin capabilities as a mask
    //  _caps = CAP_SYNCMASTER | CAP_SYNCSLAVE
    _caps = CAP_SYNCSLAVE;

    _numAudioIns = 2;
    _numAudioOuts = 2;
    _numMIDIIns = 0;
    _numMIDIOuts = 0;
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

CsoundTest::~CsoundTest()
{
    // free Csound stuff
    _csound->Cleanup();
    _csound->Reset();
    _csound = nullptr;
    //    delete _tempDisplayedParams;
}

void CsoundTest::log(string_view message)
{
    cout << "[LOG] " << message << endl;
}

void CsoundTest::openCsdFile()
{
    if (_triggerOpenDialog == 1)
    {
        log("openCsdFile" + to_string(_triggerOpenDialog));
        // initialize NFD
        NFD::Guard nfdGuard;
        // auto-freeing memory
        NFD::UniquePath outPath;
        // only allow .csd files
        nfdfilteritem_t filterItem[2] = {{"Csound CSD file", "csd"}};

        // show the dialog
        nfdresult_t result = NFD::OpenDialog(outPath, filterItem, 1);
        if (result == NFD_OKAY)
        {
            string outPathStr(outPath.get());
            log("outPath result = " + outPathStr);
            // store outPathStr to label
            _savedCsdPath = outPathStr;
            updateParameter(3, _savedCsdPath);

            // update the gui
            setDisplayLabel(outPathStr);

            // compile the new csd path
            compileCsdFile();
        }
        else if (result == NFD_CANCEL)
        {
            log("User pressed cancel.");
        }
        else
        {
            std::cout << "Error: " << NFD::GetError() << std::endl;
        }

        // reset trigger state
        _triggerOpenDialog = 0;
    }
}

void CsoundTest::compileCsdFile()
{
    log("calling compileCsdFile");
    // 1. On init, check _savedCsdPath for a stored csd path
    //      a. if nothing found, do nothing
    // 2. If path found, check for existing instance of Csound
    //      a. if Csound found, assign value of nullptr
    // 3. then create new Csound instance
    // 4. call csound->Compile(...), and store result to _csCompileResult
    // 5.

    // let's reset the csound instance, said Rory
    if (_csound.get() != nullptr)
    {
        // does Reset() need to be called if setting to nullptr?
        _csound->Reset();
        _csound = nullptr;
        // reset the _csCompileResult flag
        _csCompileResult = -1;
    }

    // re-create the csound instance
    _csound = make_unique<Csound>(new Csound());
    ///
    cout << _csound << endl;
    cout << "version:" << _csound->GetVersion() << endl;
    cout << "api version:" << _csound->GetAPIVersion() << endl;
    ///
    _csound->CreateMessageBuffer(0);
    _csound->SetHostImplementedAudioIO(1, 0);

    if (_savedCsdPath.length() > 0)
    {
        _csCompileResult = _csound->Compile(_savedCsdPath.c_str());
        log("_csCompileResult = " + to_string(_csCompileResult));

        if (_csCompileResult == 0)
        { // compiled OK...
            cout << "CSOUND_MESSAGE:" << _csound->GetFirstMessage() << endl;
            // access Csound's input/output buffer
            spout = _csound->GetSpout();
            spin = _csound->GetSpin();

            // start Csound performance
            _csound->Start();

            std::vector<char> temp(256); // allocate a buffer of size 256
            _csound->GetStringChannel("cs_params", temp.data());
            log("cs_params = " + string(temp.begin(), temp.begin() + temp.size()));
            std::string str(temp.begin(), temp.begin() + temp.size());
            updateParameter(6, str);

            cout << "Successful CSD compile, starting..." << endl;
        }
        else
        {
            while (_csound->GetMessageCnt() > 0)
            {
                cout << "CSOUND_MESSAGE:" << _csound->GetFirstMessage() << endl;
                _csound->PopFirstMessage();
            }
            cout << "CSD did not compile:" << _csCompileResult << endl;
            //            return false;
        }
    }
    else
    {
        std::cerr << "Failed to read the file!" << std::endl;
        //        return false;
    }
}

void CsoundTest::recompileCsdFile()
{
    log("recompileCsdFile called");
    if (_doRecompile == 1)
    {
        // set the outputs to 0 in the process loop?
        // reset shit
        _csCompileResult = -1;
        _doRecompile = 0;
        spin = nullptr;
        spout = nullptr;
        // call compileCsdFile and join all the stuff up again
        compileCsdFile();
    }
}

bool CsoundTest::init()
{
    log("CsoundTest init");
    return true;
}

void CsoundTest::getAudioInNames(std::vector<std::string> &vec)
{
    vec.push_back("p1");
    vec.push_back("p2");
    //    vec.push_back("p3");
    //    vec.push_back("p4");
    //    vec.push_back("p5");
    //    vec.push_back("p6");
    //    vec.push_back("p7");
    //    vec.push_back("p8");
    vec.push_back("Send Event");
}

void CsoundTest::getAudioOutNames(std::vector<std::string> &vec)
{
    vec.push_back("Csound output 1");
    vec.push_back("Csound output 2");
}

void CsoundTest::getMIDIInNames(std::vector<std::string> &vec)
{
}

void CsoundTest::getMIDIOutNames(std::vector<std::string> &vec)
{
}

void CsoundTest::getFreqInNames(std::vector<std::string> &vec)
{
}

void CsoundTest::getFreqOutNames(std::vector<std::string> &vec)
{
}

void CsoundTest::getMagInNames(std::vector<std::string> &vec)
{
}

void CsoundTest::getMagOutNames(std::vector<std::string> &vec)
{
}

void CsoundTest::getParametersInfos(ParameterInfo *pinfos)
{
    pinfos[0].id = 0;
    strcpy(pinfos[0].name, "param 1");
    pinfos[0].type = DOUBLEPARAM;
    pinfos[0].ctrlType = GUICTRL_SLIDER;
    pinfos[0].linkable = 1;
    pinfos[0].saveable = 1;
    pinfos[0].paramInfo.pd.defaultValue = 0.f;
    pinfos[0].paramInfo.pd.minValue = -1.f;
    pinfos[0].paramInfo.pd.maxValue = 1.f;
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

void CsoundTest::getParameterChoices(long id, std::vector<std::string> &vec)
{
}

void CsoundTest::setDisplayLabel(string &label)
{
    size_t lastSlashPos = label.find_last_of("/");
    _displayedCsdPath = label.substr(lastSlashPos + 1);
    updateParameter(4, _displayedCsdPath);
}

void CsoundTest::parameterUpdate(long id)
{
    if (id == 0)
    {
        getParameterValue(0, _p1);
    }
    else if (id == 1)
    {
        getParameterValue(1, _p2);
    }
    else if (id == 2)
    { // <- BTN - open file
        getParameterValue(2, _triggerOpenDialog);
        openCsdFile();
    }
    else if (id == 3)
    { // <- TEXTAREA (nogui) - store/recall the csd path
        getParameterValue(3, _savedCsdPath);
        log("parameterUpdate _savedCsdPath = " + _savedCsdPath);
        if (_savedCsdPath.length() && _csCompileResult == -1)
        {
            // update the ez label
            setDisplayLabel(_savedCsdPath);
            // compile the csd as we have a path
            compileCsdFile();
        }
    }
    else if (id == 4)
    { // <- LABEL - display current csd
      // call re-render here (if there is a valid stored path)
    }
    else if (id == 5)
    { // <- BTN - recompile the existing stored CSD path
        log("do recompile");
        // set the _doRecompile flag
        getParameterValue(5, _doRecompile);
        // call the recompile fn
        recompileCsdFile();
    }
    else if (id == 6)
    {
        log("p-fields textarea");
    }
}
void CsoundTest::process(Sample **sampleIn, Sample **sampleOut, MIDIEvents *midiIn, MIDIEvents *midiOut, Frequency ***freqIn, Frequency ***freqOut, Magnitude ***magIn, Magnitude ***magOut, SyncInfo *syncIn, SyncInfo *syncOut)
{

    long sampleFrames = _dspInfo.bufferSize;
    unsigned int channels = 2;

    Sample *s1out = sampleOut[0];
    Sample *s2out = sampleOut[1];

    while (--sampleFrames >= 0)
    {
        //        while (_csound->PerformKsmps() == 0) {
        //           while (_csound->GetMessageCnt() > 0) {
        //              cout << "CSOUND_MESSAGE:" << _csound->GetFirstMessage() << endl;
        //              _csound->PopFirstMessage();
        //           }
        //        }

        if (_csCompileResult == 0)
        {
            if (_ksmpsIndex == _csound->GetKsmps())
            {
                _csCompileResult = _csound->PerformKsmps();
                if (_csCompileResult == 0)
                {
                    //                    while (_csound->GetMessageCnt() > 0) {
                    //                        cout << "CSOUND_MESSAGE:" << _csound->GetFirstMessage() <<  endl;
                    //                        _csound->PopFirstMessage();
                    //                    }
                    _ksmpsIndex = 0;
                }
            }

            //            if (_tempDisplayedParams[0] == '\0') {
            //                _csound->GetStringChannel("Pfields", _tempDisplayedParams);
            //                updateParameter(6, _tempDisplayedParams);
            //            }

            // update the channels -- loop?
            //            if ((*s1in++) == 0.f) {
            // use the plugin ui slider
            _csound->SetChannel("p1", _p1);
            //            } else {
            //                // use the input pin
            //                _csound->SetChannel("p1", (*s1in++));
            //            }
            //
            _csound->SetChannel("p2", _p2);
            _csound->SetChannel("p3", _p3);
            _csound->SetChannel("p4", _p4);
            _csound->SetChannel("p5", _p5);
            _csound->SetChannel("p6", _p6);
            _csound->SetChannel("p7", _p7);
            _csound->SetChannel("p8", _p8);

            // send the noise out
            (*s1out++) = spout[0 + (_ksmpsIndex * channels)];
            (*s2out++) = spout[1 + (_ksmpsIndex * channels)];

            _ksmpsIndex++;
        }
        else
        {
            (*s1out++) = 0.f;
            (*s2out++) = 0.f;
        }

        if (!_isDone)
        {
            log("process");
            _isDone = true;
        }
    }
}
