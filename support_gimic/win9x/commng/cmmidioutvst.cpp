/**
 * @file	cmmidioutvst.cpp
 * @brief	MIDI OUT VST �N���X�̓���̒�`���s���܂�
 */

#include "compiler.h"
#include "cmmidioutvst.h"
#include <shlwapi.h>
#include "pccore.h"

#pragma comment(lib, "shlwapi.lib")

static void GetPath(LPTSTR lpModule, UINT cbModule)
{
	::ExpandEnvironmentStrings(TEXT("%ProgramFiles%\\Roland\\Sound Canvas VA\\SOUND Canvas VA.dll"), lpModule, cbModule);
}

/**
 * IsEnabled?
 */
bool CComMidiOutVst::IsEnabled()
{
	TCHAR szModule[MAX_PATH];
	GetPath(szModule, _countof(szModule));
	return (::PathFileExists(szModule) != FALSE);
}

/**
 * �C���X�^���X���쐬
 * @return �C���X�^���X
 */
CComMidiOutVst* CComMidiOutVst::CreateInstance()
{
	CComMidiOutVst* pVst = new CComMidiOutVst;

	TCHAR szModule[MAX_PATH];
	GetPath(szModule, _countof(szModule));
	if (!pVst->Initialize(szModule))
	{
		delete pVst;
		pVst = NULL;
	}
	return pVst;
}

/**
 * �R���X�g���N�^
 * @param[in] vermouth �n���h�� */
CComMidiOutVst::CComMidiOutVst()
	: m_nBlockSize(128)
	, m_nIndex(0)
{
}

/**
 * �f�X�g���N�^
 */
CComMidiOutVst::~CComMidiOutVst()
{
	m_wnd.Destroy();
	m_effect.Unload();
}

/**
 * ������
 */
bool CComMidiOutVst::Initialize(LPCTSTR lpPath)
{
	m_effect;
	if (!m_effect.Load(lpPath))
	{
		printf("Cloudn't attach VSTi.\n");
		return false;
	}

	// Effect ���I�[�v��
	m_effect.open();

	// �T���v�����O ���[�g��ݒ�
	m_effect.setSampleRate(static_cast<float>(np2cfg.samplingrate));

	// �u���b�N�T�C�Y��ݒ�
	m_effect.setBlockSize(m_nBlockSize);
	m_effect.resume();

	m_effect.beginSetProgram();
	m_effect.setProgram(0);
	m_effect.endSetProgram();

	m_input.Alloc(2, m_nBlockSize);
	m_output.Alloc(2, m_nBlockSize);

	::sound_streamregist(this, reinterpret_cast<SOUNDCB>(GetPcm));

	m_wnd.Create(&m_effect, TEXT("NP2 VSTi"), WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX);

	return true;
}

/**
 * �V���[�g ���b�Z�[�W
 * @param[in] nMessage ���b�Z�[�W
 */
void CComMidiOutVst::Short(UINT32 nMessage)
{
	sound_sync();
	m_event.ShortMessage(m_nIndex, nMessage);
}

/**
 * �����O ���b�Z�[�W
 * @param[in] lpMessage ���b�Z�[�W �|�C���^
 * @param[in] cbMessage ���b�Z�[�W �T�C�Y
 */
void CComMidiOutVst::Long(const UINT8* lpMessage, UINT cbMessage)
{
	sound_sync();
	m_event.LongMessage(m_nIndex, lpMessage, cbMessage);
}

/**
 * �v���Z�X
 * @param[in] pVst �n���h��
 * @param[out] lpBuffer �o�b�t�@
 * @param[in] nBufferCount �T���v����
 */
void SOUNDCALL CComMidiOutVst::GetPcm(CComMidiOutVst* pVst, SINT32* lpBuffer, UINT nBufferCount)
{
	pVst->Process(lpBuffer, nBufferCount);
}

void CComMidiOutVst::Process(SINT32* lpBuffer, UINT nBufferCount)
{
	while (nBufferCount)
	{
		if (m_nIndex >= m_nBlockSize)
		{
			m_nIndex = 0;
			m_effect.processEvents(m_event.GetEvents());
			m_effect.processReplacing(m_input.GetBuffer(), m_output.GetBuffer(), m_nBlockSize);
			m_event.Clear();
		}

		UINT nSize = m_nBlockSize - m_nIndex;
		nSize = min(nSize, nBufferCount);
		nBufferCount -= nSize;
		float** output = m_output.GetBuffer();
		do
		{
			lpBuffer[0] += output[0][m_nIndex] * 32767.0f - 0.5f;
			lpBuffer[1] += output[1][m_nIndex] * 32767.0f - 0.5f;
			lpBuffer += 2;
			m_nIndex++;
		} while (--nSize);
	}
}
