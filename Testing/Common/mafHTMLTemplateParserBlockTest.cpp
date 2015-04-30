/*=========================================================================

 Program: MAF2
 Module: mafHTMLTemplateParserBlockTest
 Authors: Gianluigi Crimi
 
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

#include "mafHTMLTemplateParserBlockTest.h"
#include "mafHTMLTemplateParserBlock.h"
//#include "mafHTMLTemplateParser.h"



//----------------------------------------------------------------------------
void mafHTMLTemplateParserBlockTest::setUp()
//----------------------------------------------------------------------------
{

}

//----------------------------------------------------------------------------
void mafHTMLTemplateParserBlockTest::tearDown()
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
void mafHTMLTemplateParserBlockTest::TestConstructorDestuctor()
//----------------------------------------------------------------------------
{
  mafHTMLTemplateParserBlock *testBlock;

  testBlock = new mafHTMLTemplateParserBlock(maf_HTML_TEMPLATE_MAIN,wxString("Main Block"));
  cppDEL(testBlock);  
}

//----------------------------------------------------------------------------
void mafHTMLTemplateParserBlockTest::TestAddGetVar()
//----------------------------------------------------------------------------
{
  mafHTMLTemplateParserBlock *testBlock;
  wxString result;
  int cmpValue;

  testBlock = new mafHTMLTemplateParserBlock(maf_HTML_TEMPLATE_MAIN,wxString("Main Block"));

  //testing Int
  wxString intString="2";
  testBlock->AddVar("Integer",2);
  result=testBlock->GetVar("Integer");
  cmpValue=result.Cmp(intString);
  CPPUNIT_ASSERT(cmpValue==0);

  //testing Double
  wxString doubleString="2.000";
  testBlock->AddVar("Double",2.0);
  result=testBlock->GetVar("Double");
  cmpValue=result.Cmp(doubleString);
  CPPUNIT_ASSERT(cmpValue==0);

  //testing String
  wxString stringString="Test String";
  testBlock->AddVar("testString","Test String");
  result=testBlock->GetVar("testString");
  cmpValue=result.Cmp(stringString);
  CPPUNIT_ASSERT(cmpValue==0);

  //testing Error
  wxString errorString="Error";
  result=testBlock->GetVar("nonexistentVar");
  cmpValue=result.Cmp(errorString);
  CPPUNIT_ASSERT(cmpValue==0);

  cppDEL(testBlock);  
}

//----------------------------------------------------------------------------
void mafHTMLTemplateParserBlockTest::TestPushGetNthVar()
//----------------------------------------------------------------------------
{
  mafHTMLTemplateParserBlock *testBlock;
  wxString result;
  int cmpValue;

  testBlock = new mafHTMLTemplateParserBlock(maf_HTML_TEMPLATE_LOOP,wxString("Loop Block"));

  testBlock->SetNLoops(2);
  
  //testing Int
  wxString intString1="1";
  wxString intString2="2";
  testBlock->PushVar("Integer",1);
  testBlock->PushVar("Integer",2);
  //asking for the last pushed value
  result=testBlock->GetNthVar("Integer");
  cmpValue=result.Cmp(intString2);
  CPPUNIT_ASSERT(cmpValue==0);
  //asking for the first pushed value
  result=testBlock->GetNthVar("Integer",0);
  cmpValue=result.Cmp(intString1);
  CPPUNIT_ASSERT(cmpValue==0);


  //testing Double
  wxString doubleString1="1.000";
  wxString doubleString2="2.000";
  testBlock->PushVar("Double",1.0);
  testBlock->PushVar("Double",2.0);
  //asking for the last pushed value
  result=testBlock->GetNthVar("Double");
  cmpValue=result.Cmp(doubleString2);
  CPPUNIT_ASSERT(cmpValue==0);
  //asking for the first pushed value
  result=testBlock->GetNthVar("Double",0);
  cmpValue=result.Cmp(doubleString1);
  CPPUNIT_ASSERT(cmpValue==0);

  
  //testing String
  wxString stringString1="Test String one";
  wxString stringString2="Test String two";
  testBlock->PushVar("testString","Test String one");
  testBlock->PushVar("testString","Test String two");
  //asking for the last pushed value
  result=testBlock->GetNthVar("testString");
  cmpValue=result.Cmp(stringString2);
  CPPUNIT_ASSERT(cmpValue==0);
  //asking for the first pushed value
  result=testBlock->GetNthVar("testString",0);
  cmpValue=result.Cmp(stringString1);
  CPPUNIT_ASSERT(cmpValue==0);


  //testing Error
  wxString errorString="Error";
  result=testBlock->GetNthVar("nonexistentVar");
  cmpValue=result.Cmp(errorString);
  CPPUNIT_ASSERT(cmpValue==0);

  //testing bounds
  //we can get only ("testString",0) and ("testString",1)
  //because we pushed it two times
  result=testBlock->GetNthVar("testString",2);
  cmpValue=result.Cmp(errorString);
  CPPUNIT_ASSERT(cmpValue==0);

  cppDEL(testBlock);  
}

//----------------------------------------------------------------------------
void mafHTMLTemplateParserBlockTest::TestAddGetBlock()
//----------------------------------------------------------------------------
{
  mafHTMLTemplateParserBlock *testBlock;
  mafHTMLTemplateParserBlock *returnedBlock;
  
  testBlock = new mafHTMLTemplateParserBlock(maf_HTML_TEMPLATE_MAIN,wxString("Main Block"));

  testBlock->AddBlock("Pippo",maf_HTML_TEMPLATE_LOOP);

  returnedBlock=testBlock->GetBlock("Pippo");
  CPPUNIT_ASSERT(returnedBlock!=NULL);

  returnedBlock=testBlock->GetBlock("Pluto");
  CPPUNIT_ASSERT(returnedBlock==NULL);

  cppDEL(testBlock);  
}

//----------------------------------------------------------------------------
void mafHTMLTemplateParserBlockTest::TestPushGetNthBlock()
//----------------------------------------------------------------------------
{

  mafHTMLTemplateParserBlock *testBlock;
  mafHTMLTemplateParserBlock *returnedBlock;

  testBlock = new mafHTMLTemplateParserBlock(maf_HTML_TEMPLATE_LOOP,wxString("Loop Block"));

  testBlock->SetNLoops(2);

  testBlock->PushBlock("Pippo",maf_HTML_TEMPLATE_IF);
  testBlock->PushBlock("Pippo",maf_HTML_TEMPLATE_IF);

  //getting last block
  returnedBlock=testBlock->GetNthBlock("Pippo");
  CPPUNIT_ASSERT(returnedBlock!=NULL);

  //getting first block
  returnedBlock=testBlock->GetNthBlock("Pippo",0);
  CPPUNIT_ASSERT(returnedBlock!=NULL);

  //getting out bound block
  returnedBlock=testBlock->GetNthBlock("Pippo",2);
  CPPUNIT_ASSERT(returnedBlock==NULL);

  returnedBlock=testBlock->GetNthBlock("Pluto");
  CPPUNIT_ASSERT(returnedBlock==NULL);

  cppDEL(testBlock);  
}

//----------------------------------------------------------------------------
void mafHTMLTemplateParserBlockTest::TestSetGetIfCondition()
//----------------------------------------------------------------------------
{
  mafHTMLTemplateParserBlock *testBlock;
  int condition;

  testBlock = new mafHTMLTemplateParserBlock(maf_HTML_TEMPLATE_IF,wxString("If Block"));

  testBlock->SetIfCondition(true);
  condition=testBlock->GetIfCondition();
  CPPUNIT_ASSERT(condition==true);

  testBlock->SetIfCondition(false);
  condition=testBlock->GetIfCondition();
  CPPUNIT_ASSERT(condition==false);

  cppDEL(testBlock);  
}

//----------------------------------------------------------------------------
void mafHTMLTemplateParserBlockTest::TestSetGetNloop()
//----------------------------------------------------------------------------
{

  mafHTMLTemplateParserBlock *testBlock;
  int nloops;

  testBlock = new mafHTMLTemplateParserBlock(maf_HTML_TEMPLATE_LOOP,wxString("Loop Block"));

  testBlock->SetNLoops(3);
  nloops=testBlock->GetNLoops();
  CPPUNIT_ASSERT(nloops==3);

  cppDEL(testBlock);  
}



