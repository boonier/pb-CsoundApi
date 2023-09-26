#ifndef PASSTHROUGHALL_H
#define PASSTHROUGHALL_H

#include "../common/BiduleSDK.h"
#include <string>
#include <memory>
#include <fstream>
#include "CsoundLib64/csound.hpp"
//#include "/Users/boonier/GIT/nativefiledialog-extended/src/include/nfd.hpp"

//extern "C" {
    #include "nfd.hpp"
//}

using namespace std;

namespace acme {
    using namespace plogue::biduleSDK;
    class CsoundTest: public BidulePlugin {

	    public:
		    CsoundTest(BiduleHost* host);
			virtual ~CsoundTest();
            virtual bool init();
			virtual void getAudioInNames(std::vector<std::string>& vec);
			virtual void getAudioOutNames(std::vector<std::string>& vec);
			virtual void getMIDIInNames(std::vector<std::string>& vec);
			virtual void getMIDIOutNames(std::vector<std::string>& vec);
			virtual void getFreqInNames(std::vector<std::string>& vec);
			virtual void getFreqOutNames(std::vector<std::string>& vec);
			virtual void getMagInNames(std::vector<std::string>& vec);
			virtual void getMagOutNames(std::vector<std::string>& vec);

            virtual void parameterUpdate(long id);
			virtual void getParametersInfos(ParameterInfo* pinfos);
			virtual void getParameterChoices(long id, vector<string>& vec);
//            virtual void updateParameter(long id, const std::string& strVal);

			virtual void process(Sample** sampleIn, Sample** sampleOut, MIDIEvents* midiIn, MIDIEvents* midiOut, Frequency*** freqIn, Frequency*** freqOut, Magnitude*** magIn, Magnitude*** magOut, SyncInfo* syncIn, SyncInfo* syncOut);
		
        protected:
        void log(const string& message);
        void openCsdFile();
        void compileCsdFile();
        void setDisplayLabel(string& label) ;
        
        string _savedCsdPath, _displayedCsdPath;
        string _filePath;
        unique_ptr<Csound> _csound;
        MYFLT *spin, *spout;
        int _csCompileResult, _ksmpsIndex, _triggerOpenDialog;
        double _blurAmt, _pitch;
    };
};

#endif
