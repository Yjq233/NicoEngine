#include "DirectMusic.h"

static WinHWND g_mainWin;

bool CreateDMSound(CSoundSystemInterface **pObj)
{
	if (!*pObj) *pObj = new CDirectMusicSystem();
	else return false;

	return true;
}

CDMSoundObject::CDMSoundObject()
{
	sndBuffer = NULL;
	lpds = NULL;
	lpdbsBuffer = NULL;
}

bool CDMSoundObject::Initialize(char *filename, bool loop)
{
	if (DirectSoundCreate(NULL, &lpds, NULL) != DS_OK)
		return false;

	if (lpds->SetCooperativeLevel(g_mainWin, DSSCL_NORMAL) != DS_OK)
		return false;

	HMMIO handle;
	MMCKINFO mmckriff, mmckIn;
	PCMWAVEFORMAT pwfm;
	memset(&mmckriff, 0, sizeof(MMCKINFO));
	if ((handle = mmioOpen(filename, NULL, MMIO_READ | MMIO_ALLOCBUF)) == NULL)
		return false;

	//进入块(chunk),查找 riff和wave的标记
	if (0 != mmioDescend(handle, &mmckriff, NULL, 0))
	{
		mmioClose(handle, 0);
		return false;
	}

	//#define FOURCC_RIFF  mmioFOURCC('R','I','F','F')
	if (mmckriff.ckid != FOURCC_RIFF || mmckriff.fccType != mmioFOURCC('W', 'A', 'V', 'E'))
	{
		mmioClose(handle, 0);
		return false;
	}

	//查找 fmt 块
	mmckIn.ckid = mmioFOURCC('f', 'm', 't', ' ');
	if (0 != mmioDescend(handle, &mmckIn, &mmckriff, MMIO_FINDCHUNK))
	{
		mmioClose(handle, 0);
		return false;
	}

	//fmt块的格式与PCMWAVEFORMAT 的格式定义相同
	//所以读入临时变量，最后写入 wmtf中
	if (mmioRead(handle, (HPSTR)&pwfm, sizeof(PCMWAVEFORMAT)) != sizeof(PCMWAVEFORMAT))
	{
		mmioClose(handle, 0);
		return false;
	}

	//检测是不是 pc的wave标准格式
	if (pwfm.wf.wFormatTag != WAVE_FORMAT_PCM)
	{
		mmioClose(handle, 0);
		return false;
	}

	//给wmfx赋值
	memcpy(&wfmx, &pwfm, sizeof(pwfm));
	wfmx.cbSize = 0;

	if (0 != mmioAscend(handle, &mmckIn, 0))
	{
		mmioClose(handle, 0);
		return false;
	}

	//查找 data chunk
	mmckIn.ckid = mmioFOURCC('d', 'a', 't', 'a');

	if (0 != mmioDescend(handle, &mmckIn, &mmckriff, MMIO_FINDCHUNK))
	{
		mmioClose(handle, 0);
		return false;
	}

	sndBuffer = new UCHAR[mmckIn.cksize];

	//数据写入sndBuffer
	mmioRead(handle, (HPSTR)sndBuffer, mmckIn.cksize);

	mmioClose(handle, 0);

	ZeroMemory(&dsbd, sizeof(DSBUFFERDESC));
	//建立directsound的辅助缓存
	dsbd.dwSize = sizeof(DSBUFFERDESC);
	dsbd.dwBufferBytes = mmckIn.cksize;
	dsbd.dwFlags = (/*control_flags | */ 224 | DSBCAPS_STATIC | DSBCAPS_LOCSOFTWARE);
	dsbd.lpwfxFormat = &wfmx;

	if (FAILED(lpds->CreateSoundBuffer(&dsbd, &lpdbsBuffer, NULL)))
	{
		delete[] sndBuffer;
		return false;
	}
	VOID* pDSLockedBuffer = NULL;
	DWORD dwDSLockedBufferSize = 0;

	//lock 住内存，现在他是可操作的，拷贝进去即可
	if (lpdbsBuffer->Lock(0, mmckIn.cksize, &pDSLockedBuffer, &dwDSLockedBufferSize, NULL, NULL, 0L))
		return false;
	memcpy(pDSLockedBuffer, sndBuffer, mmckIn.cksize);

	//Unlock 缓冲区内存
	if (FAILED(lpdbsBuffer->Unlock(pDSLockedBuffer, dwDSLockedBufferSize, NULL, 0)))
	{
		delete[] sndBuffer;
		return 0;
	}

	m_loop = loop;

	return true;
}


bool CDMSoundObject::SetupSoundParameters(float dopplerFactor,
	float rolloffFactor, float minDist, float maxDist)
{

	return true;
}


void CDMSoundObject::Play()
{
	// If it is already playing, return.
	lpdbsBuffer->Stop();
	lpdbsBuffer->SetCurrentPosition(0);
	lpdbsBuffer->Play(0, 0, m_loop);
}


void CDMSoundObject::UpdateSoundPosition(float x, float y, float z)
{
	
}


void CDMSoundObject::Stop()
{
	// Only stop if it is playing.
	lpdbsBuffer->Stop();
}


void CDMSoundObject::Shutdown()
{
	// Release all resources.
	if(sndBuffer)
		delete[] sndBuffer;

	sndBuffer = NULL;


	if (lpdbsBuffer) {

		lpdbsBuffer->Release();
		lpdbsBuffer = NULL;
	}

	if (lpds) {

		lpds->Release();
		lpds = NULL;
	}
}


CDirectMusicSystem::CDirectMusicSystem()
{
	m_totalSounds = 0;
	m_soundList = NULL;
}

bool CDirectMusicSystem::Initialize(WinHWND mainWin)
{
	g_mainWin = mainWin;
	return true;
}

int CDirectMusicSystem::IncreaseSounds()
{
	// This function increases the m_control array.

	if (!m_soundList)
	{
		m_soundList = new CDMSoundObject[1];
		if (!m_soundList) return NE_FAIL;
	}
	else
	{
		CDMSoundObject *temp;
		temp = new CDMSoundObject[m_totalSounds + 1];
		if (!temp) return NE_FAIL;

		memcpy(temp, m_soundList,
			sizeof(CDMSoundObject) * m_totalSounds);

		delete[] m_soundList;
		m_soundList = temp;
	}

	return NE_OK;
}


bool CDirectMusicSystem::AddSound(char *filename,
	bool loop, int *id)
{
	if (!IncreaseSounds()) return false;

	if (!m_soundList[m_totalSounds].Initialize(filename,
		loop)) return false;

	if (id) *id = m_totalSounds;
	m_totalSounds++;

	return true;
}


bool CDirectMusicSystem::SetupSoundParameters(int id,
	float dopplerFactor,
	float rolloffFactor,
	float min, float max)
{
	if (id >= m_totalSounds) return false;

	return m_soundList[id].SetupSoundParameters(dopplerFactor,
		rolloffFactor, min, max);
}


void CDirectMusicSystem::Play(int id)
{
	if (id >= m_totalSounds) return;

	m_soundList[id].Play();
}


void CDirectMusicSystem::UpdateSoundPosition(int id, float x,
	float y, float z)
{
	if (id >= m_totalSounds) return;

	m_soundList[id].UpdateSoundPosition(x, y, z);
}


void CDirectMusicSystem::Stop(int id)
{
	if (id >= m_totalSounds) return;

	m_soundList[id].Stop();
}


void CDirectMusicSystem::Shutdown()
{
	for (int i = 0; i < m_totalSounds; i++)
	{
		m_soundList[i].Stop();
		m_soundList[i].Shutdown();
	}

	if (m_soundList) delete[] m_soundList;
	m_soundList = NULL;
	m_totalSounds = 0;
}
