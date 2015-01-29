
#include "StdAfx.h"
#include "CCreateNetwork.h"
#include "Preferences.h"
#include <afxwin.h>
#include <highgui.h>
#include <imgproc\imgproc.hpp>
#include <cv.h>
#include <afx.h>

#include "SHLWAPI.H"	// for the path functions
#pragma comment( lib, "shlwapi.lib" )

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define UNIFORM_PLUS_MINUS_ONE ( (double)(2.0 * rand())/RAND_MAX - 1.0 )
#define UNIFORM_ZERO_THRU_ONE ( (double)(rand())/(RAND_MAX + 1 ) )

using namespace std;

CCreateNetwork::CCreateNetwork(void)
{
	// TODO: add one-time construction code here
	m_cBackprops = 0;
	m_nAfterEveryNBackprops = 1;

	
	m_iNextTestingPattern = 0;
	m_iNextTrainingPattern = 0;
	
	// allocate memory to store the distortion maps
	m_cCols = 39;
	m_cRows = 39;
	
	m_cCount = m_cCols * m_cRows;

	m_DispH = new double[ m_cCount ];
	m_DispV = new double[ m_cCount ];

	// clear m_NeuronOutputs
	m_NeuronOutputs.clear();	

	//ѵ�������ͼƬ��Ĭ��·��
	m_TrainFilePath="E:\\����ʶ��\\face point detection\\trainingImage\\lfw_5590n\\";
	m_TestFilePath="E:\\����ʶ��\\face point detection\\trainingImage\\lfw_5590n\\";

	m_TrainInfoPath="E:\\����ʶ��\\face point detection\\trainingImage\\trainInfo\\*.txt";

	m_sWeightSavePath= "E:\\����ʶ��\\face point detection\\weight";

}

CCreateNetwork::~CCreateNetwork(void)
{
}

vector<CString> CCreateNetwork::m_InfoFileNames;  
vector<CString> CCreateNetwork::m_InfoFilePaths;
vector<CString> CCreateNetwork::m_ImageFilePaths;


// ��������������������ľŲ�����
BOOL CCreateNetwork::InitNetwork(bool bLoadWeightFile)
{
	// grab the mutex for the neural network

	//****CAutoMutex tlo(m_utxNeuralNet);


	////////////////////////////////////////////////
	//   Ȩ��Ԥ��ȡģ�飺
	//		��ȡ������ѵ���õĸ�������Ȩ�ص��ļ�
	///////////////////////////////////////////////
	//�����б������洢����ĳ�ʼȨֵ
	vector<double>* vWeightOfLayer1=new vector<double>();  
	vector<double>* vWeightOfLayer3=new vector<double>();  
	vector<double>* vWeightOfLayer5=new vector<double>();  
	vector<double>* vWeightOfLayer7=new vector<double>();  
	vector<double>* vWeightOfLayer8=new vector<double>();  
	vector<double>* vWeightOfLayer9=new vector<double>(); 

	//  �����ж��û���Ҫ����Ȩ���ļ�,������Ԥ�ȴ��ļ����õ������Ȩ��ֵ����
	if(bLoadWeightFile == true)  //bLoadWeightFile�������е�Ϊ���Բ���
	{
		cout<<endl<<"����Ȩ���ļ���>>> >>>"<<endl;

		char path[100];
		
		sprintf(path,"%s\\1.txt",m_sWeightSavePath.c_str());
		this->LoadWeightFile(vWeightOfLayer1,path);
		
		sprintf(path,"%s\\3.txt",m_sWeightSavePath.c_str());
		this->LoadWeightFile(vWeightOfLayer3,path);

		sprintf(path,"%s\\5.txt",m_sWeightSavePath.c_str());
		this->LoadWeightFile(vWeightOfLayer5,path);

		sprintf(path,"%s\\7.txt",m_sWeightSavePath.c_str());
		this->LoadWeightFile(vWeightOfLayer7,path);

		sprintf(path,"%s\\8.txt",m_sWeightSavePath.c_str());
		this->LoadWeightFile(vWeightOfLayer8,path);

		sprintf(path,"%s\\9.txt",m_sWeightSavePath.c_str());
		this->LoadWeightFile(vWeightOfLayer9,path);

		cout<<"������ϣ�����"<<endl<<endl;
	}


	cout<<"���ڳ�ʼ������>>> >>>"<<endl;

	// initialize and build the neural net
	NeuralNetwork& NN = m_NN;  // for easier nomenclature
	NN.Initialize();
	
	NNLayer* pLayer;
	
	int ii, jj, kk;
	int icNeurons = 0;
	int icWeights = 0;
	double initWeight;
	CString label;

	// layer zero, the input layer.
	// Create neurons: exactly the same number of neurons as the input
	// vector of 39x39=1521 pixels, and no weights/connections
	
	pLayer = new NNLayer( _T("Layer00") );
	NN.m_Layers.push_back( pLayer );
	
	for (ii=0; ii<1521; ++ii)
	{
		label.Format(_T("Layer00_Neuron%04d_Num%06d"), ii, icNeurons );
		pLayer->m_Neurons.push_back( new NNNeuron( (LPCTSTR)label ) );
		icNeurons++;
	}
	
	// layer one:
	// This layer is a convolutional layer that has 20 feature maps.  Each feature 
	// map is 36x36, and each unit in the feature maps is a 5x5 convolutional kernel
	// of the input layer.
	// So, there are 36x36x20 = 25920 neurons, (4x4+1)x20 = 340 weights
	
	pLayer = new NNLayer( _T("Layer01"), pLayer );
	NN.m_Layers.push_back( pLayer );
	
	for ( ii=0; ii<25920; ++ii )
	{
		label.Format( _T("Layer01_Neuron%04d_Num%06d"), ii, icNeurons );
		pLayer->m_Neurons.push_back( new NNNeuron( (LPCTSTR)label ) );
		icNeurons++;
	}
	
	for ( ii=0; ii<340; ++ii )
	{
		label.Format( _T("Layer01_Weight%04d_Num%06d"), ii, icWeights );

		if(bLoadWeightFile == true)  //�Ƿ�����ѵ���õ�Ȩֵ
		{
			if(vWeightOfLayer1->size() != 340)  //����ȡ��Ȩֵ�ļ��е�Ȩ������������������Ȩ��
			{
				cout<<"Ȩֵ��Ŀ���������Ϊ�������ʼȨ��"<<endl;
				initWeight = 0.05 * UNIFORM_PLUS_MINUS_ONE;
			}
			else
			{
				initWeight = (*vWeightOfLayer1)[ii];
			}
		}
		else
		{
			initWeight = 0.05 * UNIFORM_PLUS_MINUS_ONE;
		}
		pLayer->m_Weights.push_back( new NNWeight( (LPCTSTR)label, initWeight ) );
	}
	
	// interconnections with previous layer: this is difficult
	// The previous layer is a top-down bitmap image that has been padded to size 29x29
	// Each neuron in this layer is connected to a 5x5 kernel in its feature map, which 
	// is also a top-down bitmap of size 13x13.  We move the kernel by TWO pixels, i.e., we
	// skip every other pixel in the input image
	
	int kernelTemplate[16] = {
								0,  1,  2,  3, 
								39, 40, 41, 42,                                                                                                                                                                      
								78, 79, 80, 81, 
								117, 118, 119, 120 };
		
	int iNumWeight=0;
		
	int fm=0;
	for (fm=0; fm<20; ++fm)
	{
		for ( ii=0; ii<36; ++ii )
		{
			for ( jj=0; jj<36; ++jj )
			{
				iNumWeight = fm * 17;  // 17 is the number of weights per feature map
				NNNeuron& n = *( pLayer->m_Neurons[ jj + ii*36 + fm*1296 ] );
				
				n.AddConnection( ULONG_MAX, iNumWeight++ );  // bias weight
				
				for ( kk=0; kk<16; ++kk )
				{
					// note: max val of index == 840, corresponding to 841 neurons in prev layer
					n.AddConnection( 1*jj + 39*ii + kernelTemplate[kk], iNumWeight++ );
				}
			}
		}
	}
	
	
	// layer two:
	// This layer is a Pooling layer that has 20 feature maps.  Each feature 
	// map is 18x18, and each unit in the feature maps is a 2x2 Pooling kernel
	// of corresponding areas of all 20 of the previous layers, each of which is a 36x36 feature map
	// So, there are 18x18x20 = 6480 neurons, 36x36x20 = 25920 weights

	pLayer = new NNLayer( _T("Layer02"), pLayer );
	NN.m_Layers.push_back( pLayer );

	for ( ii=0; ii<6480; ++ii )
	{
		label.Format( _T("Layer02_Neuron%04d_Num%06d"), ii, icNeurons );
		pLayer->m_Neurons.push_back( new NNNeuron( (LPCTSTR)label ) );
		icNeurons++;
	}
	//for ( ii=0; ii<25920; ++ii )
	for ( ii=0; ii<80; ++ii )
	{
		label.Format( _T("Layer02_Weight%04d_Num%06d"), ii, icWeights );
		pLayer->m_Weights.push_back( new NNWeight( (LPCTSTR)label, 0.25 ) );  //pooling�����ԪȨֵΪ0.25��Ϊ����
	}

	
	int kernelTemplate2[4] = {
		0,  1,  
		36, 37 };


	for (fm=0; fm<20; ++fm)
	{
		for ( ii=0; ii<18; ++ii )			
		{
			for ( jj=0; jj<18; ++jj )
			{
				iNumWeight = fm * 4;  // 4 is the number of weights per feature map
					
				NNNeuron& n = *( pLayer->m_Neurons[ jj + ii*18 + fm*324 ] );

				//n.AddConnection( ULONG_MAX, iNumWeight++ );  // bias weight

				for ( kk=0; kk<4; ++kk )
				{
					// note: max val of index == 1013, corresponding to 1014 neurons in prev layer
					n.AddConnection( 1296*fm + 2*jj + 72*ii + kernelTemplate2[kk], iNumWeight++ );
				}
			}
		}
	}


	// layer three:
	// This layer is a convolutional layer that has 40 feature maps.  Each feature 
	// map is 16x16, and each unit in the feature maps is a 3x3 convolutional kernel
	// of corresponding areas of all 20 of the previous layers, each of which is a 18x18 feature map
	// So, there are 16x16x40 = 10240 neurons, (3x3+1)x20x40 = 8000 weights

	pLayer = new NNLayer( _T("Layer03"), pLayer );
	NN.m_Layers.push_back( pLayer );

	for ( ii=0; ii<10240; ++ii )
	{
		label.Format( _T("Layer03_Neuron%04d_Num%06d"), ii, icNeurons );
		pLayer->m_Neurons.push_back( new NNNeuron( (LPCTSTR)label ) );
		icNeurons++;
	}

	for (ii=0; ii<8000; ++ii)
	{
		label.Format( _T("Layer03_Weight%04d_Num%06d"), ii, icWeights );
		
		if(bLoadWeightFile == true)  //�Ƿ�����ѵ���õ�Ȩֵ
		{
			if(vWeightOfLayer3->size() != 8000)  //����ȡ��Ȩֵ�ļ��е�Ȩ������������������Ȩ��
			{
				cout<<"Ȩֵ��Ŀ���������Ϊ�������ʼȨ��"<<endl;
				initWeight = 0.05 * UNIFORM_PLUS_MINUS_ONE;
			}
			else
			{
				initWeight = (*vWeightOfLayer3)[ii];
			}
		}
		else
		{
			initWeight = 0.05 * UNIFORM_PLUS_MINUS_ONE;
		}

		pLayer->m_Weights.push_back( new NNWeight( (LPCTSTR)label, initWeight ) );
	}

	int kernelTemplate3[9] = 
	{
		0,  1,  2, 
		18, 19, 20,
		36, 37, 38    
	};


	for (fm=0; fm<40; ++fm)
	{
		for ( ii=0; ii<16; ++ii )
		{
			for ( jj=0; jj<16; ++jj )
			{
				iNumWeight = fm * 10;  // 5 is the number of weights per feature map
						
				NNNeuron& n = *( pLayer->m_Neurons[ jj + ii*16 + fm*256 ] );

				// ÿһ����Ԫֻ��Ҫ����һ��ƫ�����ڶ�ǰһ������ۼӽ���ĵ��أ��Լӿ������ٶ�
				n.AddConnection(ULONG_MAX, iNumWeight++);  // bias weight

				int mm;
				for (kk=0; kk<9; ++kk)
				{
					for(mm=0;mm<20;mm++)
					{
						n.AddConnection(324*mm + jj + 18*ii + kernelTemplate3[kk], iNumWeight++ );
					}
				}

			}
		}
	}

	// layer four:
	// Pooling layer:
	// This layer is a  Pooling layer that has 50 feature maps.  Each feature 
	// map is 8x8, and each unit in the feature maps is a 2x2  Pooling kernel
	// of corresponding areas of the previous layers, each of which is a 16x16 feature map
	// So, there are 8x8x40 = 2560 neurons, 2x2x2560 =10240 weights

	pLayer = new NNLayer( _T("Layer04"), pLayer );
	NN.m_Layers.push_back( pLayer );

	for ( ii=0; ii<2560; ++ii )
	{
		label.Format( _T("Layer04_Neuron%04d_Num%06d"), ii, icNeurons );
		pLayer->m_Neurons.push_back( new NNNeuron( (LPCTSTR)label ) );
		icNeurons++;
	}

	// pooling�����������Ȩֵ�Ƿ�Ϊ1 ������������ǵĻ����Ƿ�pooling���Ȩֵ���鳤�ȱ�Ϊ1����ֵΪ1������
	for ( ii=0; ii<160; ++ii )
	{
		label.Format( _T("Layer04_Weight%04d_Num%06d"), ii, icWeights );
		//initWeight = 0.05 * UNIFORM_PLUS_MINUS_ONE;
		pLayer->m_Weights.push_back( new NNWeight( (LPCTSTR)label, 0.25 ) );
	}

		
	int kernelTemplate4[4] = {
		0,  1, 
		16, 17  };


	for ( fm=0; fm<40; ++fm)
	{
		for ( ii=0; ii<8; ++ii )
		{
			for ( jj=0; jj<8; ++jj )
			{
				iNumWeight = fm * 4;  // 10 is the number of weights per feature map
				NNNeuron& n = *( pLayer->m_Neurons[ jj + ii*8 + fm*64 ] );

				//n.AddConnection( ULONG_MAX, iNumWeight++ );  // bias weight

				for ( kk=0; kk<4; ++kk )
				{	
					n.AddConnection( 256*fm + 2*jj + 32*ii + kernelTemplate4[kk], iNumWeight++ );
				}
			}
		}
	}
			
	
	// layer five:
	// This layer is a convolutional layer that has 60 feature maps.  Each feature 
	// map is 6x6, and each unit in the feature maps is a 2x2 convolutional kernel
	// of corresponding areas of all 40 of the previous layers, each of which is a 6x6 feature map
	// So, there are 6x6x60 = 2180 neurons, (3x3+1)x40x60 = 24000 weights

	pLayer = new NNLayer( _T("Layer05"), pLayer );
	NN.m_Layers.push_back( pLayer );

	//**** for ( ii=0; ii<1980; ++ii )			
	for ( ii=0; ii<2160; ++ii )
	{
		label.Format( _T("Layer05_Neuron%04d_Num%06d"), ii, icNeurons );
		pLayer->m_Neurons.push_back( new NNNeuron( (LPCTSTR)label ) );
		icNeurons++;
	}

	for ( ii=0; ii<24000; ++ii )
	{
		label.Format( _T("Layer05_Weight%04d_Num%06d"), ii, icWeights );
		
		if(bLoadWeightFile == true)  //�Ƿ�����ѵ���õ�Ȩֵ
		{
			if(vWeightOfLayer5->size() != 24000)  //����ȡ��Ȩֵ�ļ��е�Ȩ������������������Ȩ��
			{
				cout<<"Ȩֵ��Ŀ���������Ϊ�������ʼȨ��"<<endl;
				initWeight = 0.05 * UNIFORM_PLUS_MINUS_ONE;
			}
			else
			{
				initWeight = (*vWeightOfLayer5)[ii];
			}
		}
		else
		{
			initWeight = 0.05 * UNIFORM_PLUS_MINUS_ONE;
		}

		pLayer->m_Weights.push_back( new NNWeight( (LPCTSTR)label, initWeight ) );
	}


	int kernelTemplate5[9] = 
	{
		0,  1,  3,  
		8,  9, 10,
		16, 17, 18 
	};

	for ( fm=0; fm<60; ++fm)
	{
		for ( ii=0; ii<6; ++ii )
		{
			for ( jj=0; jj<6; ++jj )
			{
				iNumWeight = fm * 10;  // 10 is the number of weights per feature map
							
				NNNeuron& n = *( pLayer->m_Neurons[ jj + ii*6 + fm*36 ] );  //���Ŀ����Ԫ�ڱ��汾��������Ԫ�������е�����
				n.AddConnection( ULONG_MAX, iNumWeight++ );  // bias weight
							
				int mm;
				for ( kk=0; kk<9; ++kk )
				{
					for( mm=0;mm<40;mm++)
					{
						n.AddConnection(64*mm + jj + 8*ii + kernelTemplate5[kk], iNumWeight++ );
					}
				}
				//ÿһ����Ԫֻ��Ҫ����һ��ƫ�����ڶ�ǰһ������ۼӽ���ĵ��أ��Լӿ������ٶ�
			}
		}
	}



	// layer six:
	// Pooling layer:
	// This layer is a  Pooling layer that has 60 feature maps.  Each feature 
	// map is 3x3, and each unit in the feature maps is a 2x2  Pooling kernel
	// of corresponding areas of the previous layers, each of which is a 16x16 feature map
	// So, there are 3x3x60 = 540 neurons, 2x2x540 =2160 weights

	pLayer = new NNLayer( _T("Layer06"), pLayer );
	NN.m_Layers.push_back( pLayer );

	for ( ii=0; ii<540; ++ii )
	{
		label.Format( _T("Layer06_Neuron%04d_Num%06d"), ii, icNeurons );
		pLayer->m_Neurons.push_back( new NNNeuron( (LPCTSTR)label ) );
		icNeurons++;
	}

	for ( ii=0; ii<240; ++ii )
	{
		label.Format( _T("Layer06_Weight%04d_Num%06d"), ii, icWeights );
		//initWeight = 0.05 * UNIFORM_PLUS_MINUS_ONE;
		pLayer->m_Weights.push_back( new NNWeight( (LPCTSTR)label, 0.25 ) );
	}

	// Interconnections with previous layer: this is difficult
	// Each feature map in the previous layer is a top-down bitmap image whose size
	// is 13x13, and there are 6 such feature maps.  Each neuron in one 5x5 feature map of this 
	// layer is connected to a 5x5 kernel positioned correspondingly in all 6 parent
	// feature maps, and there are individual weights for the six different 5x5 kernels.  As
	// before, we move the kernel by TWO pixels, i.e., we
	// skip every other pixel in the input image.  The result is 50 different 5x5 top-down bitmap
	// feature maps

	int kernelTemplate6[4] = 
	{
		0, 1, 
		6, 7  
	};


	for ( fm=0; fm<60; ++fm)
	{
		for ( ii=0; ii<3; ++ii )
		{
			for ( jj=0; jj<3; ++jj )
			{
				iNumWeight = fm * 4;  // 10 is the number of weights per feature map
				NNNeuron& n = *( pLayer->m_Neurons[ jj + ii*3 + fm*9 ] );

				//n.AddConnection( ULONG_MAX, iNumWeight++ );  // bias weight

				for ( kk=0; kk<4; ++kk )
				{
					//n.AddConnection(  2*jj + 6*ii + kernelTemplate4[kk], iNumWeight++ );
					n.AddConnection(36*fm + 2*jj + 12*ii + kernelTemplate6[kk], iNumWeight++ );

				}
			}
		}
	}


	// layer seven:
	// This layer is a convolutional layer that has 80 feature maps.  Each feature 
	// map is 2x2, and each unit in the feature maps is a 2x2 convolutional kernel
	// of corresponding areas of all 60 of the previous layers, each of which is a 3x3 feature map
	// So, there are 2x2x80 = 320 neurons, (2x2+1)x60x80 = 24000 weights

	pLayer = new NNLayer( _T("Layer07"), pLayer );
	NN.m_Layers.push_back( pLayer );

	for ( ii=0; ii<320; ++ii )
	{
		label.Format( _T("Layer07_Neuron%04d_Num%06d"), ii, icNeurons );
		pLayer->m_Neurons.push_back( new NNNeuron( (LPCTSTR)label ) );
		icNeurons++;
	}

	for ( ii=0; ii<24000; ++ii )
	{
		label.Format( _T("Layer07_Weight%04d_Num%06d"), ii, icWeights );
		
		if(bLoadWeightFile == true)  //�Ƿ�����ѵ���õ�Ȩֵ
		{
			if(vWeightOfLayer7->size() != 24000)  //����ȡ��Ȩֵ�ļ��е�Ȩ������������������Ȩ��
			{
				cout<<"Ȩֵ��Ŀ���������Ϊ�������ʼȨ��"<<endl;
				initWeight = 0.05 * UNIFORM_PLUS_MINUS_ONE;
			}
			else
			{
				initWeight = (*vWeightOfLayer7)[ii];
			}
		}
		else
		{
			initWeight = 0.05 * UNIFORM_PLUS_MINUS_ONE;
		}


		pLayer->m_Weights.push_back( new NNWeight( (LPCTSTR)label, initWeight ) );
		//pLayer->m_Weights.push_back( new NNWeight( (LPCTSTR)label, 1 ) );
	}


	int kernelTemplate7[4] = {
		0,  1, 
		3, 4   };


	for ( fm=0; fm<80; ++fm)
	{
		for ( ii=0; ii<2; ++ii )
		{
			for ( jj=0; jj<2; ++jj )
			{
				iNumWeight = fm * 5;  //5 is the number of weights per feature map
				NNNeuron& n = *( pLayer->m_Neurons[ jj + ii*2 + fm*4 ] );

				n.AddConnection( ULONG_MAX, iNumWeight++ );  // bias weight

				int mm;
				for ( kk=0; kk<4; ++kk )
				{
					for( mm=0;mm<60;mm++)
					{
						n.AddConnection(9*mm + jj + 3*ii + kernelTemplate7[kk], iNumWeight++ );
					}
				}

			}
		}
	}

	// layer eight:
	// This layer is a fully-connected layer with 100 units.  Since it is fully-connected,
	// each of the 120 neurons in the layer is connected to all 320 neurons in
	// the previous layer.
	// So, there are 120 neurons and 120*(320+1)=38520 weights
	
	pLayer = new NNLayer( _T("Layer08"), pLayer );
	NN.m_Layers.push_back( pLayer );
	
	for ( ii=0; ii<120; ++ii )
	{
		label.Format( _T("Layer08_Neuron%04d_Num%06d"), ii, icNeurons );
		pLayer->m_Neurons.push_back( new NNNeuron( (LPCTSTR)label ) );
		icNeurons++;
	}
	
	for ( ii=0; ii<38520; ++ii )
	{
		label.Format( _T("Layer08_Weight%04d_Num%06d"), ii, icWeights );
		
		if(bLoadWeightFile == true)  //�Ƿ�����ѵ���õ�Ȩֵ
		{
			if(vWeightOfLayer8->size() != 38520)  //����ȡ��Ȩֵ�ļ��е�Ȩ������������������Ȩ��
			{
				cout<<"Ȩֵ��Ŀ���������Ϊ�������ʼȨ��"<<endl;
				initWeight = 0.05 * UNIFORM_PLUS_MINUS_ONE;
			}
			else
			{
				initWeight = (*vWeightOfLayer8)[ii];
			}
		}
		else
		{
			initWeight = 0.05 * UNIFORM_PLUS_MINUS_ONE;
		}

		pLayer->m_Weights.push_back( new NNWeight( (LPCTSTR)label, initWeight ) );
	}
	
	// Interconnections with previous layer: fully-connected
	
	iNumWeight = 0;  // weights are not shared in this layer
	
	for ( fm=0; fm<120; ++fm )
	{
		NNNeuron& n = *( pLayer->m_Neurons[ fm ] );
		n.AddConnection( ULONG_MAX, iNumWeight++ );  // bias weight
		
		for ( ii=0; ii<320; ++ii )
		{
			n.AddConnection( ii, iNumWeight++ );
		}
	}
	
	// layer nine, the final (output) layer:
	// This layer is a fully-connected layer with 10 units.  Since it is fully-connected,
	// each of the 10 neurons in the layer is connected to all 120 neurons in
	// the previous layer.
	// So, there are 10 neurons and 10*(120+1)=1210 weights
	
	pLayer = new NNLayer( _T("Layer09"), pLayer );
	NN.m_Layers.push_back( pLayer );
	
	for ( ii=0; ii<10; ++ii )
	{
		label.Format( _T("Layer09_Neuron%04d_Num%06d"), ii, icNeurons ); 
		pLayer->m_Neurons.push_back( new NNNeuron( (LPCTSTR)label ) );
		icNeurons++;
	}
	
	for ( ii=0; ii<1210; ++ii )
	{
		label.Format( _T("Layer09_Weight%04d_Num%06d"), ii, icWeights );
		
		if(bLoadWeightFile == true)  //�Ƿ�����ѵ���õ�Ȩֵ
		{
			if(vWeightOfLayer9->size() != 1210)  //����ȡ��Ȩֵ�ļ��е�Ȩ������������������Ȩ��
			{
				cout<<"Ȩֵ��Ŀ���������Ϊ�������ʼȨ��"<<endl;
				initWeight = 0.05 * UNIFORM_PLUS_MINUS_ONE;
			}
			else
			{
				initWeight = (*vWeightOfLayer9)[ii];
			}
		}
		else
		{
			initWeight = 0.05 * UNIFORM_PLUS_MINUS_ONE;
		}

		pLayer->m_Weights.push_back( new NNWeight( (LPCTSTR)label, initWeight ) );
	}
	
	// Interconnections with previous layer: fully-connected
	
	iNumWeight = 0;  // weights are not shared in this layer
	
	for ( fm=0; fm<10; ++fm )
	{
		NNNeuron& n = *( pLayer->m_Neurons[ fm ] );
		n.AddConnection( ULONG_MAX, iNumWeight++ );  // bias weight
		
		for ( ii=0; ii<120; ++ii )
		{
			n.AddConnection( ii, iNumWeight++ );
		}
	}
	
	//*******
	//SetModifiedFlag( TRUE );
	cout<<"��ʼ����ɣ�����"<<endl<<endl;

	
	return TRUE;
}


// ��ȡѵ��/���Ե�ͼƬ��׼���������������
bool CCreateNetwork::ForwardPropagation(void)
{
	 // runs the current character image through the neural net
	
	// ���û�ѡ��Ԥ��Ĭ��Ŀ¼�µ�����ͼƬ��������ʱ���������ͼƬ�������磬����Ԥ����
	CString strNum;

	// now get image data
	unsigned char grayArray[ g_cImageSize * g_cImageSize] ={0};
															
	double arrFeaturePoint[10]={0};
	
	int index=0;   // ��õ�ǰͼƬ��Ŀ¼��������е�������ͬʱ���������ı궨λ�ã���������λ�ñ����ڽṹ��������

	double outputVector[10] = {0.0};
	double targetOutputVector[10] = {0.0};
	
	//ѡ����Լ�ͼƬ���м��黹��ѵ����ͼƬ���м��飬�ɽ��ж�β���
	bool isContinueTest=true;
	while(isContinueTest)
	{
		cout<<"��ѡ����Ҫ�����������������ͼƬ... ..."<<endl;
		//////////////////////////////////////
		//***	  ���Թ��̣����ļ�ѡ���ѡ����м���ͼƬ
		//////////////////////////////////////
		AfxSetResourceHandle(GetModuleHandle(NULL));
		CFileDialog fdTest(true,NULL,NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_EXPLORER);
		fdTest.m_ofn.lpstrFilter="Test Image (*.jpg)|*.jpg;*.JPG|All Files (*.*)|*.*||";
		fdTest.m_ofn.lpstrTitle="Test Images";
		fdTest.m_ofn.lpstrInitialDir=this->m_TestFilePath;

		if(fdTest.DoModal() != IDOK)
		{
			//this->m_fileTestingLabels.Close();
			//this->m_fileTestingImages.Close();

			return false;
		}
		

		CFileException fe;
		CString csFilePath=fdTest.GetPathName();
		CString	csFileName=fdTest.GetFileName();
		CString csInfoPath;
		CString csPoint;
		
		cout<<"���ѡ�� "<<csFileName<<endl;

		cout<<endl<<"���ڽ���������������>>> >>>"<<endl;

		if(m_fileTestingImages.Open((LPCTSTR)csFilePath,CFile::modeRead|CFile::shareDenyNone,&fe) != 0)
		{
			m_fileTestingImages.Read(grayArray,g_cImageSize*g_cImageSize); 

			////////////////////////////////////////////////////////////////////
			//����ѡ��������ͼƬ����ǩ�ļ��ж�ȡ��Ӧ�ı�ǩ��Ϣ����ʼ������������
			////////////////////////////////////////////////////////////////////
			
			//��ȡ�ַ����͵�����������
			csFileName=csFileName.SpanExcluding(".")+".txt";
			csInfoPath.Format("E:\\����ʶ��\\face point detection\\trainingImage\\ttInfo\\%s",csFileName);
			
			if(m_fileTestingLabels.Open((LPCTSTR)csInfoPath,CFile::modeRead) != 0)
			{
				m_fileTestingLabels.ReadString(csPoint);
			}

			//������������㲢�洢
			int index;
			int i=0;
			CString csTemp;
			string sTemp;
			while((index=csPoint.Find(" "))!=-1)
			{
				csTemp=csPoint.SpanExcluding(" ").GetString();
				sTemp=csTemp.GetString();
				targetOutputVector[i]=atof(sTemp.c_str())/39;
				csPoint.Delete(0,index+1);

				if(i<9)
				{
					i++;
				}
				else
				{
					i=0;
					break;
				}
			}
			sTemp=csPoint.GetString();
			targetOutputVector[9]=atof(sTemp.c_str())/39;

		}

		m_fileTestingImages.Close();
		m_fileTestingLabels.Close();

		//��ʼΪ���Թ��̼�ʱ
		DWORD tick = ::GetTickCount();

		double inputVector[1521];  	 //�洢Ϊ�Ҷ�ֵ��һ������������

		int ii,jj;

		//�������������й�һ��
		for ( ii=0; ii<g_cImageSize; ++ii )
		{
			for ( jj=0; jj<g_cImageSize; ++jj )
			{
				//inputVector[jj + 39*(ii) ] = (double)((int)(unsigned char)grayArray[ jj + g_cImageSize*ii ])/128.0 - 1.0;  // one is white, -one is black
				inputVector[jj + 39*(ii) ] = (double)((int)(unsigned char)grayArray[ jj + g_cImageSize*ii ])/255.0;  // one is white, -one is black
			}
		}

		// get state of "Distort input 
		BOOL bDistort = false;  // �Ƿ���Ҫ��ͼ������α� ��Ĭ�ϲ���Ҫ

		/////////////////////////////////////////////////////////
		//
		//
		//  �������������뵽�������н��м��㣬���ز����������������̨
		//
		/////////////////////////////////////////////////////////
		CalculateNeuralNet( inputVector, 1521, outputVector, 10, &m_NeuronOutputs, bDistort );

		//��ü��㻨�ѵ�ʱ�䣬��λΪ����
		DWORD diff = ::GetTickCount() - tick;

		CString	 strLine,strResult;
		double dTemp, sampleMse = 0.0;
	
		cout<<endl;
		cout<<"*************************************************"<<endl;
		strResult.Format( _T("Face point detected results:\n") );
	
		for ( ii=0; ii<10; ii++ )
		{
			strLine.Format( _T(" %2i = %+6.3f \n"), ii, outputVector[ii] );
			strResult += strLine;
		
			dTemp = targetOutputVector[ ii ] - outputVector[ ii ];
			sampleMse += dTemp * dTemp;
		}

		sampleMse = 0.5 * sampleMse;
		strLine.Format( _T("\n���ƽ�����:\n Ep = %g\n\n��ʱ:\n %i mSecs"), sampleMse, diff);
		strResult += strLine;

		//�����������ǰͼ��ļ�����������
		cout<<strResult<<endl;
		cout<<"*************************************************"<<endl;

		//ѯ���Ƿ���Ҫʵʱչʾ���Ч��
		cout<<endl<<"�Ƿ���Ҫ��������Ч����[Y(yes) or N(no)]:  ";
		char cSelected;
		cin>>cSelected;
		if(cSelected == 'Y' || cSelected == 'y')
		{
			CString csImageFilePath=csFilePath.SpanExcluding(".")+".jpg";
			this->DisplayPointDetected(csImageFilePath,outputVector);
		}

		cout<<endl<<"�Ƿ���Ҫ�������ԣ�[Y(yes) or N(no)]:  ";
		cin>>cSelected;
		if(cSelected != 'Y' && cSelected != 'y')
		{
			isContinueTest=false;
		}
	}
	return true;
}

void CCreateNetwork::CalculateNeuralNet(double *inputVector, int count, 
								        double* outputVector /* =NULL */, int oCount /* =0 */,
										std::vector< std::vector< double > >* pNeuronOutputs /* =NULL */,
										BOOL bDistort /* =FALSE */ )
{
	// wrapper function for neural net's Calculate() function, needed because the NN is a protected member
	// waits on the neural net mutex (using the CAutoMutex object, which automatically releases the
	// mutex when it goes out of scope) so as to restrict access to one thread at a time
	
	//***** CAutoMutex tlo( m_utxNeuralNet );
	
	if ( bDistort != FALSE )
	{	
		//************������������������������� ���ڱ�ϵͳ�п����ò���
		//GenerateDistortionMap();
		//ApplyDistortionMap( inputVector );
	}
	
	////////////////////////////////////////////////
	//
	//   ����NeuralNetwork���������ǰ����㺯��
	//	 m_NN.Calculate ��forward propagation
	////////////////////////////////////////////////
	m_NN.Calculate( inputVector, count, outputVector, oCount, pNeuronOutputs );
}


/////////////////////////////////////////////////////////////////////////////////
//
//    BP��������һ�� ��ʼ��BP���̵���ز������Լ�ѵ��ʵʱ��Ϣ�Ŀ��Ƶ����
//                   ���� CCreateNetwork::StartBackpropagation���о������
//
/////////////////////////////////////////////////////////////////////////////////

void CCreateNetwork::BackPropagation(void)
{
	CPreferences* pfs=CPreferences::GetPreferences();

//	this->m_cNumThreads=pfs->m_cNumBackpropThreads;	   //��ʼ��ϵͳ���Կ�����BP�߳�������
	this->m_InitialEta=pfs->m_dInitialEtaLearningRate;  //��ʼ��ѵ����ѧϰ����
	this->m_MinimumEta=pfs->m_dMinimumEtaLearningRate;  //��ʼ��ѧϰ���ʵ�����ֵ
	this->m_EtaDecay=pfs->m_dLearningRateDecay;		   //��ʼ��ѧϰ���ʵ��½��ٶ�
	this->m_AfterEvery=pfs->m_nAfterEveryNBackprops;	   //�趨��Ҫÿ��epoch���е�BP��������С�ﵽʱ�����ѧϰ����
	this->m_StartingPattern=0;						   //�趨�״ν���ѵ����ͼƬ�����ݼ��е�����
	this->m_EstimatedCurrentMSE=0.10;
	
	//this->m_bDistortPatterns=TRUE;					   //�Ƿ���Ҫ������ͼ�����н���������
	this->m_bDistortPatterns=FALSE;					   //�Ƿ���Ҫ������ͼ�����н���������

	//�����ǰ���������ѧϰ�����Լ���ǰ�������������ѵ����ͼƬ��ʶ
	cout<<endl<<"******************************************************"<<endl;
	CString strInitEta;
	strInitEta.Format("Initial Learning Rate eta (currently, eta = %11.8f)\n", GetCurrentEta());
	cout<<strInitEta;
	cout<<endl<<"******************************************************"<<endl;

	///////////////////////////////////////////////////
	//
	//	��BP��ʼǰ����ȡ��������ѵ�����ļ�Ŀ¼�µ�ѵ���ļ���Ϣ
	//
	//////////////////////////////////////////////////
	CFileFind finder;
	CString filename;
	CString filepath;

	m_InfoFileNames.clear();
	m_InfoFilePaths.clear();
	m_ImageFilePaths.clear();
	
	BOOL isWorking=finder.FindFile(m_TrainInfoPath);
	while(isWorking)
	{
		isWorking=finder.FindNextFileA();
		filename=finder.GetFileName();
		filepath=finder.GetFilePath();

		m_InfoFileNames.push_back(filename);
		m_InfoFilePaths.push_back(filepath);

		//����ӦͼƬ��·��Ҳ��������
		this->m_TrainImagePath=m_TrainFilePath+filename.SpanExcluding(".")+".jpg";
		m_ImageFilePaths.push_back(m_TrainImagePath);
	}

	//////////////////////////////////////////////////
	////
	////   ���Ĵ���m_pDoc->StartBackpropagation
	//////////////////////////////////////////////////
	BOOL bRet= this->StartBackpropagation( m_StartingPattern,
			m_InitialEta, m_MinimumEta, m_EtaDecay, m_AfterEvery, 
			m_bDistortPatterns, m_EstimatedCurrentMSE );
	if(bRet !=FALSE)
	{
		//m_iEpochsCompleted = 0;
		//m_iBackpropsPosted = 0;
		//m_dMSE = 0.0;

		////m_cMisrecognitions = 0;

		//m_dwEpochStartTime = ::GetTickCount();

		////����̨�����BP������ɵ���Ŀ
		//CString str;
		//str.Format( _T("%d Epochs completed\n"), m_iEpochsCompleted );
		//cout<<str;
		
		//cout<<"Backpropagation started... \n";
	}
}


////////////////////////////////////////////////////////
//
//   BP����������� 
//	   �����������BP��ʼ��ģ�鴫�͹�����BP��ز���ֵ
//       1)׼����ǰBP����������,�Լ��������������Ϊ����BP������׼����
//		 2)����ʵ�ʵ�BP���㺯����pThis->BackpropagateNeuralNet
//		 3)���ñ�����ִ��һ�κ󷵻أ�����MSE���ж��Ƿ���Ҫ��һ�ε�BP
//
////////////////////////////////////////////////////////
BOOL CCreateNetwork::StartBackpropagation(UINT iStartPattern /* =0 */, double initialEta /* =0.005 */, 
									 double minimumEta /* =0.000001 */, double etaDecay /* =0.990 */, 
									 UINT nAfterEvery  /* =1000 */, BOOL bDistortPatterns /* =TRUE */, double estimatedCurrentMSE /* =1.0 */)
{
	//���Ե�ǰ�Ƿ��Ѿ���BP�߳������У����򷵻�FALSE��ֻ����һ��BP�������ڣ���BP����������֧�ֶ��̣߳�
	/*if ( m_bBackpropThreadsAreRunning == TRUE )   
		return FALSE;
	*/

//	m_bBackpropThreadAbortFlag = FALSE;
//	m_bBackpropThreadsAreRunning = TRUE;
//	m_iNumBackpropThreadsRunning = 0;   //BP�̼߳���
//	m_iBackpropThreadIdentifier = 0;
	m_cBackprops = iStartPattern;   //ָʾ����ѵ�����ַ�ͼ��������
	
	//m_bNeedHessian = TRUE;          //ָʾ�Ƿ����hessian���󣬼��Ƿ���ö�����������

	m_iNextTrainingPattern = iStartPattern;

	if ( m_iNextTrainingPattern < 0 ) 
		m_iNextTrainingPattern = 0;
	if ( m_iNextTrainingPattern >= CPreferences::GetPreferences()->m_nItemsTrainingImages )   //m_nItemsTrainingImages=10000
		m_iNextTrainingPattern = CPreferences::GetPreferences()->m_nItemsTrainingImages - 1;
	
//	if ( iNumThreads < 1 ) 
//		iNumThreads = 1;
//	if ( iNumThreads > 10 )  // 10 is arbitrary upper limit
//		iNumThreads = 10;
	
	m_NN.m_etaLearningRate = initialEta;
	m_NN.m_etaLearningRatePrevious = initialEta;
	m_dMinimumEta = minimumEta;
	m_dEtaDecay = etaDecay;
	m_nAfterEveryNBackprops = nAfterEvery;
	m_bDistortTrainingPatterns = bDistortPatterns;

	//// MSE��ֵ�����������ж��Ƿ���Ҫ������һ�ε�backpropagation
	m_dEstimatedCurrentMSE = estimatedCurrentMSE;  // estimated number that will define whether a forward calculation's error is significant enough to warrant backpropagation

	///////////////////////////////////
	///
	///  ��ʼBP����
	///////////////////////////////////
	CCreateNetwork* pThis = this;
	
	ASSERT( pThis != NULL );
	
	// do the work
	
	double inputVector[1521] = {0.0};  // note: 29x29, not 28x28
	double targetOutputVector[10] = {0.0};
	double actualOutputVector[10] = {0.0};
	//double dMSE;
	//UINT scaledMSE;

	unsigned char grayLevels[g_cImageSize * g_cImageSize]; //���ڱ����ȡ��patternͼ��Ҷ�ֵ����СΪ39*39
	
	int label = 0;
	int ii, jj,index;
	UINT iSequentialNum;
	
	std::vector< std::vector< double > > memorizedNeuronOutputs;  
	
	bool isNeedQuitBP=FALSE;
	while ( isNeedQuitBP == FALSE )  //��⵱ǰ�߳��Ƿ���ֹ
	{
		//����ȡ��һ������ѵ����pattern����patternͼ��ĻҶ�ֵ���б��浽����grayLevels����1521�����ص㣬��������������λ�ñ��浽targetOutputVector[10]
		memset(grayLevels,0,39*39);

		int iRet = pThis->GetNextTrainingPattern( grayLevels, targetOutputVector, FALSE, FALSE, &iSequentialNum );
		
		double grayValue;
		for ( ii=0; ii<g_cImageSize; ++ii )
		{
			for ( jj=0; jj<g_cImageSize; ++jj )
			{
				grayValue=(double)((int)(unsigned char)grayLevels[ jj + g_cImageSize*ii ]);
				//inputVector[jj + 39*(ii) ] = grayValue/128.0 - 1.0;  // one is white, -one is black
				inputVector[jj + 39*(ii) ] = grayValue/255.0;  
			}
		}

		///����������������������targetOutputVector[10]���г�ʼ�� ������
		CString csInfoPath=m_InfoFilePaths[iSequentialNum];
		CString csPoint;
		CString csTemp;
		string sTemp;
		int i=0;

		CStdioFile m_filePoint;
		m_filePoint.Open(csInfoPath,CFile::modeRead);
		m_filePoint.ReadString(csPoint);

		while((index=csPoint.Find(" "))!=-1)
		{
			csTemp=csPoint.SpanExcluding(" ").GetString();
			sTemp=csTemp.GetString();
			targetOutputVector[i]=atof(sTemp.c_str())/39;  //��һ������
			csPoint.Delete(0,index+1);

			if(i<9)
			{
				i++;
			}
			else
			{
				i=0;
				break;
			}
		}
		sTemp=csPoint.GetString();
		targetOutputVector[9]=atof(sTemp.c_str())/39;  //��һ������
		m_filePoint.Close();
		
		/////////////////////////////////////////////////////////////////////
		//	BP��Ҫ�����������Լ���ز�����׼���ã���ʼBPʵ�ʲ���
		/////////////////////////////////////////////////////////////////////
		bool rt=pThis->BackpropagateNeuralNet( inputVector, 1521, targetOutputVector, actualOutputVector, 10, 
			&memorizedNeuronOutputs, pThis->m_bDistortTrainingPatterns );
		
		//��ʾ��������������������ʵ�ʽ��
		CString strPoint;

		cout<<"The actual output of facial point before refine is:\n";
		for(ii=0;ii<10;)
		{
			strPoint.Format("(%f , %f)\n", actualOutputVector[ii], actualOutputVector[ii+1]);
			cout<<strPoint;
			ii=ii+2;
		}
		cout<<endl<<endl;

		cout<<"The target output of facial point is:\n";
		for(ii=0;ii<10;)
		{
			strPoint.Format("(%f , %f)\n",targetOutputVector[ii],targetOutputVector[ii+1]);
			cout<<strPoint;
			ii=ii+2;
		}
		cout<<endl;
		
		memset(actualOutputVector,0,10);
		memset(targetOutputVector,0,10);

		if(rt == false)  //���С������ֵʱѵ����ɣ��˳�BP�߳�
		{
			cout<<"Skip Backpropagation!!!";
		}
		cout<<endl<<endl;
	}

	cout<<"BP exit!!"<<endl;
	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//    BP������������ BPѵ�����̵�������������
//                   1������CalculateNeuralNetǰ������Ƶ�������
//				     2��������������������������з����򴫲�������m_NN.Backpropagate( actualOutputVector, targetOutputVector��...);
//						,�Ե��ڲ���Ȩ��ֵ��
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CCreateNetwork::BackpropagateNeuralNet(double *inputVector, int iCount, double* targetOutputVector, 
									   double* actualOutputVector, int oCount, 
									   std::vector< std::vector< double > >* pMemorizedNeuronOutputs, 
									   BOOL bDistort )
{
	// function to backpropagate through the neural net. 
	ASSERT( (inputVector != NULL) && (targetOutputVector != NULL) && (actualOutputVector != NULL) );

	///////////////////////////////////////////////////////////////////////
	//
	// CODE REVIEW NEEDED:
	//
	// It does not seem worthwhile to backpropagate an error that's very small.  "Small" needs to be defined
	// and for now, "small" is set to a fixed size of pattern error ErrP <= 0.10 * MSE, then there will
	// not be a backpropagation of the error.  The current MSE is updated from the neural net dialog CDlgNeuralNet
	///////////////////////////////////////////////////////////////////////

	BOOL bWorthwhileToBackpropagate;  /////// part of code review

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//	���������������������ֲ����������������ʱ����Ҫ��CAutoMutex��������������Ӷ��ͷŶ�m_utxNeuralNet����������������ռ��
	//	��FP��������У������ռ������
	//	local scope for capture of the neural net, only during the forward calculation step,
	//	i.e., we release neural net for other threads after the forward calculation, and after we
	//	have stored the outputs of each neuron, which are needed for the backpropagation step
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	{	
		//CAutoMutex tlo( m_utxNeuralNet ); //��Դ����������������������Ȩ
		
		// determine if it's time to adjust the learning rate  �Ƿ���Ҫ����ѧϰ����(ÿ����10000��BP��Ŀǰȡ2000)
		if ( (( m_cBackprops % m_nAfterEveryNBackprops ) == 0) && (m_cBackprops != 0) )
		{
			double eta = m_NN.m_etaLearningRate;
			eta *= m_dEtaDecay;
			if ( eta < m_dMinimumEta )   //etaѧϰ���ʵ�����
				eta = m_dMinimumEta;	 							  ///////////////////////////////////////////
			m_NN.m_etaLearningRatePrevious = m_NN.m_etaLearningRate;  //  ��¼֮ǰ��ѧϰ���ʣ����ã�������
			m_NN.m_etaLearningRate = eta;                             ///////////////////////////////////////////
		}
		
		
		// determine if it's time to adjust the Hessian (currently once per epoch) �Ƿ���Ҫ����Hessian����
		// ÿ��������ѵ������Ϊ10000��BP����Ϊ��10000��training images��
		//**** ��ϵͳ��ʱ����������������hessian����
		//if ( (m_bNeedHessian != FALSE) || (( m_cBackprops % CPreferences::GetPreferences().m_nItemsTrainingImages ) == 0) )
		//{
		//	// adjust the Hessian.  This is a lengthy operation, since it must process approx 500 labels
		//	CalculateHessian();  // ����hessian����
		//	
		//	m_bNeedHessian = FALSE;
		//}
		
		// determine if it's time to randomize the sequence of training patterns (currently once per epoch)
		// ÿһ��epoch��������ǲ�ͬ��ͼ����������
		//**** ��ϵͳ��ʱ��������ѵ�����н���������
		/*if ( ( m_cBackprops %  CPreferences::GetPreferences().m_nItemsTrainingImages ) == 0 )
		{
			RandomizeTrainingPatternSequence();   
		}*/
		
		// increment counter for tracking number of backprops
		m_cBackprops++;
		
		// ǰ������Ƶ�
		CalculateNeuralNet( inputVector, iCount, actualOutputVector, oCount, pMemorizedNeuronOutputs, bDistort );

		// calculate error in the output of the neural net
		// note that this code duplicates that found in many other places, and it's probably sensible to 
		// define a (global/static ??) function for it
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//  
		//	����������ļ���ƽ�����MSE   
		//	���δ�������öȺܸߣ����Ҫ�ر�ע��ȫ��/��̬�����Ķ���
		//��targetOutputVector[10]������ʵ���������,��Ӧ���Ԥ���������λ��ֵ��
		//
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		double dMSE = 0.0;
		for ( int ii=0; ii<10; ++ii )
		{
			dMSE += ( actualOutputVector[ii]-targetOutputVector[ii] ) * ( actualOutputVector[ii]-targetOutputVector[ii] );
		}
		dMSE /= 2.0;

		//////////////////////////////////////////////////////////
		//
		//  ��Ƽ��ɣ���������ƽ������ʵ������Сʱ��������ǰBP
		//////////////////////////////////////////////////////////
		if ( dMSE <= ( 0.05 * m_dEstimatedCurrentMSE ) )
		{
			bWorthwhileToBackpropagate = FALSE;
		}
		else
		{
			bWorthwhileToBackpropagate = TRUE;
		}

		if( bWorthwhileToBackpropagate == FALSE)
		{
			//SaveLastWeight();
			return false;
		}


		//��ѵ������δ���������״�FP���ʱ... ...
		if ( (bWorthwhileToBackpropagate != FALSE) && (pMemorizedNeuronOutputs == NULL) )
		{
			// the caller has not provided a place to store neuron outputs, so we need to
			// backpropagate now, while the neural net is still captured.  Otherwise, another thread
			// might come along and call CalculateNeuralNet(), which would entirely change the neuron
			// outputs and thereby inject errors into backpropagation 
			
			m_NN.Backpropagate( actualOutputVector, targetOutputVector, oCount, NULL );
			
			//SetModifiedFlag( TRUE );
			
			// we're done, so return
			return true;
		}
		
	}//�ֲ��������������ǰ�������̽������ͷŶ��������ռ��
	
	// if we have reached here, then the mutex for the neural net has been released for other 
	// threads.  The caller must have provided a place to store neuron outputs, which we can 
	// use to backpropagate, even if other threads call CalculateNeuralNet() and change the outputs
	// of the neurons

	//��ѵ��δ�����Ҳ����״�FP���... ...
	if ( (bWorthwhileToBackpropagate != FALSE) )
	{
		//////////////////////////////
		// back propagation
		// ���˼���
		//////////////////////////////
		m_NN.Backpropagate( actualOutputVector, targetOutputVector, oCount, pMemorizedNeuronOutputs );
		
		// set modified flag to prevent closure of doc without a warning
		//SetModifiedFlag( TRUE );
	}
	 
	//��������������µļ����
	/*double actualOutputVector2[10] = {0.0};
	CalculateNeuralNet( inputVector, iCount, actualOutputVector2, oCount, pMemorizedNeuronOutputs, bDistort );

	CString strPoint;
	cout<<endl<<endl;
	cout<<"The actual output of facial point after refine is:\n";
	for(int ii=0;ii<10;)
	{
		strPoint.Format("(%f , %f)\n",actualOutputVector2[ii],actualOutputVector2[ii+1]);
		cout<<strPoint;
		ii=ii+2;
	}
	memset(actualOutputVector2,0,10);
	cout<<endl<<endl;*/

	return true;
}


UINT CCreateNetwork::GetNextTrainingPattern(unsigned char* pArray, double* pPointLocation, BOOL bFlipGrayscale ,
		BOOL bFromRandomizedPatternSequence, UINT* iSequenceNum)
{
	 UINT iPatternNum;

	 iPatternNum = m_iNextTrainingPattern;

	 ASSERT(iPatternNum < CPreferences::GetPreferences()->m_nItemsTrainingImages);

	 GetTrainingPatternArrayValues(iPatternNum,pArray,FALSE);

	 if(iSequenceNum!=NULL)
	 {
		*iSequenceNum=m_iNextTrainingPattern;
	 }

	 m_iNextTrainingPattern++;

	 if ( m_iNextTrainingPattern >= CPreferences::GetPreferences()->m_nItemsTrainingImages )
	{
		m_iNextTrainingPattern = 0;
	}

	 return  iPatternNum;

}

void CCreateNetwork::GetTrainingPatternArrayValues(int iNumImage, unsigned char* pArray,BOOL bFlipGrayscale)
{
	int cCount = g_cImageSize*g_cImageSize; 
	CFileException fe;
	string sPath=m_ImageFilePaths[iNumImage].SpanExcluding(".").GetString();
	cout<<endl<<endl<<"***** "<<iNumImage+1<<" Processing: "<<sPath.c_str()<<endl;
	if(m_fileTrainingImages.Open(sPath.c_str(),CFile::modeRead,&fe) !=0)
	{
		if ( pArray != NULL )
		{
			m_fileTrainingImages.Read(pArray,cCount);

			if ( bFlipGrayscale != FALSE )
			{
				for ( int ii=0; ii<cCount; ++ii )
				{
					pArray[ ii ] = 255 - pArray[ ii ];
				}
			}

			m_fileTrainingImages.Close();
		}
	}
}


double CCreateNetwork::GetCurrentEta()
{
	return m_NN.m_etaLearningRate;
}


double CCreateNetwork::GetPreviousEta()
{
	// provided because threads might change the current eta before we are able to read it
	
	return m_NN.m_etaLearningRatePrevious;
}


UINT CCreateNetwork::GetCurrentTrainingPatternNumber( BOOL bFromRandomizedPatternSequence /* =FALSE */ )
{
	// returns the current number of the training pattern, either from the straight sequence, or from
	// the randomized sequence
	
	UINT iRet;
	
	if ( bFromRandomizedPatternSequence == FALSE )
	{
		iRet = m_iNextTrainingPattern;
	}
	else
	{
		iRet = m_iRandomizedTrainingPatternSequence[ m_iNextTrainingPattern ];
	}
	
	return iRet;
}

//�������ѵ�����ݼ�����������
void CCreateNetwork::RandomizeTrainingPatternSequence()
{
	// randomizes the order of m_iRandomizedTrainingPatternSequence, which is a UINT array
	// holding the numbers 0..59999 in random order
	
	UINT ii, jj, iiMax, iiTemp;
	
	iiMax = CPreferences::GetPreferences()->m_nItemsTrainingImages;
	
	ASSERT( iiMax == 10000 );  // requirement of sloppy and unimaginative code
	
	// initialize array in sequential order
	
	for ( ii=0; ii<iiMax; ++ii )
	{
		m_iRandomizedTrainingPatternSequence[ ii ] = ii;  
	}
	
	// now at each position, swap with a random position
	// ������򽻻�������������
	for ( ii=0; ii<iiMax; ++ii )
	{
		jj = (UINT)( UNIFORM_ZERO_THRU_ONE * iiMax ); //UNIFORM_ZERO_THRU_ONE = 0 or 1
		
		ASSERT( jj < iiMax );
		
		iiTemp = m_iRandomizedTrainingPatternSequence[ ii ];
		m_iRandomizedTrainingPatternSequence[ ii ] = m_iRandomizedTrainingPatternSequence[ jj ];
		m_iRandomizedTrainingPatternSequence[ jj ] = iiTemp;
	}
	
}

// ����Ȩֵ�ļ�,�õ�����ѵ���õ�Ȩ��ֵ
void CCreateNetwork::LoadWeightFile(vector<double>* vWeightOfLayer, char* path)
{
	CStdioFile sfp;			//Ȩֵ�ļ����
	CString csWeightValue;  //�洢���ļ�������Ȩֵ�ַ���
	string strFilePath;     //������Ȩֵ�ļ�·��

	strFilePath=path;
	if(sfp.Open(strFilePath.c_str(),CFile::modeRead)!=0)
	{
		while(sfp.ReadString(csWeightValue))
		{
			vWeightOfLayer->push_back(atof(csWeightValue));
		}
	}
	else
	{
		cout<<"Ȩֵ�ļ�:"<<path<<"��ʧ�ܣ�"<<endl;
	}
	sfp.Close();
}


// ����Opencv��ʵʱ������ͼƬ�ϱ�ǳ���⵽��������
void CCreateNetwork::DisplayPointDetected(CString csImageFilePath, double* dPointDetected)
{
	//�����������귴��һ��,��ת��Ϊ�������ص�����
	int iPointValue[10];
	double dResidual;
	for(int ii=0;ii<10;ii++)
	{
		dPointDetected[ii]=dPointDetected[ii]*39;
		dResidual=dPointDetected[ii]-(int)dPointDetected[ii];

		if(dResidual >=0 && dResidual <= 0.5)  //������ֵȡ���������ڱ��
		{
			iPointValue[ii]=(int)dPointDetected[ii];
		}
		else
		{
			iPointValue[ii]=(int)dPointDetected[ii]+1;
		}
	}

	IplImage* image=cvLoadImage(csImageFilePath.GetBuffer(0),0);

//	cvNamedWindow("��������������չʾ");

	CvPoint ptFace;
	
	for(int ii=0;ii<10;)
	{
		ptFace.x=iPointValue[ii];
		ptFace.y=iPointValue[ii+1];
		ii=ii+2;

		cvCircle(image,ptFace,1,CV_RGB(255,0,0),-1,8,0);
	}
	cvNamedWindow("��������������չʾ",CV_WINDOW_AUTOSIZE);
	cvShowImage("��������������չʾ",image);
	cvWaitKey(0);
	cvDestroyWindow("��������������չʾ");//���ٴ���
	cvReleaseImage(&image);

}


// BP����˳�ѵ��ǰ���浱ǰ�����Ȩ��ֵ
void CCreateNetwork::SaveLastWeight(void)
{
	string strPath="";
	string strFileName="";
	char ch[30];
	char wt[15];
	double dValue;  //����Ȩ��ֵ

	int nWeightCount=0; //���浱ǰ���Ȩ�ظ���
	CFileException fe;

	VectorLayers::iterator lit = m_NN.m_Layers.end() - 1;
	int flagBP=9; //��ǵ�ǰ�Ѵ�����һ�㣬�����һ�㿪ʼ
	for ( lit; lit>m_NN.m_Layers.begin(),flagBP>0; lit--,flagBP--)
	{
		switch(flagBP)
		{
			case 9: 
			case 8:
			case 7:
			case 5:
			case 3:
			case 1:
				sprintf(ch,"\\%d-complete.txt",flagBP);
				strFileName=ch;
				strPath=m_sWeightSavePath;
				strPath+=strFileName;

				nWeightCount=((*lit)->m_Weights).size();

				if(m_fileWeight.Open(strPath.c_str(),CFile::modeCreate|CFile::modeWrite,&fe) !=0)
				{
					for(int i=0;i<nWeightCount;i++)
					{
						dValue=(((*lit)->m_Weights)[i])->value;
						sprintf(wt,"%f\r\n",dValue);   //һ��Ȩֵ��������Ϊһ�У����ڶ�ȡ
						m_fileWeight.WriteString((LPCTSTR)wt);
					}
				}
				m_fileWeight.Close();
				break;

		case 6:  //pooling�㲻��Ҫ����
		case 4:  
		case 2: 
			break;
		}
	}
}
