#pragma once
#include "defines.h"

class CSoundSystemInterface
{
public:
	CSoundSystemInterface() {}
	virtual ~CSoundSystemInterface() {}

	virtual bool Initialize(WinHWND mainWin) = 0;
	virtual bool AddSound(char *soundfile, bool loop, int *id) = 0;
	virtual bool SetupSoundParameters(int id, float dopplerFactor,
		float rolloffFactor,
		float minDist,
		float maxDist) = 0;

	virtual void Play(int id) = 0;
	virtual void UpdateSoundPosition(int id, float x, float y,
		float z) = 0;
	virtual void Stop(int id) = 0;
	virtual void Shutdown() = 0;
};


class CSoundInterface
{
public:
	CSoundInterface() {}
	virtual ~CSoundInterface() {}

	virtual bool Initialize(char *filename, bool loop) = 0;
	virtual bool SetupSoundParameters(float dopplerFactor,
		float rolloffFactor,
		float minDist,
		float maxDist) = 0;

	virtual void Play() = 0;
	virtual void UpdateSoundPosition(float x, float y,
		float z) = 0;
	virtual void Stop() = 0;
	virtual void Shutdown() = 0;
};
