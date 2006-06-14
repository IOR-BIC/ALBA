/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafXMLString.cpp,v $
  Language:  C++
  Date:      $Date: 2006-06-14 14:46:33 $
  Version:   $Revision: 1.6 $
  Authors:   Rick Parrish (rfmobile@swbell.net) adapted by Marco Petrone (CINECA)
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "mafXMLString.h"
#include <stddef.h>
#include <stdlib.h>

#ifdef XERCES_CPP_NAMESPACE_USE
// XERCES_CPP_NAMESPACE_USE
// SIL 12-apr-2006
// removed XERCES_CPP_NAMESPACE_USE and added XERCES_CPP_NAMESPACE_QUALIFIER where required
#endif

//------------------------------------------------------------------------------
mafXMLString::mafXMLString(const char *str) : m_WStr(NULL) , m_CStr(NULL)
//------------------------------------------------------------------------------
{
  m_WStr = XERCES_CPP_NAMESPACE_QUALIFIER XMLString::transcode(str);
}

//------------------------------------------------------------------------------
mafXMLString::mafXMLString(XMLCh *wstr) : m_WStr(wstr) , m_CStr(NULL) { };
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
mafXMLString::mafXMLString(const XMLCh *wstr) : m_WStr(NULL) , m_CStr (NULL)
//------------------------------------------------------------------------------
{
  m_WStr = XERCES_CPP_NAMESPACE_QUALIFIER XMLString::replicate(wstr);
}

//------------------------------------------------------------------------------
mafXMLString::mafXMLString(const mafXMLString &right) : m_WStr(NULL) , m_CStr(NULL)
//------------------------------------------------------------------------------
{
  m_WStr = XERCES_CPP_NAMESPACE_QUALIFIER XMLString::replicate(right.m_WStr);
}

//------------------------------------------------------------------------------
mafXMLString::~mafXMLString()
//------------------------------------------------------------------------------
{
  // thanks tinny!!
  if (m_WStr) XERCES_CPP_NAMESPACE_QUALIFIER XMLString::release(&m_WStr);
  if (m_CStr) XERCES_CPP_NAMESPACE_QUALIFIER XMLString::release(&m_CStr);
}

//------------------------------------------------------------------------------
bool mafXMLString::Append(const XMLCh *tail)
//------------------------------------------------------------------------------
{
  int iTailLen = XERCES_CPP_NAMESPACE_QUALIFIER XMLString::stringLen(tail);
  int iWorkLen = XERCES_CPP_NAMESPACE_QUALIFIER XMLString::stringLen(m_WStr);
  XMLCh *result = new XMLCh[ iWorkLen + iTailLen + 1 ];
  bool bOK = result != NULL;
  if (bOK)
  {
    XMLCh *target = result;
    XERCES_CPP_NAMESPACE_QUALIFIER XMLString::moveChars(target, m_WStr, iWorkLen);
    target += iWorkLen;
    XERCES_CPP_NAMESPACE_QUALIFIER XMLString::moveChars(target, tail, iTailLen);
    target += iTailLen;
    *target++ = 0;
    XERCES_CPP_NAMESPACE_QUALIFIER XMLString::release(&m_WStr);
    m_WStr = result;
  }
  return bOK;
}

//------------------------------------------------------------------------------
bool mafXMLString::Erase(const XMLCh *head, const XMLCh *tail)
//------------------------------------------------------------------------------
{
  bool bOK = head <= tail && head >= Begin() && tail <= End();
  if (bOK)
  {
    XMLCh *result = new XMLCh[ Size() - (tail - head) + 1 ];
    XMLCh *target = result;
    bOK = target != NULL;
    if (bOK)
    {
      const XMLCh *cursor = Begin();

      while (cursor != head) *target++ = *cursor++;
      cursor = tail;
      while ( cursor != End() ) *target++ = *cursor++;
      *target ++ = 0;
      XERCES_CPP_NAMESPACE_QUALIFIER XMLString::release(&m_WStr);
      m_WStr = result;
    }
  }
  return bOK;
}

//------------------------------------------------------------------------------
const XMLCh* mafXMLString::Begin() const
//------------------------------------------------------------------------------
{
  return m_WStr;
}

//------------------------------------------------------------------------------
const XMLCh* mafXMLString::End() const
//------------------------------------------------------------------------------
{
  return m_WStr + Size();
}

//------------------------------------------------------------------------------
int mafXMLString::Size() const
//------------------------------------------------------------------------------
{
  return XERCES_CPP_NAMESPACE_QUALIFIER XMLString::stringLen(m_WStr);
}

//------------------------------------------------------------------------------
XMLCh & mafXMLString::operator [] (const int i)
//------------------------------------------------------------------------------
{
  return m_WStr[i];
}

//------------------------------------------------------------------------------
const XMLCh mafXMLString::operator [] (const int i) const
//------------------------------------------------------------------------------
{
  return m_WStr[i];
}

//------------------------------------------------------------------------------
const char *mafXMLString::GetCStr()
//------------------------------------------------------------------------------
{
  if (m_WStr)
  {
    if (m_CStr==NULL)
    {
      m_CStr = XERCES_CPP_NAMESPACE_QUALIFIER XMLString::transcode(m_WStr);
    }

    return m_CStr;
  }
  
  return NULL;
}
