/**
 * @file	vsteffect.cpp
 * @brief	VST effect �N���X�̓���̒�`���s���܂�
 */

#include "compiler.h"
#include "vsteffect.h"
#include <shlwapi.h>
#include <atlbase.h>
#include "vsteffectwnd.h"

#pragma comment(lib, "shlwapi.lib")

/*! �G�t�F�N�g �n���h���[ */
std::map<AEffect*, CVstEffect*> CVstEffect::sm_effects;

/**
 * �R���X�g���N�^
 */
CVstEffect::CVstEffect()
	: m_effect(NULL)
	, m_hModule(NULL)
	, m_lpDir(NULL)
	, m_pWnd(NULL)
{
}

/**
 * �f�X�g���N�^
 */
CVstEffect::~CVstEffect()
{
	Unload();
}

/**
 * ���[�h����
 * @param[in] lpVst �v���O�C��
 * @retval true ����
 * @retval false ���s
 */
bool CVstEffect::Load(LPCTSTR lpVst)
{
	Unload();

	/* VSTi�ǂݍ��� */
	HMODULE hModule = ::LoadLibrary(lpVst);
	if (hModule == NULL)
	{
		return false;
	}
	typedef AEffect* (*FnMain)(::audioMasterCallback audioMaster);
	FnMain fnMain = reinterpret_cast<FnMain>(::GetProcAddress(hModule, "VSTPluginMain"));
	if (fnMain == NULL)
	{
		fnMain = reinterpret_cast<FnMain>(::GetProcAddress(hModule, "main"));
	}
	if (fnMain == NULL)
	{
		::FreeLibrary(hModule);
		return false;
	}

	// ������
	AEffect* effect = (*fnMain)(cAudioMasterCallback);
	if (effect == NULL)
	{
		::FreeLibrary(hModule);
		return false;
	}

	sm_effects[effect] = this;
	m_effect = effect;
	m_hModule = hModule;

	TCHAR szDir[MAX_PATH];
	::lstrcpyn(szDir, lpVst, _countof(szDir));
	::PathRemoveFileSpec(szDir);

	USES_CONVERSION;
	m_lpDir = ::strdup(T2A(szDir));

	return true;
}

/**
 * �A�����[�h����
 */
void CVstEffect::Unload()
{
	if (m_effect)
	{
		std::map<AEffect*, CVstEffect*>::iterator it = sm_effects.find(m_effect);
		if (it != sm_effects.end())
		{
			sm_effects.erase(it);
		}
		m_effect = NULL;
	}
	if (m_hModule)
	{
		::FreeLibrary(m_hModule);
		m_hModule = NULL;
	}
	if (m_lpDir)
	{
		free(m_lpDir);
		m_lpDir = NULL;
	}
}

/**
 * �E�B���h�E �A�^�b�`
 * @param[in] pWnd �n���h��
 * @return �ȑO�̃n���h��
 */
CVstEffectWnd* CVstEffect::Attach(CVstEffectWnd* pWnd)
{
	CVstEffectWnd* pRet = m_pWnd;
	m_pWnd = pWnd;
	return pRet;
}

/**
 * �f�B�X�p�b�`
 * @param[in] opcode The operation code
 * @param[in] index The index
 * @param[in] value The value
 * @param[in] ptr The pointer
 * @param[in] opt The option
 * @return The result
 */
VstIntPtr CVstEffect::dispatcher(VstInt32 opcode, VstInt32 index, VstIntPtr value, void* ptr, float opt)
{
	if (m_effect)
	{
		return (*m_effect->dispatcher)(m_effect, opcode, index, value, ptr, opt);
	}
	return 0;
}

/**
 * �v���Z�X
 * @param[in] inputs ����
 * @param[in] outputs �o��
 * @param[in] sampleFrames �T���v����
 */
void CVstEffect::processReplacing(float** inputs, float** outputs, VstInt32 sampleFrames)
{
	if (m_effect)
	{
		(*m_effect->processReplacing)(m_effect, inputs, outputs, sampleFrames);
	}
}

/**
 * �R�[���o�b�N
 * @param[in] effect The instance of effect
 * @param[in] opcode The operation code
 * @param[in] index The index
 * @param[in] value The value
 * @param[in] ptr The pointer
 * @param[in] opt The option
 * @return The result
 */
VstIntPtr CVstEffect::cAudioMasterCallback(AEffect* effect, VstInt32 opcode, VstInt32 index, VstIntPtr value, void* ptr, float opt)
{
	switch (opcode)
	{
		case audioMasterVersion:
			return 2400;

		default:
			break;
	}

	std::map<AEffect*, CVstEffect*>::iterator it = sm_effects.find(effect);
	if (it != sm_effects.end())
	{
		return it->second->audioMasterCallback(opcode, index, value, ptr, opt);
	}
	return 0;
}

/**
 * �R�[���o�b�N
 * @param[in] opcode The operation code
 * @param[in] index The index
 * @param[in] value The value
 * @param[in] ptr The pointer
 * @param[in] opt The option
 * @return The result
 */
VstIntPtr CVstEffect::audioMasterCallback(VstInt32 opcode, VstInt32 index, VstIntPtr value, void* ptr, float opt)
{
	VstIntPtr ret = 0;

	switch (opcode)
	{
		case audioMasterGetDirectory:
			ret = reinterpret_cast<VstIntPtr>(m_lpDir);
			break;

		case DECLARE_VST_DEPRECATED(audioMasterWantMidi):
			break;

		case audioMasterSizeWindow:
			if (m_pWnd)
			{
				ret = m_pWnd->OnResize(index, value);
			}
			break;

		case audioMasterUpdateDisplay:
			if (m_pWnd)
			{
				ret = m_pWnd->OnUpdateDisplay();
			}
			break;

		default:
			printf("callback: AudioMasterCallback %d %d\n", opcode, index);
			break;
	}

	return ret;
}
