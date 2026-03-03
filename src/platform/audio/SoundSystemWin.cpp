#include "SoundSystemWin.h"
#include "../../client/sound/Sound.h"
#include "../../util/Mth.h"
#include <cmath>

SoundSystemWin::SoundSystemWin()
:   available(false),
    listenerX(0), listenerY(0), listenerZ(0),
    listenerAngle(0)
{
    available = true;
}

SoundSystemWin::~SoundSystemWin()
{
    cleanupStoppedSounds();
    for (std::list<PlayingSound*>::iterator it = playingSounds.begin(); it != playingSounds.end(); ++it)
    {
        PlayingSound* ps = *it;
        if (ps->hWaveOut)
        {
            waveOutReset(ps->hWaveOut);
            waveOutUnprepareHeader(ps->hWaveOut, &ps->waveHdr, sizeof(WAVEHDR));
            waveOutClose(ps->hWaveOut);
        }
        if (ps->buffer)
            delete[] ps->buffer;
        delete ps;
    }
    playingSounds.clear();
}

void SoundSystemWin::enable(bool status)
{
}

void SoundSystemWin::setListenerPos(float x, float y, float z)
{
    listenerX = x;
    listenerY = y;
    listenerZ = z;
}

void SoundSystemWin::setListenerAngle(float deg)
{
    listenerAngle = deg;
}

void SoundSystemWin::cleanupStoppedSounds()
{
    for (std::list<PlayingSound*>::iterator it = playingSounds.begin(); it != playingSounds.end(); )
    {
        PlayingSound* ps = *it;
        if (!ps->playing)
        {
            if (ps->hWaveOut)
            {
                waveOutUnprepareHeader(ps->hWaveOut, &ps->waveHdr, sizeof(WAVEHDR));
                waveOutClose(ps->hWaveOut);
            }
            if (ps->buffer)
                delete[] ps->buffer;
            delete ps;
            it = playingSounds.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

void CALLBACK SoundSystemWin::waveOutProc(HWAVEOUT hwo, UINT uMsg, DWORD_PTR dwInstance, DWORD_PTR dwParam1, DWORD_PTR dwParam2)
{
    if (uMsg == WOM_DONE)
    {
        PlayingSound* ps = (PlayingSound*)dwInstance;
        ps->playing = false;
    }
}

bool SoundSystemWin::playPCM(const SoundDesc& desc, float volume, float pitch)
{
    if (!desc.isValid() || !desc.frames)
        return false;

    cleanupStoppedSounds();

    WAVEFORMATEX wfx;
    memset(&wfx, 0, sizeof(wfx));
    wfx.wFormatTag = WAVE_FORMAT_PCM;
    wfx.nChannels = desc.channels;
    wfx.nSamplesPerSec = desc.frameRate;
    wfx.wBitsPerSample = desc.byteWidth * 8;
    wfx.nBlockAlign = wfx.nChannels * wfx.wBitsPerSample / 8;
    wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;
    wfx.cbSize = 0;

    PlayingSound* ps = new PlayingSound();
    ps->playing = true;
    ps->hWaveOut = NULL;
    ps->buffer = NULL;
    ps->bufferSize = 0;

    MMRESULT result = waveOutOpen(&ps->hWaveOut, WAVE_MAPPER, &wfx, (DWORD_PTR)waveOutProc, (DWORD_PTR)ps, CALLBACK_FUNCTION);
    if (result != MMSYSERR_NOERROR)
    {
        delete ps;
        return false;
    }

    DWORD vol = (DWORD)(volume * 0xFFFF);
    vol = (vol << 16) | vol;
    waveOutSetVolume(ps->hWaveOut, vol);

    ps->bufferSize = desc.size;
    ps->buffer = new char[desc.size];
    memcpy(ps->buffer, desc.frames, desc.size);

    memset(&ps->waveHdr, 0, sizeof(WAVEHDR));
    ps->waveHdr.lpData = ps->buffer;
    ps->waveHdr.dwBufferLength = desc.size;
    ps->waveHdr.dwFlags = 0;

    result = waveOutPrepareHeader(ps->hWaveOut, &ps->waveHdr, sizeof(WAVEHDR));
    if (result != MMSYSERR_NOERROR)
    {
        waveOutClose(ps->hWaveOut);
        delete[] ps->buffer;
        delete ps;
        return false;
    }

    result = waveOutWrite(ps->hWaveOut, &ps->waveHdr, sizeof(WAVEHDR));
    if (result != MMSYSERR_NOERROR)
    {
        waveOutUnprepareHeader(ps->hWaveOut, &ps->waveHdr, sizeof(WAVEHDR));
        waveOutClose(ps->hWaveOut);
        delete[] ps->buffer;
        delete ps;
        return false;
    }

    playingSounds.push_back(ps);
    return true;
}

void SoundSystemWin::playAt(const SoundDesc& desc, float x, float y, float z, float volume, float pitch)
{
    if (!available || !desc.isValid())
        return;

    if (volume <= 0.01f)
        return;

    playPCM(desc, volume, pitch);
}
