#include "ReadIniFile.h"
#include "stdafx.h"

extern CPreferences GetPreferences();

CReadIniFile::CReadIniFile(void)
{
}

CReadIniFile theApp;


CReadIniFile::~CReadIniFile(void)
{
}


BOOL CReadIniFile::InitInstance(void)
{
	// initialize path name for current module
	this->m_sModulePath.Empty();
	::GetModuleFileName(NULL,m_sModulePath.GetBuffer(255),255);	   //��ȡ��ǰ�����Ѽ���ģ����ļ�������·��
	::PathMakePrettyA(m_sModulePath.GetBuffer(255));			   //ת���ļ�·����ΪСд��ʽ
	::PathRemoveFileSpecA(m_sModulePath.GetBuffer(255));		   //�õ��ļ���·����ȥ���ļ���
	m_sModulePath.ReleaseBuffer();

	free((void*)m_pszProfileName);

	// Next, change the name of the .INI file.
	// The CWinApp destructor will free the memory.
	//�õ�ϵͳ�����ļ���·��
	CString tINI = m_sModulePath + _T("\\CNN.ini");
	
	m_pszProfileName=_tcsdup( tINI );

	//////////////////////////////////////////////////////
	//
	// Finally, initialize preferences
	// ��ȡ�����ļ�����ʼ���������ò���
	//
	//////////////////////////////////////////////////////

	CPreferences::GetPreferences()->ReadParameterFile( this );

	return TRUE;
}
