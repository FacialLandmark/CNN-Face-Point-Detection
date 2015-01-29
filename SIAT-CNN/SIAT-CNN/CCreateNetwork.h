// disable the template warning C4786 : identifier was truncated to '255' characters in the browser information

#pragma warning( push )
#pragma warning( disable : 4786 )


#include "NeuralNetwork.h"	


using namespace std;

#include <vector>
#include <afxmt.h>  // for critical section, multi-threaded etc

typedef std::vector< double >  VectorDoubles;

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CCreateNetwork
{
public:
	CCreateNetwork(void);

// Implementation
// ���ܲ���Ҫ�����������봦��
public:

	void ApplyDistortionMap( double* inputVector );
	//void GenerateDistortionMap( double severityFactor = 1.0 );
	double* m_DispH;  // horiz distortion map array
	double* m_DispV;  // vert distortion map array

	//double m_GaussianKernel[ GAUSSIAN_FIELD_SIZE ] [ GAUSSIAN_FIELD_SIZE ];

	int m_cCols;  // size of the distortion maps
	int m_cRows;
	int m_cCount;

	CStdioFile m_fileTrainingLabels; //ѵ����ǩ��Ϣ�ı�
	CFile m_fileTrainingImages; //ѵ��������
	CStdioFile m_fileTestingLabels;  //���Ա�ǩ��Ϣ�ı�
	CFile m_fileTestingImages;	//���Լ�����

	CStdioFile m_fileWeight; 

	BOOL bTraining;
private:
	CString m_TrainFilePath;
	CString m_TestFilePath;

	CString m_TrainInfoPath;

	CString m_TrainImagePath;
	

public:
	//inline double& At( double* p, int row, int col )  // zero-based indices, starting at bottom-left
	//	{ int location = row * m_cCols + col;
	//	  ASSERT( location>=0 && location<m_cCount && row<m_cRows && row>=0 && col<m_cCols && col>=0 );
	//	  return p[ location ];
	//	}

	double GetCurrentEta();
	double GetPreviousEta();
	bool BackpropagateNeuralNet(double *inputVector, int iCount, double* targetOutputVector, 
		double* actualOutputVector, int oCount, 
		std::vector< std::vector< double > >* pMemorizedNeuronOutputs, 
		BOOL bDistort );	
	void CalculateNeuralNet(double* inputVector, int count, double* outputVector = NULL, 
		int oCount = 0, std::vector< std::vector< double > >* pNeuronOutputs = NULL, BOOL bDistort = FALSE );
	void BackPropagation(void);

	vector< VectorDoubles > m_NeuronOutputs;

private:
	//����ѵ����ǩĿ¼�µ����б�ǩ�ļ����Լ��ļ�·��
	static vector<CString> m_InfoFileNames;
	static vector<CString> m_InfoFilePaths;
	static vector<CString> m_ImageFilePaths;

public:

	// backpropagation and training-related members
	volatile UINT m_cBackprops;

	// volatile BOOL m_bNeedHessian;  ����Ҫ����hessian����
	
	// HWND m_hWndForBackpropPosting;
	UINT m_nAfterEveryNBackprops;
	double m_dEtaDecay;
	double m_dMinimumEta;
	volatile double m_dEstimatedCurrentMSE;  // this number will be changed by one thread and used by others
	
	//��ֲ�˶Ի���Ĳ������ñ���
	double m_InitialEta;
	double m_MinimumEta;
	double m_EtaDecay;
	UINT m_AfterEvery;
	UINT m_StartingPattern;
	double m_EstimatedCurrentMSE;
	BOOL m_bDistortPatterns;

	double m_dMSE;
	//UINT m_cMisrecognitions;
	DWORD m_dwEpochStartTime;
	UINT m_iEpochsCompleted;
	UINT m_iBackpropsPosted;

	BOOL m_bDistortTrainingPatterns;  //����Ҫ��������

	BOOL StartBackpropagation(UINT iStartPattern = 0, 
			double initialEta = 0.005, double minimumEta = 0.000001, double etaDecay = 0.990,
			UINT nAfterEvery = 1000, BOOL bDistortPatterns = TRUE, double estimatedCurrentMSE = 1.0 );
	
	//���ڱ�ʶѵ�����ݼ�����������
	volatile UINT m_iNextTrainingPattern;
	volatile UINT m_iRandomizedTrainingPatternSequence[ 10000 ];  //ѵ������ĿΪ10000

	void RandomizeTrainingPatternSequence();
	UINT GetCurrentTrainingPatternNumber( BOOL bFromRandomizedPatternSequence = FALSE );
	void GetTrainingPatternArrayValues( int iNumImage = 0,unsigned char* pArray = NULL,BOOL bFlipGrayscale =FALSE );

	/*UINT GetNextTrainingPattern(unsigned char* pArray = NULL, int* pLabel = NULL, BOOL bFlipGrayscale = TRUE,
		BOOL bFromRandomizedPatternSequence = TRUE, UINT* iSequenceNum = NULL );*/
	UINT GetNextTrainingPattern(unsigned char* pArray = NULL, double* pPointLocation = NULL, BOOL bFlipGrayscale = FALSE,
		BOOL bFromRandomizedPatternSequence = FALSE, UINT* iSequenceNum = NULL );
	UINT GetRandomTrainingPattern(unsigned char* pArray=NULL, int* pLabel=NULL, BOOL bFlipGrayscale=FALSE);

	// testing-related members
	volatile UINT m_iNextTestingPattern;
	
#ifdef _DEBUG
	//virtual void AssertValid() const;
	//virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	NeuralNetwork m_NN;

public:
	~CCreateNetwork(void);

	// ��ʼ������ṹ
	BOOL InitNetwork(bool bLoadWeightFile);

	// ��ȡѵ��/���Ե�ͼƬ��׼���������������
	bool ForwardPropagation(void);
	// ��ʼ��BP���̵���ز���
	void PreBackPropagaton(void);
	
private:
	// Ȩ���ļ�����·��
	string m_sWeightSavePath;
public:
	// ���μ���Ȩֵ�ļ�
	void LoadWeightFile(vector<double>* vWeightOfLayer, char* path);
private:
	// ����Opencv��ʵʱ��������ǳ���⵽��������
	void DisplayPointDetected(CString csImageFilePath, double* dPointDetected);
	// BP����˳�ѵ��ǰ���浱ǰ�����Ȩ��ֵ
	void SaveLastWeight(void);
};

// re-enable warning C4786 re : identifier was truncated to '255' characters in the browser information

#pragma warning( pop )