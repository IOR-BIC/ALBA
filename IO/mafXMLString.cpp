#include "mafXMLString.h"
#include <stddef.h>
#include <stdlib.h>

#ifdef XERCES_CPP_NAMESPACE_USE
XERCES_CPP_NAMESPACE_USE
#endif

//------------------------------------------------------------------------------
mafXMLString::mafXMLString(const char *str) : m_WStr(NULL) , m_CStr(NULL)
//------------------------------------------------------------------------------
{
  m_WStr = XMLString::transcode(str);
}

//------------------------------------------------------------------------------
mafXMLString::mafXMLString(XMLCh *wstr) : m_WStr(wstr) , m_CStr(NULL) { };
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
mafXMLString::mafXMLString(const XMLCh *wstr) : m_WStr(NULL) , m_CStr (NULL)
//------------------------------------------------------------------------------
{
  m_WStr = XMLString::replicate(wstr);
}

//------------------------------------------------------------------------------
mafXMLString::mafXMLString(const mafXMLString &right) : m_WStr(NULL) , m_CStr(NULL)
//------------------------------------------------------------------------------
{
  m_WStr = XMLString::replicate(right.m_WStr);
}

//------------------------------------------------------------------------------
mafXMLString::~mafXMLString()
//------------------------------------------------------------------------------
{
  // thanks tinny!!
  if (m_WStr) XMLString::release(&m_WStr);
  if (m_CStr) XMLString::release(&m_CStr);
}

//------------------------------------------------------------------------------
bool mafXMLString::Append(const XMLCh *tail)
//------------------------------------------------------------------------------
{
  int iTailLen = XMLString::stringLen(tail);
  int iWorkLen = XMLString::stringLen(m_WStr);
  XMLCh *result = new XMLCh[ iWorkLen + iTailLen + 1 ];
  bool bOK = result != NULL;
  if (bOK)
  {
    XMLCh *target = result;
    XMLString::moveChars(target, m_WStr, iWorkLen);
    target += iWorkLen;
    XMLString::moveChars(target, tail, iTailLen);
    target += iTailLen;
    *target++ = 0;
    XMLString::release(&m_WStr);
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
      XMLString::release(&m_WStr);
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
  return XMLString::stringLen(m_WStr);
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
      m_CStr = XMLString::transcode(m_WStr);
    }

    return m_CStr;
  }
  
  return NULL;
}