/* -------------------------------------------------------------------------
// WINX: a C++ template GUI library - MOST SIMPLE BUT EFFECTIVE
// 
// This file is a part of the WINX Library.
// The use and distribution terms for this software are covered by the
// Common Public License 1.0 (http://opensource.org/licenses/cpl.php)
// which can be found in the file CPL.txt at this distribution. By using
// this software in any fashion, you are agreeing to be bound by the terms
// of this license. You must not remove this notice, or any other, from
// this software.
// 
// Module: wrapper/pcreapi.h
// Creator: xushiwei
// Email: xushiweizh@gmail.com
// Date: 2009-7-2 23:57:09
// 
// $Id: pcreapi.h,v 2009-7-2 23:57:09 xushiwei Exp $
// -----------------------------------------------------------------------*/
#ifndef WRAPPER_PCREAPI_H
#define WRAPPER_PCREAPI_H

// -------------------------------------------------------------------------

#ifdef __cplusplus
#  ifndef PCRECPP_EXP_DECL
#    define PCRECPP_EXP_DECL  extern
#  endif
#  ifndef PCRECPP_EXP_DEFN
#    define PCRECPP_EXP_DEFN
#  endif
#endif

// -------------------------------------------------------------------------

#ifndef _PCRE_H
#include "pcre/pcre.h"
#endif

// -------------------------------------------------------------------------
// Link pcre.lib

#if !defined(Wrapper_Linked_pcre)
#define Wrapper_Linked_pcre
#pragma comment(lib, "pcre")
#endif

// -------------------------------------------------------------------------

#endif /* WRAPPER_PCREAPI_H */
