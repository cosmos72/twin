




/* This file was automatically generated from m4/tt.m4hh, do not edit! */


/*
 *  ttmethod.h  --  public header for libTT declarations related to "ttmethod"
 *
 *  Copyright (C) 2001-2002 by Massimiliano Ghilardi
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 */



#ifndef _TT_TTMETHOD_H
#define _TT_TTMETHOD_H







/* object structure */
#ifndef s_ttmethod



/**
 * ttmethod: a reflective class, represents a method in an object.
 * It allows runtime inspecting the name, argument types and return types
 * of a method (i.e. almost any libTT function) and allows calling it.
 * 
 * There are 5 different ways to invoke a method using ttmethod objects:
 * TTCall{A,L,R,V,Y}_ttmethod().
 * Each one takes the method to call, the number of arguments you are passing to it
 * (but passing more arguments than needed is harmless), and respectively,
 * a (ttarg []), C variadic arg list, ttvector, (va_list *) or (tany []), of the arguments.
 *
 * A few libTT functions are not represented by ttmethods for various reasons,
 * thus cannot be inspected nor called using ttmethods:
 *
 * 1) functions not related to objects:
 *    TTCheckMagic(), TTOpen(), TTReopen(), TTClose(), TTCloseQuickNDirty(),
 *    TTConnectionFd(), TTInPanic(), TTFlush(), TTTimidFlush(), TTSync(),
 *    TTConfigAllocMem(), TTAllocMem(), TTReallocMem(), TTFreeMem(),
 *    TTMainLoop(), TTMainLoopOnce(), TTExitMainLoop()
 *
 * 2) variadic functions:
 *    TTCreateL_ttvector(), TTCallL_ttmethod()
 *
 */                    
        struct s_ttmethod;
typedef struct s_ttmethod *	ttmethod;

#endif /* s_ttmethod */



/* global runtime class type; returned by TTCLASSOF(<some object>) */
/** class of ttmethod */
extern ttclass	TTClass_ttmethod;





/* ttmethod methods */

/** get the method with given name */
TT_ARG_READ ttmethod TTGet_ttmethod(TT_ARG_READ TT_ARG_ARRAY_Z ttbyte * name);

/** get the method with given address */
TT_ARG_READ ttmethod TTGetByAddress_ttmethod(TT_ARG_READ ttfunction_fn address);

/** get the needed array length for the given method arg */
ttopaque TTGetArraySizeA_ttmethod(TT_ARG_READ ttmethod m,ttopaque arg_array_n,ttopaque args_n,TT_ARG_READ TT_ARG_ARRAY((_P(3))) ttarg * args);

/** check whether the given args have correct array lengths for the method */
ttbyte TTCheckArraySizesA_ttmethod(TT_ARG_READ ttmethod m,ttopaque args_n,TT_ARG_READ TT_ARG_ARRAY((_P(2))) ttarg * args);

/** invoke the method `m' with given args */
ttbyte TTCallA_ttmethod(TT_ARG_READ ttmethod m,TT_ARG_WRITE TT_ARG_ARRAY((1)) ttarg * return_value,ttopaque args_n,TT_ARG_READ TT_ARG_ARRAY((_P(3))) ttarg * args);

/** invoke the method `m' with given args */
ttbyte TTCallL_ttmethod(TT_ARG_READ ttmethod m,TT_ARG_WRITE TT_ARG_ARRAY((1)) ttarg * return_value,ttopaque args_n,... );

/** invoke the method `m' with given args */
ttbyte TTCallR_ttmethod(TT_ARG_READ ttmethod m,TT_ARG_WRITE TT_ARG_ARRAY((1)) ttarg * return_value,TT_ARG_READ ttvector args);

/** invoke the method `m' with given args */
ttbyte TTCallV_ttmethod(TT_ARG_READ ttmethod m,TT_ARG_WRITE TT_ARG_ARRAY((1)) ttarg * return_value,ttopaque args_n,TT_ARG_ARRAY((1)) va_list * args);

/** invoke the method `m' with given args */
ttbyte TTCallY_ttmethod(TT_ARG_READ ttmethod m,TT_ARG_WRITE TT_ARG_ARRAY((1)) ttarg * return_value,ttopaque args_n,TT_ARG_READ TT_ARG_ARRAY((_P(3))) ttany * args);









/* TTGet*_ttmethod and TTSet*_ttmethod methods */
    
/** get `name_len' of ttmethod `o' */
ttopaque 	TTGetNameLen_ttmethod(TT_ARG_READ ttmethod o);
    
/** get `name' of ttmethod `o' */
TT_ARG_READ TT_ARG_ARRAY((_F(name_len))) ttbyte * 	TTGetName_ttmethod(TT_ARG_READ ttmethod o);
    
/** get `address' of ttmethod `o' */
ttfunction_fn 	TTGetAddress_ttmethod(TT_ARG_READ ttmethod o);
    
/** get `return_type' of ttmethod `o' */
ttopaque 	TTGetReturnType_ttmethod(TT_ARG_READ ttmethod o);
    
/** get `parameter_type' of ttmethod `o' */
ttvector 	TTGetParameterType_ttmethod(TT_ARG_READ ttmethod o);










#endif /* _TT_TTMETHOD_H */

