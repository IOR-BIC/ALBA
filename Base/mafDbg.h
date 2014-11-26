/*========================================================================= 
  Program: Multimod Application Framework RELOADED 
  Module: $RCSfile: mafDbg.h,v $ 
  Language: C++ 
  Date: $Date: 2009-05-14 11:42:25 $ 
  Version: $Revision: 1.1.2.1 $ 
  Authors: Josef Kohout (Josef.Kohout *AT* beds.ac.uk)
  ========================================================================== 
  Copyright (c) 2008 University of Bedfordshire (www.beds.ac.uk)
  See the COPYINGS file for license details 
  =========================================================================
  This header files contain some useful ASSERT and VERIFY type checks.  
  Both macros contain condition that must be fullfiled. If the condition
  (given in an expression)is violated, it is handled in some way.
  While ASSERT macros are present in the code compiled in _DEBUG mode only (they
  are removed from code in RELEASE), VERIFY macros work in both modes, although
  their behaviour might be slightly different (e.g., VERIFY does not display
  messages into debug window. The behaviour of macros is, indeed, subject
  to platform under which the code is compiled.

  Macros may have one or more suffices: 
  [_EXPR][_CMD][_RPT][[_RET] or [_RETVAL] or [_THROW]]

  _EXPR - have the expression (message) to be displayed if the condition is violated
  _CMD - if the condition is violated, a user given command is executed
  _RTP - if the condition is violated, it is reported but the program is not stopped
  _RET - if the condition is violated, the call of the current routine is terminated
  _RETVAL - similar to _RET but specifies the value to be returned
  _THROW - throws an exception if the condition is violated
*/
#ifndef mafDbg_h__
#define mafDbg_h__

#pragma once

#if defined(_MSC_VER)
#include <crtdbg.h>
#endif

#include <assert.h>

//PLATFORM DEPENDENT BASIC MACROS
#if defined(_MSC_VER) && defined(_DEBUG)
#define _DBG_RPT(msg) _CrtDbgReport(_CRT_WARN, __FILE__, __LINE__, NULL, msg)
#define _DBG_BREAK(msg)  _CrtDbgReport(_CRT_ASSERT, __FILE__, __LINE__, NULL, msg)

#undef _ASSERT
#define _ASSERT(expr) _ASSERTE(expr)
#else
#define _DBG_RPT(msg) fprintf(stderr, "%s at %s(%d)\n", \
  (msg == NULL ? "Assert " : msg), __FILE__, __LINE__)
#define _DBG_BREAK(msg)  assert(false)

#ifndef _ASSERT_EXPR
#define _ASSERT_EXPR(expr, msg) assert(expr)
#endif

#ifndef _ASSERT
#define _ASSERT(expr)   _ASSERT_EXPR((expr), #expr)
#endif

#ifndef _ASSERTE
#define _ASSERTE(expr)  _ASSERT_EXPR((expr), #expr)
#endif
#endif


//ALL PLATFORMS
#ifdef _DEBUG

#define _ASSERT_EXPR_RPT(expr, msg) if (!(expr)) _DBG_RPT(msg)
#define _ASSERT_RTP(expr)  _ASSERT_EXPR_RPT(expr, #expr)

#define _ASSERT_EXPR_CMD(expr, msg, command) if (!(expr)) {_DBG_BREAK(msg); command; }
#define _ASSERT_CMD(expr, command)  _ASSERT_EXPR_CMD(expr, #expr, command)
#define _ASSERT_EXPR_CMD_RPT(expr, msg, command) if (!(expr)) {_DBG_RPT(msg); command; }
#define _ASSERT_CMD_RPT(expr, command)  _ASSERT_EXPR_CMD_RPT(expr, #expr, command)

#define _ASSERT_EXPR_RET(expr, msg) _ASSERT_EXPR_CMD(expr, msg, return)
#define _ASSERT_RET(expr)  _ASSERT_EXPR_CMD(expr, #expr, return)
#define _ASSERT_EXPR_RPT_RET(expr, msg) _ASSERT_EXPR_CMD_RPT(expr, msg, return)
#define _ASSERT_PRT_RET(expr)  _ASSERT_EXPR_CMD_RPT(expr, #expr, return)

#define _ASSERT_EXPR_RETVAL(expr, msg, retval) _ASSERT_EXPR_CMD(expr, msg, return (retval))
#define _ASSERT_RETVAL(expr, retval)  _ASSERT_EXPR_CMD(expr, #expr, return (retval))
#define _ASSERT_EXPR_RPT_RETVAL(expr, msg, retval) _ASSERT_EXPR_CMD_RPT(expr, msg, return (retval))
#define _ASSERT_PRT_RETVAL(expr, retval)  _ASSERT_EXPR_CMD_RPT(expr, #expr, return (retval))

#define _ASSERT_EXPR_THROW(expr, msg, exc) _ASSERT_EXPR_CMD(expr, msg, throw (exc))
#define _ASSERT_THROW(expr, exc)  _ASSERT_EXPR_CMD(expr, #expr, throw (exc))
#define _ASSERT_EXPR_RPT_THROW(expr, msg, exc) _ASSERT_EXPR_CMD_RPT(expr, msg, throw (exc))
#define _ASSERT_PRT_THROW(expr, exc)  _ASSERT_EXPR_CMD_RPT(expr, #expr, throw (exc))

#define _VERIFY_EXPR(expr, msg) _ASSERT_EXPR(expr, msg)
#define _VERIFY(expr) _ASSERT(expr)

#define _VERIFY_EXPR_RPT(expr, msg) if (!(expr)) _DBG_RPT(msg)
#define _VERIFY_RTP(expr) _ASSERT_RTP(expr)

#define _VERIFY_EXPR_CMD(expr, msg, command) if (!(expr)) {_DBG_BREAK(msg); command; }
#define _VERIFY_CMD(expr, command) _ASSERT_CMD(expr, command)
#define _VERIFY_EXPR_CMD_RPT(expr, msg, command) if (!(expr)) {_DBG_RPT(msg); command; }
#define _VERIFY_CMD_RPT(expr, command) _ASSERT_CMD_RPT(expr, command)

#define _VERIFY_EXPR_RET(expr, msg) _ASSERT_EXPR_RET(expr, msg)
#define _VERIFY_RET(expr) _ASSERT_RET(expr)
#define _VERIFY_EXPR_RPT_RET(expr, msg) _ASSERT_EXPR_RPT_RET(expr, msg)
#define _VERIFY_PRT_RET(expr) _ASSERT_PRT_RET(expr)

#define _VERIFY_EXPR_RETVAL(expr, msg, retval) _ASSERT_EXPR_RETVAL(expr, msg, retval)
#define _VERIFY_RETVAL(expr, retval) _ASSERT_RETVAL(expr, retval)
#define _VERIFY_EXPR_RPT_RETVAL(expr, msg, retval) _ASSERT_EXPR_RPT_RETVAL(expr, msg, retval)
#define _VERIFY_PRT_RETVAL(expr, retval) _ASSERT_PRT_RETVAL(expr, retval)

#define _VERIFY_EXPR_THROW(expr, msg, exc) _ASSERT_EXPR_THROW(expr, msg, exc)
#define _VERIFY_THROW(expr, exc) _ASSERT_THROW(expr, exc)
#define _VERIFY_EXPR_RPT_THROW(expr, msg, exc) _ASSERT_EXPR_RPT_THROW(expr, msg, exc)
#define _VERIFY_PRT_THROW(expr, exc) _ASSERT_PRT_THROW(expr, exc)
#else
//RELEASE mode

#define _ASSERT_EXPR_RPT(expr, msg) ((void)0)
#define _ASSERT_RTP(expr) ((void)0)

#define _ASSERT_EXPR_CMD(expr, msg, command) ((void)0)
#define _ASSERT_CMD(expr, command) ((void)0)
#define _ASSERT_EXPR_CMD_RPT(expr, msg, command) ((void)0)
#define _ASSERT_CMD_RPT(expr, command) ((void)0)

#define _ASSERT_EXPR_RET(expr, msg) ((void)0)
#define _ASSERT_RET(expr) ((void)0)
#define _ASSERT_EXPR_RPT_RET(expr, msg) ((void)0)
#define _ASSERT_PRT_RET(expr) ((void)0)

#define _ASSERT_EXPR_RETVAL(expr, msg, retval) ((void)0)
#define _ASSERT_RETVAL(expr, retval) ((void)0)
#define _ASSERT_EXPR_RPT_RETVAL(expr, msg, retval) ((void)0)
#define _ASSERT_PRT_RETVAL(expr, retval) ((void)0)

#define _ASSERT_EXPR_THROW(expr, msg, exc) ((void)0)
#define _ASSERT_THROW(expr, exc) ((void)0)
#define _ASSERT_EXPR_RPT_THROW(expr, msg, exc) ((void)0)
#define _ASSERT_PRT_THROW(expr, exc)  _ASSERT_EXPR_CMD_RPT(expr, #expr, throw (exc))

#define _VERIFY_EXPR(expr, msg) expr
#define _VERIFY(expr) expr

#define _VERIFY_EXPR_RPT(expr, msg) expr
#define _VERIFY_RTP(expr) expr

#define _VERIFY_EXPR_CMD(expr, msg, command) if (!(expr)) { command; }
#define _VERIFY_CMD(expr, command) if (!(expr)) { command; }
#define _VERIFY_EXPR_CMD_RPT(expr, msg, command) if (!(expr)) { command; }
#define _VERIFY_CMD_RPT(expr, command) if (!(expr)) { command; }

#define _VERIFY_EXPR_RET(expr, msg) _VERIFY_CMD(expr, return)
#define _VERIFY_RET(expr) _VERIFY_CMD(expr, return)
#define _VERIFY_EXPR_RPT_RET(expr, msg) _VERIFY_CMD(expr, return)
#define _VERIFY_PRT_RET(expr) _VERIFY_CMD(expr, return)

#define _VERIFY_EXPR_RETVAL(expr, msg, retval) _VERIFY_CMD(expr, return (retval))
#define _VERIFY_RETVAL(expr, retval) _VERIFY_CMD(expr, return (retval))
#define _VERIFY_EXPR_RPT_RETVAL(expr, msg, retval) _VERIFY_CMD(expr, return (retval))
#define _VERIFY_PRT_RETVAL(expr, retval) _VERIFY_CMD(expr, return (retval))

#define _VERIFY_EXPR_THROW(expr, msg, exc) _VERIFY_CMD(expr, throw (exc))
#define _VERIFY_THROW(expr, exc) _VERIFY_CMD(expr, throw (exc))
#define _VERIFY_EXPR_RPT_THROW(expr, msg, exc) _VERIFY_CMD(expr, throw (exc))
#define _VERIFY_PRT_THROW(expr, exc) _VERIFY_CMD(expr, throw (exc))

#endif


#endif // mafDbg_h__