#include "mafXMLString.h"
#include <stddef.h>
#include <stdlib.h>

#ifdef XERCES_CPP_NAMESPACE_USE
XERCES_CPP_NAMESPACE_USE
#endif

//------------------------------------------------------------------------------
mafXMLString::mafXMLString(const char *str) : m_WStr(NULL)
//------------------------------------------------------------------------------
{
  m_WStr = XMLString::transcode(str);
}

//------------------------------------------------------------------------------
mafXMLString::mafXMLString(XMLCh *wstr) : m_WStr(wstr) { };
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
mafXMLString::mafXMLString(const XMLCh *wstr) : m_WStr(NULL)
//------------------------------------------------------------------------------
{
  m_WStr = XMLString::replicate(wstr);
}

//------------------------------------------------------------------------------
mafXMLString::mafXMLString(const mafXMLString &right) : m_WStr(NULL)
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
}

//------------------------------------------------------------------------------
bool mafXMLString::append(const XMLCh *tail)
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
bool mafXMLString::erase(const XMLCh *head, const XMLCh *tail)
//------------------------------------------------------------------------------
{
  bool bOK = head <= tail && head >= begin() && tail <= end();
  if (bOK)
  {
    XMLCh *result = new XMLCh[ size() - (tail - head) + 1 ];
    XMLCh *target = result;
    bOK = target != NULL;
    if (bOK)
    {
      const XMLCh *cursor = begin();

      while (cursor != head) *target++ = *cursor++;
      cursor = tail;
      while ( cursor != end() ) *target++ = *cursor++;
      *target ++ = 0;
      XMLString::release(&m_WStr);
      m_WStr = result;
    }
  }
  return bOK;
}

//------------------------------------------------------------------------------
const XMLCh* mafXMLString::begin() const
//------------------------------------------------------------------------------
{
  return m_WStr;
}

//------------------------------------------------------------------------------
const XMLCh* mafXMLString::end() const
//------------------------------------------------------------------------------
{
  return m_WStr + size();
}

//------------------------------------------------------------------------------
int mafXMLString::size() const
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
