/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafXMLStorage.cpp,v $
  Language:  C++
  Date:      $Date: 2004-12-27 18:22:26 $
  Version:   $Revision: 1.1 $
  Authors:   Marco Petrone m.petrone@cineca.it
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "mafXMLStorage.h"
#include "mafXMLElement.h"
#include "mafXMLString.h"
#include "mafStorable.h"
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/framework/StdOutFormatTarget.hpp>
#include <xercesc/dom/impl/DOMWriterImpl.hpp>

//#ifdef XERCES_CPP_NAMESPACE_USE
//XERCES_CPP_NAMESPACE_USE
//#endif

//------------------------------------------------------------------------------
mafXMLStorage::mafXMLStorage()
//------------------------------------------------------------------------------
{
  m_XMLDoctype = NULL;
  m_XMLDoc = NULL;
  m_XMLNode = NULL;
  m_XMLImplement = NULL;
  // these two are needed to display DOM output.
  m_XMLSerializer = NULL;
  m_XMLTarget = NULL;
}

//------------------------------------------------------------------------------
mafXMLStorage::~mafXMLStorage()
//------------------------------------------------------------------------------
{
}

//------------------------------------------------------------------------------
//mafXMLElement *NewXMLElement(const char *name)
//------------------------------------------------------------------------------
//{
//  return new mafXMLElement()
//}

//------------------------------------------------------------------------------
void mafXMLStorage::Store(const char *root_tag)
//------------------------------------------------------------------------------
{
  // initialize the XML library
  XMLPlatformUtils::Initialize();

  // get a serializer, an instance of DOMWriter (the "LS" stands for load-save).
  m_XMLImplement = DOMImplementationRegistry::getDOMImplementation(mafXMLString("LS"));
  m_XMLSerializer = ( (DOMImplementationLS*)m_XMLImplement )->createDOMWriter();
  m_XMLTarget = new StdOutFormatTarget();
  // set user specified end of line sequence and output encoding
  m_XMLSerializer->setNewLine( mafXMLString("\n") );

  // set feature if the serializer supports the feature/mode
  if ( m_XMLSerializer->canSetFeature(XMLUni::fgDOMWRTSplitCdataSections, false) )
  	m_XMLSerializer->setFeature(XMLUni::fgDOMWRTSplitCdataSections, false);

  if ( m_XMLSerializer->canSetFeature(XMLUni::fgDOMWRTDiscardDefaultContent, false) )
  	m_XMLSerializer->setFeature(XMLUni::fgDOMWRTDiscardDefaultContent, false);

  if ( m_XMLSerializer->canSetFeature(XMLUni::fgDOMWRTFormatPrettyPrint, true) )
  	m_XMLSerializer->setFeature(XMLUni::fgDOMWRTFormatPrettyPrint, true);

  if ( m_XMLSerializer->canSetFeature(XMLUni::fgDOMWRTBOM, false) )
  	m_XMLSerializer->setFeature(XMLUni::fgDOMWRTBOM, false);

  // create a document
  m_XMLDoc = m_XMLImplement->createDocument( NULL, mafXMLString("MSF"), NULL ); // NO URI and NO DTD
  if (m_XMLDoc)
  {
    // output related nodes are prefixed with "svg"
    // to distinguish them from input nodes.
	m_XMLDoc->setEncoding( mafXMLString("UTF-8") );
	m_XMLDoc->setStandalone(true);
	m_XMLDoc->setVersion( mafXMLString("1.0") );

  DOMElement *m_RootElement = m_XMLDoc->getDocumentElement();

  // call Store function of the m_Root object
  assert(m_Root);
  m_Root->Store(root_tag);

  // destroy all intermediate objects
  cppDEL (m_XMLTarget);
  cppDEL (m_XMLSerializer);
  cppDEL (m_XMLDoc);
  cppDEL (m_XMLDoctype);
 
  // terminate the XML library
  XMLPlatformUtils::Terminate();
}

//------------------------------------------------------------------------------
int mafXMLStorage::InternalRestore()
//------------------------------------------------------------------------------
{
}
