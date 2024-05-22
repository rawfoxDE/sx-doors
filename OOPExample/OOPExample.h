#ifndef OOPEXAMPLE_H
#define OOPEXAMPLE_H



/*****  IMPORTANT NOTE  ******  IMPORTANT NOTE  ******  IMPORTANT NOTE  ******


This source code uses the tag base TAGBASE_BLIZZY, which is defined as
(TAG_USER | (1465UL << 16)).

PLEASE *DO* *NOT* *USE* THIS TAG BASE IN YOUR OWN APPLICATIONS!!!!!!!!!!!!
It is a personalized tag base!!!!!!!!!!

If you need such a tag base for creating attributes and methods identifiers,
you *MUST* write to mccreg@sasg.com and ask for a new tag base for you!!!!!

This is really important!!!


******  IMPORTANT NOTE  ******  IMPORTANT NOTE  ******  IMPORTANT NOTE  *****/



#include <exec/types.h>
#include <intuition/classusr.h>
#include <intuition/classes.h>
#include <utility/tagitem.h>
#include <clib/alib_protos.h>
#include <string.h>


/* copied from <blizzy/constants.h> */
#ifndef TAGBASE_BLIZZY
#define TAGBASE_BLIZZY (TAG_USER | (1465UL << 16))
#endif


/* copied from <blizzy/compilers.h> */
#ifndef REG
#ifdef _DCC
#define REG(x) __##x
#elif defined __GNUC__
#define REG(x)
#else
#define REG(x) register __##x
#endif
#endif
#ifndef GCCREG
#ifdef __GNUC__
#define GCCREG(x) __asm(#x)
#else
#define GCCREG(x)
#endif
#endif
#ifndef REGS
#define REGS(x,p) REG(x) p GCCREG(x)
#endif

#ifndef ASM
#if defined __MAXON__ || defined __STORM__ || defined _DCC || defined __GNUC__
#define ASM
#else
#define ASM __asm
#endif
#endif

#ifndef SAVEDS
#if defined __SASC || defined __STORM__ || defined __GNUC__
#define SAVEDS __saveds
#elif defined _GCC || defined _DCC
#define SAVEDS __geta4
#else
#define SAVEDS
#endif
#endif


/* copied from <blizzy/mui.h> */
#ifndef InitialAttrs
#define InitialAttrs() TAG_MORE, ((struct opSet *) msg)->ops_AttrList
#endif

#ifndef CallSuperClass
#define CallSuperClass() DoSuperMethodA(cl, obj, (Msg) msg)
#endif

#ifndef ClearInstData
#define ClearInstData() memset(data, 0, sizeof(*data))
#endif

#ifndef InitObjAttrs
#define InitObjAttrs() ((Msg) msg)->MethodID = OM_SET; DoMethodA(obj, (Msg) msg); ((Msg) msg)->MethodID = OM_NEW
#endif

#ifndef GetInstData
#define GetInstData() INST_DATA(cl, obj)
#endif

#ifndef StoreAttrValue
#define StoreAttrValue(val) ((*((struct opGet *) msg)->opg_Storage) = (ULONG) (val))
#endif

#ifndef ReturnAttrValue
#define ReturnAttrValue(id,val) case (id): StoreAttrValue(val); return(TRUE)
#endif



#endif   /* OOPEXAMPLE_H */
