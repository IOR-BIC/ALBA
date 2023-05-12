/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaXMLString
 Authors: Rick Parrish (rfmobile@swbell.net) adapted by Marco Petrone (CINECA)
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "albaXMLString.h"
#include <stddef.h>
#include <stdlib.h>

#ifdef XERCES_CPP_NAMESPACE_USE
// XERCES_CPP_NAMESPACE_USE
// SIL 12-apr-2006
// removed XERCES_CPP_NAMESPACE_USE and added XERCES_CPP_NAMESPACE_QUALIFIER where required
#endif

//------------------------------------------------------------------------------
albaXMLString::albaXMLString(const char *str) : m_WStr(NULL) , m_CStr(NULL)
//------------------------------------------------------------------------------
{
  m_WStr = XERCES_CPP_NAMESPACE_QUALIFIER XMLString::transcode(str);
}

//------------------------------------------------------------------------------
albaXMLString::albaXMLString(XMLCh *wstr) : m_WStr(wstr) , m_CStr(NULL) { };
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
albaXMLString::albaXMLString(const XMLCh *wstr) : m_WStr(NULL) , m_CStr (NULL)
//------------------------------------------------------------------------------
{
  m_WStr = XERCES_CPP_NAMESPACE_QUALIFIER XMLString::replicate(wstr);
}

//------------------------------------------------------------------------------
albaXMLString::albaXMLString(const albaXMLString &right) : m_WStr(NULL) , m_CStr(NULL)
//------------------------------------------------------------------------------
{
  m_WStr = XERCES_CPP_NAMESPACE_QUALIFIER XMLString::replicate(right.m_WStr);
}

albaXMLString::albaXMLString(wxString str)
{
	m_WStr = XERCES_CPP_NAMESPACE_QUALIFIER XMLString::transcode((const char *)str.c_str());
}

//------------------------------------------------------------------------------
albaXMLString::~albaXMLString()
//------------------------------------------------------------------------------
{
  // thanks tinny!!
  if (m_WStr) XERCES_CPP_NAMESPACE_QUALIFIER XMLString::release(&m_WStr);
  if (m_CStr) XERCES_CPP_NAMESPACE_QUALIFIER XMLString::release(&m_CStr);
}

//------------------------------------------------------------------------------
bool albaXMLString::Append(const XMLCh *tail)
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
bool albaXMLString::Erase(const XMLCh *head, const XMLCh *tail)
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
const XMLCh* albaXMLString::Begin() const
//------------------------------------------------------------------------------
{
  return m_WStr;
}

//------------------------------------------------------------------------------
const XMLCh* albaXMLString::End() const
//------------------------------------------------------------------------------
{
  return m_WStr + Size();
}

//------------------------------------------------------------------------------
int albaXMLString::Size() const
//------------------------------------------------------------------------------
{
  return XERCES_CPP_NAMESPACE_QUALIFIER XMLString::stringLen(m_WStr);
}

//------------------------------------------------------------------------------
XMLCh & albaXMLString::operator [] (const int i)
//------------------------------------------------------------------------------
{
  return m_WStr[i];
}

//------------------------------------------------------------------------------
const XMLCh albaXMLString::operator [] (const int i) const
//------------------------------------------------------------------------------
{
  return m_WStr[i];
}

//------------------------------------------------------------------------------
const char *albaXMLString::GetCStr()
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
