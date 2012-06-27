/*=========================================================================

 Program: MAF2
 Module: mafXMLStringTest
 Authors: Alberto Losi
 
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

#include "mafXMLString.h"
#include "mafXMLStringTest.h"

//----------------------------------------------------------------------------
void mafXMLStringTest::setUp()
//----------------------------------------------------------------------------
{
  XERCES_CPP_NAMESPACE_QUALIFIER XMLPlatformUtils::Initialize();
}
//----------------------------------------------------------------------------
void mafXMLStringTest::tearDown()
//----------------------------------------------------------------------------
{
  XERCES_CPP_NAMESPACE_QUALIFIER XMLPlatformUtils::Terminate();
}
//----------------------------------------------------------------------------
void mafXMLStringTest::TestStaticAllocation()
//----------------------------------------------------------------------------
{
	mafXMLString s;
}
//----------------------------------------------------------------------------
void mafXMLStringTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
	mafXMLString *s = new mafXMLString();
	cppDEL(s);
}
//----------------------------------------------------------------------------
void mafXMLStringTest::TestAllConstructors()
//----------------------------------------------------------------------------
{
  mafXMLString s1 = mafXMLString();

  const mafXMLString s2 = mafXMLString("test string");

  mafXMLString s3 = mafXMLString(s2);

  XMLCh *xml_s = 0L;

  mafXMLString s4 = mafXMLString(xml_s);
}
//----------------------------------------------------------------------------
void mafXMLStringTest::TestAppend()
//----------------------------------------------------------------------------
{
  mafXMLString s1 = mafXMLString("test string for");
  mafXMLString s2 = mafXMLString(" Append");

  s1.Append(s2);

  const char *cstr_s1 = s1.GetCStr();

  CPPUNIT_ASSERT(strcmp(cstr_s1, "test string for Append") == 0);
}
//----------------------------------------------------------------------------
void mafXMLStringTest::TestErase()
//----------------------------------------------------------------------------
{
  mafXMLString s = mafXMLString("test string for Erase");

  s.Erase(s.Begin(),s.Begin() + 5);

  const char *cstr_s = s.GetCStr();

  CPPUNIT_ASSERT(strcmp(cstr_s, "string for Erase") == 0);
}
//----------------------------------------------------------------------------
void mafXMLStringTest::TestBegin()
//----------------------------------------------------------------------------
{
  mafXMLString s = mafXMLString("test string for Begin");

  const XMLCh *beg_c = s.Begin();
  XMLCh c = s[0];

  CPPUNIT_ASSERT(c == *beg_c);
}
//----------------------------------------------------------------------------
void mafXMLStringTest::TestEnd()
//----------------------------------------------------------------------------
{
  mafXMLString s = mafXMLString("test string for End");

  const XMLCh *end_c = s.End();
  XMLCh c = s[s.Size()];

  CPPUNIT_ASSERT(c == *end_c);
}
//----------------------------------------------------------------------------
void mafXMLStringTest::TestSize()
//----------------------------------------------------------------------------
{
  mafXMLString s = mafXMLString("test string for Size");

  CPPUNIT_ASSERT(s.Size() == 20);
}
//----------------------------------------------------------------------------
void mafXMLStringTest::TestGetCStr()
//----------------------------------------------------------------------------
{
  mafXMLString s = mafXMLString("test string for GetCStr");

  const char *cstr_s = s.GetCStr();

  CPPUNIT_ASSERT(strcmp(cstr_s, "test string for GetCStr") == 0);
}
//----------------------------------------------------------------------------
void mafXMLStringTest::TestAllOperators()
//----------------------------------------------------------------------------
{
  mafXMLString s = mafXMLString("test string for Operators");

  // test [] operator
  XMLCh c = s[2];
  CPPUNIT_ASSERT(c == *(s.Begin() + 2));

  // test * operator
  const XMLCh *xml_c =  (const XMLCh*)s;
  for(int i = 0; i < s.Size(); i++)
  {
    CPPUNIT_ASSERT(xml_c[i] == s[i]);
  }
}