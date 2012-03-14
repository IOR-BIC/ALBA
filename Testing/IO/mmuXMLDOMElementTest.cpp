/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mmuXMLDOMElementTest.cpp,v $
Language:  C++
Date:      $Date: 2010-12-03 12:25:43 $
Version:   $Revision: 1.1.2.1 $
Authors:   Daniele Giunchi
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include <cppunit/config/SourcePrefix.h>

#include <xercesc/sax/ErrorHandler.hpp>
#include <xercesc/sax/SAXParseException.hpp>

#include "mmuXMLDOMElementTest.h"
#include "mmuXMLDOMElement.h"


//----------------------------------------------------------------------------
void mmuXMLDOMElementTest::setUp()
//----------------------------------------------------------------------------
{
  XERCES_CPP_NAMESPACE_QUALIFIER XMLPlatformUtils::Initialize();
}
//----------------------------------------------------------------------------
void mmuXMLDOMElementTest::tearDown()
//----------------------------------------------------------------------------
{
  XERCES_CPP_NAMESPACE_QUALIFIER XMLPlatformUtils::Terminate();
}
//----------------------------------------------------------------------------
void mmuXMLDOMElementTest::TestStaticAllocation()
//----------------------------------------------------------------------------
{
	mmuXMLDOMElement m;
}
//----------------------------------------------------------------------------
void mmuXMLDOMElementTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
	mmuXMLDOMElement *m = new mmuXMLDOMElement();
	cppDEL(m);
}