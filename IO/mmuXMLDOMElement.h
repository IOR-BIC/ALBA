/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: mmuXMLDOMElement
 Authors: Marco Petrone
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __mmuXMLDOMElement_h__
#define __mmuXMLDOMElement_h__

#include "albaUtility.h"


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

/** mmuXMLDOMElement utility class for PIMPL of XML DOMElement types.
 */  
//------------------------------------------------------------------------------
// mmuXMLDOMElement
//------------------------------------------------------------------------------
/** PIMPL class to hide XercesC specific XML DOM objects. 
  This class is internally used by albaXMLElement for accessing XML DOM objects.
*/
class ALBA_EXPORT mmuXMLDOMElement: public albaUtility
{
public:
  /** constructor */
  mmuXMLDOMElement( XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *element=NULL): m_XMLElement(element) {}

  XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *m_XMLElement; ///< XML element wrapped by this object 
};

#endif // _mmuXMLDOMElement_h_
