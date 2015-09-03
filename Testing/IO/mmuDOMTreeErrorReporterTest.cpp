/*=========================================================================

 Program: MAF2
 Module: mmuDOMTreeErrorReporterTest
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

#include "mmuDOMTreeErrorReporterTest.h"
#include "mmuDOMTreeErrorReporter.h"
#include "mafXMLString.h"

//----------------------------------------------------------------------------
void mmuDOMTreeErrorReporterTest::BeforeTest()
//----------------------------------------------------------------------------
{
  XERCES_CPP_NAMESPACE_QUALIFIER XMLPlatformUtils::Initialize();
}
//----------------------------------------------------------------------------
void mmuDOMTreeErrorReporterTest::AfterTest()
//----------------------------------------------------------------------------
{
  XERCES_CPP_NAMESPACE_QUALIFIER XMLPlatformUtils::Terminate();
}
//----------------------------------------------------------------------------
void mmuDOMTreeErrorReporterTest::TestStaticAllocation()
//----------------------------------------------------------------------------
{
	mmuDOMTreeErrorReporter m;
}
//----------------------------------------------------------------------------
void mmuDOMTreeErrorReporterTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
	mmuDOMTreeErrorReporter *m = new mmuDOMTreeErrorReporter();
	cppDEL(m);
}
//----------------------------------------------------------------------------
void mmuDOMTreeErrorReporterTest::TestWarning()
//----------------------------------------------------------------------------
{
  mmuDOMTreeErrorReporter *m = new mmuDOMTreeErrorReporter();
  CPPUNIT_ASSERT(!m->GetSawErrors());
  XERCES_CPP_NAMESPACE_QUALIFIER SAXParseException parseError(
    NULL
    , NULL
    , NULL
    , 50
    , 30
    /*, XMLPlatformUtils::fgMemoryManager*/
    );

  m->warning(parseError);
  CPPUNIT_ASSERT(!m->GetSawErrors());
  cppDEL(m);
}
//----------------------------------------------------------------------------
void mmuDOMTreeErrorReporterTest::TestError()
//----------------------------------------------------------------------------
{
  mmuDOMTreeErrorReporter *m = new mmuDOMTreeErrorReporter();
  m->SetTestMode(true);
  CPPUNIT_ASSERT(!m->GetSawErrors());
  XERCES_CPP_NAMESPACE_QUALIFIER SAXParseException parseError(
    NULL
    , NULL
    , NULL
    , 50
    , 30
    /*, XMLPlatformUtils::fgMemoryManager*/
    );

  m->error(parseError);
  CPPUNIT_ASSERT(m->GetSawErrors());
  cppDEL(m);
}
//----------------------------------------------------------------------------
void mmuDOMTreeErrorReporterTest::TestFatalError()
//----------------------------------------------------------------------------
{
  mmuDOMTreeErrorReporter *m = new mmuDOMTreeErrorReporter();
  m->SetTestMode(true);
  CPPUNIT_ASSERT(!m->GetSawErrors());
  XERCES_CPP_NAMESPACE_QUALIFIER SAXParseException parseError(
    NULL
    , NULL
    , NULL
    , 50
    , 30
    /*, XMLPlatformUtils::fgMemoryManager*/
    );

  m->fatalError(parseError);
  CPPUNIT_ASSERT(m->GetSawErrors());
  cppDEL(m);
}
//----------------------------------------------------------------------------
void mmuDOMTreeErrorReporterTest::TestResetErrors()
//----------------------------------------------------------------------------
{
  mmuDOMTreeErrorReporter *m = new mmuDOMTreeErrorReporter();
  m->SetTestMode(true);
  CPPUNIT_ASSERT(!m->GetSawErrors());
  XERCES_CPP_NAMESPACE_QUALIFIER SAXParseException parseError(
    NULL
    , NULL
    , NULL
    , 50
    , 30
    /*, XMLPlatformUtils::fgMemoryManager*/
    );

  m->fatalError(parseError);
  CPPUNIT_ASSERT(m->GetSawErrors());

  m->resetErrors();
  CPPUNIT_ASSERT(!m->GetSawErrors());
  cppDEL(m);
}