// Dib.h: interface for the CDib class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DIB_H__CF2B2C9B_FFFE_4678_A973_C5823C7F4444__INCLUDED_)
#define AFX_DIB_H__CF2B2C9B_FFFE_4678_A973_C5823C7F4444__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CDib  
{
public:
	CDib();
	virtual ~CDib();

	bool LoadFile(const char *FileName);           //����ͼ��
	bool SaveFile( const char* pFileName);         //����ͼ��
	void Draw(CDC *pDC);                           //��ͼ��
	long* GetGreyCountNumber();                    //
    long GetPaletteEntries();                      //����ɫ��
    /* ��ͼ����*/
	void Invert();                                 //��Ƭ
	void Liner(int X1,int Y1,int X2,int Y2);       //���Ա任
	void Histave();                                //ֱ��ͼ����
	void PseuColor();                              //α��
	void Colortogrey();                            //��ɫ��Ҷ�
    /*ͼ����ǿ*/
	void AddPepperSaltNoise();                     //�ӽ�������
	void AddRandomNoise();                         //���������
	void Lapchange();                              //������˹��
	void GradSharp();                              //�ݶ���
	void Midchange();                              //��ֵ�˲�
	void Pinghua();                                //ƽ���˲�
    /*ͼ�񼸺α任*/
	void Pingyi(int offsetX,int offsetY);          //ƽ��
	bool Xuanzhuan(int angle);                     //��ת �ڽ���ֵ
	bool Xuanzhuan2(int angle);                    //��ת ˫���Բ�ֵ
	bool Suoxiao(float xratio,float yratio);       //���� �ڽ���ֵ
	bool Suoxiao2(float xratio,float yratio);      //���� ˫���Բ�ֵ
	unsigned char interpolationDblLinear(unsigned char array[2][2], float xpos, float ypos);
	void MirrorHori();                             //ˮƽ����
	void MirrorVerti();                            //��ֱ����
	bool PointInRect( CPoint point );              //�ж��Ƿ��ھ�����
	void SetDestRect( CSize SizeOffset );          //�ı���δ�Сλ��
	void Zoom( float ratio );                      //ͼ����������
    /*��̬ѧ���ָ�*/
	void Erzhi();                                  //�Բ�ɫͼ���ֵ��
	int FindT();                                   //�ҵ�����
	void Fushi();                                  //��ʴ    
	void Pengzhang();                              //����
	void TiquRobert();                             //��ȡ�߽�Robert����
	void TiquLaplacian();                          //��ȡ�߽�������˹����
	void TiquSobel();                              //��ȡ�߽�sobel����                  
	void Tiqu2();                                  //��ȡ�߽���̬ѧ
	void Jiabianjie();                             //�߽�ӵ�ԭͼ��
	void RegionGrow();                             //��������

private:
	unsigned char *m_pDib;
	unsigned char *m_pDibout;
	unsigned char *m_pDibback;
	unsigned char *m_pDibBits;
	unsigned char *m_pDibBitsout;
	unsigned char *m_pDibBitsback;
	CPalette m_Palette;
	int PaletteEntries;
	long m_Width;
	long pixelbyte;
	long wide;
	long m_Height;
	long m_Widthout;
	long m_Heightout;
	BITMAPINFOHEADER *m_pBIH;
	BITMAPFILEHEADER BFH;
	RGBQUAD *m_pPalette;
	CRect m_rtPosSour,m_rtPosDest;
};

#endif // !defined(AFX_DIB_H__CF2B2C9B_FFFE_4678_A973_C5823C7F4444__INCLUDED_)
