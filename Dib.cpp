// Dib.cpp: implementation of the CDib class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DIP_Final.h"
#include "Dib.h"
#include <math.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/***********************************************************************
* �������ƣ�
* CDib()
*˵�����޲����Ĺ��캯�����Գ�Ա�������г�ʼ��
***********************************************************************/
CDib::CDib()
{
	m_pDib=NULL;
    m_pDibout=NULL;
	m_pDibback=NULL;
	m_rtPosSour.SetRectEmpty();
	m_rtPosDest.SetRectEmpty();
}
/***********************************************************************
* �������ƣ�
* ~CDib()
*˵�����޲����������������Գ�Ա������������
***********************************************************************/
CDib::~CDib()
{
	if (m_pDib!=NULL)
	{
		delete []m_pDib;
	}
	if (m_pDibout!=NULL)
	{
		delete []m_pDibout;
	}
	if (m_pDibback!=NULL)
	{
		delete []m_pDibback;
	}
}

/***********************************************************************
* �������ƣ�
* LoadFile()
*˵��������Ϊ��ͼ����ļ����������ļ����ڴ�
***********************************************************************/
bool CDib::LoadFile( const char *FileName )
{
	CFile File;
	File.Open(FileName,CFile::modeRead|CFile::shareDenyRead,NULL);
	DWORD dwDibSize=File.GetLength()-sizeof(BITMAPFILEHEADER);
	File.Read(&BFH,sizeof(BITMAPFILEHEADER));
	if (BFH.bfType!='MB')
	{
		AfxMessageBox("This file is not a BMP",MB_OK);
		return false;
	}
	m_pDib=new unsigned char[dwDibSize];
	m_pDibout=new unsigned char[dwDibSize];
	m_pDibback=new unsigned char[dwDibSize];
	File.Read(m_pDib,dwDibSize);
	m_pBIH=(BITMAPINFOHEADER *)m_pDib;
	
	
	m_Width = ((m_pBIH->biWidth * m_pBIH->biBitCount)+31)/32*4;
	pixelbyte=m_pBIH->biBitCount/8;
	wide=m_pBIH->biWidth;
	m_Height=m_pBIH->biHeight;
	m_Widthout=m_Width;
	m_Heightout=m_Height;
	
	m_rtPosSour.SetRect(CPoint(0,0),CPoint(wide,m_Height));
	m_rtPosDest = m_rtPosSour;
	
	
	if (m_pBIH->biBitCount>8)
	{
		PaletteEntries=0;
	} 
	else
	{
		PaletteEntries=256;
	}
	m_pDibBits=m_pDib+sizeof(BITMAPINFOHEADER)+PaletteEntries*sizeof(RGBQUAD);
	m_pDibBitsout=m_pDibout+sizeof(BITMAPINFOHEADER)+PaletteEntries*sizeof(RGBQUAD);
	m_pDibBitsback=m_pDibback+sizeof(BITMAPINFOHEADER)+PaletteEntries*sizeof(RGBQUAD);
	
	m_pPalette = (RGBQUAD*) (m_pDib + sizeof(BITMAPINFOHEADER));
	
	memcpy(m_pDibBitsback,m_pDibBits,m_Height*m_Width);
	return true;
}
/***********************************************************************
* �������ƣ�
* SaveFile()
*˵��������ͼ��
***********************************************************************/
bool CDib::SaveFile( const char* pFileName )
{
	if (m_pDib ==NULL) 
	{
		AfxMessageBox("This isn't a BMP File!",MB_OK);
		return false;
	}
	
	CFile file(pFileName, CFile::modeCreate|CFile::modeWrite);
	file.Write(&BFH,sizeof(BITMAPFILEHEADER));
	file.Write(m_pDib,m_Width*m_Height+PaletteEntries*sizeof(RGBQUAD)+sizeof(BITMAPINFOHEADER));
	
	file.Close();
	
	return true;
}
/***********************************************************************
* �������ƣ�
* Draw()
*˵��������ΪpDC��������ʾͼ��
***********************************************************************/
void CDib::Draw( CDC *pDC )
{
	if (m_pDib==NULL)
	{
		return;
	} 
	else
	{
		StretchDIBits(pDC->m_hDC,m_rtPosDest.left,m_rtPosDest.top,m_rtPosDest.Width(),m_rtPosDest.Height(),m_rtPosSour.left,m_rtPosSour.top,m_rtPosSour.Width(),m_rtPosSour.Height(),m_pDibBits,(BITMAPINFO *)m_pBIH,DIB_RGB_COLORS,SRCCOPY);
		/*StretchDIBits(pDC->m_hDC,0,0,wide,m_Height,0,0,wide,m_Height,m_pDibBits,(BITMAPINFO *)m_pBIH,BI_RGB,SRCCOPY);*/
	}
}
/***********************************************************************
* �������ƣ�
* Invert()
*˵����������ֵ������Ƭ
***********************************************************************/
void CDib::Invert()
{
	for (int i=0;i<m_Height;i++)
	{
		for (int j=0;j<m_Width;j++)
		{
			*(m_pDibBits+i*m_Width+j)=255-*(m_pDibBits+i*m_Width+j);
		}
	}
}
/***********************************************************************
* �������ƣ�
* Liner()
*˵��������Ϊԭͼ�ĵͻҶ�ֵ���任��ĵͻҶ�ֵ��ԭͼ�ĸ߻Ҷ�ֵ���任��ĸ߻Ҷ�ֵ��������ֵ�����Ա任
***********************************************************************/
void CDib::Liner( int X1,int Y1,int X2,int Y2 )
{
	double nSlope;
	nSlope = 0.0 + (Y2-Y1)/(X2-X1);
	for(int i=0; i< m_Height; i++)
	{
		for(int j=0; j< m_Width; j++)
		{
			if ((*(m_pDibBits+i*m_Width+j) <= X2 ) &&(*(m_pDibBits+i*m_Width+j) >=X1))
			{
				*(m_pDibBits+i*m_Width+j) =(BYTE) (nSlope * (*(m_pDibBits+i*m_Width+j) -X1)); 
			} 
			else if((*(m_pDibBits+i*m_Width+j) < X2 ) &&(*(m_pDibBits+i*m_Width+j) <X1))
			{
				*(m_pDibBits+i*m_Width+j) =0; 
			}
			else
			{
				*(m_pDibBits+i*m_Width+j) =255; 
			}
		}
	}
}
/***********************************************************************
* �������ƣ�
* GetGreyCountNumber()
*˵�����޲���������ֵΪÿ������ֵ�ĵ�ĸ���
***********************************************************************/
long* CDib::GetGreyCountNumber()
{
	long * pData=new long [256];
	memset(pData,0,256*sizeof(long));
	for (int i=0;i<m_Height;i++)
	{
		for (int j=0;j<wide;j+=3)
		{
			pData[(*(m_pDibBits+i*wide+j)+*(m_pDibBits+i*wide+j+1)+*(m_pDibBits+i*wide+j+2))/3]++;
		}
	}
	return pData;
}
/***********************************************************************
* �������ƣ�
* GetPaletteEntries()
*˵�����޲���������ֵΪʵ���õ�����ɫ�����Ҷ�ͼ��Ϊ256����ɫͼ��Ϊ0��ֱ��ͼ������256��
***********************************************************************/
long CDib::GetPaletteEntries()
{
	return 256;
}
/***********************************************************************
* �������ƣ�
* Histave()
*˵�����޲�������ͼ�����ֱ��ͼ����
***********************************************************************/
void CDib::Histave()
{
	int s,i,j,hist;
	double map[256];
	int sum,tmp;
	sum=0;
	hist=0;
	
	for (s=0;s<256;s++)
	{
		for (i=0;i<m_Height;i++)
		{
			for (j=0;j<m_Width;j++)
			{
				if (*(m_pDibBits+i*m_Width+j)==s)
				{
					hist++;
				}
			}
		}
		sum=hist+sum;
		map[s]=(double)sum*255/(m_Height*m_Width)+0.5;
		hist=0;
	}
	
	for (i=0;i<m_Height;i++)
	{
		for (j=0;j<m_Width;j++)
		{
			tmp=*(m_pDibBits+i*m_Width+j);
			*(m_pDibBits+i*m_Width+j)=(int)map[tmp];
		}
	}
}
/***********************************************************************
* �������ƣ�
* PseuColor()
*˵����α��ʵ��
***********************************************************************/
void CDib::PseuColor()
{	
	LPBYTE m_Palette2;						//������ɫ��
	m_Palette2=new BYTE[256*4];	
	m_Palette2=(LPBYTE)m_pBIH+40;				//ָ���ɫ����λ
	for(int i=0;i<256;i++){
		if (i<50)
		{
			*(m_Palette2+i*4)=50-i;				//b
			*(m_Palette2+i*4+1)=255-i;			//g
			*(m_Palette2+i*4+2)=i;				//r
	    	*(m_Palette2+i*4+3)=i;
		}else if(i>200)
		{
			*(m_Palette2+i*4)=255-0.8*i;			//b	
			*(m_Palette2+i*4+1)=255-i;		        //g
			*(m_Palette2+i*4+2)=i;			        //r
	    	*(m_Palette2+i*4+3)=i;
		}else
		{
			*(m_Palette2+i*4)=i;				//B
			*(m_Palette2+i*4+1)=i;				//G
			*(m_Palette2+i*4+2)=i;				//R
	    	*(m_Palette2+i*4+3)=i;
		}
	
	}
}
/***********************************************************************
* �������ƣ�
* Colortogrey()
*˵��������ɫͼ���Ϊ�Ҷ�ͼ����ʾ�����ı��ɫ�壩
***********************************************************************/
void CDib::Colortogrey()
{
	int i,j,k;	
	//�Ҷȱ任
	for(i=0;i<m_Height;i++)
		for(j=0;j<wide;j++)
		{
			BYTE b=*(m_pDibBits+i*m_Width+j*pixelbyte);
			BYTE g=*(m_pDibBits+i*m_Width+j*pixelbyte+1);
			BYTE r=*(m_pDibBits+i*m_Width+j*pixelbyte+2);
			BYTE value =(BYTE)(0.299 * r   +   0.587 * g   +   0.114 * b);
			
			for(k=0;k<pixelbyte;k++)
			{
				*(m_pDibBits+i*m_Width+j*pixelbyte+k)=value;
			}
		}
}
/***********************************************************************
* �������ƣ�
* AddPepperSaltNoise()
*˵�����޲��������뽷������
***********************************************************************/
void CDib::AddPepperSaltNoise()
{
	memcpy (m_pDibBitsout , m_pDibBits , m_Width * m_Height);
	int bytecount,i;
	bytecount = m_Width*m_Height; //��ȡλͼ���������ֽ���

	if (PaletteEntries!=0)      //����Ҷ�ͼ��
	{  
        for (i=0;i<bytecount;i++)
        {
			if(rand()>32000)  m_pDibBitsout[i]=0;
			if(rand()<200)    m_pDibBitsout[i]=255;
        }
	}
	else	                 //24λ���ɫͼ��
	{
        for (i=0;i<bytecount;i=i+3)
        {
			int num=rand();
            if (num>32000) 
			{
				m_pDibBitsout[i]=(rand())%255;        //����ÿһ�����ص�RGBֵ
				m_pDibBitsout[i+1]=(rand())%255;
				m_pDibBitsout[i+2]=(rand())%255;
			}
			if (num<200) 
			{
				m_pDibBitsout[i]=(rand())%255;
				m_pDibBitsout[i+1]=(rand())%255;
				m_pDibBitsout[i+2]=(rand())%255;
			}
        }
	}
	//�����µ�DIBλͼ
	for(i=0;i<m_Height;i++)
	{
		for(int j=0;j<m_Width;j++)
			*(m_pDibBits+i*m_Width+j)= *(m_pDibBitsout+i*m_Width+j);
	}
}
/***********************************************************************
* �������ƣ�
* AddRandomNoise()
*˵�����޲����������������
***********************************************************************/
void CDib::AddRandomNoise()
{
	memcpy (m_pDibBitsout , m_pDibBits , m_Width * m_Height);
	int bytecount,i;
	bytecount = m_Width*m_Height; //��ȡλͼ���������ֽ���
    for( i=0;i<bytecount;i++)
	{
		LONG Temp=rand();
		Temp = m_pDibBitsout[i]*224/256+Temp/512;
		m_pDibBitsout[i] = Temp >= 255 ? 255 : Temp;
    }
	//�����µ�DIBλͼ
	for(i=0;i<m_Height;i++)
	{
		for(int j=0;j<m_Width;j++)
			*(m_pDibBits+i*m_Width+j)= *(m_pDibBitsout+i*m_Width+j);
	}
}
/***********************************************************************
* �������ƣ�
* Lapchange()
*˵�����޲�������ͼ�����������˹��
***********************************************************************/
void CDib::Lapchange()
{
	memcpy(m_pDibBitsout,m_pDibBits,m_Width*m_Height);	
	int pr,pg,pb;     //B G R��ͨ��
	for(int i=1;i<m_Height-1;i++)
		for(int j=1;j<wide-1;j++)
		{
			//    3*3ģ��
			pb=5*m_pDibBits[i*m_Width+j*pixelbyte]-m_pDibBits[i*m_Width+(j-1)*pixelbyte]-m_pDibBits[i*m_Width+(j+1)*pixelbyte]-m_pDibBits[(i-1)*m_Width+j*pixelbyte]-m_pDibBits[(i+1)*m_Width+j*pixelbyte];
			pg=5*m_pDibBits[i*m_Width+j*pixelbyte+1]-m_pDibBits[i*m_Width+(j-1)*pixelbyte+1]-m_pDibBits[i*m_Width+(j+1)*pixelbyte+1]-m_pDibBits[(i-1)*m_Width+j*pixelbyte+1]-m_pDibBits[(i+1)*m_Width+j*pixelbyte+1];	
			pr=5*m_pDibBits[i*m_Width+j*pixelbyte+2]-m_pDibBits[i*m_Width+(j-1)*pixelbyte+2]-m_pDibBits[i*m_Width+(j+1)*pixelbyte+2]-m_pDibBits[(i-1)*m_Width+j*pixelbyte+2]-m_pDibBits[(i+1)*m_Width+j*pixelbyte+2];
			
			if(pb<0)m_pDibBitsout[i*m_Width+j*pixelbyte]=0;
			else  if(pb>255)m_pDibBitsout[i*m_Width+j*pixelbyte]=255;
			else m_pDibBitsout[i*m_Width+j*pixelbyte]=pb;
			
			
			if(pg<0)m_pDibBitsout[i*m_Width+j*pixelbyte+1]=0;
			else if(pg>255)m_pDibBitsout[i*m_Width+j*pixelbyte+1]=255;
			else m_pDibBitsout[i*m_Width+j*pixelbyte+1]=pg;
			
			if(pr<0)m_pDibBitsout[i*m_Width+j*pixelbyte+2]=0;
			else if(pr>255)m_pDibBitsout[i*m_Width+j*pixelbyte+2]=255;
			else m_pDibBitsout[i*m_Width+j*pixelbyte+2]=pr;
		}
	memcpy(m_pDibBits,m_pDibBitsout,m_Width*m_Height);
}
/***********************************************************************
* �������ƣ�
* GarSharp()
*˵�����޲�������ͼ������ݶ���
***********************************************************************/
void CDib::GradSharp()
{
	memcpy (m_pDibBitsout , m_pDibBits , m_Width * m_Height);
	unsigned char bThre=30;
	unsigned char*	pSrc;       // ָ��Դͼ���ָ��
	unsigned char*	pDst; 	
	unsigned char*	pSrc1;
	unsigned char*	pSrc2;	
	LONG	i,j;				// ѭ������
	int	bTemp;
	int m_nBitCount=m_pBIH->biBitCount;
	if(m_nBitCount != 8)
	{
		AfxMessageBox("ֻ�ܴ���8λ�Ҷ�ͼ��!");
		return ;
	}
	//����Ҫ���Ƶ�ͼ������
	
	for(i = 0; i < m_Height; i++)		// ÿ��
	{		
		for(j = 0; j < m_Width; j++)		// ÿ��
		{
			//ָ����DIB��i�е�j�е����ص�ָ��
			pDst = m_pDibBits + m_Width * (m_Height -1 - i) + j;
			
			// �����ݶ�����
			// ָ��DIB��i�У���j�����ص�ָ��
			pSrc  = (unsigned char*)m_pDibBitsout + m_Width * (m_Height - 1 - i) + j;			
			// ָ��DIB��i+1�У���j�����ص�ָ��
			pSrc1 = (unsigned char*)m_pDibBitsout + m_Width * (m_Height - 2 - i) + j;			
			// ָ��DIB��i�У���j+1�����ص�ָ��
			pSrc2 = (unsigned char*)m_pDibBitsout + m_Width * (m_Height - 1 - i) 
				+ j + 1;
			
			bTemp = abs((*pSrc)-(*pSrc1)) + abs((*pSrc)-(*pSrc2));
			
			// �ж��Ƿ�С����ֵ
			if ((bTemp+120) < 255)
			{
				// �ж��Ƿ������ֵ������С��������Ҷ�ֵ���䡣
				if (bTemp >= bThre)
				{
					*pSrc = bTemp + 120;
				}
			}
			else
			{
				*pSrc = 255;
			}
			//�����µ�DIB����ֵ
			*pDst = *pSrc;
		}
	}
}
/***********************************************************************
* �������ƣ�
* Midchange()
*˵�����޲�������ͼ����ֵ�˲�
***********************************************************************/
void CDib::Midchange()
{
	memcpy (m_pDibBitsout , m_pDibBits , m_Width * m_Height);
	int middle_temp[9],midp;
	int i,j;
	int xx, yy, m;
	int chg,medi;
	for (i = 1; i < m_Height; i++)//��
	{
		for(j = 1; j < wide; j++)//��
		{
			for (int k=0;k<pixelbyte;k++)
			{
				m=0;
				for(xx= i-1; xx<= i+1; xx++)
					for(yy=j-1; yy<=j+1; yy++)
					{
						middle_temp[m]=m_pDibBits [xx*m_Width+yy*pixelbyte+k];
						m++;
					}//��
					do{
						chg=0;
						for(m=0 ; m<8 ;m++)
						{
							if(middle_temp[m]<middle_temp[m+1])
							{
								midp=middle_temp[m];
								middle_temp[m]=middle_temp[m+1];
								middle_temp[m+1]=midp;
								chg=1;
							}
						}
					}while(chg==1);
					medi = middle_temp[4];
				*(m_pDibBitsout+i*m_Width+j*pixelbyte+k)=medi;
			}		
		}
	}
	memcpy(m_pDibBits ,m_pDibBitsout,m_Width*m_Height);
}
/***********************************************************************
* �������ƣ�
* Pinghua()
*˵�����޲�������ͼ��ƽ���˲�
***********************************************************************/
void CDib::Pinghua()
{
	memcpy (m_pDibBitsout , m_pDibBits , m_Width* m_Height);
	int i,j,m,n;
	long temp;
	for (i=1;i<m_Height-1;i++)
	{
		for (j=1;j<wide-1;j++)
		{
			for (int k=0;k<pixelbyte;k++)
			{
				temp=0;
				for (m=-1;m<=1;m++)
				{
					for (n=-1;n<=1;n++)
					{
						temp=temp+*(m_pDibBits+(i+m)*m_Width+(j+n)*pixelbyte+k);
					}
				}
				temp=temp/9;
		    	*(m_pDibBitsout+i*m_Width+j*pixelbyte+k)=temp;
			}
		}
	}
	
	memcpy(m_pDibBits,m_pDibBitsout,m_Width*m_Height);
}
/***********************************************************************
* �������ƣ�
* Pingyi()
*˵��������ΪX��ƫ������Y��ƫ��������ͼ��ƽ��
***********************************************************************/
void CDib::Pingyi( int offsetX,int offsetY )
{
	memcpy(m_pDibBitsout,m_pDibBits,m_Height*m_Width);
	memset(m_pDibBits,255,m_Height*m_Width);
	if (PaletteEntries!=0)
	{
		for (int i=0;i<m_Heightout;i++)
		{
			for (int j=0;j<m_Widthout;j++)
			{
				if (i-offsetY>=0&&i-offsetY<m_Heightout&&j-offsetX>=0&&j-offsetX<m_Widthout)
				{
					*(m_pDibBits+i*m_Width+j)=*(m_pDibBitsout+(i-offsetY)*m_Widthout+(j-offsetX));
				}
				
			}
		}
	} 
	else
	{
		for (int i=0;i<m_Heightout;i++)
		{
			for (int j=0;j<wide;j++)
			{
				for (int k=0;k<3;k++)
				{
					if (i-offsetY>=0&&i-offsetY<m_Heightout&&j-offsetX>=0&&j-offsetX<wide)
					{
						*(m_pDibBits+i*m_Width+j*3+k)=*(m_pDibBitsout+(i-offsetY)*m_Widthout+(j-offsetX)*3+k);
					}
				}
			}
		}
	}
}
/***********************************************************************
* �������ƣ�
* Xuanzhuan()
*˵��������Ϊѡ��Ƕȣ������ڽ���ֵ�ķ�����תͼ��
***********************************************************************/
bool CDib::Xuanzhuan( int angle )
{
	int newwidth,newheight;
	unsigned char *lp;
	unsigned char *lpsrc,*lpdst;
	BITMAPINFOHEADER *lpinfor;
	double arcangle;
	float fsin,fcos;
	int linebytes,OldLineBytes;
	float num1,num2;
	
	int i, j, i0, j0;
	float SrcX1,SrcY1,SrcX2,SrcY2,SrcX3,SrcY3,SrcX4,SrcY4;
	float DstX1,DstY1,DstX2,DstY2,DstX3,DstY3,DstX4,DstY4;
	
	SrcX1 = (float)(-(wide-1)/2);
	SrcY1 = (float)((m_Height-1)/2);
	SrcX2 = (float)((wide-1)/2);
	SrcY2 = (float)((m_Height-1)/2);
	SrcX3 = (float)(-(wide-1)/2);
	SrcY3 = (float)(-(m_Height-1)/2);
	SrcX4 = (float)((wide-1)/2);
	SrcY4 = (float)(-(m_Height-1)/2);
	
	arcangle=(angle*3.14)/180.0;
	fsin = (float) sin(arcangle);
	fcos = (float) cos(arcangle);
	
	DstX1 = fcos * SrcX1 + fsin * SrcY1;
	DstY1 = - fsin * SrcX1 + fcos * SrcY1;
	DstX2 = fcos * SrcX2 + fsin * SrcY2;
	DstY2 = - fsin * SrcX2 + fcos * SrcY2;
	DstX3 = fcos * SrcX3 + fsin * SrcY3;
	DstY3 = - fsin * SrcX3 + fcos * SrcY3;
	DstX4 = fcos * SrcX4 + fsin * SrcY4;
	DstY4 = - fsin * SrcX4 + fcos * SrcY4;
	
	newwidth = (int) (max(fabs(DstX4-DstX1) , fabs(DstX3-DstX2) ) + 0.5);
	newheight = (int) (max(fabs(DstY4-DstY1) , fabs(DstY3-DstY2) ) + 0.5);
	
	m_rtPosDest.SetRect(CPoint(0,0),CPoint(newwidth,newheight));
	m_rtPosSour = m_rtPosDest;
	
	
	num1=(float)(-0.5* newwidth *fcos -0.5 * newheight *fsin + 0.5 * wide);
	num2=(float)(0.5* newwidth *fsin -0.5 * newheight *fcos + 0.5 * m_Height);
	
	linebytes = (newwidth*8*pixelbyte+31)/32*4;
	OldLineBytes = (wide*8*pixelbyte+31)/32*4;
	
	unsigned char *lpDib = new unsigned char [linebytes*newheight+sizeof(BITMAPINFOHEADER)+ PaletteEntries*sizeof(RGBQUAD)];
	if (lpDib == NULL){
		return(FALSE);}
	memcpy(lpDib,m_pDib,sizeof(BITMAPINFOHEADER)+ PaletteEntries*sizeof(RGBQUAD));
	
	lpinfor = (BITMAPINFOHEADER*) lpDib;
	lpinfor->biWidth= newwidth;
	lpinfor->biHeight= newheight;
	
	lp = lpDib+sizeof(BITMAPINFOHEADER)+ PaletteEntries*sizeof(RGBQUAD);
	if (lp==NULL){
		return (FALSE);}
	lp = new unsigned char [linebytes*newheight];
	memset(lp , (BYTE)255 , linebytes*newheight);
	for (i = 0; i < newheight; i++)
	{
		for (j = 0; j < newwidth; j++)
		{
			for(int k=0;k<pixelbyte;k++)
			{
				lpdst=(unsigned char*)lp+(newheight-1-i)*linebytes+j*pixelbyte+k;
				i0=(long)(((float)i)*fcos-1.0f*((float) j)*fsin+num2+0.5);
				j0=(long)(((float)i)*fsin+((float) j)*fcos+num1+0.5);
				if((i0>=0)&&(i0<wide)&&(j0>=0)&&(j0<m_Height))
				{
					lpsrc=(unsigned char*)m_pDibBits+OldLineBytes*(m_Height-1-i0)+j0*pixelbyte+k;
					*lpdst=*lpsrc;
				}
			}
		}
	}
	
	delete []m_pDib;
	m_pDib = lpDib;
	m_pDibBits = lp;
	m_pBIH = lpinfor;
	m_pBIH->biSizeImage = linebytes* newheight;
	wide = lpinfor->biWidth;
	m_Width=linebytes;
	m_Height = lpinfor->biHeight;
	return TRUE;
}
/***********************************************************************
* �������ƣ�
* Xuanzhuan2()
*˵��������Ϊ��ת�Ƕȣ�����˫���Բ�ֵ������תͼ��
***********************************************************************/
bool CDib::Xuanzhuan2( int angle )
{
		int newwidth,newheight;
	unsigned char *lp;
	BITMAPINFOHEADER *lpinfor;
	double arcangle;
	float fsin,fcos;
	int linebytes,OldLineBytes;
	float num1,num2;
	unsigned char array[2][2];
	
	int i, j;
	float SrcX1,SrcY1,SrcX2,SrcY2,SrcX3,SrcY3,SrcX4,SrcY4;
	float DstX1,DstY1,DstX2,DstY2,DstX3,DstY3,DstX4,DstY4;
	
	SrcX1 = (float)(-(wide-1)/2);
	SrcY1 = (float)((m_Height-1)/2);
	SrcX2 = (float)((wide-1)/2);
	SrcY2 = (float)((m_Height-1)/2);
	SrcX3 = (float)(-(wide-1)/2);
	SrcY3 = (float)(-(m_Height-1)/2);
	SrcX4 = (float)((wide-1)/2);
	SrcY4 = (float)(-(m_Height-1)/2);
	
	arcangle=(angle*3.14)/180.0;
	fsin = (float) sin(arcangle);
	fcos = (float) cos(arcangle);
	
	DstX1 = fcos * SrcX1 + fsin * SrcY1;
	DstY1 = - fsin * SrcX1 + fcos * SrcY1;
	DstX2 = fcos * SrcX2 + fsin * SrcY2;
	DstY2 = - fsin * SrcX2 + fcos * SrcY2;
	DstX3 = fcos * SrcX3 + fsin * SrcY3;
	DstY3 = - fsin * SrcX3 + fcos * SrcY3;
	DstX4 = fcos * SrcX4 + fsin * SrcY4;
	DstY4 = - fsin * SrcX4 + fcos * SrcY4;
	
	newwidth = (int) (max(fabs(DstX4-DstX1) , fabs(DstX3-DstX2) ) + 0.5);
	newheight = (int) (max(fabs(DstY4-DstY1) , fabs(DstY3-DstY2) ) + 0.5);
	
	m_rtPosDest.SetRect(CPoint(0,0),CPoint(newwidth,newheight));
	m_rtPosSour = m_rtPosDest;
	
	
	//num1=(float)(-0.5* newwidth *fcos -0.5 * newheight *fsin + 0.5 * wide);
	//num2=(float)(0.5* newwidth *fsin -0.5 * newheight *fcos + 0.5 * m_Height);

	num1 = (float) (-0.5 * (newwidth - 1) * fcos 
		+ 0.5 * (newheight - 1) * fsin + 0.5 * (wide  - 1));
	num2 = (float) (-0.5 * (newwidth - 1) * fsin
		                - 0.5 * (newheight - 1) * fcos + 0.5 * (m_Height - 1));
	
	linebytes = (newwidth*8*pixelbyte+31)/32*4;
	OldLineBytes = (wide*8*pixelbyte+31)/32*4;
	
	unsigned char *lpDib = new unsigned char [linebytes*newheight+sizeof(BITMAPINFOHEADER)+ PaletteEntries*sizeof(RGBQUAD)];
	if (lpDib == NULL){
		return(FALSE);}
	memcpy(lpDib,m_pDib,sizeof(BITMAPINFOHEADER)+ PaletteEntries*sizeof(RGBQUAD));
	
	lpinfor = (BITMAPINFOHEADER*) lpDib;
	lpinfor->biWidth= newwidth;
	lpinfor->biHeight= newheight;
	
	lp = lpDib+sizeof(BITMAPINFOHEADER)+ PaletteEntries*sizeof(RGBQUAD);
	if (lp==NULL){
		return (FALSE);}
	lp = new unsigned char [linebytes*newheight];
	memset(lp , (BYTE)255 , linebytes*newheight);

    for(i = 0; i < newheight; i++)
	{
		for(j = 0; j < newwidth; j++)
		{		
			// ���ͼ������(j,i)ӳ�䵽����ͼ�������
			float coordinateX = j * fcos - i * fsin + num1;
			float coordinateY = j * fsin + i * fcos + num2;

			//������ȡ��
			int Iu=(int)coordinateX;
			int Iv=(int)coordinateY;

			// �ж��Ƿ���ԭͼ��Χ��
			if( (coordinateX >= 0) && (coordinateX < wide-1) && (coordinateY >= 0) 
				&& (coordinateY < m_Height-1))
			{
				//��ͼ��ÿ��ͨ�������ݽ��зֱ��ֵ����ɫͼ��pixelByteΪ3��
				//�Ҷ�ͼ��pixelByteΪ1
				for(int k=0;k<pixelbyte;k++){
					//����ǰ����������Χ2*2�������ؿ���������array
					array[0][0]=*(m_pDibBits+ Iv*OldLineBytes+ Iu*pixelbyte+k);
					array[0][1]=*(m_pDibBits+ Iv*OldLineBytes+ (Iu+1)*pixelbyte+k);
					array[1][0]=*(m_pDibBits+ (Iv+1)*OldLineBytes+ Iu*pixelbyte+k);
					array[1][1]=*(m_pDibBits+ (Iv+1)*OldLineBytes+ (Iu+1)*pixelbyte+k);
					*(lp + i * linebytes + j*pixelbyte+k)
						=interpolationDblLinear(array, coordinateX-Iu, coordinateY-Iv);
				}
			}
			else
			{
				// ���ڲ���ԭͼ��Χ�ڵ������أ���ֵΪ255
				for(int k=0;k<pixelbyte;k++)
					*(lp+i*linebytes+j*pixelbyte+k) = 255;
			}
			
		}
		
	}
	
	delete []m_pDib;
	m_pDib = lpDib;
	m_pDibBits = lp;
	m_pBIH = lpinfor;
	m_pBIH->biSizeImage = linebytes* newheight;
	wide = lpinfor->biWidth;
	m_Width=linebytes;
	m_Height = lpinfor->biHeight;
	return TRUE;
}
/***********************************************************************
* �������ƣ�
* Suoxiao()
*˵��������ΪX�����ű�����Y�����ű����������ڽ���ֵ��������ͼ��
***********************************************************************/
bool CDib::Suoxiao( float xratio,float yratio )
{
	m_rtPosDest.SetRect(CPoint(0,0),CPoint(m_Width*xratio,m_Height*yratio));
	m_rtPosSour = m_rtPosDest;
	
	long newwidth = (LONG) (wide * xratio +0.5);
	long newheight = (LONG) (m_Height * yratio+0.5);
	long linebytes = (newwidth*8*pixelbyte+31)/32*4;
	long OldLineBytes = (wide*8*pixelbyte+31)/32*4;
	unsigned char *lpSrc,*lpDst;
	unsigned char *lp;
	BITMAPINFOHEADER *lpinfor;
	int i , j , i0 , j0;
	
	unsigned char *lpDib = new unsigned char [linebytes*newheight+sizeof(BITMAPINFOHEADER)+ PaletteEntries*sizeof(RGBQUAD)];
	if (lpDib == NULL){
		return(FALSE);}
	memcpy(lpDib,m_pDib,sizeof(BITMAPINFOHEADER)+ PaletteEntries*sizeof(RGBQUAD));
	
	lpinfor = (BITMAPINFOHEADER*) lpDib;
	lpinfor->biWidth= newwidth;
	lpinfor->biHeight= newheight;
	
	lp = lpDib+sizeof(BITMAPINFOHEADER)+ PaletteEntries*sizeof(RGBQUAD);
	if (lp==NULL){
		return (FALSE);}
	lp = new unsigned char [linebytes*newheight];
	memset(lp , (BYTE)255 , linebytes*newheight);
	
	for(j = 0; j < newheight - 1; j++)
	{
		for(i = 0; i < newwidth - 1; i++)
		{	
			for(int k=0;k<pixelbyte;k++){
				lpDst = lp + linebytes * j + i*pixelbyte+k;    
				i0=(int)(i / xratio + 0.5);
				j0=(int)(j / yratio + 0.5);	
				if( (i0 >= 0) && (i0 < wide) && (j0 >= 0) && (j0 < m_Height))
				{
					lpSrc=m_pDibBits + OldLineBytes * j0 + i0*pixelbyte+k;	
					*lpDst=*lpSrc;
				}
				else
				{
					*lpDst=255;
				}
		}	}
	} 
	
	delete []m_pDib;
	m_pDib = lpDib;
	m_pDibBits = lp;
	m_pBIH = lpinfor;
	m_pBIH->biSizeImage = linebytes* newheight;
	wide = lpinfor->biWidth;
	m_Width=linebytes;
	m_Height = lpinfor->biHeight;
	return TRUE;
}
/***********************************************************************
* �������ƣ�
* Suoxiao2()
*˵��������ΪX�����ű�����Y�����ű���������˫���Բ�ֵ��������ͼ��
***********************************************************************/
bool CDib::Suoxiao2( float xratio,float yratio )
{
	m_rtPosDest.SetRect(CPoint(0,0),CPoint(m_Width*xratio,m_Height*yratio));
	m_rtPosSour = m_rtPosDest;
	
	unsigned char array[2][2];
	long newwidth = (LONG) (wide * xratio +0.5);
	long newheight = (LONG) (m_Height * yratio+0.5);
	long linebytes = (newwidth*8*pixelbyte+31)/32*4;
	long OldLineBytes = (wide*8*pixelbyte+31)/32*4;
	unsigned char *lp;
	BITMAPINFOHEADER *lpinfor;
	int i , j ;
	
	unsigned char *lpDib = new unsigned char [linebytes*newheight+sizeof(BITMAPINFOHEADER)+ PaletteEntries*sizeof(RGBQUAD)];
	if (lpDib == NULL){
		return(FALSE);}
	memcpy(lpDib,m_pDib,sizeof(BITMAPINFOHEADER)+ PaletteEntries*sizeof(RGBQUAD));
	
	lpinfor = (BITMAPINFOHEADER*) lpDib;
	lpinfor->biWidth= newwidth;
	lpinfor->biHeight= newheight;
	
	lp = lpDib+sizeof(BITMAPINFOHEADER)+ PaletteEntries*sizeof(RGBQUAD);
	if (lp==NULL){
		return (FALSE);}
	lp = new unsigned char [linebytes*newheight];
	memset(lp , (BYTE)255 , linebytes*newheight);
	for(i=0; i< newheight; i++){
		for(j=0; j< newwidth ; j++){   
			
			//���ͼ������Ϊ(j,i)������ӳ�䵽ԭͼ�е�����ֵ������ֵλ��
			float coordinateX=j/xratio;
			float coordinateY=i/yratio;
			
			//�Բ�ֵλ������ȡ��
			int Iu=(int)coordinateX;
			int Iv=(int)coordinateY;
			
			//����ֵλ��������ͼ��Χ�ڣ���˫���Բ�ֵ
			if(0<=coordinateX&&coordinateX<wide
				&& coordinateY>=0&&coordinateY<m_Height){
				//��ͼ��ÿ������ͨ�������ݽ��зֱ��ֵ��
				//��ͼpixelByteΪ3���Ҷ�ͼ��pixelByteΪ1
				for(int k=0;k<pixelbyte;k++){
					//����k��ͨ�����ĸ��������ݿ�����array������
					array[0][0]=*(m_pDibBits+ Iv*OldLineBytes+ Iu*pixelbyte+k);
					array[0][1]=*(m_pDibBits+ Iv*OldLineBytes+ (Iu+1)*pixelbyte+k);
					array[1][0]=*(m_pDibBits+ (Iv+1)*OldLineBytes+ Iu*pixelbyte+k);
					array[1][1]=*(m_pDibBits+ (Iv+1)*OldLineBytes+ (Iu+1)*pixelbyte+k);
					
					//����˫���Բ�ֵ������ֵ�������pImgOut��
					*(lp+ i * linebytes + j*pixelbyte+k)
						=interpolationDblLinear(array, coordinateX-Iu, coordinateY-Iv);
				}
			}
			else{//��Ե���ز��ý��ڲ�ֵ
				for(int k=0;k<pixelbyte;k++)
					*(lp + i * linebytes + j*pixelbyte + k) =
					*(m_pDibBits+ Iv*OldLineBytes+ Iu*pixelbyte+k);
			}
			
		}
	}
	
	delete []m_pDib;
	m_pDib = lpDib;
	m_pDibBits = lp;
	m_pBIH = lpinfor;
	m_pBIH->biSizeImage = linebytes* newheight;
	wide = lpinfor->biWidth;
	m_Width=linebytes;
	m_Height = lpinfor->biHeight;
	return TRUE;
}
/***********************************************************************
* �������ƣ�
* Zoom()
*˵����ͨ��������ʵ��ͼ�������
***********************************************************************/
void CDib::Zoom( float ratio )
{
	if (ratio >=0)
	{
		m_rtPosDest.InflateRect((long)(m_rtPosDest.Width() * ratio),(long)(m_rtPosDest.Height() * ratio));
	} else
	{
		long x = (long)(m_rtPosDest.Width() * fabs(ratio));
		long y = (long)(m_rtPosDest.Height() * fabs(ratio));
		m_rtPosDest.DeflateRect(x,y);
	}
}
/***********************************************************************
* �������ƣ�
* interpolationDblLinear()
*˵��������:   array��2*2���飬���ͼ�����ֵλ����Χ�ĸ����ػҶ�ֵ
               xpos��yposΪ����������ĸ����صĲ�ֵλ�����꣬ȡС��
       ����ֵ: ��ֵ�����
       �ú�����ԭͼ�����ֵλ�õ��ĸ��������ݿ���������array�У����ݲ�ֵλ�������
    Ӧ��array��λ�ã�������array���ĸ�Ԫ�ؽ���˫���Բ�ֵ������ֵΪ��ֵ�Ľ����
***********************************************************************/
unsigned char CDib::interpolationDblLinear( unsigned char array[2][2], float xpos, float ypos )
{
	//�����м����
	int t1, t2;	
	//�ȴ�ֱ�������Բ�ֵ
	t1 = (1-ypos)*array[0][0] + ypos*array[1][0];
	t2 = (1-ypos)*array[0][1] + ypos*array[1][1];	
	//��ˮƽ�������Բ�ֵ
	float t=(int)((1-xpos)*t1 + xpos*t2);	
	//����ֵ���С��0�������0
	if(t<0)
		t=0;	
	//����ֵ�������255�������255
	if(t>255) 
		t=255;	
	//��ֵ�������
	return (unsigned char)t;
}
/***********************************************************************
* �������ƣ�
* MirrorVerti()
*˵������ͼ��ֱ����
***********************************************************************/
void CDib::MirrorHori()
{
	memcpy(m_pDibBitsout,m_pDibBits,m_Width*m_Height);
	for(int i=0;i<m_Height;i++)
	{
		for(int j=0;j<wide;j++)
		{
			for(int k=0;k<pixelbyte;k++)
				*(m_pDibBits+i*m_Width+j*pixelbyte+k)
				=*(m_pDibBitsout+i*m_Widthout+(wide-1-j)*pixelbyte+k);
		}
	}
}
/***********************************************************************
* �������ƣ�
* MirrorVerti()
*˵������ͼ��ֱ����
***********************************************************************/
void CDib::MirrorVerti()
{
	memcpy(m_pDibBitsout,m_pDibBits,m_Width*m_Height);
	for(int i=0;i<m_Height;i++)
	{
		for(int j=0;j<wide;j++)
		{
			for(int k=0;k<pixelbyte;k++)
				*(m_pDibBits+i*m_Width+j*pixelbyte+k)
				=*(m_pDibBitsout+(m_Height-1-i)*m_Widthout+j*pixelbyte+k);
		}
	}
}
/***********************************************************************
* �������ƣ�
* PointInRect()
*˵����ȷ��ָ���ĵ��Ƿ���CRect֮��
***********************************************************************/
bool CDib::PointInRect( CPoint point )
{
	return m_rtPosDest.PtInRect(point);
}
/***********************************************************************
* �������ƣ�
* SetDestRect()
*˵������CRect�ƶ���ָ����ƫ��
***********************************************************************/
void CDib::SetDestRect( CSize SizeOffset )
{
	m_rtPosDest.OffsetRect(SizeOffset);
}
/***********************************************************************
* �������ƣ�
* Erzhi()
*˵������ͼ���ֵ��
***********************************************************************/
void CDib::Erzhi()
{
	int T=FindT();   //TֵԼΪ205
	long sum;
	for (int i=0;i<m_Height;i++)
	{
		for (int j=0;j<wide;j++)
		{
			sum=0;
			for (int k=0;k<3;k++)
			{
				sum=sum+*(m_pDibBits+i*m_Width+j*3+k);
			}
			sum=sum/3;
			//С����ֵ���ú�
			if (sum<T)
			{
				for (int k=0;k<3;k++)
				{
					*(m_pDibBits+i*m_Width+j*3+k)=0;
				}				
			}
			//������ֵ���ð�
			else
			{
				for (int k=0;k<3;k++)
				{
					*(m_pDibBits+i*m_Width+j*3+k)=255;
				}
			}
		}
	}
}
/***********************************************************************
* �������ƣ�
* FindT()
*˵�����ҵ���ֵ���ڶ�ֵ��
***********************************************************************/
int CDib::FindT()
{
	int T=127;
	int T1;
	long T0=10;
	long sum=0;
	long sum1=0;
	long sum2=0;
	int  num1=0;
	int  num2=0;
	long u1,u2;
	while(T0>1)
	{
		for (int i=0;i<m_Height;i++)
		{
			for(int j=0;j<wide;j++)
			{
				sum=0;
				for (int k=0;k<3;k++)
				{
					sum=sum+*(m_pDibBits+i*m_Width+j*3+k);
				}
				sum=sum/3;
				if (sum<T)
				{
					sum1=sum1+sum;
					num1++;
				}
				else
				{
					sum2=sum2+sum;
					num2++;
				}
			}
		}
		u1=sum1/num1;
		u2=sum2/num2;
		T1=(u1+u2)/2;
		T0=abs(T-T1);
		T=T1;
	}
	return T;
}
/***********************************************************************
* �������ƣ�
* Fushi()
*˵������ͼ����и�ʴ����
***********************************************************************/
void CDib::Fushi()
{
	//�԰�ɫΪ����
	int i,j;
	memcpy(m_pDibBitsout,m_pDibBits,m_Width*m_Height);
	for (i=3;i<m_Height-3;i++)
	{
		for (j=3;j<wide-3;j++)
		{
			for (int k=0;k<pixelbyte;k++)
			{
				for(int m=i-3;m<i+4;m++)
				{
					for (int n=j-3;n<j+4;n++)
					{
						if (*(m_pDibBitsout+m*m_Width+n*pixelbyte+k)==255)
						{
							*(m_pDibBits+i*m_Width+j*pixelbyte+k)=255;
						}
					}
				}
				
			}
		}
	} 
//7*7ģ��
}
/***********************************************************************
* �������ƣ�
* Pengzhang()
*˵������ͼ��������ʹ���
***********************************************************************/
void CDib::Pengzhang()
{
	//�԰�ɫΪ����
	int i,j;
	memcpy(m_pDibBitsout,m_pDibBits,m_Width*m_Height);
	for (i=3;i<m_Height-3;i++)
	{
		for (j=3;j<wide-3;j++)
		{
			for (int k=0;k<pixelbyte;k++)
			{
				for(int m=i-3;m<i+4;m++)
				{
					for (int n=j-3;n<j+4;n++)
					{
						if (*(m_pDibBitsout+m*m_Width+n*pixelbyte+k)==0)
						{
							*(m_pDibBits+i*m_Width+j*pixelbyte+k)=0;
						}
					}
				}
				
			}
		}
	} 
//7*7ģ��
}
/***********************************************************************
* �������ƣ�
* TiquRobert()
*˵��������Robert���ӽ��б߽���ȡ
***********************************************************************/
void CDib::TiquRobert()
{
	int i,j,k;
	int x, y, t;
	memcpy(m_pDibBitsout,m_pDibBits,m_Width*m_Height);
	for(i=1;i<m_Height-1;i++)
	{
		for(j=1;j<wide-1;j++)
		{
			for(k=0;k<pixelbyte;k++)
			{
				//������ģ��
				//x�����ݶ�
				x=*(m_pDibBits+i*m_Width+j*pixelbyte+k)
					-*(m_pDibBits+(i+1)*m_Width+j*pixelbyte+k);
				
				//y�����ݶ�
				y=*(m_pDibBits+i*m_Width+j*pixelbyte+k)
					-*(m_pDibBits+i*m_Width+(j+1)*pixelbyte+k);
				
				t=sqrt(x*x+y*y)+0.5;
				if(t>255)
					t=255;
				*(m_pDibBitsout+i*m_Width+j*pixelbyte+k)=t;
			}
		}
	}
	memcpy(m_pDibBits,m_pDibBitsout,m_Width*m_Height);
}
/***********************************************************************
* �������ƣ�
* Jiabianjie()
*˵�������߽�ӵ�ԭͼ
***********************************************************************/
void CDib::Jiabianjie()
{
	memcpy(m_pDibBitsout,m_pDibBits,m_Width*m_Height);
	for (int i=0;i<m_Height;i++)
	{
		for (int j=0;j<wide;j++)
		{
            for (int k=0;k<pixelbyte;k++)
			{
				if (*(m_pDibBitsout+i*m_Width+j*pixelbyte+k)==255)
				{
					if (i==0|i==m_Height-1|j==0|j==wide-1)
					{
						continue;
					}
					*(m_pDibBitsback+i*m_Width+j*pixelbyte+k)=0;
				}
			}
		}
	}
	memcpy(m_pDibBits,m_pDibBitsback,m_Width*m_Height);
}
/***********************************************************************
* �������ƣ�
* Tiqu2()
*˵������̬ѧ�������б߽���ȡ
***********************************************************************/
void CDib::Tiqu2()
{
	if(PaletteEntries!=0){
		long linebytes = (wide*8+31)/32*4;
		unsigned char *lpSrc,*lpDst;
		unsigned char *lp;
		int i,j;
		lpSrc=m_pDibBits;
		lp = new unsigned char [linebytes*m_Height];
		memset(lp , (BYTE)255 , linebytes*m_Height);
		float temp1;
		for(i = 1; i < m_Height-1; i++)
			for(j = 1; j < linebytes-1; j++)
			{	
				
				lpDst=(unsigned char*)lp+i*linebytes+j;										
				if ((lpSrc[i*linebytes+j]==255) &&(lpSrc[i*linebytes+j+1]==255)&&
					(lpSrc[i*linebytes+j-1]==255)&&
					(lpSrc[(i-1)*linebytes+j-1]==255)&&(lpSrc[(i+1)*linebytes+j-1]==255)&&
					(lpSrc[(i+1)*linebytes+j]==255)&&
					(lpSrc[(i-1)*linebytes+j]==255)&&(lpSrc[(i-1)*linebytes+j+1]==255)&&
					(lpSrc[(i+1)*linebytes+j+1]==255))
				{
					*lpDst=255;
				}
				else
				{
					*lpDst=0;
				}
				
			}
			for(i = 0; i < m_Height; i++)
				for(j = 0; j < linebytes; j++)
				{
					lpDst=(unsigned char*)lp+i*linebytes+j;	
					lpSrc=m_pDibBits+i*linebytes+j;
					if(*lpSrc==*lpDst)
					{
						*lpDst=0;
					}
                    else{
						*lpDst=255;}
				}
				memcpy(m_pDibBits,lp,linebytes*m_Height);
				//delete []lp;
	}
	else{
		long linebytes = (wide*8*3+31)/32*4;
		unsigned char *lpSrc,*lpDst;
		unsigned char *lp;
		int i,j;
		lpSrc=m_pDibBits;
		lp = new unsigned char [linebytes*m_Height];
		memset(lp , (BYTE)255 , linebytes*m_Height);
		float temp1;
		for(i = 1; i < m_Height-1; i++)
			for(j = 1; j < linebytes-1; j+=3)
			{	
				
				lpDst=(unsigned char*)lp+i*linebytes+3*j;										
				if ((lpSrc[i*linebytes+3*j]==255) &&(lpSrc[i*linebytes+3*j+3*1]==255)&&
					(lpSrc[i*linebytes+3*j-3*1]==255)&&
					(lpSrc[(i-1)*linebytes+3*j-3*1]==255)&&(lpSrc[(i+1)*linebytes+3*j-3*1]==255)&&
					(lpSrc[(i+1)*linebytes+3*j]==255)&&
					(lpSrc[(i-1)*linebytes+3*j]==255)&&(lpSrc[(i-1)*linebytes+3*j+3*1]==255)&&
					(lpSrc[(i+1)*linebytes+3*j+3*1]==255))
				{
					*lpDst=255;
					*(lpDst+1)=255;
					*(lpDst+2)=255;
				}
				else
				{
					*lpDst=0;
					*(lpDst+1)=0;
					*(lpDst+2)=0;
				}
				
			}
			for(i = 0; i < m_Height; i++)
				for(j = 0; j < linebytes; j++)
				{
					lpDst=(unsigned char*)lp+i*linebytes+3*j;	
					lpSrc=m_pDibBits+i*linebytes+3*j;
					if(*lpSrc==*lpDst)
					{
						*lpDst=0;
						*(lpDst+1)=0;
						*(lpDst+2)=0;
					}
                    else{
						*lpDst=255;
						*(lpDst+1)=255;
						*(lpDst+2)=255;}
				}
				memcpy(m_pDibBits,lp,linebytes*m_Height);
				//	delete []lp;
	}
}
/***********************************************************************
* �������ƣ�
* TiquLaplacian()
*˵����������˹������ȡ�߽�
***********************************************************************/
void CDib::TiquLaplacian()
{
	memcpy(m_pDibBitsout ,m_pDibBits , m_Width*m_Height);
	int i,j,k;
	
	//�м����
	int t;
	
	//Laplacian����
	for(i=1;i<m_Height-1;i++){
		for(j=1;j<wide-1;j++){
			for(k=0;k<pixelbyte;k++){
				//ֻ��һ��ģ��
				//�ɸõ�ĻҶ�ֵ��ȥ�õ������ƽ���Ҷ�ֵ
				t= 4 * *(m_pDibBits+i*m_Width+j*pixelbyte+k)- *(m_pDibBits+(i-1)*m_Width+j*pixelbyte+k)
					- *(m_pDibBits+(i+1)*m_Width+j*pixelbyte+k)- *(m_pDibBits+i*m_Width+(j-1)*pixelbyte+k)
					- *(m_pDibBits+i*m_Width+(j+1)*pixelbyte+k);
				t=abs(t)+0.5;
				if(t>255)
					t=255;
				*(m_pDibBitsout+i*m_Width+j*pixelbyte+k)=t;
			}
		}
	}
	memcpy(m_pDibBits,m_pDibBitsout,m_Width*m_Height);
}
/***********************************************************************
* �������ƣ�
* TiquSobel()
*˵����sobel������ȡ�߽�
***********************************************************************/
void CDib::TiquSobel()
{
	memcpy(m_pDibBitsout ,m_pDibBits , m_Width*m_Height);
	int i,j,k;
	
	//�м����
	int x, y, t;
	
	//Sobel����
	for(i=1;i<m_Height-1;i++){
		for(j=1;j<wide-1;j++){
			for(k=0;k<pixelbyte;k++){
				//x�����ݶ�
				x= *(m_pDibBits+(i-1)*m_Width+(j+1)*pixelbyte+k)
					+ 2 * *(m_pDibBits+i*m_Width+(j+1)*pixelbyte+k)
					+ *(m_pDibBits+(i+1)*m_Width+(j+1)*pixelbyte+k)
					- *(m_pDibBits+(i-1)*m_Width+(j-1)*pixelbyte+k)
					- 2 * *(m_pDibBits+i*m_Width+(j-1)*pixelbyte+k)
					- *(m_pDibBits+(i+1)*m_Width+(j-1)*pixelbyte+k);
				
				//y�����ݶ�
				y= *(m_pDibBits+(i-1)*m_Width+(j-1)*pixelbyte+k)
					+ 2 * *(m_pDibBits+(i-1)*m_Width+j*pixelbyte+k)
					+ *(m_pDibBits+(i-1)*m_Width+(j+1)*pixelbyte+k)
					- *(m_pDibBits+(i+1)*m_Width+(j-1)*pixelbyte+k)
					- 2 * *(m_pDibBits+(i+1)*m_Width+j*pixelbyte+k)
					- *(m_pDibBits+(i+1)*m_Width+(j+1)*pixelbyte+k);
				
				t=sqrt(x*x+y*y)+0.5;
				if(t>255)
					t=255;
				*(m_pDibBitsout+i*m_Width+j*pixelbyte+k)=t;
			}
		}
	}
	memcpy(m_pDibBits,m_pDibBitsout,m_Width*m_Height);
}
/***********************************************************************
* �������ƣ�
* RegionGrow()
*˵��������������ģ�庯������ֵ�趨Ϊ80����������(130,80)
***********************************************************************/
void CDib::RegionGrow()
{
	//ѭ������
	int i, j;
	
	CPoint seed;
	int thresh;
	
	seed=(130,80);
	thresh=80;           //������������ѡ���ǹؼ�������
	m_pDibBitsout=new unsigned char[m_Width*m_Height];
	
	
	//�����ͼ���ʼ����255,��0�������ص��������
	for(i=0;i<m_Height;i++){
		for(j=0;j<wide;j++){              //����Ҫ��linebytes
			*(m_pDibBitsout+i*m_Width+j)=255;
		}
	}
	
	//��ά����direction�����������ص�8����������õ���x��y�����ϵ�ƫ��,
	//���е�һ��Ϊx�����ƫ��,�ڶ���Ϊy�����ƫ��
	int direction[8][2]={{0,1},{1,1},{1,0},{1,-1},{0,-1},{-1,-1},{-1,0},{-1,1}};
	
	//ջ���룬�˴��ٶ���ջ���������Ϊͼ����������
	CPoint *stack=new CPoint[wide*m_Height];
	
	//ջ��ָ��
	int top;
	
	//��ǰ������ĵ�͵����ĵ�
	CPoint currentPoint, popPoint;
	
	//ѭ������������array����ĵ�һά�±�
	int k;
	
	//��Ǳ���
	int label;
	
	//��ʱ����
	int temp1, temp2;
	
	//��¼�������صĻҶ�ֵ
	temp1=*(m_pDibBits+seed.y*m_Width+seed.x);
	
	//���������ӵ��ñ��0,��ջ
	*(m_pDibBitsout+seed.y*m_Width+seed.x)=0;
	top=0;
	stack[top].x=seed.x;
	stack[top].y=seed.y;
	
	//��ջ
	while(top>-1){
		//����ջ��Ԫ��,��Ԫ���Ѿ�������
		popPoint.x=stack[top].x;
		popPoint.y=stack[top].y;
		top--;
		
		//���쵯��������Χ�Ƿ���û������������
		for(k=0;k<8;k++){
			
			//������������
			currentPoint.x=popPoint.x+direction[k][0];
			currentPoint.y=popPoint.y+direction[k][1];
			
			//���������ĵ㲻��ͼ���ڣ�������
			if(currentPoint.x<0||currentPoint.x>wide-1||
				currentPoint.y<0||currentPoint.y>m_Height-1)
				continue;
			
			//�õ���
			label=*(m_pDibBitsout+currentPoint.y*m_Width+currentPoint.x);
			
			//�����ĵ���Χ����û�����ĵ�
			if(label==255){
				temp2=*(m_pDibBits+currentPoint.y*m_Width+currentPoint.x);
				//�����ǰ����������ػҶ�ֵ�����ӵ�Ҷ�ֵ֮��С�ڸ�������ֵ,
				//����Ϊ����,�����ջ����
				if(abs(temp1-temp2)<thresh){
					//���õ����������0
					*(m_pDibBitsout+currentPoint.y*m_Width+currentPoint.x)=0;
					top++;
					stack[top].x=currentPoint.x;
					stack[top].y=currentPoint.y;
				}
			}
		}
		
	}
	memcpy(m_pDibBits,m_pDibBitsout,m_Width*m_Height);
	
	//���������
	delete []stack;
}
