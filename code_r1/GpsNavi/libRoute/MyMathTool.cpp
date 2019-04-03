// Copyright (c) 2005 PLink Corp.  All rights reserved.
// $Author: daniel $
// $Date: 2011/05/18 03:21:01 $
// $Revision: 1.6 $
// MyMathTool.cpp: implementation of the CMyMathTool class.
//
//////////////////////////////////////////////////////////////////////

#include "MyMathTool.h"

CMyMathTool			**g_MyMathTool = NULL;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMyMathTool::CMyMathTool()
{

}

CMyMathTool::~CMyMathTool()
{

}

nuVOID CMyMathTool::Normalize(NURORECT &pRect)
{
	nuLONG temp = 0;
	if( pRect.left > pRect.right )
	{
		temp = pRect.left;
		pRect.left = pRect.right;
		pRect.right = temp;
	}
	if( pRect.top > pRect.bottom)
	{
		temp = pRect.top;
		pRect.top = pRect.bottom;
		pRect.bottom = temp;
	}
}

nuBOOL CMyMathTool::PointInRect(NUROPOINT pt, NURORECT &rect)
{
	nuBOOL crossX = nuFALSE;
	nuBOOL crossY = nuFALSE;
	Normalize(rect);
	crossX = ( pt.x >= rect.left && pt.x <= rect.right );
	crossY = ( pt.y >= rect.top && pt.y <= rect.bottom );
	return (crossX & crossY);
}

nuVOID CMyMathTool::FixinLine(NUROPOINT NowPt, NUROPOINT LinePt1, NUROPOINT LinePt2, NUROPOINT &FixedPt, nuLONG &Dis)
{
	nuFLOAT k = 1;
	nuLONG DisToPt1 = 0;
	nuLONG DisToPt2 = 0;
	Dis=999999;
	if( LinePt1.x == LinePt2.x )
	{
		if( LinePt1.y == LinePt2.y )
		{
			FixedPt.x = LinePt1.x;
			FixedPt.y = LinePt1.y;
		}
		else
		{
			FixedPt.x = LinePt1.x;
			FixedPt.y = NowPt.y;
			if( FixedPt.y > NURO_MAX(LinePt1.y, LinePt2.y) ) FixedPt.y = NURO_MAX(LinePt1.y, LinePt2.y);
			if(	FixedPt.y < NURO_MIN(LinePt1.y, LinePt2.y) ) FixedPt.y = NURO_MIN(LinePt1.y, LinePt2.y);
		}
	}
	else
	{
		if( LinePt1.y == LinePt2.y )
		{
			FixedPt.x = NowPt.x;
			FixedPt.y = LinePt1.y;
			if( FixedPt.x > NURO_MAX(LinePt1.x, LinePt2.x) ) FixedPt.x = NURO_MAX(LinePt1.x, LinePt2.x);
			if(	FixedPt.x < NURO_MIN(LinePt1.x, LinePt2.x) ) FixedPt.x = NURO_MIN(LinePt1.x, LinePt2.x);
		}
		else
		{
			k=(nuFLOAT)((NowPt.y-LinePt1.y)*(LinePt2.x - LinePt1.x)-(NowPt.x-LinePt1.x)*(LinePt2.y - LinePt1.y))/((LinePt2.x - LinePt1.x)*(LinePt2.x - LinePt1.x)+(LinePt2.y - LinePt1.y)*(LinePt2.y - LinePt1.y));
			FixedPt.x	=	NowPt.x + ((LinePt2.y - LinePt1.y)*((NowPt.y-LinePt1.y)*(LinePt2.x - LinePt1.x)-(NowPt.x-LinePt1.x)*(LinePt2.y - LinePt1.y)))/((LinePt2.x - LinePt1.x)*(LinePt2.x - LinePt1.x)+(LinePt2.y - LinePt1.y)*(LinePt2.y - LinePt1.y));
			FixedPt.y	=	NowPt.y - ((LinePt2.x - LinePt1.x)*((NowPt.y-LinePt1.y)*(LinePt2.x - LinePt1.x)-(NowPt.x-LinePt1.x)*(LinePt2.y - LinePt1.y)))/((LinePt2.x - LinePt1.x)*(LinePt2.x - LinePt1.x)+(LinePt2.y - LinePt1.y)*(LinePt2.y - LinePt1.y));

			if( FixedPt.x > NURO_MAX(LinePt1.x, LinePt2.x) || FixedPt.x < NURO_MIN(LinePt1.x, LinePt2.x)    )
			{
				DisToPt1 = NURO_ABS( FixedPt.x - LinePt1.x );
				DisToPt2 = NURO_ABS( FixedPt.x - LinePt2.x );
				if( DisToPt1 > DisToPt2 )
				{
					FixedPt.x = LinePt2.x;
					FixedPt.y = LinePt2.y;
				}
				else
				{
					FixedPt.x = LinePt1.x;
					FixedPt.y = LinePt1.y;
				}
			}
		}
	}

	double dx, dy, dnum;
	dx = (FixedPt.x - NowPt.x);
	dy = (FixedPt.y - NowPt.y);
	dnum=(dx*dx)+(dy*dy);
	if(dnum<0)
	{
		Dis = -1;
	}
	else
	{
		Dis = (nuLONG)nuSqrt( dnum );
	}
}
/*
¨¤«×    sin     cos    tan
0.0000	0.0000	1.0000 0.0000
1.0000	0.0175	0.9998 0.0175
2.0000	0.0349	0.9994 0.0349
3.0000	0.0523	0.9986 0.0524
4.0000	0.0698	0.9976 0.0699
5.0000	0.0872	0.9962 0.0875
6.0000	0.1045	0.9945 0.1051
7.0000	0.1219	0.9925 0.1228
8.0000	0.1392	0.9903 0.1405
9.0000	0.1564	0.9877 0.1584
10.0000	0.1736	0.9848 0.1763
11.0000 0.1908	0.9816 0.1944
12.0000 0.2079	0.9781 0.2126
13.0000 0.2250	0.9744 0.2309
14.0000 0.2419	0.9703 0.2493
15.0000 0.2588	0.9659 0.2679
16.0000 0.2756	0.9613 0.2867
17.0000 0.2924	0.9563 0.3057
18.0000 0.3090	0.9511 0.3249
19.0000 0.3256	0.9455 0.3443
20.0000 0.3420	0.9397 0.3640
21.0000 0.3584	0.9336 0.3839
22.0000 0.3746	0.9272 0.4040
23.0000 0.3907	0.9205 0.4245
24.0000 0.4067	0.9135 0.4452
25.0000 0.4226	0.9063 0.4663
26.0000 0.4384 0.8988 0.4877
27.0000 0.4540 0.8910 0.5095
28.0000 0.4695 0.8829 0.5317
29.0000 0.4848 0.8746 0.5543
30.0000 0.5000 0.8660 0.5774
31.0000 0.5150 0.8572 0.6009
32.0000 0.5299 0.8480 0.6249
33.0000 0.5446 0.8387 0.6494
34.0000 0.5592 0.8290 0.6745
35.0000 0.5736 0.8192 0.7002
36.0000 0.5878 0.8090 0.7265
37.0000 0.6018 0.7986 0.7536
38.0000 0.6157 0.7880 0.7813
39.0000 0.6293 0.7771 0.8098
40.0000 0.6428 0.7660 0.8391
41.0000 0.6561 0.7547 0.8693
42.0000 0.6691 0.7431 0.9004
43.0000 0.6820 0.7314 0.9325
44.0000 0.6947 0.7193 0.9657
45.0000 0.7071 0.7071 1.0000
46.0000 0.7193 0.6947 1.0355
47.0000 0.7314 0.6820 1.0724
48.0000 0.7431 0.6691 1.1106
49.0000 0.7547 0.6561 1.1504
50.0000 0.7660 0.6428 1.1918
51.0000 0.7771 0.6293 1.2349
52.0000 0.7880 0.6157 1.2799
53.0000 0.7986 0.6018 1.3270
54.0000 0.8090 0.5878 1.3764
55.0000 0.8192 0.5736 1.4281
56.0000 0.8290 0.5592 1.4826
57.0000 0.8387 0.5446 1.5399
58.0000 0.8480 0.5299 1.6003
59.0000 0.8572 0.5150 1.6643
60.0000 0.8660 0.5000 1.7321
61.0000 0.8746 0.4848 1.8040
62.0000 0.8829 0.4695 1.8807
63.0000 0.8910 0.4540 1.9626
64.0000 0.8988 0.4384 2.0503
65.0000 0.9063 0.4226 2.1445
66.0000 0.9135 0.4067 2.2460
67.0000 0.9205 0.3907 2.3559
68.0000 0.9272 0.3746 2.4751
69.0000 0.9336 0.3584 2.6051
70.0000 0.9397 0.3420 2.7475
71.0000 0.9455 0.3256 2.9042
72.0000 0.9511 0.3090 3.0777
73.0000 0.9563 0.2924 3.2709
74.0000 0.9613 0.2756 3.4874
75.0000 0.9659 0.2588 3.7321
76.0000 0.9703 0.2419 4.0108
77.0000 0.9744 0.2250 4.3315
78.0000 0.9781 0.2079 4.7046
79.0000 0.9816 0.1908 5.1446
80.0000 0.9848 0.1736 5.6713
81.0000 0.9877 0.1564 6.3138
82.0000 0.9903 0.1392 7.1154
83.0000 0.9925 0.1219 8.1443
84.0000 0.9945 0.1045 9.5144
85.0000 0.9962 0.0872 11.4301
86.0000 0.9976 0.0698 14.3007
87.0000 0.9986 0.0523 19.0811
88.0000 0.9994 0.0349 28.6363
89.0000 0.9998 0.0175 57.2900
90.0000 1.0000 0.0000 µL­­¤j 
*/
