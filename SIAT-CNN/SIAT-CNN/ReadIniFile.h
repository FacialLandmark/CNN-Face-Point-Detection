#pragma once

#include "Preferences.h"
#include "StdAfx.h"

class CReadIniFile : public CWinApp
{
public:
	CReadIniFile(void);
	~CReadIniFile(void);
	virtual BOOL InitInstance(void);

public:
	//CPreferences m_Preferences;   //ȫ�ֲ����࣬���ڸ�ģ�飨�繦��ҳ�棩�Ľ��̹�����ģ������ݹ���
	CString m_sModulePath;        //����������еĿ�ִ�г���ĵ�ǰ·��
};

