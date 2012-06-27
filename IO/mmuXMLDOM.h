/*=========================================================================

 Program: MAF2
 Module: mmuXMLDOM
 Authors: Marco Petrone
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __mmuXMLDOM_h__
#define __mmuXMLDOM_h__

#include "mafUtility.h"

#include <xercesc/dom/DOM.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>
#include <xercesc/dom/impl/DOMWriterImpl.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>

#ifdef XERCES_CPP_NAMESPACE_USE
// XERCES_CPP_NAMESPACE_USE
// SIL 12-apr-2006
// removed XERCES_CPP_NAMESPACE_USE and added XERCES_CPP_NAMESPACE_QUALIFIER where required
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
class MAF_EXPORT mmuXMLDOM: public mafUtility
{
public:
  /** constructor */
  mmuXMLDOM(): m_XMLDoctype(NULL),m_XMLDoc(NULL),m_XMLNode(NULL), \
    m_XMLImplement(NULL),m_XMLSerializer(NULL), \
    m_XMLTarget(NULL),m_XMLParser(NULL) {}

  XERCES_CPP_NAMESPACE_QUALIFIER DOMDocumentType   *m_XMLDoctype;
  XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument       *m_XMLDoc;  
  XERCES_CPP_NAMESPACE_QUALIFIER DOMNode           *m_XMLNode;
  XERCES_CPP_NAMESPACE_QUALIFIER DOMImplementation *m_XMLImplement;
  XERCES_CPP_NAMESPACE_QUALIFIER DOMWriter         *m_XMLSerializer;
  XERCES_CPP_NAMESPACE_QUALIFIER XMLFormatTarget   *m_XMLTarget;
  XERCES_CPP_NAMESPACE_QUALIFIER XercesDOMParser   *m_XMLParser;
};

#endif // _mmuXMLDOM_h_
