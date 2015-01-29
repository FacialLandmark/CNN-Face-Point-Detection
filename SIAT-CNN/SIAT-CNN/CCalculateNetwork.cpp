#include "CCalculateNetwork.h"
#include "StdAfx.h"

using namespace std;

CCalculateNetwork::CCalculateNetwork(void)
{
	m_NeuronOutputs.clear();


}


CCalculateNetwork::~CCalculateNetwork(void)
{
}


//������ݵ�׼������������ͼƬ����������������м��
void CCalculateNetwork::CalculateNetwork(void)
{
	// runs the current character image through the neural net
	
	// ���û�ѡ��Ԥ��Ĭ��Ŀ¼�µ�����ͼƬ��������ʱ���������ͼƬ�������磬����Ԥ����
	CString strNum;

	// now get image data
	unsigned char grayArray[ g_cImageSize * g_cImageSize] ={0};

	double arrFeaturePoint[10]={0};
	// ��������ͼƬ����ֵ������ grayAray��,indexΪ��ͼƬ������ֵ
	//int index=.....;   // ��õ�ǰͼƬ��������ͬʱ���������ı궨λ�ã���������λ�ñ����ڽṹ��������

	BOOL bTraining = ����������;  // ��ǵ�ǰ��ѵ�����ǲ��Թ���
	
	if ( bTraining != FALSE )
	{

		//////////////////////////////////////
		//	  ����ɣ�����������
		//////////////////////////////////////
		GetTrainImageArrayValues(index,grayArray,&arrFeaturePoint);
	}
	else
	{

		//////////////////////////////////////
		//	  ����ɣ�����������
		//////////////////////////////////////
		GetTestImageArrayValues(index,grayArray,&arrFeaturePoint);
	}

	//��ʼΪ���Թ��̼�ʱ
	DWORD tick = ::GetTickCount();

	double inputVector[1521];  	 //�洢Ϊ�Ҷ�ֵ��һ������������

	for ( ii=0; ii<g_cImageSize; ++ii )
	{
		for ( jj=0; jj<g_cImageSize; ++jj )
		{
			inputVector[jj + 39*(ii) ] = (double)((int)(unsigned char)grayArray[ jj + g_cImageSize*ii ])/128.0 - 1.0;  // one is white, -one is black
		}
	}

	// get state of "Distort input 
	BOOL bDistort = false;  // �Ƿ���Ҫ��ͼ������α� ��Ĭ�ϲ���Ҫ

	double outputVector[10] = {0.0};
	double targetOutputVector[10] = {0.0};

	// initialize target output vector (i.e., desired values)
	// ���浱ǰͼƬ��10����������λ��
	for(ii=0; ii<10; ii++)
	{
		targetOutputVector[ii]=arrFeaturePoint;
	}

	/////////////////////////////////////////////////////////
	//
	//
	//  �������������뵽�������н��м��㣬���ؼ�����
	//
	/////////////////////////////////////////////////////////
	CalculateNeuralNet( inputVector, 1521, outputVector, 10, &m_NeuronOutputs, bDistort );



}


////////////////////////////////////////////////////////////////////////////
//	����������ƣ����ŵ���m_NN.Calculate(....),
//	m_NN.Calculate(...)��inputvector��Ϊ���������룬������浽outputVector
//
////////////////////////////////////////////////////////////////////////////
void CCalculateNetwork::CalculateNeuralNet(double *inputVector, int count, 
								   double* outputVector /* =NULL */, int oCount /* =0 */,
								   std::vector< std::vector< double > >* pNeuronOutputs /* =NULL */,
								   BOOL bDistort /* =FALSE */ )
{
	// wrapper function for neural net's Calculate() function, needed because the NN is a protected member
	// waits on the neural net mutex (using the CAutoMutex object, which automatically releases the
	// mutex when it goes out of scope) so as to restrict access to one thread at a time
	
	CAutoMutex tlo( m_utxNeuralNet );
	
	if ( bDistort != FALSE )
	{	
		//���������������
		GenerateDistortionMap();
		ApplyDistortionMap( inputVector );
	}
	
	////////////////////////////////////////////////
	//
	//   ����NeuralNetwork���������ǰ����㺯��
	//	 m_NN.Calculate ��forward propagation
	////////////////////////////////////////////////
	m_NN.Calculate( inputVector, count, outputVector, oCount, pNeuronOutputs );
	
}
