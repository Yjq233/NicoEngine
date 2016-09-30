#pragma once

#include <windows.h>
#include <mmsystem.h>
#include <dsound.h>
#include "SoundInterface.h"
#include "defines.h"

#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"dsound.lib")
#pragma comment(lib,"winmm.lib")

class CDMSoundObject : public CSoundInterface
{
public:
	CDMSoundObject();
	~CDMSoundObject() { }

	bool Initialize(char *filename, bool loop);
	bool SetupSoundParameters(float dopplerFactor,
		float rolloffFactor,
		float minDist, float maxDist);

	void Play();
	void UpdateSoundPosition(float x, float y, float z);
	void Stop();
	void Shutdown();


private:

	WAVEFORMATEX wfmx;
	UCHAR *sndBuffer;
	DSBUFFERDESC dsbd;

	LPDIRECTSOUND lpds;
	LPDIRECTSOUNDBUFFER lpdbsBuffer;

	bool m_loop;
};


class CDirectMusicSystem : public CSoundSystemInterface
{
public:
	CDirectMusicSystem();
	~CDirectMusicSystem() { Shutdown(); }

	bool Initialize(WinHWND mainWin);
	bool AddSound(char *soundfile, bool loop, int *id);
	bool SetupSoundParameters(int id, float dopplerFactor,
		float rolloffFactor,
		float minDist, float maxDist);

	void Play(int id);
	void UpdateSoundPosition(int id, float x,
		float y, float z);
	void Stop(int id);
	void Shutdown();


private:
	int IncreaseSounds();


private:
	int m_totalSounds;
	CDMSoundObject *m_soundList;
};

bool CreateDMSound(CSoundSystemInterface **pObj);
