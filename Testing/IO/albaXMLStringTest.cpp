/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaXMLStringTest
 Authors: Alberto Losi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "albaDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include <cppunit/config/SourcePrefix.h>

#include <xercesc/sax/ErrorHandler.hpp>
#include <xercesc/sax/SAXParseException.hpp>

#include "albaXMLString.h"
#include "albaXMLStringTest.h"

//----------------------------------------------------------------------------
void albaXMLStringTest::BeforeTest()
//----------------------------------------------------------------------------
{
  XERCES_CPP_NAMESPACE_QUALIFIER XMLPlatformUtils::Initialize();
}
//----------------------------------------------------------------------------
void albaXMLStringTest::AfterTest()
//----------------------------------------------------------------------------
{
  XERCES_CPP_NAMESPACE_QUALIFIER XMLPlatformUtils::Terminate();
}
//----------------------------------------------------------------------------
void albaXMLStringTest::TestStaticAllocation()
//----------------------------------------------------------------------------
{
	albaXMLString s;
}
//----------------------------------------------------------------------------
void albaXMLStringTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
	albaXMLString *s = new albaXMLString();
	cppDEL(s);
}
//----------------------------------------------------------------------------
void albaXMLStringTest::TestAllConstructors()
//----------------------------------------------------------------------------
{
#ifndef ALBA_EXPORT

  albaXMLString s1 = albaXMLString();

  albaXMLString s2 = albaXMLString("test string");

  albaXMLString s3 = albaXMLString(s2);

  XMLCh *xml_s = 0L;

  albaXMLString s4 = albaXMLString(xml_s);

#endif  

}
//----------------------------------------------------------------------------
void albaXMLStringTest::TestAppend()
//----------------------------------------------------------------------------
{
#ifndef ALBA_EXPORT
  albaXMLString s1 = albaXMLString("test string for");
  albaXMLString s2 = albaXMLString(" Append");

  s1.Append(s2);

  const char *cstr_s1 = s1.GetCStr();

  CPPUNIT_ASSERT(strcmp(cstr_s1, "test string for Append") == 0);
#endif
}
//----------------------------------------------------------------------------
void albaXMLStringTest::TestErase()
//----------------------------------------------------------------------------
{
#ifndef ALBA_EXPORT
  albaXMLString s = albaXMLString("test string for Erase");

  s.Erase(s.Begin(),s.Begin() + 5);

  const char *cstr_s = s.GetCStr();

  CPPUNIT_ASSERT(strcmp(cstr_s, "string for Erase") == 0);
#endif
}
//----------------------------------------------------------------------------
void albaXMLStringTest::TestBegin()
//----------------------------------------------------------------------------
{
#ifndef ALBA_EXPORT

  albaXMLString s = albaXMLString("test string for Begin");

  const XMLCh *beg_c = s.Begin();
  XMLCh c = s[0];

  CPPUNIT_ASSERT(c == *beg_c);
#endif
}
//----------------------------------------------------------------------------
void albaXMLStringTest::TestEnd()
//----------------------------------------------------------------------------
{
#ifndef ALBA_EXPORT

  albaXMLString s = albaXMLString("test string for End");

  const XMLCh *end_c = s.End();
  XMLCh c = s[s.Size()];

  CPPUNIT_ASSERT(c == *end_c);
#endif
}
//----------------------------------------------------------------------------
void albaXMLStringTest::TestSize()
//----------------------------------------------------------------------------
{
#ifndef ALBA_EXPORT

  albaXMLString s = albaXMLString("test string for Size");

  CPPUNIT_ASSERT(s.Size() == 20);
#endif
}
//----------------------------------------------------------------------------
void albaXMLStringTest::TestGetCStr()
//----------------------------------------------------------------------------
{
#ifndef ALBA_EXPORT

  albaXMLString s = albaXMLString("test string for GetCStr");

  const char *cstr_s = s.GetCStr();

  CPPUNIT_ASSERT(strcmp(cstr_s, "test string for GetCStr") == 0);

#endif
}
//----------------------------------------------------------------------------
void albaXMLStringTest::TestAllOperators()
//----------------------------------------------------------------------------
{
#ifndef ALBA_EXPORT

  albaXMLString s = albaXMLString("test string for Operators");

  // test [] operator
  XMLCh c = s[2];
  CPPUNIT_ASSERT(c == *(s.Begin() + 2));

  // test * operator
  const XMLCh *xml_c =  (const XMLCh*)s;
  for(int i = 0; i < s.Size(); i++)
  {
    CPPUNIT_ASSERT(xml_c[i] == s[i]);
  }

#endif
}