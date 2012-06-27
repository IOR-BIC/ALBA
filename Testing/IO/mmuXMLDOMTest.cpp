/*=========================================================================

 Program: MAF2
 Module: mmuXMLDOMTest
 Authors: Daniele Giunchi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

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

#include "mmuXMLDOMTest.h"
#include "mmuXMLDOM.h"


//----------------------------------------------------------------------------
void mmuXMLDOMTest::setUp()
//----------------------------------------------------------------------------
{
  XERCES_CPP_NAMESPACE_QUALIFIER XMLPlatformUtils::Initialize();
}
//----------------------------------------------------------------------------
void mmuXMLDOMTest::tearDown()
//----------------------------------------------------------------------------
{
  XERCES_CPP_NAMESPACE_QUALIFIER XMLPlatformUtils::Terminate();
}
//----------------------------------------------------------------------------
void mmuXMLDOMTest::TestStaticAllocation()
//----------------------------------------------------------------------------
{
	mmuXMLDOM m;
}
//----------------------------------------------------------------------------
void mmuXMLDOMTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
	mmuXMLDOM *m = new mmuXMLDOM();
	cppDEL(m);
}