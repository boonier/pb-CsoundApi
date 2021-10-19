
#include "CsoundTest.h"
#include "CsoundLib64/csound.hpp"
#include <iostream>
#include <string>

using namespace plogue::biduleSDK;
using namespace acme;
using namespace std;

CsoundTest::CsoundTest(BiduleHost* host):
BidulePlugin(host){
	//fill in plugin capabilities as a mask
    //  _caps = CAP_SYNCMASTER | CAP_SYNCSLAVE
    _caps = CAP_SYNCSLAVE;

	_numAudioIns=2;
	_numAudioOuts=2;
	_numMIDIIns=0;
	_numMIDIOuts=0;
	_numFreqIns=0;
	_numFreqOuts=0;
	_numMagIns=0;
	_numMagOuts=0;
    _numUIColumns=1;
    
}

CsoundTest::~CsoundTest(){
    //free Csound object
    delete csound;
}

bool CsoundTest::init() {
    cout << "Init CsoundTest" << endl;
    csound = new Csound();
    cout << csound << endl;
    cout << "version:" << csound->GetVersion() << endl;
    cout << "api version:" << csound->GetAPIVersion() << endl;
    
    //compile instance of csound.
    int csCompileResult = csound->Compile("/GIT-work/Csound/csoundAPI_examples/cpp/test8.csd");
    // csound->CompileCsdText(csoundText.c_str());
    
    if (csCompileResult == 0) { // compiled OK...
        //access Csound's input/output buffer
        spout = csound->GetSpout();
        spin  = csound->GetSpin();
        
        //start Csound performance
        csound->Start();
        cout << "Successful CSD compile, starting..." << endl;
        return true;
    } else {
        cout << "CSD did not compile" << endl;
        return false;
    }

}

void 
CsoundTest::getAudioInNames(std::vector<std::string>& vec){
	vec.push_back("Csound input 1");
    vec.push_back("Csound input 2");
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
}

void 
CsoundTest::getParameterChoices(long id, std::vector<std::string>& vec) {
}

void 
CsoundTest::parameterUpdate(long id) {
}

void 
CsoundTest::process(Sample** sampleIn, Sample** sampleOut, MIDIEvents* midiIn, MIDIEvents* midiOut, Frequency*** freqIn, Frequency*** freqOut, 
						Magnitude*** magIn, Magnitude*** magOut, SyncInfo* syncIn, SyncInfo* syncOut){
	

    
    int sampleCount=0;

    //For sake of simplicity we are generating a simple waveform.
    //In most cases a signal will be taken directly from your sound
    //card using an audio library.
    MYFLT simpleRamp[48000];
    
    for (int i=0; i<48000; i++) {
        simpleRamp[i] = (float)i/44100.f;
//        if (i == 0)
//            cout << "loop" << endl;
    }


    
    while(csound->PerformKsmps() == 0)
    {
        //this is our main processing loop. External audio can be sent
        //to Csound in this loop by writing directly to the spin
        //buffers. Csound picks up the input signal using its audio input opcodes.
        //see test8.csd.
        for (int i=0;i<csound->GetKsmps();i++)
        {
            spin[i] = simpleRamp[sampleCount];
            sampleCount=sampleCount==48000 ? 0 : sampleCount+1;

            //Csound's output signal can also be accessed through spout and output to
            //a soundcard using an audio library.
        }
    }

    
	//VC6 has scope problems with for loop indices
	unsigned int i = 0;

	//1 - you'll always have _dspInfo.bufferSize samples to process
	//2 - you'll always have _dspInfo.fftOverlapPerBuffer overlap per mag/freq channels to process each having _dspInfo.fftWindowSizeHalf bins
	//3 - if you're not synced to anything, syncIn will be NULL
	//4 - midiIn events will always be sorted by bufTick
	//5 - midiOut events will be sorted afterwards so it's at your convenience to create them sorted or not.
	
	for(i = 0; i < _numAudioIns; ++i)
		memcpy(sampleOut[i], sampleIn[i], _dspInfo.bufferSize * sizeof(Sample));

//	long sampleFrames = _dspInfo.bufferSize;
//	float paramValf = (float)_dParamValue;
	
//	Sample* s1 = sampleOut[1];
//	while(--sampleFrames >= 0)
//		(*s1++) = paramValf;
		
		
//	Sample* s2 = sampleOut[2];
//	if(!_afConnected)
//		memset(s2, 0, _dspInfo.bufferSize * sizeof(Sample));
//	else {
//		sampleFrames = _dspInfo.bufferSize;
//		while(--sampleFrames >= 0) {
			//dumb, dumb
//			if(_dumbAFCounter >= _af.numSamples)
//				_dumbAFCounter = 0;
//				(*s2++) = _af.channels[0][_dumbAFCounter];
//				_dumbAFCounter++;
//		}
//	}

//	for(i = 0; i < _numMIDIIns; ++i) {
//		midiOut[i].numEvents = midiIn[i].numEvents;
//		for(j = 0; j < midiIn[i].numEvents; ++j) {
//			midiOut[i].events[j].midiData[0] = midiIn[i].events[j].midiData[0];
//			midiOut[i].events[j].midiData[1] = midiIn[i].events[j].midiData[1];
//			midiOut[i].events[j].midiData[2] = midiIn[i].events[j].midiData[2];
//			midiOut[i].events[j].midiData[3] = midiIn[i].events[j].midiData[3];
//			midiOut[i].events[j].bufTick = midiIn[i].events[j].bufTick;
//			midiOut[i].events[j].fdetune = midiIn[i].events[j].fdetune;
//		}
//	}
//
//	for(i = 0; i < _numFreqIns; ++i)
//		for(j = 0; j < (size_t)_dspInfo.fftOverlapPerBuffer; ++j)
//			memcpy(freqOut[i][j], freqIn[i][j], _dspInfo.fftWindowSizeHalf * sizeof(Frequency));
//
//	for(i = 0; i < _numMagIns; ++i)
//		for(j = 0; j < (size_t)_dspInfo.fftOverlapPerBuffer; ++j)
//			memcpy(magOut[i][j], magIn[i][j], _dspInfo.fftWindowSizeHalf * sizeof(Magnitude));

}

