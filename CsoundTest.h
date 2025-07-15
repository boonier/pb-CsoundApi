#pragma once

#include <string>
#include <memory>
#include <fstream>
#include "../common/BiduleSDK.h"
#include "csound.hpp"
#include "nfd.hpp"

using namespace std;

namespace acme
{
    using namespace plogue::biduleSDK;
    class CsoundTest : public BidulePlugin
    {

    public:
        CsoundTest(BiduleHost *host);
        virtual ~CsoundTest();
        virtual bool init() override;
        virtual void getAudioInNames(std::vector<std::string> &vec) override;
        virtual void getAudioOutNames(std::vector<std::string> &vec) override;
        virtual void getMIDIInNames(std::vector<std::string> &vec) override;
        virtual void getMIDIOutNames(std::vector<std::string> &vec) override;
        virtual void getFreqInNames(std::vector<std::string> &vec) override;
        virtual void getFreqOutNames(std::vector<std::string> &vec) override;
        virtual void getMagInNames(std::vector<std::string> &vec) override;
        virtual void getMagOutNames(std::vector<std::string> &vec) override;

        virtual void parameterUpdate(long id) override;
        virtual void getParametersInfos(ParameterInfo *pinfos) override;
        virtual void getParameterChoices(long id, vector<string> &vec) override;
        // virtual void updateParameter(long id, const std::string& strVal) override;

        virtual void process(Sample **sampleIn, Sample **sampleOut, MIDIEvents *midiIn, MIDIEvents *midiOut, Frequency ***freqIn, Frequency ***freqOut, Magnitude ***magIn, Magnitude ***magOut, SyncInfo *syncIn, SyncInfo *syncOut) override;

    protected:
        void log(string_view message);
        void openCsdFile();
        void compileCsdFile();
        void recompileCsdFile();
        void setDisplayLabel(string &label);

        string _savedCsdPath, _displayedCsdPath, _displayedParams;
        // string _filePath; // used?
        unique_ptr<Csound> _csound;
        MYFLT *spin, *spout;
        int _csCompileResult, _ksmpsIndex, _triggerOpenDialog, _doRecompile;
        double _blurAmt, _pitch, _p1, _p2, _p3, _p4, _p5, _p6, _p7, _p8;
        unique_ptr<char[]> _tempDisplayedParams;
        bool _isDone;
        // char _isRunning;
    };
};
