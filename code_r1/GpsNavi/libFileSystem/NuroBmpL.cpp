
#include "NuroBmpL.h"

CNuroBmpL::CNuroBmpL()
{

}

CNuroBmpL::~CNuroBmpL()
{

}

/*用来确保每次从文件中读出的长度小于或等于1024*/
#ifndef _USE_DDB_BITMAP
nuWORD tlbytesread(nuBYTE *des,nuSIZE n=1, nuSIZE item=0,nuFILE *fp=NULL)
{
	nuWORD length=item*n,clength=0,templength=0;
	if(length>1024)
	{
		
		while(length>1024)
		{
			if((templength=nuFread(des,1,1024,fp))!=1024)
			{
				clength=clength+templength;
				return clength;
			};
			clength=clength+templength;
			des=des+1024;
			length=length-1024;
		}
		templength=nuFread(des,1,length,fp);
		clength=clength+templength;
		return clength;
	}
	else
	{
		templength=nuFread(des,n,item,fp);
		return templength;
	}
}

/*函数的功能是将nmp文件的所有数据信息读到内存中*/
const nuBOOL COLOR_L=3;
nuBOOL CNuroBmpL::ReadNuroBmp(const nuTCHAR *pszFilename, PNURO_BMP temp)
{
	NURO_BMP bytemp = {0};
	nuFILE *fp = NULL;
	if(!(fp=(nuFILE*)nuTfopen(pszFilename, NURO_FS_RB)))
	{	
		return nuFALSE;
	}
	if(nuFread(&bytemp,1, sizeof(bytemp),fp) != sizeof(bytemp))
	{
		return nuFALSE;
	}
	temp->bmpBitCount = bytemp.bmpBitCount;
	temp->bmpReserve = bytemp.bmpReserve;
	temp->bmpType = bytemp.bmpType;
	temp->bmpTransfColor = bytemp.bmpTransfColor;
	temp->bmpWidth = bytemp.bmpWidth;
	temp->bmpHeight = bytemp.bmpHeight;
	temp->bmpBuffLen = bytemp.bmpBuffLen;
	temp->pBmpBuff = new nuBYTE[temp->bmpBuffLen];
	if(tlbytesread(temp->pBmpBuff,sizeof(nuBYTE),temp->bmpBuffLen,fp)!=temp->bmpBuffLen)
	{
		delete []temp->pBmpBuff;
		temp->pBmpBuff=NULL;
		return nuFALSE;
	}
	nuFclose(fp);
	return nuTRUE;
}
/*函数将nmp文件读到内存中*/
nuBOOL CNuroBmpL::FillNuroBmp(const nuTCHAR *pszFilename, PNURO_BMP temp)
{
	NURO_BMP bytemp = {0};
	nuFILE *fp = NULL;
	if(!(fp=(nuFILE*)nuTfopen(pszFilename, NURO_FS_RB)))
	{	
		return nuFALSE;
	}
	if(nuFread(&bytemp,1,sizeof(bytemp),fp) != sizeof(bytemp))
	{
		return nuFALSE;
	}
	temp->bmpBitCount = bytemp.bmpBitCount;
	temp->bmpReserve = bytemp.bmpReserve;
	temp->bmpType = bytemp.bmpType;
	temp->bmpTransfColor = bytemp.bmpTransfColor;
	nuWORD i = 0;
	nuBYTE *des = NULL;
	nuBYTE *src = NULL;
	bytemp.pBmpBuff = new nuBYTE[bytemp.bmpBuffLen];
	if( (nuFread(bytemp.pBmpBuff, 1, bytemp.bmpBuffLen, fp)) != bytemp.bmpBuffLen )
	{
		return nuFALSE;
	}
	/*读取颜色信息时一定要注意颜色信息数据存储方式是倒转顺序存储的*/
	nuWORD tempx=0,tempdx=0;
	//分别表示用户每行的字节长度和实际每行的自己长度
	if((temp->bmpWidth*COLOR_L)%4!=0)
	{
		tempx=temp->bmpWidth*COLOR_L+4-(temp->bmpWidth*COLOR_L)%4;
		//计算出了实际每行多少个字节
	}
	else
	{
		tempx=temp->bmpWidth*COLOR_L;
	}
	if((bytemp.bmpWidth*COLOR_L)%4!=0)
	{
		tempdx=bytemp.bmpWidth*COLOR_L+4-(bytemp.bmpWidth*COLOR_L)%4;
	}
	else
	{
		tempdx=bytemp.bmpWidth*COLOR_L;
	}
	//第一种情况
	if((bytemp.bmpWidth>temp->bmpWidth)&&(bytemp.bmpHeight>temp->bmpHeight))
	{

		for(i=0;i<temp->bmpHeight;i++)
		{
			des=temp->pBmpBuff+i*tempx;
			src=bytemp.pBmpBuff+i*tempdx;
			nuMemcpy(des,src,temp->bmpWidth*COLOR_L);	
		}

	}
	//第二种情况用户定义的宽小于实际的，用户的高大于实际的
	else if(bytemp.bmpWidth>temp->bmpWidth)
	{
		for(i=0;i<bytemp.bmpHeight;i++)
		{
			des=temp->pBmpBuff+i*tempx;
			src=bytemp.pBmpBuff+i*tempdx;
			nuMemcpy(des,src,temp->bmpWidth*COLOR_L);
		}
			
	}
	//第三种情况用户的高小于实际的，但用户的宽大于实际的
	else if(bytemp.bmpHeight>temp->bmpHeight)
	{
		
		for(i=0;i<temp->bmpHeight;i++)
		{
			des=temp->pBmpBuff+i*tempx;
			src=bytemp.pBmpBuff+i*tempdx;
			nuMemcpy(des,src,bytemp.bmpWidth*COLOR_L);
		}
	}
	else
	{	
		for(i=0;i<bytemp.bmpHeight;i++)
		{
			des=des=temp->pBmpBuff+i*tempx;
			src=bytemp.pBmpBuff+i*tempdx;
			nuMemcpy(des,src,bytemp.bmpWidth*COLOR_L);
		}
	}
	delete []bytemp.pBmpBuff;
	bytemp.pBmpBuff=NULL;
	nuFclose(fp);
	return nuTRUE;
}
#endif
