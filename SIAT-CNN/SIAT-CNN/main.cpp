#include <iostream>
#include <afx.h>

#include "CCreateNetwork.h"
#include "ReadIniFile.h"

void SetThreadName( DWORD dwThreadID, LPCSTR szThreadName);

//extern CReadIniFile theApp;

int main()
{
	// set main thread's name (useful for debugging etc)
	char str[] = "MAIN";  // must use chars, not TCHARs, for SetThreadname function
	SetThreadName( -1, str );

	// Seed the random-number generator with current time so that
    // the numbers will be different every time we run.
	srand( (unsigned)time( NULL ) );
	
	AfxGetApp()->InitInstance();
	CCreateNetwork crtNetwork;
	

	int iSelect;
	cout<<"******************************************************"<<endl;
	cout<<"��ϵͳ�ṩ���²���:"<<endl<<endl;
	cout<<"	1��ѵ������"<<endl;
	cout<<"	2����������"<<endl;
	cout<<"	0���˳�"<<endl;
	cout<<"******************************************************"<<endl;

	cout<<"������Ҫ���еĲ���(0-2):";
	
	while(cin>>iSelect)
	{
		if(iSelect==0)
		{
			return 0;
		}
		else if(iSelect==1)
		{
			////// ʵ��BP�����������
			char ch;
			bool flag;  
			cout<<endl<<"��ʼѵ��ǰ�Ƿ���Ҫ����Ԥ��Ȩ�أ�[Y(yes) or N(no)]: ";
			while(cin>>ch)
			{
				if(ch == 'Y' || ch == 'y')
				{
					flag=true;
					break;
				}
				else if(ch == 'N' || ch == 'n')
				{
					flag=false;
					break;
				}
				else
				{
					cout<<endl<<"���Ϸ������룬���������룡��"<<endl;
					cout<<endl<<"��ʼѵ��ǰ�Ƿ���Ҫ����Ԥ��Ȩ�أ�[Y(yes) or N(no)]: ";
				}
			}

			crtNetwork.InitNetwork(flag);
			
			crtNetwork.BackPropagation();   // ��ʼ��BP���̵���ز���

			cout<<"������ѵ����ɣ�"<<endl<<endl;
		}
		else if(iSelect==2)
		{
			////// ʵ��FPǰ��������
			char ch;
			bool flag;  
			cout<<endl<<"��ʼѵ��ǰ�Ƿ���Ҫ����Ԥ��Ȩ�أ�[Y(yes) or N(no)]: ";
			while(cin>>ch)
			{
				if(ch == 'Y' || ch == 'y')
				{
					flag=true;
					break;
				}
				else if(ch == 'N' || ch == 'n')
				{
					flag=false;
					break;
				}
				else
				{
					cout<<endl<<"���Ϸ������룬���������룡��"<<endl;
					cout<<endl<<"��ʼѵ��ǰ�Ƿ���Ҫ����Ԥ��Ȩ�أ�[Y(yes) or N(no)]: ";
				}
			}

			crtNetwork.InitNetwork(flag);
			bool rt=crtNetwork.ForwardPropagation();
			if(rt == true)
			{
				cout<<endl<<"*************************************************"<<endl;
				cout<<"�����������������ɣ�";
				cout<<endl<<"*************************************************"<<endl<<endl;
			}
			else
			{
				cout<<"�û������Ƿ�����ǰ������ֹ��"<<endl<<endl;				
			}

		}
		else
		{
			cout<<"��������������ѡ�������"<<endl;
		}

		cout<<"******************************************************"<<endl;
		cout<<"��ϵͳ�ṩ���²���:"<<endl<<endl;
		cout<<"	1��ѵ������"<<endl;
		cout<<"	2�����Թ���"<<endl;
		cout<<"	0���˳�"<<endl;
		cout<<"******************************************************"<<endl;

		cout<<"������Ҫ���еĲ���(0-2):";
	}
	return 0;
}


void SetThreadName( DWORD dwThreadID, LPCSTR szThreadName)
{
	
	struct THREADNAME_INFO
	{
		DWORD dwType; // must be 0x1000
		LPCSTR szName; // pointer to name (in user addr space)
		DWORD dwThreadID; // thread ID (-1=caller thread)
		DWORD dwFlags; // reserved for future use, must be zero
	} ;
	
	THREADNAME_INFO info;
	info.dwType = 0x1000;
	info.szName = szThreadName;
	info.dwThreadID = dwThreadID;
	info.dwFlags = 0;
	
	__try
	{
		RaiseException( 0x406D1388, 0, sizeof(info)/sizeof(DWORD), (DWORD*)&info );
	}
	__except(EXCEPTION_CONTINUE_EXECUTION)
	{
	}
}
