/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafXMLString.h,v $
  Language:  C++
  Date:      $Date: 2004-12-29 18:00:28 $
  Version:   $Revision: 1.3 $
  Authors:   Rick Parrish (rfmobile@swbell.net) adapted by Marco Petrone (CINECA)
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mafXMLString_h__
#define __mafXMLString_h__

#include <xercesc/util/XMLString.hpp>

#ifdef XERCES_CPP_NAMESPACE_USE
XERCES_CPP_NAMESPACE_USE
#endif

/** string type for converting const char * to and from XMLString types
  This is an internal string type to be used only in conjuction with XercecC XML library.
  This object is able to convert const char* to and from XMLStrings. This code has been
  adapted from example code "class XercesString" found in articles "Make the most of Xerces-C++"
  by Rick Parrish (rfmobile@swbell.net) that can be found at "www.ibm.com/developerworks/xml".
*/
class mafXMLString
{
public:
	mafXMLString() : m_WStr(0L) { };
	mafXMLString(const char *str);
	mafXMLString(XMLCh *wstr);
	mafXMLString(const XMLCh *wstr);
	mafXMLString(const mafXMLString &copy);
	~mafXMLString();
	bool Append(const XMLCh *tail);
	bool Erase(const XMLCh *head, const XMLCh *tail);
	const XMLCh* Begin() const;
	const XMLCh* End() const;
	int Size() const;
  const char *GetCStr();
	XMLCh & operator [] (const int i);
	const XMLCh operator [] (const int i) const;
	operator const XMLCh * () const { return m_WStr; };
  operator const char * () {return GetCStr();}
protected:
  XMLCh *m_WStr;
  char *m_CStr;
};

#endif
