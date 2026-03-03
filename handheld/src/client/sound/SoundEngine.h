#ifndef NET_MINECRAFT_CLIENT_SOUND__SoundEngine_H__
#define NET_MINECRAFT_CLIENT_SOUND__SoundEngine_H__

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#if defined(ANDROID) && !defined(PRE_ANDROID23)
	#include "../../platform/audio/SoundSystemSL.h"
#elif defined(__APPLE__)
    #include "../../platform/audio/SoundSystemAL.h"
#elif defined(WIN32)
	#include "../../platform/audio/SoundSystemWin.h"
#else
	#include "../../platform/audio/SoundSystem.h"
#endif
#include "SoundRepository.h"
#include "../../util/Random.h"

class Minecraft;
class Mob;
class Options;

class SoundEngine
{
    static const int SOUND_DISTANCE = 16;

	#if defined(ANDROID) && !defined(PRE_ANDROID23) && !defined(RPI)
		SoundSystemSL soundSystem;
    #elif defined(__APPLE__)
        SoundSystemAL soundSystem;
	#elif defined(WIN32)
	    SoundSystemWin soundSystem;
	#else
	    SoundSystem soundSystem;
	#endif

	Options* options;
	int idCounter;
	Random random;
	float _x;
	float _y;
	float _z;
    float _yRot;
	float _invMaxDistance;

public:
	SoundEngine(float maxDistance);

	~SoundEngine();

    void init(Minecraft* mc, Options* options);
	void destroy();

	void enable(bool status);

	void updateOptions();
    void update(Mob* player, float a);

	void play(const std::string& name, float x, float y, float z, float volume, float pitch);
	void playUI(const std::string& name, float volume, float pitch);

	float _getVolumeMult(float x, float y, float z);
private:
	void loadLibrary() {}
    SoundDesc _pp(const std::string& fn);
    
	SoundRepository sounds;
	Minecraft* mc;
};

#endif
