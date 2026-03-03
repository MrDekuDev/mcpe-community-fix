#ifndef SoundSystemWin_H__
#define SoundSystemWin_H__

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <mmsystem.h>

#include "SoundSystem.h"
#include <vector>
#include <list>

#pragma comment(lib, "winmm.lib")

class SoundSystemWin: public SoundSystem
{
public:
    SoundSystemWin();
    ~SoundSystemWin();

    virtual bool isAvailable() { return available; }
    virtual void enable(bool status);

    virtual void setListenerPos(float x, float y, float z);
    virtual void setListenerAngle(float deg);

    virtual void playAt(const SoundDesc& desc, float x, float y, float z, float volume, float pitch);

private:
    struct PlayingSound
    {
        HWAVEOUT hWaveOut;
        WAVEHDR waveHdr;
        char* buffer;
        int bufferSize;
        bool playing;
    };

    bool available;
    float listenerX, listenerY, listenerZ;
    float listenerAngle;
    
    std::list<PlayingSound*> playingSounds;
    
    bool playPCM(const SoundDesc& desc, float volume, float pitch);
    void cleanupStoppedSounds();
    
    static void CALLBACK waveOutProc(HWAVEOUT hwo, UINT uMsg, DWORD_PTR dwInstance, DWORD_PTR dwParam1, DWORD_PTR dwParam2);
};

#endif
