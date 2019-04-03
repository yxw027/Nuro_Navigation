#include "FileMapTMC.h"


char RN_File[]="D:\\tmc\\TW.RN";
char TMC[]="D:\\tmc\\TW.TMC";
char TMC_DATA_File[]="D:\\tmc\\TMC.txt";
char TMC_LOCATION[]="D:\\tmc\\name.txt";

CFileMapTMC::CFileMapTMC()
{
}

CFileMapTMC::~CFileMapTMC()
{
//	Close();
}


void CFileMapTMC::GetTMC()
{

    FILE *fp = fopen(TMC_DATA_File,"r");   

	// ?ȩwŪN?? 200
    r2=new StructTMCPassData[200];
    r1=new TMC_Info_Router[200];
    u1=new TMC_Info_UI[200];

    
	int q=-1;  arry_idx=0;
	while(q++<19){
	
	char v[30];
	
    int n=0,nn=0;

	memset(&v,0,30);

	char tmc_information[13][2]; 
	char tmc_information2[13][2]; //?ഫANSI?ΰ}?C 
	
	int i=-1; //Ū?J?@?? TMC_INFORMATION
	while(i++<12)
	{
		fscanf(fp,"%s",&tmc_information[i][0]); 
	}

	//?ഫANSI (read txt)
   i=-1;
   while(i++<12)
   {
	   for(int jj=0;jj<2;jj++)
	   {	   
			if(tmc_information[i][jj]<58)
			{
				tmc_information2[i][jj]=(tmc_information[i][jj]-48); 
			}
			else
			{
				tmc_information2[i][jj]=(tmc_information[i][jj]-48-7);
			}
	   }
   }

   //?P?_?O?_?????T???T check_sum
    int error=0;

	i=1; //2-9
	while(i++<9)
	{
	   error^=((tmc_information2[i][0])*16+tmc_information2[i][1]);
	}
    if(error!=tmc_information2[10][0]*16+tmc_information2[10][1])
	{ 
		continue;	
	}

    char ss[16]={0};
	int j=0;


	// B BLOCK
    i=5;

	char s[8]={0};
	int jj=0;

	 for (int q = 7; q>-1; q--)
	 {      
       s[q] = tmc_information2[i][1]%2;
       tmc_information2[i][1] = tmc_information2[i][1]/2;
	 }

	int T =s[3];
	int F =s[4];
	int DP=s[7]+s[6]*2+s[5]*4;
	
	r2[arry_idx].TMCTime=DP;

	// C BLOCK

	i=5;

	while(i++<7)
	{
		for (int ii = 3; ii>-1; ii--)
		{
			ss[4*j+ii] = tmc_information2[i][0]%2;
			tmc_information2[i][0] = tmc_information2[i][0]/2; 
		} 
		j++;

		for (ii = 3; ii>-1; ii--)
		{
			ss[4*j+ii] = tmc_information2[i][1]%2;
			tmc_information2[i][1] = tmc_information2[i][1]/2; 
        } 
		j++;
	}
        
    int note=ss[0];
    int direction=ss[1];
	int extent[3];
    
	for(int zz=0;zz<3;zz++)
	{
		extent[zz]=ss[zz+2];
	}	
	
	int event[11];
	
	for(int z=0;z<11;z++)
	{
		event[z]=ss[z+5];
	}

	int sum=0;

    for (int ii=10,int k=1; ii>0; ii--)
	{
		sum+=event[ii]*k;
		k = k * 2;       
    }

    r1[arry_idx].event_id=sum;
	
	ShowEvent(sum); //???o?ƥ????T
    // return true;
	
	// D BLOCK
	char D_block[16]={0};
   	
	i=7;j=0;

	while(i++<9)
	{
		for (int ii = 3; ii>-1; ii--)
		{
			D_block[4*j+ii] = tmc_information2[i][0]%2;
			tmc_information2[i][0] = tmc_information2[i][0]/2;      
		} 
		j++;

		for (ii = 3; ii>-1; ii--)
		{
			D_block[4*j+ii] = tmc_information2[i][1]%2;
			tmc_information2[i][1] = tmc_information2[i][1]/2; 
		}
		j++;
	}

	int event2[16];
	
	for(zz=0;zz<16;zz++)
	{
		event2[zz]=D_block[zz];
	}

	sum=0;

	for (int iii=15,kk=1; iii>0; iii--)
	{
		sum+=event2[iii]*kk;
		kk = kk * 2;
		r1[arry_idx].location_id=sum;
		ShowLocation(sum); //???o???m???T

		arry_idx++;
	}

	fclose(fp);
}

void CFileMapTMC::Close()
{

}


void CFileMapTMC::ShowEvent(int code)
{
	FILE *fp = fopen(TMC,"rb");     
    FILE *fp2 = fopen("D:\\tmc\\tmc_info.txt","w"); 

	char v[30];

    int n=0,nn=0;

	memset(&v,0,30);

	TMC_Header header;
	
    fread(&header,16,1,fp);
	
	fseek(fp,header.TrafficDataAddr,SEEK_SET);
	
    nuWORD Traffic_Num;
    nuWORD Traffic_NameLen;
	
	short Traffic_EventWeighting;
	short Traffic_EventTime;
	
	int j=-1;
	
	wchar_t *myString[30] = {0x0000};
    
     nuWORD name;

	 bool flag = false; 
	 
	 nuWORD Traffic_Name[50];
     memset(&Traffic_Name,0,50);

	 while(j++<header.TrafficCnt)
	 {
		fread(&Traffic_Num,sizeof(nuWORD),1,fp);
	    fread(&Traffic_NameLen,sizeof(nuWORD),1,fp);
		fread(&Traffic_EventWeighting,2,1,fp);
		fread(&Traffic_EventTime,2,1,fp);
		fread(&Traffic_Name,Traffic_NameLen,1,fp);
     
		setlocale(LC_ALL, "");
		fwprintf(fp2,L"n***********%ls***********\n",Traffic_Name);

		if((int)Traffic_Num==(int)code)
		{
			//?????n?]?w?v?ȥ?
			//r2[arry_idx].TMCWeighting=;
     
			//?ư??????P?_
			//r2[arry_idx].TMCPass=;
     
			u1[arry_idx].location_len=Traffic_NameLen;  
			r2[arry_idx].TMCWeighting=Traffic_EventWeighting;
			r2[arry_idx].TMCTime=Traffic_EventTime;

			memcpy(u1[arry_idx].event_name,Traffic_Name,50); 
	 
			flag=true;
		}
		//  if(flag) break;	
	}

	 if(!flag)
	 {
		u1[arry_idx].event_len=7*2;  
    
		memcpy(u1[arry_idx].event_name,"?ȵL?????q???T",50); 
	 }

	fclose(fp);
	fclose(fp2);
} 


void CFileMapTMC::ShowLocation(int code)
{
	FILE *tmc_fp = fopen(TMC,"rb");   
    FILE *rn_fp = fopen(RN_File,"rb");  
	
	char v[30];

    int n=0,nn=0;

	memset(&v,0,30);

	TMC_Header header;

	fread(&header,16,1,tmc_fp);

    fseek(tmc_fp,header.VertexDataAddr,SEEK_SET);
	
	TMC_DATA tmc;
	int ii=0;

    bool check=false;

	while(ii++<=header.DataCnt)
	{
		fread(&tmc,sizeof(TMC_DATA),1,tmc_fp);
		fseek(tmc_fp,tmc.LocationVertexAddr,SEEK_SET);
	

		TMC_VertexData *VertexData=new TMC_VertexData[tmc.LocationCodeNum];

		if(tmc.LocationCode==code)
		{ 
			//save for router 
			//tmc.LocationVertexAddr
			//tmc.LocationCodeNum 

			r2[arry_idx].TMCLocationAddr=tmc.LocationVertexAddr;
			r2[arry_idx].TMCnodes=tmc.LocationCodeNum;
	
			wchar_t myString[3] = {0x0000};
    
    
			nuWORD *Traffic_Name;
			nuCHAR len;
    
			int t;

			int i=-1;

			while(i++<tmc.LocationCodeNum)
			{
				fread(&VertexData[i],sizeof(TMC_VertexData),1,tmc_fp);

				if(VertexData[i].rn_Addr==4294967295UL)
				{
					continue;
				}
				fseek(rn_fp,VertexData[i].rn_Addr,SEEK_SET);
				fread(&len,1,1,rn_fp);
				fseek(rn_fp,3,SEEK_CUR);

				nuWORD name[50];
				fread(&name,sizeof(nuWORD), len,rn_fp);
				u1[arry_idx].location_len=len;  
				memcpy(u1[arry_idx].location_name,name,len);  
	
				check=true;
			}
		}

		if(check) 
		{
			break;
		}

		fseek(tmc_fp,tmc.LocationDownAddr,SEEK_SET);
    }

    if(!check)
	{
		u1[arry_idx].location_len=2*7;  
		memcpy(u1[arry_idx].location_name,"?ȵL?????m???T",50);  
	}

	fclose(tmc_fp);
	fclose(rn_fp);
}
