/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafXMLString.h,v $
  Language:  C++
  Date:      $Date: 2004-12-27 18:22:26 $
  Version:   $Revision: 1.1 $
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
	bool append(const XMLCh *tail);
	bool erase(const XMLCh *head, const XMLCh *tail);
	const XMLCh* begin() const;
	const XMLCh* end() const;
	int size() const;
	XMLCh & operator [] (const int i);
	const XMLCh operator [] (const int i) const;
	operator const XMLCh * () const { return m_WStr; };
protected:
  XMLCh *m_WStr;
};

#endif
