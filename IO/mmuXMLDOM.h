/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmuXMLDOM.h,v $
  Language:  C++
  Date:      $Date: 2005-02-20 23:43:19 $
  Version:   $Revision: 1.3 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __mmuXMLDOM_h__
#define __mmuXMLDOM_h__

#include "mmuUtility.h"

#include <xercesc/dom/DOM.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>
#include <xercesc/dom/impl/DOMWriterImpl.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>

#ifdef XERCES_CPP_NAMESPACE_USE
XERCES_CPP_NAMESPACE_USE
#endif

/** mmuXMLDOM utility class for PIMPL of XML DOM types.
 */  
//------------------------------------------------------------------------------
// mmuXMLDOM
//------------------------------------------------------------------------------
/** PIMPL class to hide XercesC specific objects. 
  This class is internally used by mafXMLStorage and mafXMLElement for accessing
  XML DOM objects.
*/
class mmuXMLDOM: public mmuUtility
{
public:
  mmuXMLDOM(): m_XMLDoctype(NULL),m_XMLDoc(NULL),m_XMLNode(NULL), \
    m_XMLImplement(NULL),m_XMLSerializer(NULL), \
    m_XMLTarget(NULL),m_XMLParser(NULL) {}
  DOMDocumentType   *m_XMLDoctype;
  DOMDocument       *m_XMLDoc;
  DOMNode           *m_XMLNode;
  DOMImplementation *m_XMLImplement;
  DOMWriter         *m_XMLSerializer;
  XMLFormatTarget   *m_XMLTarget;
  XercesDOMParser   *m_XMLParser;
};

#endif // _mmuXMLDOM_h_
