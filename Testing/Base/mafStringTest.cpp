/*=========================================================================

 Program: MAF2
 Module: mafStringTest
 Authors: Marco Petrone, Stefano Perticoni
 
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
#include "mafStringTest.h"
#include "mafString.h"
#include <iostream>

void mafStringTest::Test()
{
  // Test multiple string features with a single test 
  // instead of writing several test cases... this is more handy

  // create a text string and a numerical string
  mafString hello = "Hello World";
  mafString number(150.2567890123456); // 16 digits precision
  
  // streaming towards standard output
  std::cout<<hello.GetCStr()<<std::endl;
  std::cout<<number.GetCStr()<<std::endl;

  // test the contents and the length of the numerical string
  CPPUNIT_ASSERT(number=="150.2567890123456");
  CPPUNIT_ASSERT(hello.Length()==11);
  
  // try the concatenation of mafString's
  mafString italy = " and Italy";
  
  // this concatenates two strings to the 'tmp' string
  mafString tmp;
  tmp << hello << italy;
  CPPUNIT_ASSERT(tmp=="Hello World and Italy");

  // test query functions
  CPPUNIT_ASSERT(tmp.EndsWith("Italy"));
  CPPUNIT_ASSERT(tmp.StartsWith("Hello"));
  CPPUNIT_ASSERT(tmp.FindFirst("and")==12);

  // test concatenation of (const char *)
  mafString tmp2 = "Ciao";
  tmp2 << " Bel" << " Mondo!";

  CPPUNIT_ASSERT(tmp2 == "Ciao Bel Mondo!");

  mafString tmp3 = "Numbers = ";
  tmp3 << (long)10 << " " << (float).12345671 << " " << (double).12345678901234561;
  CPPUNIT_ASSERT(tmp3 == "Numbers = 10 0.1234567 0.1234567890123456");
  
  // test copy  feature
  mafString copy=tmp;

  CPPUNIT_ASSERT(tmp==copy);
  CPPUNIT_ASSERT(tmp.GetCStr()!=copy.GetCStr());
  
  //test ExtractPathName()
  mafString f = "C:\\dir1\\dir2\\file.ext";
  mafString path = "C:\\dir1\\dir2";
  f.ExtractPathName();
  CPPUNIT_ASSERT(f == path);

  // test erase feature
  tmp.Erase(6,15);
  CPPUNIT_ASSERT(tmp=="Hello Italy");

  tmp.Erase(5);
  CPPUNIT_ASSERT(tmp=="Hello");

  // test the FindLast function to find last occurrence of the "Hello" string.
  tmp<<" everybody";
  tmp<<italy;
  tmp<<". Hello!";
  
  CPPUNIT_ASSERT(tmp.FindLast("Hello")==(tmp.Length()-mafString::Length("Hello!")));
  
  // test single character accessor 
  CPPUNIT_ASSERT(tmp[tmp.FindLast("Hello")]=='H');

  // test += operator
  mafString red_box="The Red";
  red_box+=" Box";

  CPPUNIT_ASSERT(red_box=="The Red Box");

  // test alphabetical comparison
  mafString num5=5;
  mafString num1=1;
  mafString num10=10;

  CPPUNIT_ASSERT(num1<num5);
  CPPUNIT_ASSERT(num5>num1);
  CPPUNIT_ASSERT(num5>num10); // alphabetical order, not numerical!
  
  // some extra comparison. Notice the "a" string is not copied!
  mafCString achar="a";
  mafCString bchar="ab";
  CPPUNIT_ASSERT(achar<="a");
  CPPUNIT_ASSERT(achar>="a");
  CPPUNIT_ASSERT(achar<bchar);
  CPPUNIT_ASSERT(bchar>achar);

  // test the referencing mechansim of mafString. Notice mafCString is
  // equivalent to mafString::Set() 
  const char *char_str="const string";
  mafCString test_str(char_str);

  CPPUNIT_ASSERT(char_str==test_str.GetCStr()); // No memory copy!

  test_str << " not overwritten!"; 
  
  // trying to modify a const string makes the string to be automatically
  // copied and the final string memory is different.
  CPPUNIT_ASSERT(char_str!=test_str.GetCStr());
  CPPUNIT_ASSERT(test_str=="const string not overwritten!");

  // test with another kind of modification
  mafCString const_str(char_str);
  const_str.Erase(6,7);
  CPPUNIT_ASSERT(const_str.GetCStr()!=char_str);
  CPPUNIT_ASSERT(const_str=="const ring"); // test contents  
}
  
