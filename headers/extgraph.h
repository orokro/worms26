/******************************************************************************
*
* E X T G R A P H    v 2 . 0 0    Beta  3
* Copyright © 2001-2004 TICT (TI-Chess Team)
*
*******************************************************************************
*
* ExtGraph is a compile-time library which contains speed optimized graphics
* routines for the TIGCC cross-compiler for TI-89, TI-92+ and TI-V200.
* NOTE: using ExtGraph 2.xx requires TIGCC 0.95 and newer.
* Recompiling ExtGraph is currently not very easy for someone else, but
* that's not a problem since ExtGraph can be used as is (extgraph.h +
* tilemap.h + extgraph.a + tilemap.a); recompiling ExtGraph is therefore
* neither necessary nor recommended.
*
*
* library maintained by:
*                    Thomas Nussbaumer  (thomas.nussbaumer@gmx.net)
*                    Lionel Debroux     (lionel_debroux@yahoo.fr)
*                    Julien Richard-Foy (julien.rf@wanadoo.fr)
*                    various contributors.
*
*
*******************************************************************************
* [internal version] $Id: extgraph.h,v 1.13 2002/05/22 09:19:20 tnussb Exp $
*******************************************************************************/

#if !defined(__EXTGRAPH__)
#define __EXTGRAPH__

#include <gray.h>   // necessary for GrayGetPlane() macros in the grayscale
                    // support routines !!

//-----------------------------------------------------------------------------
// Reject TIGCC 0.94 and less (0.93 is not the official release anymore and
// it doesn't support __attribute__((__regparm__)) and 
// __attribute__((__stkparm__)); 0.94 won't compile some attributes used in
// the demos; 0.95 has a new good linker and is better overall)...
// Warn about other compilers because ExtGraph was tested only under TIGCC
// until further notice.
//-----------------------------------------------------------------------------
#include <version.h>

#ifndef __TIGCC__
#warning This compiler is unknown to ExtGraph. Until further notice, ExtGraph was tested only under standards-compliant TIGCC. Other compilers may not support ExtGraph.
#endif

#if (defined(__TIGCC__)&&(__TIGCC__<1)&&(defined(__TIGCC_MINOR__)&&(__TIGCC_MINOR__<95)))
#error ExtGraph needs TIGCC 0.95 or later ! Update TIGCC.
#endif


//-----------------------------------------------------------------------------
// to embed the extgraph version as string into your program the extgraph
// library contains the following global string variable which SHOULD be used
// for this!
//
// the string looks like this: "ExtGraph vX.YY" where X is the major version
// number and YY is the subversion number.
//
// EXTGRAPH_VERSION_PWDSTR looks like this: "powered by ExtGraph vX.YY"
//-----------------------------------------------------------------------------
#define EXTGRAPH_VERSION_STR      __egvs__
#define EXTGRAPH_VERSION_PWDSTR   __egvpwds__
extern char __egvs__[];
extern char __egvpwds__[];


//-----------------------------------------------------------------------------
// global variables embedded in the library are used for the version numbers
// to garantee that the version is reported correctly even if you use an
// outdated headerfile
//-----------------------------------------------------------------------------
#define EXTGRAPH_VERSION_MAIN    __egvmain__
#define EXTGRAPH_VERSION_SUB     __egvsub__
extern short __egvmain__;
extern short __egvsub__;


// THESE MACROS ARE NOT AWARE OF THE V200 AND THE TI-89 TITANIUM ("TITANIC") !
// THEREFORE, YOU SHOULD NOT USE THEM ANYMORE. USE THE TIGCCLIB compat.h
// DEFINITIONS INSTEAD.
//-----------------------------------------------------------------------------
// useful macros which may help to protect a program from running on a wrong
// calctype
//
// usage:
//
// if (!CheckHWMatch()) {
//     ST_helpMsg("ERROR: can only run on a "DESIRED_CALCTYPE);
//     return;
// }
//-----------------------------------------------------------------------------
/*
#define __TI89CHECK__  ((((void*)((*(unsigned long*)0xC8)&0x600000))==(void*)0x400000)==0)

#if defined(USE_TI89) && !defined(USE_TI92P)

#define CheckHWMatch()    (__TI89CHECK__ ? 1 : 0)
#define DESIRED_CALCTYPE  "TI89"

#elif !defined(USE_TI89) && defined(USE_TI92P)

#define CheckHWMatch()  (__TI89CHECK__ ? 0 : 1)
#define DESIRED_CALCTYPE  "TI92+"

#else

#define CheckHWMatch()    (1)
#define DESIRED_CALCTYPE  "TI89/92+"

#endif
*/

// macro which returns the absolute value of a given short
#define EXT_SHORTABS(a)  ({register short ta=(a); (ta>=0) ? ta : -ta;})

// macro which returns the absolute value of a given long
#define EXT_LONGABS(a)  ({register long ta=(a); (ta>=0) ? ta : -ta;})

// macro which exchanges the content of two variables using the ASM exg instruction
// 2.00 adds "a" to the constraints, since exg applies also to address registers.
// This already optimized TestCollide8/16 routines, before I rewrote them in ASM for
// better code.
#define EXT_XCHG(a,b)    asm volatile ("exg %0,%1" : "=da" (a), "=da" (b) : "0" (a), "1" (b) : "cc")

// macro which returns the word swapped value (upper and lower word swapped) of the given long
#define EXT_LONGSWAP(val) ({register unsigned long tmp = val;asm volatile ("swap %0" : "=d" (tmp) : "0" (tmp));tmp;})

// macro which checks two bounding rectangles starting at (x0/y0) and (x1/y1) for
// collision. w is the width in pixels and h the height of the two bounding rectangles
#define BOUNDS_COLLIDE(x0,y0,x1,y1,w,h) \
   (((EXT_SHORTABS((x1)-(x0)))<(w))&&((EXT_SHORTABS((y1)-(y0)))<(h)))

// handy aliases for standard tile sizes (8x8 / 16x16 / 32x32)
#define BOUNDS_COLLIDE8(x0,y0,x1,y1)  BOUNDS_COLLIDE(x0,y0,x1,y1,8,8)
#define BOUNDS_COLLIDE16(x0,y0,x1,y1) BOUNDS_COLLIDE(x0,y0,x1,y1,16,16)
#define BOUNDS_COLLIDE32(x0,y0,x1,y1) BOUNDS_COLLIDE(x0,y0,x1,y1,32,32)

// checks for collision between 2 sprites of width 8
short TestCollide8(short x0,short y0,short x1,short y1,short height,unsigned char* data0, unsigned char* data1) __attribute__((__stkparm__));
short TestCollide8_R(register short x0 asm("%d0"),register short y0 asm("%d1"),register short x1 asm("%d2"),register short y1 asm("%d3"),short height,register unsigned char* data0 asm("%a0"),register unsigned char* data1 asm("%a1")) __attribute__((__stkparm__));

// checks for collision between 2 sprites of width 16
short TestCollide16(short x0,short y0,short x1,short y1,short height,unsigned short* data0, unsigned short* data1) __attribute__((__stkparm__));
short TestCollide16_R(register short x0 asm("%d0"),register short y0 asm("%d1"),register short x1 asm("%d2"),register short y1 asm("%d3"),short height,register unsigned short* data0 asm("%a0"),register unsigned short* data1 asm("%a1")) __attribute__((__stkparm__));


//-----------------------------------------------------------------------------
// enums used by some extgraph functions
// 2.00 changes the values in enum GrayColors for more optimized routines.
//-----------------------------------------------------------------------------
enum GrayColors {COLOR_WHITE=0,COLOR_LIGHTGRAY=1,COLOR_DARKGRAY=2,COLOR_BLACK=3};
enum FillAttrs  {RECT_EMPTY=0,RECT_FILLED=1};
enum ExtAttrs   {A_CENTERED=0x40,A_SHADOWED=0x80};


//-----------------------------------------------------------------------------
// speedy pixel access routines
// 2.00 introduces new macros, coded and fixed by Julien Richard-Foy,
// Kevin Kofler, Lionel Debroux.
//-----------------------------------------------------------------------------
#define EXT_PIXOFFSET(x,y) ((((y)<<4)-(y))*2+((x)>>3))
#define EXT_PIXADDR(p,x,y) (((char *)(p))+EXT_PIXOFFSET(x,y))
#define EXT_PIXMASK(x) (0x80>>((x)&7))
#define EXT_PIXNBIT(x) (~(x))


#define EXT_SETPIX_AM(a,m)   (*(a) |= (m))
#define EXT_CLRPIX_AM(a,m)   (*(a) &= ~(m))
#define EXT_XORPIX_AM(a,m)   (*(a) ^= (m))
#define EXT_GETPIX_AM(a,m)   (*(a) & (m))


#define EXT_PIXUP(a,m) ((a)-=30)
#define EXT_PIXDOWN(a,m) ((a)+=30)
#define EXT_PIXLEFT_AM(a,m)   asm("rol.b  #1,%0;bcc.s  0f;subq.l #1,%1;0:"\
                                  : "=d" (m), "=g" (a) : "0" (m), "1" (a))
#define EXT_PIXRIGHT_AM(a,m)  asm("ror.b  #1,%0;bcc.s  0f;addq.l #1,%1;0:"\
                                  : "=d" (m), "=g" (a) : "0" (m), "1" (a))


// These macros are more optimized than EXT_..._AM ones, but less readable...
#define EXT_SETPIX_AN(a,offset,n) ({if(__builtin_constant_p(offset)) \
	{	\
		if(__builtin_constant_p(a))	\
			asm("bset.b %0,%c1" : : "di" (n), "dai" ((offset)+(unsigned char*)(a)));	\
		else	\
			if (offset !=0) \
				asm("bset.b %0,%c1(%2)" : : "di" (n), "dai" (offset), "a" (a));	\
			else \
				asm("bset.b %0,(%1)" : : "di" (n), "a" (a));	\
	}	\
	else	\
		asm("bset.b %0,0(%2,%1.w)" : : "di" (n), "da" (offset), "a" (a));	\
	})

#define EXT_CLRPIX_AN(a,offset,n) ({if(__builtin_constant_p(offset)) \
	{	\
		if(__builtin_constant_p(a))	\
			asm("bclr.b %0,%c1" : : "di" (n), "dai" ((offset)+(unsigned char*)(a)));	\
		else	\
			if (offset !=0) \
				asm("bclr.b %0,%c1(%2)" : : "di" (n), "dai" (offset), "a" (a));	\
			else \
				asm("bclr.b %0,(%1)" : : "di" (n), "a" (a));	\
	}	\
	else	\
		asm("bclr.b %0,0(%2,%1.w)" : : "di" (n), "da" (offset), "a" (a));	\
	})

#define EXT_XORPIX_AN(a,offset,n) ({if(__builtin_constant_p(offset)) \
	{ \
		if(__builtin_constant_p(a)) \
			asm("bchg.b %0,%c1" : : "di" (n), "dai" ((offset)+(unsigned char*)(a))); \
		else \
			if (offset !=0) \
				asm("bchg.b %0,%c1(%2)" : : "di" (n), "dai" (offset), "a" (a)); \
			else \
				asm("bchg.b %0,(%1)" : : "di" (n), "a" (a)); \
	} \
	else \
		asm("bchg.b %0,0(%2,%1.w)" : : "di" (n), "da" (offset), "a" (a)); \
	})


#define EXT_GETPIX_AN(a,offset,n) ({char __result; if(__builtin_constant_p(offset)) \
	{ \
		if(__builtin_constant_p(a)) \
			asm("btst.b %1,%c2; sne.b %0" : "=d" (__result) : "di" (n), "dai" ((offset)+(unsigned char*)(a))); \
		else \
			if (offset !=0) \
				asm("btst.b %1,%c2(%3); sne.b %0" : "=d" (__result) : "di" (n), "dai" (offset), "a" (a)); \
			else \
				asm("btst.b %1,(%2); sne.b %0" : "=d" (__result) : "di" (n), "a" (a)); \
	} \
	else \
		asm("btst.b %1,0(%2,%3.w); sne.b %0" : "=d" (__result) : "di" (n), "da" (offset), "da" (a)); \
	__result;})


#define EXT_SETPIX(p,x,y) EXT_SETPIX_AN(p,EXT_PIXOFFSET(x,y),~(x))
#define EXT_CLRPIX(p,x,y) EXT_CLRPIX_AN(p,EXT_PIXOFFSET(x,y),~(x))
#define EXT_XORPIX(p,x,y) EXT_XORPIX_AN(p,EXT_PIXOFFSET(x,y),~(x))
#define EXT_GETPIX(p,x,y) EXT_GETPIX_AN(p,EXT_PIXOFFSET(x,y),~(x))



//-----------------------------------------------------------------------------
// double buffer variants of grayscale support routines
// NOTE: graph functions modify active plane with PortSet() !!
// 2.00 introduces new names, more compliant with the new names in TIGCCLIB.
//-----------------------------------------------------------------------------
extern void GrayClearScreen2B(void* lightplane,void* darkplane) __attribute__((__stkparm__));
extern void GrayClearScreen2B_R(register void* lightplane asm("%a0"),register void* darkplane asm("%a1")) __attribute__((__regparm__(2)));
extern void GrayDrawRect2B(short x0,short y0,short x1, short y1, short color,short fill,void* lightplane,void* darkplane) __attribute__((__stkparm__));
extern void GrayInvertRect2B(short x0,short y0,short x1, short y1,void* lightplane,void* darkplane) __attribute__((__stkparm__));
extern void GrayDrawLine2B(short x0,short y0,short x1, short y1, short color,void* lightplane,void* darkplane) __attribute__((__stkparm__));
extern void GrayFastDrawLine2B(short x0,short y0,short x1, short y1, short color,void* lightplane,void* darkplane) __attribute__((__stkparm__));
extern void GrayFastDrawHLine2B(short x0,short x1, short y, short color,void* lightplane,void* darkplane) __attribute__((__stkparm__));
extern void GrayDrawChar2B(short x,short y,char c,short attr,void* lightplane,void* darkplane) __attribute__((__stkparm__));
extern void GrayDrawStr2B(short x,short y,const char* s,short attr,void* lightplane,void* darkplane) __attribute__((__stkparm__));
extern void GrayDrawStrExt2B(short x,short y,const char* s,short attr,short font,void* lightplane,void* darkplane) __attribute__((__stkparm__));

// Deprecated names...
#define ClearGrayScreen2B GrayClearScreen2B
#define ClearGrayScreen2B_R GrayClearScreen2B_R
#define DrawGrayRect2B GrayDrawRect2B
#define InvertGrayRect2B GrayInvertRect2B
#define DrawGrayLine2B GrayDrawLine2B
#define FastDrawGrayLine2B GrayFastDrawLine2B
#define FastDrawGrayHLine2B GrayFastDrawHLine2B
#define DrawGrayChar2B GrayDrawChar2B
#define DrawGrayStr2B GrayDrawStr2B
#define DrawGrayStrExt2B GrayDrawStrExt2B


//-----------------------------------------------------------------------------
// Helper macros for functions from grayutil.h.
// NOTE: graph functions modify active plane with PortSet() !!
// 2.00 introduces new names, more compliant with the new names in TIGCCLIB.
//-----------------------------------------------------------------------------
#define GrayClearScreen()                    GrayClearScreen2B(GrayGetPlane(LIGHT_PLANE),GrayGetPlane(DARK_PLANE))
#define GrayClearScreen_R()                  GrayClearScreen2B_R(GrayGetPlane(LIGHT_PLANE),GrayGetPlane(DARK_PLANE))
#define GrayDrawRect(x0,y0,x1,y1,color,fill) GrayDrawRect2B(x0,y0,x1,y1,color,fill,GrayGetPlane(LIGHT_PLANE),GrayGetPlane(DARK_PLANE))
#define GrayInvertRect(x0,y0,x1,y1)          GrayInvertRect2B(x0,y0,x1,y1,GrayGetPlane(LIGHT_PLANE),GrayGetPlane(DARK_PLANE))
#define GrayDrawLine(x0,y0,x1,y1,color)      GrayDrawLine2B(x0,y0,x1,y1,color,GrayGetPlane(LIGHT_PLANE),GrayGetPlane(DARK_PLANE))
#define GrayFastDrawHLine(x0,x1,y,color)     GrayFastDrawHLine2B(x0,x1,y,color,GrayGetPlane(LIGHT_PLANE),GrayGetPlane(DARK_PLANE))
#define GrayFastDrawLine(x0,y0,x1,y1,color)  GrayFastDrawLine2B(x0,y0,x1,y1,color,GrayGetPlane(LIGHT_PLANE),GrayGetPlane(DARK_PLANE))
#define GrayDrawChar(x,y,c,attr)             GrayDrawChar2B(x,y,c,attr,GrayGetPlane(LIGHT_PLANE),GrayGetPlane(DARK_PLANE))
#define GrayDrawStr(x,y,s,attr)              GrayDrawStr2B(x,y,s,attr,GrayGetPlane(LIGHT_PLANE),GrayGetPlane(DARK_PLANE))
#define GrayDrawStrExt(x,y,s,attr,font)      GrayDrawStrExt2B(x,y,s,attr,font,GrayGetPlane(LIGHT_PLANE),GrayGetPlane(DARK_PLANE))

// Deprecated names...
#define ClearGrayScreen GrayClearScreen
#define ClearGrayScreen_R GrayClearScreen_R
#define DrawGrayRect GrayDrawRect
#define InvertGrayRect GrayInvertRect
#define DrawGrayLine GrayDrawLine
#define FastDrawGrayHLine GrayFastDrawHLine
#define FastDrawGrayLine GrayFastDrawLine
#define DrawGrayChar GrayDrawChar
#define DrawGrayStr GrayDrawStr
#define DrawGrayStrExt GrayDrawStrExt


//-----------------------------------------------------------------------------
// screen scrolling routines
//-----------------------------------------------------------------------------
extern void ScrollLeft160(unsigned short* buffer,unsigned short lines) __attribute__((__stkparm__));
extern void ScrollLeft240(unsigned short* buffer,unsigned short lines) __attribute__((__stkparm__));
extern void ScrollRight160(unsigned short* buffer,unsigned short lines) __attribute__((__stkparm__));
extern void ScrollRight240(unsigned short* buffer,unsigned short lines) __attribute__((__stkparm__));
extern void ScrollUp160(unsigned short* buffer,unsigned short lines) __attribute__((__stkparm__));
extern void ScrollUp240(unsigned short* buffer,unsigned short lines) __attribute__((__stkparm__));
extern void ScrollDown160(unsigned short* buffer,unsigned short lines) __attribute__((__stkparm__));
extern void ScrollDown240(unsigned short* buffer,unsigned short lines) __attribute__((__stkparm__));

extern void ScrollLeft160_R(unsigned short* buffer,unsigned short lines) __attribute__((__regparm__(2)));
extern void ScrollLeft240_R(unsigned short* buffer,unsigned short lines) __attribute__((__regparm__(2)));
extern void ScrollRight160_R(unsigned short* buffer,unsigned short lines) __attribute__((__regparm__(2)));
extern void ScrollRight240_R(unsigned short* buffer,unsigned short lines) __attribute__((__regparm__(2)));
extern void ScrollUp160_R(unsigned short* buffer,unsigned short lines) __attribute__((__regparm__(2)));
extern void ScrollUp240_R(unsigned short* buffer,unsigned short lines) __attribute__((__regparm__(2)));
extern void ScrollDown160_R(unsigned short* buffer,unsigned short lines) __attribute__((__regparm__(2)));
extern void ScrollDown240_R(unsigned short* buffer,unsigned short lines) __attribute__((__regparm__(2)));


//-----------------------------------------------------------------------------
// fast alternative functions for line drawing
// FastDrawLine(_R) rewritten by ExtendeD and a bit optimized by Lionel.
// FastLine_... written by jackiechan.
//-----------------------------------------------------------------------------
extern void FastDrawLine(void* plane,short x1,short y1,short x2,short y2,short mode) __attribute__((__stkparm__));
extern void FastDrawLine_R(register void* plane asm("%a0"),register short x1 asm("%d0"),register short y1 asm("%d1"),register short x2 asm("%d2"),register short y2 asm("%d3"),short mode) __attribute__((__stkparm__));

extern void FastDrawHLine(void* plane,short x1,short x2,short y,short mode) __attribute__((__stkparm__));
extern void FastDrawHLine_R(register void* plane asm("a0"), register short x1 asm("d0"), register short x2 asm("d1"), register short y asm("d2"),short mode) __attribute__((__stkparm__));

extern void FastDrawVLine(void* plane,short x,short y1,short y2,short mode) __attribute__((__stkparm__));
extern void FastDrawVLine_R(register void* plane asm("%a0"),register short x asm("%d0"),register short y1 asm("%d1"),register short y2 asm("%d2"),short mode) __attribute__((__stkparm__));

extern void FastLine_Draw_R(void *plane asm("%a0"),short x1 asm("%d0"),short y1 asm("%d1"),short x2 asm("%d2"),short y2 asm("%d3")) __attribute__((__regparm__));
extern void FastLine_Erase_R(void *plane asm("%a0"),short x1 asm("%d0"),short y1 asm("%d1"),short x2 asm("%d2"),short y2 asm("%d3")) __attribute__((__regparm__));
extern void FastLine_Invert_R(void *plane asm("%a0"),short x1 asm("%d0"),short y1 asm("%d1"),short x2 asm("%d2"),short y2 asm("%d3")) __attribute__((__regparm__));

//-----------------------------------------------------------------------------
// Functions for rectangle drawing. Much faster than the AMS routines.
//-----------------------------------------------------------------------------
extern void FastFillRect(void* plane,short x1,short y1,short x2,short y2,short mode) __attribute__((__stkparm__));
extern void FastFillRect_R(register void* plane asm("%a0"),register short x1 asm("%d0"),register short y1 asm("%d1"),register short x2 asm("%d2"),register short y2 asm("%d3"),short mode) __attribute__((__stkparm__));
extern void GrayFastFillRect_R(register void* dest1 asm("%a0"),register void* dest2 asm("%a1"), register short x1 asm("%d0"),register short y1 asm("%d1"),register short x2 asm("%d2"),register short y2 asm("%d3"),short color) __attribute__((__stkparm__));

extern void FastOutlineRect(void* plane,short x1,short y1,short x2,short y2,short mode) __attribute__((__stkparm__));
extern void FastOutlineRect_R(register void* plane asm("%a0"),register short x1 asm("%d0"),register short y1 asm("%d1"),register short x2 asm("%d2"),register short y2 asm("%d3"),short mode) __attribute__((__stkparm__));
extern void GrayFastOutlineRect_R(register void* dest1 asm("%a0"),register void* dest2 asm("%a1"), register short x1 asm("%d0"),register short y1 asm("%d1"),register short x2 asm("%d2"),register short y2 asm("%d3"),short color) __attribute__((__stkparm__));

extern void FastEraseRect160_R(register void* plane asm("%a0"),register short starty asm("%d0"), register unsigned short lines asm("%d1")) __attribute__((__regparm__(3)));
extern void FastEraseRect240_R(register void* plane asm("%a0"),register short starty asm("%d0"), register unsigned short lines asm("%d1")) __attribute__((__regparm__(3)));
extern void GrayFastEraseRect2B160_R(register void* lightplane asm("%a0"),register void* darkplane asm("%a1"),register short starty asm("%d0"), register unsigned short lines asm("%d1")) __attribute__((__regparm__(4)));
extern void GrayFastEraseRect2B240_R(register void* lightplane asm("%a0"),register void* darkplane asm("%a1"),register short starty asm("%d0"), register unsigned short lines asm("%d1")) __attribute__((__regparm__(4)));
extern void FastFillRect160_R(register void* plane asm("%a0"),register short starty asm("%d0"), register unsigned short lines asm("%d1")) __attribute__((__regparm__(3)));
extern void FastFillRect240_R(register void* plane asm("%a0"),register short starty asm("%d0"), register unsigned short lines asm("%d1")) __attribute__((__regparm__(3)));
extern void GrayFastFillRect2B160_R(register void* lightplane asm("%a0"),register void* darkplane asm("%a1"),register short starty asm("%d0"), register unsigned short lines asm("%d1")) __attribute__((__regparm__(4)));
extern void GrayFastFillRect2B240_R(register void* lightplane asm("%a0"),register void* darkplane asm("%a1"),register short starty asm("%d0"), register unsigned short lines asm("%d1")) __attribute__((__regparm__(4)));
extern void FastInvertRect160_R(register void* plane asm("%a0"),register short starty asm("%d0"), register unsigned short lines asm("%d1")) __attribute__((__regparm__(3)));
extern void FastInvertRect240_R(register void* plane asm("%a0"),register short starty asm("%d0"), register unsigned short lines asm("%d1")) __attribute__((__regparm__(3)));
extern void GrayFastInvertRect2B160_R(register void* lightplane asm("%a0"),register void* darkplane asm("%a1"),register short starty asm("%d0"), register unsigned short lines asm("%d1")) __attribute__((__regparm__(4)));
extern void GrayFastInvertRect2B240_R(register void* lightplane asm("%a0"),register void* darkplane asm("%a1"),register short starty asm("%d0"), register unsigned short lines asm("%d1")) __attribute__((__regparm__(4)));

extern void FastEraseRectX8_R(register void* plane asm("%a0"),register short startcol asm("%d0"),register short startrow asm("%d1"),register unsigned short nrlines asm("%d2"),register unsigned short bytewidth asm("%d3")) __attribute__((__regparm__(5)));
extern void FastFillRectX8_R(register void* plane asm("%a0"),register short startcol asm("%d0"),register short startrow asm("%d1"),register unsigned short nrlines asm("%d2"),register unsigned short bytewidth asm("%d3")) __attribute__((__regparm__(5)));
extern void FastInvertRectX8_R(register void* plane asm("%a0"),register short startcol asm("%d0"),register short startrow asm("%d1"),register unsigned short nrlines asm("%d2"),register unsigned short bytewidth asm("%d3")) __attribute__((__regparm__(5)));

extern void FastFilledRect_Draw_R(register void* plane asm("%a0"),register short x1 asm("%d0"),register short y1 asm("%d1"),register short x2 asm("%d2"),register short y2 asm("%d3")) __attribute__((__regparm__(5)));
extern void FastFilledRect_Erase_R(register void* plane asm("%a0"),register short x1 asm("%d0"),register short y1 asm("%d1"),register short x2 asm("%d2"),register short y2 asm("%d3")) __attribute__((__regparm__(5)));
extern void FastFilledRect_Invert_R(register void* plane asm("%a0"),register short x1 asm("%d0"),register short y1 asm("%d1"),register short x2 asm("%d2"),register short y2 asm("%d3")) __attribute__((__regparm__(5)));


//-----------------------------------------------------------------------------
// sprite scaling routines by Jim Haskell (jimhaskell@yahoo.com)
//
// scales a square input sprite to any extent (WARNING: no clipping is done!)
// [routines slightly modified to fit the needs, very slightly optimized]
//
//
// Two scaling routines optimized for a particular size.
//-----------------------------------------------------------------------------
extern void ScaleSprite8_OR(unsigned char *sprite,unsigned char *dest,short x0,short y0,short sizex,short sizey) __attribute__((__stkparm__));
extern void ScaleSprite16_OR(unsigned short *sprite,unsigned char *dest,short x0,short y0,short sizex,short sizey) __attribute__((__stkparm__));
extern void ScaleSprite32_OR(unsigned long *sprite,unsigned char *dest,short x0,short y0,short sizex,short sizey) __attribute__((__stkparm__));
extern void ScaleSprite64_OR(unsigned long long *sprite,unsigned char *dest,short x0,short y0,short sizex,short sizey) __attribute__((__stkparm__));

extern void ScaleSprite8_AND(unsigned char *sprite,unsigned char *dest,short x0,short y0,short sizex,short sizey) __attribute__((__stkparm__));
extern void ScaleSprite16_AND(unsigned short *sprite,unsigned char *dest,short x0,short y0,short sizex,short sizey) __attribute__((__stkparm__));
extern void ScaleSprite32_AND(unsigned long *sprite,unsigned char *dest,short x0,short y0,short sizex,short sizey) __attribute__((__stkparm__));
extern void ScaleSprite64_AND(unsigned long long *sprite,unsigned char *dest,short x0,short y0,short sizex,short sizey) __attribute__((__stkparm__));

extern void ScaleSprite8_XOR(unsigned char *sprite,unsigned char *dest,short x0,short y0,short sizex,short sizey) __attribute__((__stkparm__));
extern void ScaleSprite16_XOR(unsigned short *sprite,unsigned char *dest,short x0,short y0,short sizex,short sizey) __attribute__((__stkparm__));
extern void ScaleSprite32_XOR(unsigned long *sprite,unsigned char *dest,short x0,short y0,short sizex,short sizey) __attribute__((__stkparm__));
extern void ScaleSprite64_XOR(unsigned long long *sprite,unsigned char *dest,short x0,short y0,short sizex,short sizey) __attribute__((__stkparm__));


extern void DoubleSpriteDimensionsX8_R(register short h asm("%d0"),register unsigned char* src asm("%a0"),register short bytewidth asm("%d1"),register unsigned char* dest asm("%a1")) __attribute__((__regparm__(4)));
extern void DoubleSpriteDimensions16x16_R(register unsigned short* src asm("%a0"),register unsigned long* dest asm("%a1")) __attribute__((__regparm__(2)));




//-----------------------------------------------------------------------------
// fast copy routine for a complete screen (240x128 pixels == 3840 bytes)
//-----------------------------------------------------------------------------
extern void FastCopyScreen(void* src,void* dest) __attribute__((__stkparm__));
extern void FastCopyScreen_R(register void* src asm("%a0"),register void* dest asm("%a1")) __attribute__((__regparm__(2)));


//-----------------------------------------------------------------------------
// floodfill routines (courtesy of Zeljko Juric)
//-----------------------------------------------------------------------------
extern void FloodFill(short x,short y,unsigned short shade,void* tmpplane,void* dest) __attribute__((__stkparm__));
extern void FloodFill_R(short x,short y,unsigned short shade,void* tmpplane,void* dest) __attribute__((__regparm__(5)));

extern void FloodFillMF(short x,short y,unsigned short shade,void* dest) __attribute__((__stkparm__));
extern void FloodFillMF_R(short x,short y,unsigned short shade,void* dest) __attribute__((__regparm__(4)));


//-----------------------------------------------------------------------------
// NON-CLIPPED SINGLE PLANE SPRITE FUNCTIONS
// Both __stkparm__ and __regparm__ functions are available.
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// fast alternative functions for SpriteX() functions: AND, OR, XOR.
// fast additional sprite functions: BLIT, RPLC (BLIT with hard-coded white
// blitmask) MASK.
// fast alternative functions for BitmapGet() function (incompatible): Get.
//-----------------------------------------------------------------------------
extern void Sprite8_AND(short x,short y,short h,unsigned char *sprt,void *dest) __attribute__((__stkparm__));
extern void Sprite8_AND_R(register short x asm("%d0"),register short y asm("%d1"),register short h asm("%d2"),register unsigned char *sprt asm("%a1"),register void *dest asm("%a0")) __attribute__((__regparm__));

extern void Sprite8_BLIT(short x,short y,short h,unsigned char *sprt,unsigned char maskval,void *dest) __attribute__((__stkparm__));
extern void Sprite8_BLIT_R(register short x asm("%d0"),register short y asm("%d1"),register short h asm("%d2"),register unsigned char *sprt asm("%a1"),register unsigned char maskval asm("%d3"),register void *dest asm("%a0"));

extern void Sprite8Get(short x,short y,short h,void* src,unsigned char* dest) __attribute__((__stkparm__));
extern void Sprite8Get_R(register short x asm("%d0"),register short y asm("%d1"),register short h asm("%d2"),register void* src asm("%a0"),register unsigned char* dest asm("%a1")) __attribute__((__regparm__));

extern void Sprite8_MASK(short x,short y,short h,unsigned char *sprt,unsigned char *mask,void *dest) __attribute__((__stkparm__));
extern void Sprite8_MASK_R(register short x asm("%d0"),register short y asm("%d1"),register short h asm("%d2"),register unsigned char *sprt asm("%a1"),unsigned char *mask,register void *dest asm("%a0")) __attribute__((__stkparm__));

extern void Sprite8_OR(short x,short y,short h,unsigned char *sprt,void *dest) __attribute__((__stkparm__));
extern void Sprite8_OR_R(register short x asm("%d0"),register short y asm("%d1"),register short h asm("%d2"),register unsigned char *sprt asm("%a1"),register void *dest asm("%a0")) __attribute__((__regparm__));

extern void Sprite8_RPLC_R(register short x asm("%d0"),register short y asm("%d1"),register short h asm("%d2"),register unsigned char *sprt asm("%a1"),register void *dest asm("%a0")) __attribute__((__regparm__));

extern void Sprite8_XOR(short x,short y,short h,unsigned char *sprt,void *dest) __attribute__((__stkparm__));
extern void Sprite8_XOR_R(register short x asm("%d0"),register short y asm("%d1"),register short h asm("%d2"),register unsigned char *sprt asm("%a1"),register void *dest asm("%a0")) __attribute__((__regparm__));


extern void Sprite16_AND(short x,short y,short h,unsigned short *sprt,void *dest) __attribute__((__stkparm__));
extern void Sprite16_AND_R(register short x asm("%d0"),register short y asm("%d1"),register short h asm("%d2"),register unsigned short *sprt asm("%a1"),register void *dest asm("%a0")) __attribute__((__regparm__));

extern void Sprite16_BLIT(short x,short y,short h,unsigned short *sprt,unsigned short maskval,void *dest) __attribute__((__stkparm__));
extern void Sprite16_BLIT_R(register short x asm("%d0"),register short y asm("%d1"),register short h asm("%d2"),register unsigned short *sprt asm("%a1"),register unsigned short maskval asm("%d3"),register void *dest asm("%a0"));

extern void Sprite16Get(short x,short y,short h,void* src,unsigned short* dest) __attribute__((__stkparm__));
extern void Sprite16Get_R(register short x asm("%d0"),register short y asm("%d1"),register short h asm("%d2"),register void* src asm("%a0"),register unsigned short* dest asm("%a1")) __attribute__((__regparm__));

extern void Sprite16_MASK(short x,short y,short h,unsigned short *sprt,unsigned short *mask,void *dest) __attribute__((__stkparm__));
extern void Sprite16_MASK_R(register short x asm("%d0"),register short y asm("%d1"),register short h asm("%d2"),register unsigned short *sprt asm("%a1"),unsigned short *mask,register void *dest asm("%a0")) __attribute__((__stkparm__));

extern void Sprite16_OR(short x,short y,short h,unsigned short *sprt,void *dest) __attribute__((__stkparm__));
extern void Sprite16_OR_R(register short x asm("%d0"),register short y asm("%d1"),register short h asm("%d2"),register unsigned short *sprt asm("%a1"),register void *dest asm("%a0")) __attribute__((__regparm__));

extern void Sprite16_RPLC_R(register short x asm("%d0"),register short y asm("%d1"),register short h asm("%d2"),register unsigned short *sprt asm("%a1"),register void *dest asm("%a0")) __attribute__((__regparm__));

extern void Sprite16_XOR(short x,short y,short h,unsigned short *sprt,void *dest) __attribute__((__stkparm__));
extern void Sprite16_XOR_R(register short x asm("%d0"),register short y asm("%d1"),register short h asm("%d2"),register unsigned short *sprt asm("%a1"),register void *dest asm("%a0")) __attribute__((__regparm__));


extern void Sprite32_AND(short x,short y,short h,unsigned long *sprt,void *dest) __attribute__((__stkparm__));
extern void Sprite32_AND_R(register short x asm("%d0"),register short y asm("%d1"),register short h asm("%d2"),register unsigned long *sprt asm("%a1"),register void *dest asm("%a0")) __attribute__((__regparm__));

extern void Sprite32_BLIT(short x,short y,short h,unsigned long *sprt,unsigned long maskval,void *dest) __attribute__((__stkparm__));
extern void Sprite32_BLIT_R(register short x asm("%d0"),register short y asm("%d1"),register short h asm("%d2"),register unsigned long *sprt asm("%a1"),register unsigned long maskval asm("%d3"),register void *dest asm("%a0"));

extern void Sprite32Get(short x,short y,short h,void* src,unsigned long* dest) __attribute__((__stkparm__));
extern void Sprite32Get_R(register short x asm("%d0"),register short y asm("%d1"),register short h asm("%d2"),register void* src asm("%a0"),register unsigned long* dest asm("%a1")) __attribute__((__regparm__));

extern void Sprite32_MASK(short x,short y,short h,unsigned long *sprt,unsigned long *mask,void *dest) __attribute__((__stkparm__));
extern void Sprite32_MASK_R(register short x asm("%d0"),register short y asm("%d1"),register short h asm("%d2"),register unsigned long *sprt asm("%a1"),unsigned long *mask,register void *dest asm("%a0")) __attribute__((__stkparm__));

extern void Sprite32_OR(short x,short y,short h,unsigned long *sprt,void *dest) __attribute__((__stkparm__));
extern void Sprite32_OR_R(register short x asm("%d0"),register short y asm("%d1"),register short h asm("%d2"),register unsigned long *sprt asm("%a1"),register void *dest asm("%a0")) __attribute__((__regparm__));

extern void Sprite32_RPLC_R(register short x asm("%d0"),register short y asm("%d1"),register short h asm("%d2"),register unsigned long *sprt asm("%a1"),register void *dest asm("%a0")) __attribute__((__regparm__));

extern void Sprite32_XOR(short x,short y,short h,unsigned long *sprt,void *dest) __attribute__((__stkparm__));
extern void Sprite32_XOR_R(register short x asm("%d0"),register short y asm("%d1"),register short h asm("%d2"),register unsigned long *sprt asm("%a1"),register void *dest asm("%a0")) __attribute__((__regparm__));


extern void SpriteX8_AND(short x,short y,unsigned char *sprt,short w,void *dest) __attribute__((__stkparm__));
extern void SpriteX8_AND_R(register short x asm("%d0"), register short y asm("%d1"),register short h asm("%d2"),register unsigned char *sprt asm("%a1"),register short w asm("%d3"),register void *dest asm("%a0")) __attribute__((__regparm__));

extern void SpriteX8_BLIT(short x,short y,short h,unsigned char *sprt,unsigned char *maskval,short bytewidth,void *dest) __attribute__((__stkparm__));
extern void SpriteX8_BLIT_R(register short x asm("%d0"),register short y asm("%d1"),register short h asm("%d2"),register unsigned char *sprt asm("%a1"),unsigned char *maskval,register short w asm("%d3"),register void *dest asm("%a0")) __attribute__((__stkparm__));

extern void SpriteX8Get(short x,short y,short h,void* src,unsigned char* dest,short bytewidth) __attribute__((__stkparm__));
extern void SpriteX8Get_R(register short x asm("%d0"),register short y asm("%d1"),register short h asm("%d2"),register void *src asm("%a1"),register unsigned char *dest asm("%a0"),register short bytewidth asm("%d3")) __attribute__((__regparm__));

extern void SpriteX8_MASK(short x,short y,short h,unsigned char *sprt,unsigned char *mask,short bytewidth,void *dest) __attribute__((__stkparm__));
extern void SpriteX8_MASK_R(register short x asm("%d0"),register short y asm("%d1"),register short h asm("%d2"),register unsigned char *sprt asm("%a1"),unsigned char *mask,register short w asm("%d3"),register void *dest asm("%a0")) __attribute__((__stkparm__));

extern void SpriteX8_OR(short x,short y,short h,unsigned char *sprt,short w,void *dest) __attribute__((__stkparm__));
extern void SpriteX8_OR_R(register short x asm("%d0"), register short y asm("%d1"),register short h asm("%d2"),register unsigned char *sprt asm("%a1"),register short w asm("%d3"),register void *dest asm("%a0")) __attribute__((__regparm__));

extern void SpriteX8_XOR(short x,short y,short h,unsigned char *sprt,short w,void *dest) __attribute__((__stkparm__));
extern void SpriteX8_XOR_R(register short x asm("%d0"), register short y asm("%d1"),register short h asm("%d2"),register unsigned char *sprt asm("%a1"),register short w asm("%d3"),register void *dest asm("%a0")) __attribute__((__regparm__));


//-----------------------------------------------------------------------------
// CLIPPED SINGLE PLANE SPRITE FUNCTIONS
// Only __regparm__ functions are available, __stkparm__ functions are NOT
// planned.
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// fast alternative functions for SpriteX() functions: AND, OR, XOR.
// fast additional sprite functions: BLIT, RPLC (BLIT with hard-coded white
// blitmask) MASK.
//-----------------------------------------------------------------------------
extern void ClipSprite8_AND_R(register short x asm("%d0"),register short y asm("%d1"),register short h asm("%d2"),register unsigned char *sprt asm("%a1"),register void *dest asm("%a0"));
extern void ClipSprite8_BLIT_R(register short x asm("%d0"),register short y asm("%d1"),register short h asm("%d2"),register unsigned char *sprt asm("%a1"),register unsigned char maskval asm("%d3"),register void *dest asm("%a0"));
extern void ClipSprite8_MASK_R(register short x asm("%d0"),register short y asm("%d1"),register short h asm("%d2"),register unsigned char *sprt asm("%a1"),unsigned char *mask,register void *dest asm("%a0")) __attribute__((__stkparm__));
extern void ClipSprite8_OR_R(register short x asm("%d0"),register short y asm("%d1"),register short h asm("%d2"),register unsigned char *sprt asm("%a1"),register void *dest asm("%a0"));
extern void ClipSprite8_RPLC_R(register short x asm("%d0"),register short y asm("%d1"),register short h asm("%d2"),register unsigned char *sprt asm("%a1"),register void *dest asm("%a0"));
extern void ClipSprite8_XOR_R(register short x asm("%d0"),register short y asm("%d1"),register short h asm("%d2"),register unsigned char *sprt asm("%a1"),register void *dest asm("%a0"));


extern void ClipSprite16_AND_R(register short x asm("%d0"),register short y asm("%d1"),register short h asm("%d2"),register unsigned short *sprt asm("%a1"),register void *dest asm("%a0"));
extern void ClipSprite16_BLIT_R(register short x asm("%d0"),register short y asm("%d1"),register short h asm("%d2"),register unsigned short *sprt asm("%a1"),register unsigned short maskval asm("%d3"),register void *dest asm("%a0"));
extern void ClipSprite16_MASK_R(register short x asm("%d0"),register short y asm("%d1"),register short h asm("%d2"),register unsigned short *sprt asm("%a1"),unsigned short *mask,register void *dest asm("%a0")) __attribute__((__stkparm__));
extern void ClipSprite16_OR_R(register short x asm("%d0"),register short y asm("%d1"),register short h asm("%d2"),register unsigned short *sprt asm("%a1"),register void *dest asm("%a0"));
extern void ClipSprite16_RPLC_R(register short x asm("%d0"),register short y asm("%d1"),register short h asm("%d2"),register unsigned short *sprt asm("%a1"),register void *dest asm("%a0"));
extern void ClipSprite16_XOR_R(register short x asm("%d0"),register short y asm("%d1"),register short h asm("%d2"),register unsigned short *sprt asm("%a1"),register void *dest asm("%a0"));


extern void ClipSprite32_AND_R(register short x asm("%d0"),register short y asm("%d1"),register short h asm("%d2"),register unsigned long *sprt asm("%a1"),register void *dest asm("%a0"));
extern void ClipSprite32_BLIT_R(register short x asm("%d0"),register short y asm("%d1"),register short h asm("%d2"),register unsigned long *sprt asm("%a1"),register unsigned long maskval asm("%d3"),register void *dest asm("%a0"));
extern void ClipSprite32_MASK_R(register short x asm("%d0"),register short y asm("%d1"),register short h asm("%d2"),register unsigned long *sprt asm("%a1"),unsigned long *mask,register void *dest asm("%a0")) __attribute__((__stkparm__));
extern void ClipSprite32_OR_R(register short x asm("%d0"),register short y asm("%d1"),register short h asm("%d2"),register unsigned long *sprt asm("%a1"),register void *dest asm("%a0"));
extern void ClipSprite32_RPLC_R(register short x asm("%d0"),register short y asm("%d1"),register short h asm("%d2"),register unsigned long *sprt asm("%a1"),register void *dest asm("%a0"));
extern void ClipSprite32_XOR_R(register short x asm("%d0"),register short y asm("%d1"),register short h asm("%d2"),register unsigned long *sprt asm("%a1"),register void *dest asm("%a0"));


// MastaZog asked for ClipSpriteX8_X_R routines. They will be written, although
// their priority is rather low.



//-----------------------------------------------------------------------------
// NON-CLIPPED TWO-PLANE SPRITE FUNCTIONS
// Both __stkparm__ and __regparm__ functions are available.
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// fast alternative functions for SpriteX() functions: AND, OR, XOR.
// fast additional sprite functions: BLIT, RPLC (BLIT with hard-coded white
// blitmask), MASK, TRANW/TRANB, SHADOW/ISHADOW, SHADOW2/ISHADOW2 
// (SHADOW/ISHADOW with fixed blitmask).
//-----------------------------------------------------------------------------
extern void GraySprite8_AND(short x,short y,short h,unsigned char* sprite1,unsigned char* sprite2,void* dest1,void* dest2) __attribute__((__stkparm__));
extern void GraySprite8_AND_R(register short x asm("%d0"),register short y asm("%d1"),register short h asm("%d2"),unsigned char *sprt0,unsigned char *sprt1,register void *dest0 asm("%a0"),register void *dest1 asm("%a1")) __attribute__((__stkparm__));

extern void GraySprite8_BLIT(short x,short y,short h,unsigned char* sprite1,unsigned char* sprite2,unsigned char maskval,void* dest1,void* dest2) __attribute__((__stkparm__));
extern void GraySprite8_BLIT_R(register short x asm("%d0"),register short y asm("%d1"),register short h asm("%d2"),unsigned char *sprt0,unsigned char *sprt1,register unsigned char maskval asm("%d3"),register void *dest0 asm("%a0"),register void *dest1 asm("%a1")) __attribute__((__stkparm__));

extern void GraySprite8_ISHADOW_R(register short x asm("%d0"),register short y asm("%d1"),register short h asm("%d2"),unsigned char* mask,register void *dest0 asm("%a0"),register void *dest1 asm("%a1")) __attribute__((__stkparm__));
extern void GraySprite8_ISHADOW2_R(register short x asm("%d0"),register short y asm("%d1"),register short h asm("%d2"),register unsigned char blitmask asm("%d3"),register void *dest0 asm("%a0"),register void *dest1 asm("%a1")) __attribute__((__regparm__(6)));

extern void GraySprite8_MASK(short x,short y,short h,unsigned char* sprite1,unsigned char* sprite2,unsigned char* mask1,unsigned char* mask2,void* dest1,void* dest2) __attribute__((__stkparm__));
extern void GraySprite8_MASK_R(register short x asm("%d0"),register short y asm("%d1"),register short h asm("%d2"),unsigned char *sprt0,unsigned char *sprt1,unsigned char *mask0,unsigned char *mask1,register void *dest0 asm("%a0"),register void *dest1 asm("%a1")) __attribute__((__stkparm__));

extern void GraySprite8_OR(short x,short y,short h,unsigned char* sprite1,unsigned char* sprite2,void* dest1,void* dest2) __attribute__((__stkparm__));
extern void GraySprite8_OR_R(register short x asm("%d0"),register short y asm("%d1"),register short h asm("%d2"),unsigned char *sprt0,unsigned char *sprt1,register void *dest0 asm("%a0"),register void *dest1 asm("%a1")) __attribute__((__stkparm__));

extern void GraySprite8_RPLC_R(register short x asm("%d0"),register short y asm("%d1"),register short h asm("%d2"),unsigned char *sprt0,unsigned char *sprt1,register void *dest0 asm("%a0"),register void *dest1 asm("%a1")) __attribute__((__stkparm__));

extern void GraySprite8_SHADOW_R(register short x asm("%d0"),register short y asm("%d1"),register short h asm("%d2"),unsigned char* mask,register void *dest0 asm("%a0"),register void *dest1 asm("%a1")) __attribute__((__stkparm__));
extern void GraySprite8_SHADOW2_R(register short x asm("%d0"),register short y asm("%d1"),register short h asm("%d2"),register unsigned char blitmask asm("%d3"),register void *dest0 asm("%a0"),register void *dest1 asm("%a1")) __attribute__((__regparm__(6)));

extern void GraySprite8_TRANB_R(register short x asm("%d0"),register short y asm("%d1"),register short h asm("%d2"),unsigned char *sprt0,unsigned char *sprt1,register void *dest0 asm("%a0"),register void *dest1 asm("%a1")) __attribute__((__stkparm__));
extern void GraySprite8_TRANW_R(register short x asm("%d0"),register short y asm("%d1"),register short h asm("%d2"),unsigned char *sprt0,unsigned char *sprt1,register void *dest0 asm("%a0"),register void *dest1 asm("%a1")) __attribute__((__stkparm__));

extern void GraySprite8_XOR(short x,short y,short h,unsigned char* sprite1,unsigned char* sprite2,void* dest1,void* dest2) __attribute__((__stkparm__));
extern void GraySprite8_XOR_R(register short x asm("%d0"),register short y asm("%d1"),register short h asm("%d2"),unsigned char *sprt0,unsigned char *sprt1,register void *dest0 asm("%a0"),register void *dest1 asm("%a1")) __attribute__((__stkparm__));


extern void GraySprite16_AND(short x,short y,short h,unsigned short* sprite1,unsigned short* sprite2,void* dest1,void* dest2) __attribute__((__stkparm__));
extern void GraySprite16_AND_R(register short x asm("%d0"),register short y asm("%d1"),register short h asm("%d2"),unsigned short *sprt0,unsigned short *sprt1,register void *dest0 asm("%a0"),register void *dest1 asm("%a1")) __attribute__((__stkparm__));

extern void GraySprite16_BLIT(short x,short y,short h,unsigned short* sprite1,unsigned short* sprite2,unsigned short maskval,void* dest1,void* dest2) __attribute__((__stkparm__));
extern void GraySprite16_BLIT_R(register short x asm("%d0"),register short y asm("%d1"),register short h asm("%d2"),unsigned short *sprt0,unsigned short *sprt1,register unsigned short maskval asm("%d3"),register void *dest0 asm("%a0"),register void *dest1 asm("%a1")) __attribute__((__stkparm__));

extern void GraySprite16_ISHADOW_R(register short x asm("%d0"),register short y asm("%d1"),register short h asm("%d2"),unsigned short* mask,register void *dest0 asm("%a0"),register void *dest1 asm("%a1")) __attribute__((__stkparm__));
extern void GraySprite16_ISHADOW2_R(register short x asm("%d0"),register short y asm("%d1"),register short h asm("%d2"),register unsigned short blitmask asm("%d3"),register void *dest0 asm("%a0"),register void *dest1 asm("%a1")) __attribute__((__regparm__(6)));

extern void GraySprite16_MASK(short x,short y,short h,unsigned short* sprite1,unsigned short* sprite2,unsigned short* mask1,unsigned short* mask2,void* dest1,void* dest2) __attribute__((__stkparm__));
extern void GraySprite16_MASK_R(register short x asm("%d0"),register short y asm("%d1"),register short h asm("%d2"),unsigned short *sprt0,unsigned short *sprt1,unsigned short *mask0,unsigned short *mask1,register void *dest0 asm("%a0"),register void *dest1 asm("%a1")) __attribute__((__stkparm__));

extern void GraySprite16_OR(short x,short y,short h,unsigned short* sprite1,unsigned short* sprite2,void* dest1,void* dest2) __attribute__((__stkparm__));
extern void GraySprite16_OR_R(register short x asm("%d0"),register short y asm("%d1"),register short h asm("%d2"),unsigned short *sprt0,unsigned short *sprt1,register void *dest0 asm("%a0"),register void *dest1 asm("%a1")) __attribute__((__stkparm__));

extern void GraySprite16_RPLC_R(register short x asm("%d0"),register short y asm("%d1"),register short h asm("%d2"),unsigned short *sprt0,unsigned short *sprt1,register void *dest0 asm("%a0"),register void *dest1 asm("%a1")) __attribute__((__stkparm__));

extern void GraySprite16_SHADOW_R(register short x asm("%d0"),register short y asm("%d1"),register short h asm("%d2"),unsigned short* mask,register void *dest0 asm("%a0"),register void *dest1 asm("%a1")) __attribute__((__stkparm__));
extern void GraySprite16_SHADOW2_R(register short x asm("%d0"),register short y asm("%d1"),register short h asm("%d2"),register unsigned short blitmask asm("%d3"),register void *dest0 asm("%a0"),register void *dest1 asm("%a1")) __attribute__((__regparm__(6)));

extern void GraySprite16_TRANB_R(register short x asm("%d0"),register short y asm("%d1"),register short h asm("%d2"),unsigned short *sprt0,unsigned short *sprt1,register void *dest0 asm("%a0"),register void *dest1 asm("%a1")) __attribute__((__stkparm__));
extern void GraySprite16_TRANW_R(register short x asm("%d0"),register short y asm("%d1"),register short h asm("%d2"),unsigned short *sprt0,unsigned short *sprt1,register void *dest0 asm("%a0"),register void *dest1 asm("%a1")) __attribute__((__stkparm__));

extern void GraySprite16_XOR(short x,short y,short h,unsigned short* sprite1,unsigned short* sprite2,void* dest1,void* dest2) __attribute__((__stkparm__));
extern void GraySprite16_XOR_R(register short x asm("%d0"),register short y asm("%d1"),register short h asm("%d2"),unsigned short *sprt0,unsigned short *sprt1,register void *dest0 asm("%a0"),register void *dest1 asm("%a1")) __attribute__((__stkparm__));


extern void GraySprite32_AND(short x,short y,short h,unsigned long* sprite1,unsigned long* sprite2,void* dest1,void* dest2) __attribute__((__stkparm__));
extern void GraySprite32_AND_R(register short x asm("%d0"),register short y asm("%d1"),register short h asm("%d2"),unsigned long *sprt0,unsigned long *sprt1,register void *dest0 asm("%a0"),register void *dest1 asm("%a1")) __attribute__((__stkparm__));

extern void GraySprite32_BLIT(short x,short y,short h,unsigned long* sprite1,unsigned long* sprite2,unsigned long maskval,void* dest1,void* dest2) __attribute__((__stkparm__));
extern void GraySprite32_BLIT_R(register short x asm("%d0"),register short y asm("%d1"),register short h asm("%d2"),unsigned long *sprt0,unsigned long *sprt1,register unsigned long maskval asm("%d3"),register void *dest0 asm("%a0"),register void *dest1 asm("%a1")) __attribute__((__stkparm__));

extern void GraySprite32_ISHADOW_R(register short x asm("%d0"),register short y asm("%d1"),register short h asm("%d2"),unsigned long* mask,register void *dest0 asm("%a0"),register void *dest1 asm("%a1")) __attribute__((__stkparm__));
extern void GraySprite32_ISHADOW2_R(register short x asm("%d0"),register short y asm("%d1"),register short h asm("%d2"),register unsigned long blitmask asm("%d3"),register void *dest0 asm("%a0"),register void *dest1 asm("%a1")) __attribute__((__regparm__(6)));

extern void GraySprite32_MASK(short x,short y,short h,unsigned long* sprite1,unsigned long* sprite2,unsigned long* mask1,unsigned long* mask2,void* dest1,void* dest2) __attribute__((__stkparm__));
extern void GraySprite32_MASK_R(register short x asm("%d0"),register short y asm("%d1"),register short h asm("%d2"),unsigned long *sprt0,unsigned long *sprt1,unsigned long *mask0,unsigned long *mask1,register void *dest0 asm("%a0"),register void *dest1 asm("%a1")) __attribute__((__stkparm__));

extern void GraySprite32_OR(short x,short y,short h,unsigned long* sprite1,unsigned long* sprite2,void* dest1,void* dest2) __attribute__((__stkparm__));
extern void GraySprite32_OR_R(register short x asm("%d0"),register short y asm("%d1"),register short h asm("%d2"),unsigned long *sprt0,unsigned long *sprt1,register void *dest0 asm("%a0"),register void *dest1 asm("%a1")) __attribute__((__stkparm__));

extern void GraySprite32_RPLC_R(register short x asm("%d0"),register short y asm("%d1"),register short h asm("%d2"),unsigned long *sprt0,unsigned long *sprt1,register void *dest0 asm("%a0"),register void *dest1 asm("%a1")) __attribute__((__stkparm__));

extern void GraySprite32_SHADOW_R(register short x asm("%d0"),register short y asm("%d1"),register short h asm("%d2"),unsigned long* mask,register void *dest0 asm("%a0"),register void *dest1 asm("%a1")) __attribute__((__stkparm__));
extern void GraySprite32_SHADOW2_R(register short x asm("%d0"),register short y asm("%d1"),register short h asm("%d2"),register unsigned long blitmask asm("%d3"),register void *dest0 asm("%a0"),register void *dest1 asm("%a1")) __attribute__((__regparm__(6)));

extern void GraySprite32_TRANB_R(register short x asm("%d0"),register short y asm("%d1"),register short h asm("%d2"),unsigned long *sprt0,unsigned long *sprt1,register void *dest0 asm("%a0"),register void *dest1 asm("%a1")) __attribute__((__stkparm__));
extern void GraySprite32_TRANW_R(register short x asm("%d0"),register short y asm("%d1"),register short h asm("%d2"),unsigned long *sprt0,unsigned long *sprt1,register void *dest0 asm("%a0"),register void *dest1 asm("%a1")) __attribute__((__stkparm__));

extern void GraySprite32_XOR(short x,short y,short h,unsigned long* sprite1,unsigned long* sprite2,void* dest1,void* dest2) __attribute__((__stkparm__));
extern void GraySprite32_XOR_R(register short x asm("%d0"),register short y asm("%d1"),register short h asm("%d2"),unsigned long *sprt0,unsigned long *sprt1,register void *dest0 asm("%a0"),register void *dest1 asm("%a1")) __attribute__((__stkparm__));


// Not currently rewritten, transparency routines and __regparm__ versions
// were not written either...
extern void GraySpriteX8_AND(short x,short y,short h,unsigned char* sprite1,unsigned char* sprite2,short bytewidth,void* dest1,void* dest2) __attribute__((__stkparm__));
extern void GraySpriteX8_BLIT(short x,short y,short h,unsigned char* sprite1,unsigned char* sprite2,unsigned char* maskval,short bytewidth,void* dest1,void* dest2) __attribute__((__stkparm__));
extern void GraySpriteX8_MASK(short x,short y,short h,unsigned char* sprite1,unsigned char* sprite2,unsigned char* mask1,unsigned char* mask2,short bytewidth,void* dest1,void* dest2) __attribute__((__stkparm__));
extern void GraySpriteX8_OR(short x,short y,short h,unsigned char* sprite1,unsigned char* sprite2,short bytewidth,void* dest1,void* dest2) __attribute__((__stkparm__));
extern void GraySpriteX8_XOR(short x,short y,short h,unsigned char* sprite1,unsigned char* sprite2,short bytewidth,void* dest1,void* dest2) __attribute__((__stkparm__));


//-----------------------------------------------------------------------------
// CLIPPED TWO-PLANE SPRITE FUNCTIONS
// Only __regparm__ functions are available, __stkparm__ functions are NOT
// planned.
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// fast alternative functions for SpriteX() functions: AND, OR, XOR.
// fast additional sprite functions: BLIT, RPLC (BLIT with hard-coded white
// blitmask), MASK, TRANW/TRANB, SHADOW/ISHADOW, SHADOW2/ISHADOW2 
// (SHADOW/ISHADOW with fixed blitmask).
//-----------------------------------------------------------------------------
extern void GrayClipSprite8_AND_R(register short x asm("%d0"),register short y asm("%d1"),register short h asm("%d2"),unsigned char *sprt0,unsigned char *sprt1,register void *dest0 asm("%a0"),register void *dest1 asm("%a1")) __attribute__((__stkparm__));
extern void GrayClipSprite8_BLIT_R(register short x asm("%d0"),register short y asm("%d1"),register short h asm("%d2"),unsigned char *sprt0,unsigned char *sprt1,register unsigned char maskval asm("%d3"),register void *dest0 asm("%a0"),register void *dest1 asm("%a1")) __attribute__((__stkparm__));
extern void GrayClipSprite8_ISHADOW_R(register short x asm("%d0"),register short y asm("%d1"),register short h asm("%d2"),unsigned char* mask,register void *dest0 asm("%a0"),register void *dest1 asm("%a1")) __attribute__((__stkparm__));
extern void GrayClipSprite8_ISHADOW2_R(register short x asm("%d0"),register short y asm("%d1"),register short h asm("%d2"),register unsigned char blitmask asm("%d3"),register void *dest0 asm("%a0"),register void *dest1 asm("%a1")) __attribute__((__regparm__(6)));
extern void GrayClipSprite8_MASK_R(register short x asm("%d0"),register short y asm("%d1"),register short h asm("%d2"),unsigned char *sprt0,unsigned char *sprt1,unsigned char *mask0,unsigned char *mask1,register void *dest0 asm("%a0"),register void *dest1 asm("%a1")) __attribute__((__stkparm__));
extern void GrayClipSprite8_OR_R(register short x asm("%d0"),register short y asm("%d1"),register short h asm("%d2"),unsigned char *sprt0,unsigned char *sprt1,register void *dest0 asm("%a0"),register void *dest1 asm("%a1")) __attribute__((__stkparm__));
extern void GrayClipSprite8_RPLC_R(register short x asm("%d0"),register short y asm("%d1"),register short h asm("%d2"),unsigned char *sprt0,unsigned char *sprt1,register void *dest0 asm("%a0"),register void *dest1 asm("%a1")) __attribute__((__stkparm__));
extern void GrayClipSprite8_SHADOW_R(register short x asm("%d0"),register short y asm("%d1"),register short h asm("%d2"),unsigned char* mask,register void *dest0 asm("%a0"),register void *dest1 asm("%a1")) __attribute__((__stkparm__));
extern void GrayClipSprite8_SHADOW2_R(register short x asm("%d0"),register short y asm("%d1"),register short h asm("%d2"),register unsigned char blitmask asm("%d3"),register void *dest0 asm("%a0"),register void *dest1 asm("%a1")) __attribute__((__regparm__(6)));
extern void GrayClipSprite8_TRANB_R(register short x asm("%d0"),register short y asm("%d1"),register short h asm("%d2"),unsigned char *sprt0,unsigned char *sprt1,register void *dest0 asm("%a0"),register void *dest1 asm("%a1")) __attribute__((__stkparm__));
extern void GrayClipSprite8_TRANW_R(register short x asm("%d0"),register short y asm("%d1"),register short h asm("%d2"),unsigned char *sprt0,unsigned char *sprt1,register void *dest0 asm("%a0"),register void *dest1 asm("%a1")) __attribute__((__stkparm__));
extern void GrayClipSprite8_XOR_R(register short x asm("%d0"),register short y asm("%d1"),register short h asm("%d2"),unsigned char *sprt0,unsigned char *sprt1,register void *dest0 asm("%a0"),register void *dest1 asm("%a1")) __attribute__((__stkparm__));


extern void GrayClipSprite16_AND_R(register short x asm("%d0"),register short y asm("%d1"),register short h asm("%d2"),unsigned short *sprt0,unsigned short *sprt1,register void *dest0 asm("%a0"),register void *dest1 asm("%a1")) __attribute__((__stkparm__));
extern void GrayClipSprite16_BLIT_R(register short x asm("%d0"),register short y asm("%d1"),register short h asm("%d2"),unsigned short *sprt0,unsigned short *sprt1,register unsigned short maskval asm("%d3"),register void *dest0 asm("%a0"),register void *dest1 asm("%a1")) __attribute__((__stkparm__));
extern void GrayClipSprite16_ISHADOW_R(register short x asm("%d0"),register short y asm("%d1"),register short h asm("%d2"),unsigned short* mask,register void *dest0 asm("%a0"),register void *dest1 asm("%a1")) __attribute__((__stkparm__));
extern void GrayClipSprite16_ISHADOW2_R(register short x asm("%d0"),register short y asm("%d1"),register short h asm("%d2"),register unsigned short blitmask asm("%d3"),register void *dest0 asm("%a0"),register void *dest1 asm("%a1")) __attribute__((__regparm__(6)));
extern void GrayClipSprite16_MASK_R(register short x asm("%d0"),register short y asm("%d1"),register short h asm("%d2"),unsigned short *sprt0,unsigned short *sprt1,unsigned short *mask0,unsigned short *mask1,register void *dest0 asm("%a0"),register void *dest1 asm("%a1")) __attribute__((__stkparm__));
extern void GrayClipSprite16_OR_R(register short x asm("%d0"),register short y asm("%d1"),register short h asm("%d2"),unsigned short *sprt0,unsigned short *sprt1,register void *dest0 asm("%a0"),register void *dest1 asm("%a1")) __attribute__((__stkparm__));
extern void GrayClipSprite16_RPLC_R(register short x asm("%d0"),register short y asm("%d1"),register short h asm("%d2"),unsigned short *sprt0,unsigned short *sprt1,register void *dest0 asm("%a0"),register void *dest1 asm("%a1")) __attribute__((__stkparm__));
extern void GrayClipSprite16_SHADOW_R(register short x asm("%d0"),register short y asm("%d1"),register short h asm("%d2"),unsigned short* mask,register void *dest0 asm("%a0"),register void *dest1 asm("%a1")) __attribute__((__stkparm__));
extern void GrayClipSprite16_SHADOW2_R(register short x asm("%d0"),register short y asm("%d1"),register short h asm("%d2"),register unsigned short blitmask asm("%d3"),register void *dest0 asm("%a0"),register void *dest1 asm("%a1")) __attribute__((__regparm__(6)));
extern void GrayClipSprite16_TRANB_R(register short x asm("%d0"),register short y asm("%d1"),register short h asm("%d2"),unsigned short *sprt0,unsigned short *sprt1,register void *dest0 asm("%a0"),register void *dest1 asm("%a1")) __attribute__((__stkparm__));
extern void GrayClipSprite16_TRANW_R(register short x asm("%d0"),register short y asm("%d1"),register short h asm("%d2"),unsigned short *sprt0,unsigned short *sprt1,register void *dest0 asm("%a0"),register void *dest1 asm("%a1")) __attribute__((__stkparm__));
extern void GrayClipSprite16_XOR_R(register short x asm("%d0"),register short y asm("%d1"),register short h asm("%d2"),unsigned short *sprt0,unsigned short *sprt1,register void *dest0 asm("%a0"),register void *dest1 asm("%a1")) __attribute__((__stkparm__));


extern void GrayClipSprite32_AND_R(register short x asm("%d0"),register short y asm("%d1"),register short h asm("%d2"),unsigned long *sprt0,unsigned long *sprt1,register void *dest0 asm("%a0"),register void *dest1 asm("%a1")) __attribute__((__stkparm__));
extern void GrayClipSprite32_BLIT_R(register short x asm("%d0"),register short y asm("%d1"),register short h asm("%d2"),unsigned long *sprt0,unsigned long *sprt1,register unsigned long maskval asm("%d3"),register void *dest0 asm("%a0"),register void *dest1 asm("%a1")) __attribute__((__stkparm__));
extern void GrayClipSprite32_ISHADOW_R(register short x asm("%d0"),register short y asm("%d1"),register short h asm("%d2"),unsigned long* mask,register void *dest0 asm("%a0"),register void *dest1 asm("%a1")) __attribute__((__stkparm__));
extern void GrayClipSprite32_ISHADOW2_R(register short x asm("%d0"),register short y asm("%d1"),register short h asm("%d2"),register unsigned long blitmask asm("%d3"),register void *dest0 asm("%a0"),register void *dest1 asm("%a1")) __attribute__((__regparm__(6)));
extern void GrayClipSprite32_MASK_R(register short x asm("%d0"),register short y asm("%d1"),register short h asm("%d2"),unsigned long *sprt0,unsigned long *sprt1,unsigned long *mask0,unsigned long *mask1,register void *dest0 asm("%a0"),register void *dest1 asm("%a1")) __attribute__((__stkparm__));
extern void GrayClipSprite32_OR_R(register short x asm("%d0"),register short y asm("%d1"),register short h asm("%d2"),unsigned long *sprt0,unsigned long *sprt1,register void *dest0 asm("%a0"),register void *dest1 asm("%a1")) __attribute__((__stkparm__));
extern void GrayClipSprite32_RPLC_R(register short x asm("%d0"),register short y asm("%d1"),register short h asm("%d2"),unsigned long *sprt0,unsigned long *sprt1,register void *dest0 asm("%a0"),register void *dest1 asm("%a1")) __attribute__((__stkparm__));
extern void GrayClipSprite32_SHADOW_R(register short x asm("%d0"),register short y asm("%d1"),register short h asm("%d2"),unsigned long* mask,register void *dest0 asm("%a0"),register void *dest1 asm("%a1")) __attribute__((__stkparm__));
extern void GrayClipSprite32_SHADOW2_R(register short x asm("%d0"),register short y asm("%d1"),register short h asm("%d2"),register unsigned long blitmask asm("%d3"),register void *dest0 asm("%a0"),register void *dest1 asm("%a1")) __attribute__((__regparm__(6)));
extern void GrayClipSprite32_TRANB_R(register short x asm("%d0"),register short y asm("%d1"),register short h asm("%d2"),unsigned long *sprt0,unsigned long *sprt1,register void *dest0 asm("%a0"),register void *dest1 asm("%a1")) __attribute__((__stkparm__));
extern void GrayClipSprite32_TRANW_R(register short x asm("%d0"),register short y asm("%d1"),register short h asm("%d2"),unsigned long *sprt0,unsigned long *sprt1,register void *dest0 asm("%a0"),register void *dest1 asm("%a1")) __attribute__((__stkparm__));
extern void GrayClipSprite32_XOR_R(register short x asm("%d0"),register short y asm("%d1"),register short h asm("%d2"),unsigned long *sprt0,unsigned long *sprt1,register void *dest0 asm("%a0"),register void *dest1 asm("%a1")) __attribute__((__stkparm__));


// MastaZog asked for GrayClipSpriteX8_X_R routines. They will be written, although
// their priority is rather low.



//-----------------------------------------------------------------------------
// CLIPPED TWO PLANE INTERLACED SPRITE FUNCTIONS
// The interlaced sprite format is as follows:
// * all routines except MASK and SHADOW+derivatives: L/D or D/L (depending
//   on the convention you use for dest0 and dest1, usually LIGHT_PLANE and
//   DARK_PLANE respectively, but you can do otherwise), without mask.
// * MASK and SHADOW+derivatives: M/L/D or M/D/L (depending on the convention
//   you use for dest0 and dest1, usually LIGHT_PLANE and DARK_PLANE
//   respectively, but you can do otherwise), mask is applied to both
//   planes. SHADOW+derivatives use only the mask part of the sprite.
//-----------------------------------------------------------------------------
// Coming after Beta 3.



//-----------------------------------------------------------------------------
// NON-CLIPPED SINGLE PLANE TILE (ALIGNED SPRITE, USUAL SPRITE FORMAT) FUNCTIONS
// (nothing to do with the tilemap engine).
//-----------------------------------------------------------------------------
extern void Tile8x8_AND_R(register short col asm("%d0"),register short y asm("%d1"),register unsigned char *sprite asm("%a1"),register void *plane asm("%a0"));
extern void Tile8x8_BLIT_R(register short col asm("%d0"),register short y asm("%d1"),register unsigned char *sprite asm("%a1"),register unsigned char maskval asm("%d3"),register void *plane asm("%a0"));
extern void Tile8x8_MASK_R(register short col asm("%d0"),register short y asm("%d1"),register unsigned char *sprite asm("%a1"),unsigned char* mask,register void *plane asm("%a0"));
extern void Tile8x8_OR_R(register short col asm("%d0"),register short y asm("%d1"),register unsigned char *sprite asm("%a1"),register void *plane asm("%a0"));
extern void Tile8x8_RPLC_R(register short col asm("%d0"),register short y asm("%d1"),register unsigned char *sprite asm("%a1"),register void *plane asm("%a0"));
extern void Tile8x8_XOR_R(register short col asm("%d0"),register short y asm("%d1"),register unsigned char *sprite asm("%a1"),register void *plane asm("%a0"));

extern void Tile16x16_AND_R(register short col asm("%d0"),register short y asm("%d1"),register unsigned short *sprite asm("%a1"),register void *plane asm("%a0"));
extern void Tile16x16_BLIT_R(register short col asm("%d0"),register short y asm("%d1"),register unsigned short *sprite asm("%a1"),register unsigned short maskval asm("%d3"),register void *plane asm("%a0"));
extern void Tile16x16_MASK_R(register short col asm("%d0"),register short y asm("%d1"),register unsigned short *sprite asm("%a1"),unsigned short* mask,register void *plane asm("%a0"));
extern void Tile16x16_OR_R(register short col asm("%d0"),register short y asm("%d1"),register unsigned short *sprite asm("%a1"),register void *plane asm("%a0"));
extern void Tile16x16_RPLC_R(register short col asm("%d0"),register short y asm("%d1"),register unsigned short *sprite asm("%a1"),register void *plane asm("%a0"));
extern void Tile16x16_XOR_R(register short col asm("%d0"),register short y asm("%d1"),register unsigned short *sprite asm("%a1"),register void *plane asm("%a0"));

extern void Tile32x32_AND_R(register short col asm("%d0"),register short y asm("%d1"),register unsigned long *sprite asm("%a1"),register void *plane asm("%a0"));
extern void Tile32x32_BLIT_R(register short col asm("%d0"),register short y asm("%d1"),register unsigned long *sprite asm("%a1"),register unsigned long maskval asm("%d3"),register void *plane asm("%a0"));
extern void Tile32x32_MASK_R(register short col asm("%d0"),register short y asm("%d1"),register unsigned long *sprite asm("%a1"),unsigned long* mask,register void *plane asm("%a0"));
extern void Tile32x32_OR_R(register short col asm("%d0"),register short y asm("%d1"),register unsigned long *sprite asm("%a1"),register void *plane asm("%a0"));
extern void Tile32x32_RPLC_R(register short col asm("%d0"),register short y asm("%d1"),register unsigned long *sprite asm("%a1"),register void *plane asm("%a0"));
extern void Tile32x32_XOR_R(register short col asm("%d0"),register short y asm("%d1"),register unsigned long *sprite asm("%a1"),register void *plane asm("%a0"));



//-----------------------------------------------------------------------------
// NON-CLIPPED TWO PLANE TILE (ALIGNED SPRITE, USUAL SPRITE FORMAT) FUNCTIONS
// (nothing to do with the tilemap engine).
//-----------------------------------------------------------------------------
extern void GrayTile8x8_AND_R(register short col asm("%d0"),register short y asm("%d1"),unsigned char *sprt0,unsigned char *sprt1,register void *dest0 asm("%a0"),register void *dest1 asm("%a1")) __attribute__((__stkparm__));
extern void GrayTile8x8_BLIT_R(register short col asm("%d0"),register short y asm("%d1"),unsigned char *sprt0,unsigned char *sprt1,register unsigned char maskval asm("%d3"),register void *dest0 asm("%a0"),register void *dest1 asm("%a1")) __attribute__((__stkparm__));
extern void GrayTile8x8_MASK_R(register short col asm("%d0"),register short y asm("%d1"),unsigned char *sprt0,unsigned char *sprt1,unsigned char* mask1,unsigned char* mask2,register void *dest0 asm("%a0"),register void *dest1 asm("%a1")) __attribute__((__stkparm__));
extern void GrayTile8x8_OR_R(register short col asm("%d0"),register short y asm("%d1"),unsigned char *sprt0,unsigned char *sprt1,register void *dest0 asm("%a0"),register void *dest1 asm("%a1")) __attribute__((__stkparm__));
extern void GrayTile8x8_RPLC_R(register short col asm("%d0"),register short y asm("%d1"),unsigned char *sprt0,unsigned char *sprt1,register void *dest0 asm("%a0"),register void *dest1 asm("%a1")) __attribute__((__stkparm__));
extern void GrayTile8x8_SHADOW_R(register short col asm("%d0"),register short y asm("%d1"),unsigned char *mask,register void *dest0 asm("%a0"),register void *dest1 asm("%a1")) __attribute__((__stkparm__));
extern void GrayTile8x8_TRANB_R(register short col asm("%d0"),register short y asm("%d1"),unsigned char *sprt0,unsigned char *sprt1,register void *dest0 asm("%a0"),register void *dest1 asm("%a1")) __attribute__((__stkparm__));
extern void GrayTile8x8_TRANW_R(register short col asm("%d0"),register short y asm("%d1"),unsigned char *sprt0,unsigned char *sprt1,register void *dest0 asm("%a0"),register void *dest1 asm("%a1")) __attribute__((__stkparm__));
extern void GrayTile8x8_XOR_R(register short col asm("%d0"),register short y asm("%d1"),unsigned char *sprt0,unsigned char *sprt1,register void *dest0 asm("%a0"),register void *dest1 asm("%a1")) __attribute__((__stkparm__));

extern void GrayTile16x16_AND_R(register short col asm("%d0"),register short y asm("%d1"),unsigned short *sprt0,unsigned short *sprt1,register void *dest0 asm("%a0"),register void *dest1 asm("%a1")) __attribute__((__stkparm__));
extern void GrayTile16x16_BLIT_R(register short col asm("%d0"),register short y asm("%d1"),unsigned short *sprt0,unsigned short *sprt1,register unsigned short maskval asm("%d3"),register void *dest0 asm("%a0"),register void *dest1 asm("%a1")) __attribute__((__stkparm__));
extern void GrayTile16x16_MASK_R(register short col asm("%d0"),register short y asm("%d1"),unsigned short *sprt0,unsigned short *sprt1,unsigned short* mask1,unsigned short* mask2,register void *dest0 asm("%a0"),register void *dest1 asm("%a1")) __attribute__((__stkparm__));
extern void GrayTile16x16_OR_R(register short col asm("%d0"),register short y asm("%d1"),unsigned short *sprt0,unsigned short *sprt1,register void *dest0 asm("%a0"),register void *dest1 asm("%a1")) __attribute__((__stkparm__));
extern void GrayTile16x16_RPLC_R(register short col asm("%d0"),register short y asm("%d1"),unsigned short *sprt0,unsigned short *sprt1,register void *dest0 asm("%a0"),register void *dest1 asm("%a1")) __attribute__((__stkparm__));
extern void GrayTile16x16_SHADOW_R(register short col asm("%d0"),register short y asm("%d1"),unsigned short *mask,register void *dest0 asm("%a0"),register void *dest1 asm("%a1")) __attribute__((__stkparm__));
extern void GrayTile16x16_TRANB_R(register short col asm("%d0"),register short y asm("%d1"),unsigned short *sprt0,unsigned short *sprt1,register void *dest0 asm("%a0"),register void *dest1 asm("%a1")) __attribute__((__stkparm__));
extern void GrayTile16x16_TRANW_R(register short col asm("%d0"),register short y asm("%d1"),unsigned short *sprt0,unsigned short *sprt1,register void *dest0 asm("%a0"),register void *dest1 asm("%a1")) __attribute__((__stkparm__));
extern void GrayTile16x16_XOR_R(register short col asm("%d0"),register short y asm("%d1"),unsigned short *sprt0,unsigned short *sprt1,register void *dest0 asm("%a0"),register void *dest1 asm("%a1")) __attribute__((__stkparm__));

extern void GrayTile32x32_AND_R(register short col asm("%d0"),register short y asm("%d1"),unsigned long *sprt0,unsigned long *sprt1,register void *dest0 asm("%a0"),register void *dest1 asm("%a1")) __attribute__((__stkparm__));
extern void GrayTile32x32_BLIT_R(register short col asm("%d0"),register short y asm("%d1"),unsigned long *sprt0,unsigned long *sprt1,register unsigned long maskval asm("%d3"),register void *dest0 asm("%a0"),register void *dest1 asm("%a1")) __attribute__((__stkparm__));
extern void GrayTile32x32_MASK_R(register short col asm("%d0"),register short y asm("%d1"),unsigned long *sprt0,unsigned long *sprt1,unsigned long* mask1,unsigned long* mask2,register void *dest0 asm("%a0"),register void *dest1 asm("%a1")) __attribute__((__stkparm__));
extern void GrayTile32x32_OR_R(register short col asm("%d0"),register short y asm("%d1"),unsigned long *sprt0,unsigned long *sprt1,register void *dest0 asm("%a0"),register void *dest1 asm("%a1")) __attribute__((__stkparm__));
extern void GrayTile32x32_RPLC_R(register short col asm("%d0"),register short y asm("%d1"),unsigned long *sprt0,unsigned long *sprt1,register void *dest0 asm("%a0"),register void *dest1 asm("%a1")) __attribute__((__stkparm__));
extern void GrayTile32x32_SHADOW_R(register short col asm("%d0"),register short y asm("%d1"),unsigned long *mask,register void *dest0 asm("%a0"),register void *dest1 asm("%a1")) __attribute__((__stkparm__));
extern void GrayTile32x32_TRANB_R(register short col asm("%d0"),register short y asm("%d1"),unsigned long *sprt0,unsigned long *sprt1,register void *dest0 asm("%a0"),register void *dest1 asm("%a1")) __attribute__((__stkparm__));
extern void GrayTile32x32_TRANW_R(register short col asm("%d0"),register short y asm("%d1"),unsigned long *sprt0,unsigned long *sprt1,register void *dest0 asm("%a0"),register void *dest1 asm("%a1")) __attribute__((__stkparm__));
extern void GrayTile32x32_XOR_R(register short col asm("%d0"),register short y asm("%d1"),unsigned long *sprt0,unsigned long *sprt1,register void *dest0 asm("%a0"),register void *dest1 asm("%a1")) __attribute__((__stkparm__));



//-----------------------------------------------------------------------------
// NON-CLIPPED TWO PLANE TILE (ALIGNED SPRITE, INTERLACED SPRITE FORMAT) FUNCTIONS
// (nothing to do with the tilemap engine).
// The interlaced sprite format is as follows:
// * all routines except MASK and SHADOW+derivatives: L/D or D/L (depending
//   on the convention you use for dest0 and dest1, usually LIGHT_PLANE and
//   DARK_PLANE respectively, but you can do otherwise), without mask.
// * MASK and SHADOW+derivatives: M/L/D or M/D/L (depending on the convention
//   you use for dest0 and dest1, usually LIGHT_PLANE and DARK_PLANE
//   respectively, but you can do otherwise), mask is applied to both
//   planes. SHADOW+derivatives use only the mask part of the sprite.
//-----------------------------------------------------------------------------
extern void GrayITile8x8_AND_R(register short col asm("%d0"),register short y asm("%d1"),unsigned char *sprite,register void *dest0 asm("%a0"),register void *dest1 asm("%a1")) __attribute__((__stkparm__));
extern void GrayITile8x8_BLIT_R(register short col asm("%d0"),register short y asm("%d1"),unsigned char *sprite,register unsigned char maskval asm("%d3"),register void *dest0 asm("%a0"),register void *dest1 asm("%a1")) __attribute__((__stkparm__));
extern void GrayITile8x8_MASK_R(register short col asm("%d0"),register short y asm("%d1"),unsigned char *sprite,register void *dest0 asm("%a0"),register void *dest1 asm("%a1")) __attribute__((__stkparm__));
extern void GrayITile8x8_OR_R(register short col asm("%d0"),register short y asm("%d1"),unsigned char *sprite,register void *dest0 asm("%a0"),register void *dest1 asm("%a1")) __attribute__((__stkparm__));
extern void GrayITile8x8_RPLC_R(register short col asm("%d0"),register short y asm("%d1"),unsigned char *sprite,register void *dest0 asm("%a0"),register void *dest1 asm("%a1")) __attribute__((__stkparm__));
extern void GrayITile8x8_SHADOW_R(register short col asm("%d0"),register short y asm("%d1"),unsigned char *mask,register void *dest0 asm("%a0"),register void *dest1 asm("%a1")) __attribute__((__stkparm__));
extern void GrayITile8x8_TRANB_R(register short col asm("%d0"),register short y asm("%d1"),unsigned char *sprite,register void *dest0 asm("%a0"),register void *dest1 asm("%a1")) __attribute__((__stkparm__));
extern void GrayITile8x8_TRANW_R(register short col asm("%d0"),register short y asm("%d1"),unsigned char *sprite,register void *dest0 asm("%a0"),register void *dest1 asm("%a1")) __attribute__((__stkparm__));
extern void GrayITile8x8_XOR_R(register short col asm("%d0"),register short y asm("%d1"),unsigned char *sprite,register void *dest0 asm("%a0"),register void *dest1 asm("%a1")) __attribute__((__stkparm__));

extern void GrayITile16x16_AND_R(register short col asm("%d0"),register short y asm("%d1"),unsigned short *sprite,register void *dest0 asm("%a0"),register void *dest1 asm("%a1")) __attribute__((__stkparm__));
extern void GrayITile16x16_BLIT_R(register short col asm("%d0"),register short y asm("%d1"),unsigned short *sprite,register unsigned short maskval asm("%d3"),register void *dest0 asm("%a0"),register void *dest1 asm("%a1")) __attribute__((__stkparm__));
extern void GrayITile16x16_MASK_R(register short col asm("%d0"),register short y asm("%d1"),unsigned short *sprite,register void *dest0 asm("%a0"),register void *dest1 asm("%a1")) __attribute__((__stkparm__));
extern void GrayITile16x16_OR_R(register short col asm("%d0"),register short y asm("%d1"),unsigned short *sprite,register void *dest0 asm("%a0"),register void *dest1 asm("%a1")) __attribute__((__stkparm__));
extern void GrayITile16x16_RPLC_R(register short col asm("%d0"),register short y asm("%d1"),unsigned short *sprite,register void *dest0 asm("%a0"),register void *dest1 asm("%a1")) __attribute__((__stkparm__));
extern void GrayITile16x16_SHADOW_R(register short col asm("%d0"),register short y asm("%d1"),unsigned short *mask,register void *dest0 asm("%a0"),register void *dest1 asm("%a1")) __attribute__((__stkparm__));
extern void GrayITile16x16_TRANB_R(register short col asm("%d0"),register short y asm("%d1"),unsigned short *sprite,register void *dest0 asm("%a0"),register void *dest1 asm("%a1")) __attribute__((__stkparm__));
extern void GrayITile16x16_TRANW_R(register short col asm("%d0"),register short y asm("%d1"),unsigned short *sprite,register void *dest0 asm("%a0"),register void *dest1 asm("%a1")) __attribute__((__stkparm__));
extern void GrayITile16x16_XOR_R(register short col asm("%d0"),register short y asm("%d1"),unsigned short *sprite,register void *dest0 asm("%a0"),register void *dest1 asm("%a1")) __attribute__((__stkparm__));

extern void GrayITile32x32_AND_R(register short col asm("%d0"),register short y asm("%d1"),unsigned long *sprite,register void *dest0 asm("%a0"),register void *dest1 asm("%a1")) __attribute__((__stkparm__));
extern void GrayITile32x32_BLIT_R(register short col asm("%d0"),register short y asm("%d1"),unsigned long *sprite,register unsigned long maskval asm("%d3"),register void *dest0 asm("%a0"),register void *dest1 asm("%a1")) __attribute__((__stkparm__));
extern void GrayITile32x32_MASK_R(register short col asm("%d0"),register short y asm("%d1"),unsigned long *sprite,register void *dest0 asm("%a0"),register void *dest1 asm("%a1")) __attribute__((__stkparm__));
extern void GrayITile32x32_OR_R(register short col asm("%d0"),register short y asm("%d1"),unsigned long *sprite,register void *dest0 asm("%a0"),register void *dest1 asm("%a1")) __attribute__((__stkparm__));
extern void GrayITile32x32_RPLC_R(register short col asm("%d0"),register short y asm("%d1"),unsigned long *sprite,register void *dest0 asm("%a0"),register void *dest1 asm("%a1")) __attribute__((__stkparm__));
extern void GrayITile32x32_SHADOW_R(register short col asm("%d0"),register short y asm("%d1"),unsigned long *mask,register void *dest0 asm("%a0"),register void *dest1 asm("%a1")) __attribute__((__stkparm__));
extern void GrayITile32x32_TRANB_R(register short col asm("%d0"),register short y asm("%d1"),unsigned long *sprite,register void *dest0 asm("%a0"),register void *dest1 asm("%a1")) __attribute__((__stkparm__));
extern void GrayITile32x32_TRANW_R(register short col asm("%d0"),register short y asm("%d1"),unsigned long *sprite,register void *dest0 asm("%a0"),register void *dest1 asm("%a1")) __attribute__((__stkparm__));
extern void GrayITile32x32_XOR_R(register short col asm("%d0"),register short y asm("%d1"),unsigned long *sprite,register void *dest0 asm("%a0"),register void *dest1 asm("%a1")) __attribute__((__stkparm__));



//-----------------------------------------------------------------------------
// TRANSITION EFFECTS.
// Many are currently missing.
//-----------------------------------------------------------------------------
extern void FadeOutToBlack_CWS1_R(register void* lightplane asm("%a0"),register void* darkplane asm("%a1"),register short height asm("%d0"),register short bytewidth asm("%d1"),register short wait asm("%d2"));
extern void FadeOutToWhite_CWS1_R(register void* lightplane asm("%a0"),register void* darkplane asm("%a1"),register short height asm("%d0"),register short bytewidth asm("%d1"),register short wait asm("%d2"));

extern void FadeOutToBlack_CCWS1_R(register void* lightplane asm("%a0"),register void* darkplane asm("%a1"),register short height asm("%d0"),register short bytewidth asm("%d1"),register short wait asm("%d2"));
extern void FadeOutToWhite_CCWS1_R(register void* lightplane asm("%a0"),register void* darkplane asm("%a1"),register short height asm("%d0"),register short bytewidth asm("%d1"),register short wait asm("%d2"));


extern void FadeOutToBlack_LR_R(register void *lightplane asm("%a0"),register void *darkplane asm("%a1"),register short height asm("%d0"),register short bytewidth asm("%d1"),register short wait asm("%d2"));
extern void FadeOutToWhite_LR_R(register void *lightplane asm("%a0"),register void *darkplane asm("%a1"),register short height asm("%d0"),register short bytewidth asm("%d1"),register short wait asm("%d2"));
extern void FadeOutToBlack_LR18_R(register void *lightplane asm("%a0"),register void *darkplane asm("%a1"),register short height asm("%d0"),register short bytewidth asm("%d1"),register short wait asm("%d2"));
extern void FadeOutToWhite_LR18_R(register void *lightplane asm("%a0"),register void *darkplane asm("%a1"),register short height asm("%d0"),register short bytewidth asm("%d1"),register short wait asm("%d2"));
extern void FadeOutToBlack_LR28_R(register void *lightplane asm("%a0"),register void *darkplane asm("%a1"),register short height asm("%d0"),register short bytewidth asm("%d1"),register short wait asm("%d2"));
extern void FadeOutToWhite_LR28_R(register void *lightplane asm("%a0"),register void *darkplane asm("%a1"),register short height asm("%d0"),register short bytewidth asm("%d1"),register short wait asm("%d2"));

extern void FadeOutToBlack_RL_R(register void *lightplane asm("%a0"),register void *darkplane asm("%a1"),register short height asm("%d0"),register short bytewidth asm("%d1"),register short wait asm("%d2"));
extern void FadeOutToWhite_RL_R(register void *lightplane asm("%a0"),register void *darkplane asm("%a1"),register short height asm("%d0"),register short bytewidth asm("%d1"),register short wait asm("%d2"));
extern void FadeOutToBlack_RL18_R(register void *lightplane asm("%a0"),register void *darkplane asm("%a1"),register short height asm("%d0"),register short bytewidth asm("%d1"),register short wait asm("%d2"));
extern void FadeOutToWhite_RL18_R(register void *lightplane asm("%a0"),register void *darkplane asm("%a1"),register short height asm("%d0"),register short bytewidth asm("%d1"),register short wait asm("%d2"));
extern void FadeOutToBlack_RL28_R(register void *lightplane asm("%a0"),register void *darkplane asm("%a1"),register short height asm("%d0"),register short bytewidth asm("%d1"),register short wait asm("%d2"));
extern void FadeOutToWhite_RL28_R(register void *lightplane asm("%a0"),register void *darkplane asm("%a1"),register short height asm("%d0"),register short bytewidth asm("%d1"),register short wait asm("%d2"));


extern void FadeOutToBlack_TB_R(register void *lightplane asm("%a0"),register void *darkplane asm("%a1"),register short height asm("%d0"),register short bytewidth asm("%d1"),register short wait asm("%d2"));
extern void FadeOutToWhite_TB_R(register void *lightplane asm("%a0"),register void *darkplane asm("%a1"),register short height asm("%d0"),register short bytewidth asm("%d1"),register short wait asm("%d2"));

extern void FadeOutToBlack_BT_R(register void *lightplane asm("%a0"),register void *darkplane asm("%a1"),register short height asm("%d0"),register short bytewidth asm("%d1"),register short wait asm("%d2"));
extern void FadeOutToWhite_BT_R(register void *lightplane asm("%a0"),register void *darkplane asm("%a1"),register short height asm("%d0"),register short bytewidth asm("%d1"),register short wait asm("%d2"));


extern void GrayIShadowPlanesX16_R(register short height asm("%d0"),register short wordwidth asm("%d1"), register void *dest0 asm("%a0"),register void *dest1 asm("%a1")) __attribute__((__regparm__(4)));
extern void GrayShadowPlanesX16_R(register short height asm("%d0"),register short wordwidth asm("%d1"), register void *dest0 asm("%a0"),register void *dest1 asm("%a1")) __attribute__((__regparm__(4)));
extern void GrayIShadowPlanes240_R(register short height asm("%d0"),register void *dest0 asm("%a0"),register void *dest1 asm("%a1")) __attribute__((__regparm__(3)));
extern void GrayShadowPlanes240_R(register short height asm("%d0"),register void *dest0 asm("%a0"),register void *dest1 asm("%a1")) __attribute__((__regparm__(3)));
extern void GrayIShadowPlanesTo_R(register void *src0 asm("%a0"),register void *src1 asm("%a1"),void *dest0,void *dest1) __attribute__((__stkparm__));
extern void GrayShadowPlanesTo_R(register void *src0 asm("%a0"),register void *src1 asm("%a1"),void *dest0,void *dest1) __attribute__((__stkparm__));



//-----------------------------------------------------------------------------
// Helper functions to create shadows and inverse shadows from sprites, in
// order to draw them with OR routines (significantly faster than SHADOW
// routines). This way takes more memory but is significantly more efficient at
// run-time.
//-----------------------------------------------------------------------------
extern void CreateSpriteIShadow8_R(register short height asm("%d0"),register unsigned char* src0 asm("%a0"),register unsigned char* src1 asm("%a1"),unsigned char* mask, unsigned char* dest0, unsigned char* dest1) __attribute__((__stkparm__));
extern void CreateSpriteIShadow16_R(register short height asm("%d0"),register unsigned short* src0 asm("%a0"),register unsigned short* src1 asm("%a1"),unsigned short* mask, unsigned short* dest0, unsigned short* dest1) __attribute__((__stkparm__));
extern void CreateSpriteIShadow32_R(register short height asm("%d0"),register unsigned long* src0 asm("%a0"),register unsigned long* src1 asm("%a1"),unsigned long* mask, unsigned long* dest0, unsigned long* dest1) __attribute__((__stkparm__));
extern void CreateSpriteIShadowX8_R(register short height asm("%d0"),register short bytewidth asm("%d1"),register unsigned char* src0 asm("%a0"),register unsigned char* src1 asm("%a1"),unsigned char* mask, unsigned char* dest0, unsigned char* dest1) __attribute__((__stkparm__));

extern void CreateSpriteShadow8_R(register short height asm("%d0"),register unsigned char* src0 asm("%a0"),register unsigned char* src1 asm("%a1"),unsigned char* mask, unsigned char* dest0, unsigned char* dest1) __attribute__((__stkparm__));
extern void CreateSpriteShadow16_R(register short height asm("%d0"),register unsigned short* src0 asm("%a0"),register unsigned short* src1 asm("%a1"),unsigned short* mask, unsigned short* dest0, unsigned short* dest1) __attribute__((__stkparm__));
extern void CreateSpriteShadow32_R(register short height asm("%d0"),register unsigned long* src0 asm("%a0"),register unsigned long* src1 asm("%a1"),unsigned long* mask, unsigned long* dest0, unsigned long* dest1) __attribute__((__stkparm__));
extern void CreateSpriteShadowX8_R(register short height asm("%d0"),register short bytewidth asm("%d1"),register unsigned char* src0 asm("%a0"),register unsigned char* src1 asm("%a1"),unsigned char* mask, unsigned char* dest0, unsigned char* dest1) __attribute__((__stkparm__));

extern void CreateISpriteIShadow8_R(register short height asm("%d0"),register unsigned char* src asm("%a0"),register unsigned char* dest asm("%a1"));
extern void CreateISpriteIShadow16_R(register short height asm("%d0"),register unsigned short* src asm("%a0"),register unsigned short* dest asm("%a1"));
extern void CreateISpriteIShadow32_R(register short height asm("%d0"),register unsigned long* src asm("%a0"),register unsigned long* dest asm("%a1"));

extern void CreateISpriteShadow8_R(register short height asm("%d0"),register unsigned char* src asm("%a0"),register unsigned char* dest asm("%a1"));
extern void CreateISpriteShadow16_R(register short height asm("%d0"),register unsigned short* src asm("%a0"),register unsigned short* dest asm("%a1"));
extern void CreateISpriteShadow32_R(register short height asm("%d0"),register unsigned long* src asm("%a0"),register unsigned long* dest asm("%a1"));



//-----------------------------------------------------------------------------
// Tilemap Engine.
//-----------------------------------------------------------------------------
// See tilemap.h.
// NOTE1: doublebuffering is currently impossible with the tilemap engine.
// You may try the intermediate method between no doublebuffering and real
// doublebuffering, available in gray.h.
// NOTE2: prototypes and usage of the tilemap engine functions may/will
// change before ExtGraph 2.00 Release.



//-----------------------------------------------------------------------------
// Preshifted sprite functions.
//-----------------------------------------------------------------------------
// See preshift.h.



//-----------------------------------------------------------------------------
// Miscellanous SpriteX8(X8) functions (mirror, rotate).
// SpriteX8X8_ROTATE_RIGHT_R and SpriteX8X8_ROTATE_LEFT_R do not require the
// source sprite to be in a writable area. However, they require that both
// dimensions are multiples of 8 (they are coded for simplicity; coding
// routines supporting any height isn't extremely difficult).
//
// SpriteX8X8_RR_MH_R has a funny story: it was actually supposed to be
// SpriteX8X8_ROTATE_RIGHT_R, but I made mistakes in the implementation and 
// when trying to fix it, after several modifications, it started creating 
// r-rotated h-mirrored sprites.
// Therefore I saved that routine and coded a working SpriteX8X8_ROTATE_RIGHT_R.
// Afterwards, to complete the series of functions, I coded
// SpriteX8X8_RL_MH_R.
// 
// I had forgotten to write SpriteX8_MIRROR_HV_R for a long period of time...
//-----------------------------------------------------------------------------
extern void SpriteX8_MIRROR_H(short h,unsigned char* src,short bytewidth,unsigned char* dest) __attribute__((__stkparm__));
extern void SpriteX8_MIRROR_H_R(register short h asm("%d0"),register unsigned char* src asm("%a0"),register short bytewidth asm("%d1"),register unsigned char* dest asm("%a1")) __attribute__((__regparm__(4)));

extern void SpriteX8_MIRROR_V(short h,unsigned char* src,short bytewidth,unsigned char* dest) __attribute__((__stkparm__));
extern void SpriteX8_MIRROR_V_R(register short h asm("%d0"),register unsigned char* src asm("%a1"),register short bytewidth asm("%d2"),register unsigned char* dest asm("%a0")) __attribute__((__regparm__(4)));

extern void SpriteX8_MIRROR_HV_R(register short h asm("%d0"),register unsigned char* src asm("%a0"),register short bytewidth asm("%d1"),register unsigned char* dest asm("%a1")) __attribute__((__regparm__(4)));

extern void SpriteX8X8_ROTATE_RIGHT_R(register short h asm("%d0"),register unsigned char* src asm("%a0"),register short bytewidth asm("%d1"),register unsigned char* dest asm("%a1")) __attribute__((__regparm__(4)));
extern void SpriteX8X8_ROTATE_LEFT_R(register short h asm("%d0"),register unsigned char* src asm("%a0"),register short bytewidth asm("%d1"),register unsigned char* dest asm("%a1")) __attribute__((__regparm__(4)));
extern void SpriteX8X8_RR_MH_R(register short h asm("%d0"),register unsigned char* src asm("%a0"),register short bytewidth asm("%d1"),register unsigned char* dest asm("%a1")) __attribute__((__regparm__(4)));
extern void SpriteX8X8_RL_MH_R(register short h asm("%d0"),register unsigned char* src asm("%a0"),register short bytewidth asm("%d1"),register unsigned char* dest asm("%a1")) __attribute__((__regparm__(4)));


//=============================================================================
//=============================================================================
//=============================================================================
//
// TTUNPACK.H (now extgraph.h contains everything necessary for unpacking)
// taken from TIGCC Tools Suite (c) 2000,2001,2002 TICT
//
//=============================================================================
//=============================================================================
//=============================================================================
#ifndef __TTUNPACK_H__
#define __TTUNPACK_H__

typedef struct {
    unsigned char  osize_lo;   // original size lowbyte
    unsigned char  osize_hi;   // original size highbyte
    unsigned char  magic1;     // must be equal to TTUNPACK_MAGIC1
    unsigned char  magic2;     // must be equal to TTUNPACK_MAGIC2
    unsigned char  csize_lo;   // compressed size lowbyte
    unsigned char  csize_hi;   // compressed size highbyte
    unsigned char  esc1;       // escape >> (8-escBits)
    unsigned char  notused3;
    unsigned char  notused4;
    unsigned char  esc2;       // escBits
    unsigned char  gamma1;     // maxGamma + 1
    unsigned char  gamma2;     // (1<<maxGamma)
    unsigned char  extralz;    // extraLZPosBits
    unsigned char  notused1;
    unsigned char  notused2;
    unsigned char  rleentries; // rleUsed
} TTUNPACK_HEADER;

#define TTUNPACK_MAGIC1 0x54
#define TTUNPACK_MAGIC2 0x50

#define ttunpack_size(_p_)  ((unsigned short)(((TTUNPACK_HEADER*)(_p_))->osize_lo | (((TTUNPACK_HEADER*)(_p_))->osize_hi << 8)))
#define ttunpack_valid(_p_) (((TTUNPACK_HEADER*)(_p_))->magic1 == TTUNPACK_MAGIC1 && ((TTUNPACK_HEADER*)(_p_))->magic2 == TTUNPACK_MAGIC2)

#define TTUNPACK_OKAY             0
#define TTUNPACK_NOESCFOUND     248
#define TTUNPACK_ESCBITS        249
#define TTUNPACK_MAXGAMMA       250
#define TTUNPACK_EXTRALZP       251
#define TTUNPACK_NOMAGIC        252
#define TTUNPACK_OUTBUFOVERRUN  253
#define TTUNPACK_LZPOSUNDERRUN  254

#define ttunpack_decompress UnpackBuffer
short UnpackBuffer(unsigned char *src, unsigned char *dest) __attribute__((__stkparm__));

#define ttunpack_decompress_gray UnpackBufferGray
short UnpackBufferGray(unsigned char *src, unsigned char *dest) __attribute__((__stkparm__));


#else
#error EXTGRAPH.H already contains TTUNPACK.H defines (remove ttunpack.h include!)
#endif

//=============================================================================
//=============================================================================
//=============================================================================
//
// TTARCHIVE.H (now extgraph.h contains everything for archive handling)
// taken from TIGCC Tools Suite (c) 2000,2001,2002 TICT
//
//=============================================================================
//=============================================================================
//=============================================================================
#ifndef __TTARCHIVE_H__
#define __TTARCHIVE_H__

//-----------------------------------------------------------------------------
// entry of archive structure
//-----------------------------------------------------------------------------
typedef struct {
   unsigned short offset;     // offset to the entry data from end of entry list
   unsigned short length;     // length of entry
   char           name[8];    // entry name
   unsigned char  misc1[2];   // info from cfg file (may be queried by a program)
   unsigned char  misc2[2];   // info from cfg file (may be queried by a program)
} TTARCHIVE_ENTRY;

//-----------------------------------------------------------------------------
// header of a TTARCHIVE file
//-----------------------------------------------------------------------------
typedef struct {
   unsigned long   magic;    // must be equal to TTARCHIVE_MAGIC
   unsigned short  nr;       // number of entries
   TTARCHIVE_ENTRY entry[0]; // here comes the list of TTARCHIVE_ENTRY structures
} TTARCHIVE_HEADER;

#define TTARCHIVE_MAGIC 0x74746100L

//-----------------------------------------------------------------------------
// smart macros to access ttarchive
//
// _p_   ... pointer to archive start address
// _i_   ... index of entry
//
// NOTE: No checking is done in the macros !!
//-----------------------------------------------------------------------------

#define ttarchive_valid(_p_)     (((TTARCHIVE_HEADER*)(_p_))->magic == TTARCHIVE_MAGIC)
#define ttarchive_entries(_p_)   (((TTARCHIVE_HEADER*)(_p_))->nr)
#define ttarchive_desc(_p_,_i_)  (&(((TTARCHIVE_HEADER*)(_p_))->entry[_i_]))
#define ttarchive_data(_p_,_i_)  (((unsigned char*)&(((TTARCHIVE_HEADER*)(_p_))->entry[((TTARCHIVE_HEADER*)(_p_))->nr]))+\
                                 ((TTARCHIVE_HEADER*)(_p_))->entry[_i_].offset)

#define ttarchive_size(_p_)      ({TTARCHIVE_ENTRY* e=&(((TTARCHIVE_HEADER*)(_p_))->entry[((TTARCHIVE_HEADER*)(_p_))->nr-1]);\
                                  ((unsigned char*)e)+sizeof(TTARCHIVE_ENTRY)+e->offset+e->length-(unsigned char*)(_p_);})

#define ttarchive_info(_p_)      ({char* p=((char*)(_p_))+ttarchive_size(_p_)+21;\
                                  ((!(*p) && (*(p+1) ==((char)0xad)))?(p-20):NULL);})

#define TTARCHIVE_INFOLENGTH 20

#else
#error EXTGRAPH.H already contains TTARCHIVE.H defines (remove ttarchive.h include!)
#endif

#endif

//#############################################################################
// Revision History
//#############################################################################
//
// $Log: extgraph.h,v $
// Revision 2.00
// Huge changes for v2.00 (rewrites, internal organization of library...).
// The complete changelog is in the documentation and will stay there.
//
//
//
// Revision 1.13  2002/05/22 09:19:20  tnussb
// for TIGCC versions greater than 0.93 all functions are declared with
// "__attribute__((__stkparm__))" to work correctly with compilation
// switch -mregparm
//
// Revision 1.12  2002/05/08 19:32:46  tnussb
// version number raised (just a bug in unpack function was fixed)
//
// Revision 1.11  2002/04/05 13:47:28  tnussb
// changes for v1.00
//
// Revision 1.10  2002/04/02 18:10:35  tnussb
// version number raised
//
// Revision 1.9  2002/03/21 12:20:39  tnussb
// FastDrawVLine added and version number changed
//
// Revision 1.8  2002/02/25 17:01:01  tnussb
// ttunpack stuff moved in front of ttarchive stuff
//
// Revision 1.7  2002/02/25 13:16:42  tnussb
// ttarchive.h and ttunpack.h integrated
//
// Revision 1.6  2002/02/22 17:13:57  tnussb
// fixed double use of EXTGRAPH_VERSION_MAIN and EXTGRAPH_VERSION_SUB (thanx, Kevin!)
//
// Revision 1.5  2002/02/22 16:31:03  tnussb
// (1) version depended stuff added (like EXTGRAPH_VERSION_STR)
// (2) complete header revised
// (3) CheckHWMatch() and DESIRED_CALCTYPE added
//
// Revision 1.4  2002/02/11 13:43:54  tnussb
// version number raised
//
// Revision 1.3  2002/02/11 10:24:37  tnussb
// generic commit for v0.87
//
// Revision 1.2  2001/06/22 14:33:01  Thomas Nussbaumer
// grayscale sprite drawing routines added
//
// Revision 1.1  2001/06/20 21:34:08  Thomas Nussbaumer
// initial check-in
//
//
