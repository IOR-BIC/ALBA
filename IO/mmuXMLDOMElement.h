/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmuXMLDOMElement.h,v $
  Language:  C++
  Date:      $Date: 2005-02-20 23:43:19 $
  Version:   $Revision: 1.2 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __mmuXMLDOMElement_h__
#define __mmuXMLDOMElement_h__

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

/** mmuXMLDOMElement utility class for PIMPL of XML DOMElement types.
 */  
//------------------------------------------------------------------------------
// mmuXMLDOMElement
//------------------------------------------------------------------------------
/** PIMPL class to hide XercesC specific XML DOM objects. 
  This class is internally used by mafXMLElement for accessing XML DOM objects.
*/
class mmuXMLDOMElement: public mmuUtility
{
public:
  mmuXMLDOMElement(DOMElement *element=NULL): m_XMLElement(element) {}

  DOMElement *m_XMLElement; ///< XML element wrapped by this object 
};

#endif // _mmuXMLDOMElement_h_
