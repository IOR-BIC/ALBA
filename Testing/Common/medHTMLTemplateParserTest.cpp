/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medHTMLTemplateParserTest.cpp,v $
Language:  C++
Date:      $Date: 2012-02-29 09:36:43 $
Version:   $Revision: 1.1.2.3 $
Authors:   Gianluigi Crimi
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/


#include "medDefines.h"
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "medHTMLTemplateParserTest.h"
#include "medHTMLTemplateParserBlock.h"
#include "medHTMLTemplateParser.h"



//----------------------------------------------------------------------------
void medHTMLTemplateParserTest::setUp()
//----------------------------------------------------------------------------
{
  
}

//----------------------------------------------------------------------------
void medHTMLTemplateParserTest::tearDown()
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
void medHTMLTemplateParserTest::TestConstructorDestuctor()
//----------------------------------------------------------------------------
{
  medHTMLTemplateParser *testBlock;

  testBlock = new medHTMLTemplateParser();

  cppDEL(testBlock);
}

//----------------------------------------------------------------------------
void medHTMLTemplateParserTest::TestSetWriteTemplateFile()
//----------------------------------------------------------------------------
{
  medHTMLTemplateParser *testBlock;
  wxString root=MED_DATA_ROOT;
  wxString OutputFileString, OutputString;
  wxString RightString="MAF TEMPLATE OUTPUT TEST";
  int cmpValue;

  testBlock = new medHTMLTemplateParser();

  testBlock->SetTemplateFromFile(root+"\\Test_HTMLTemplateParser\\TestTemplate.html");
  testBlock->Parse();
  testBlock->WriteOutputFile(root+"\\Test_HTMLTemplateParser\\TestResult.html");
  OutputFileString=testBlock->GetOutputString();

  //Use medHTMLTemplateParser for reading from the output of the test :-)
  medHTMLTemplateParser *readerBlock;

  readerBlock = new medHTMLTemplateParser();

  //this reader is reading from the output file
  readerBlock->SetTemplateFromFile(root+"\\Test_HTMLTemplateParser\\TestResult.html");
  readerBlock->Parse();
  OutputString=readerBlock->GetOutputString();
  

  //testing template reading
  cmpValue=RightString.Cmp(OutputFileString);
  CPPUNIT_ASSERT(cmpValue==0);
  
  //testing template writing
  cmpValue=OutputString.Cmp(OutputFileString);
  CPPUNIT_ASSERT(cmpValue==0);

  cppDEL(readerBlock);
  cppDEL(testBlock);
}

//----------------------------------------------------------------------------
void medHTMLTemplateParserTest::TestSetGetTemplateString()
//----------------------------------------------------------------------------
{
  wxString RightString="MAF TEMPLATE STRING TEST";
  medHTMLTemplateParser *testBlock;
  wxString OutputString;
  int cmpValue;


  testBlock = new medHTMLTemplateParser();

  testBlock->SetTemplateFromString(RightString);
  testBlock->Parse();
  OutputString=testBlock->GetOutputString();

  //testing template reading
  cmpValue=RightString.Cmp(OutputString);
  CPPUNIT_ASSERT(cmpValue==0);
  
  cppDEL(testBlock);
}

//----------------------------------------------------------------------------
void medHTMLTemplateParserTest::TestParse()
//----------------------------------------------------------------------------
{
  medHTMLTemplateParser *testBlock;
  wxString OutputString;
  int cmpValue;


  //Variable Substitution test
  wxString Template="Output: [MAFVariable Pippo]";
  wxString VariableString="My name is Pippo";
  wxString rightOutput="Output: My name is Pippo";
  testBlock = new medHTMLTemplateParser();
  testBlock->SetTemplateFromString(Template);
  testBlock->AddVar("Pippo", "My name is Pippo");
  testBlock->Parse();
  OutputString=testBlock->GetOutputString();
  cmpValue=rightOutput.Cmp(OutputString);
  CPPUNIT_ASSERT(cmpValue==0);
  cppDEL(testBlock);


  //If Substitution test (true)
  Template="Output: [MAFIf condition] true [MAFElse condition] false [/MAFIf condition]";
  rightOutput="Output:  true ";
  testBlock = new medHTMLTemplateParser();
  testBlock->SetTemplateFromString(Template);
  testBlock->AddBlock("condition",MED_HTML_TEMPLATE_IF);
  testBlock->GetBlock("condition")->SetIfCondition(true);
  testBlock->Parse();
  OutputString=testBlock->GetOutputString();
  cmpValue=rightOutput.Cmp(OutputString);
  CPPUNIT_ASSERT(cmpValue==0);
  cppDEL(testBlock);

  //If Substitution test (false)
  rightOutput="Output:  false ";
  testBlock = new medHTMLTemplateParser();
  testBlock->SetTemplateFromString(Template);
  testBlock->AddBlock("condition",MED_HTML_TEMPLATE_IF);
  testBlock->GetBlock("condition")->SetIfCondition(false);
  testBlock->Parse();
  OutputString=testBlock->GetOutputString();
  cmpValue=rightOutput.Cmp(OutputString);
  CPPUNIT_ASSERT(cmpValue==0);
  cppDEL(testBlock);

  //Loop Substitution test 
  Template="Output: [MAFLoop counter] [MAFVariable count] [/MAFLoop counter]";
  rightOutput="Output:  0  1  2  3  4 ";
  testBlock = new medHTMLTemplateParser();
  testBlock->SetTemplateFromString(Template);
  testBlock->AddBlock("counter",MED_HTML_TEMPLATE_LOOP);
  testBlock->GetBlock("counter")->SetNLoops(5);
  for (int i=0;i<testBlock->GetBlock("counter")->GetNLoops();i++)
    testBlock->GetBlock("counter")->PushVar("count",i);

  testBlock->Parse();
  OutputString=testBlock->GetOutputString();
  cmpValue=rightOutput.Cmp(OutputString);
  CPPUNIT_ASSERT(cmpValue==0);
  cppDEL(testBlock);

  //Complex Substitution test
  Template="Output: [MAFLoop counter] [MAFVariable count] is [MAFIf oddEven]Odd[MAFElse oddEven]Even[/MAFIf oddEven] - [/MAFLoop counter]";
  rightOutput="Output:  1 is Odd -  2 is Even -  3 is Odd -  4 is Even -  5 is Odd - " ;
  testBlock = new medHTMLTemplateParser();
  testBlock->SetTemplateFromString(Template);
  testBlock->AddBlock("counter",MED_HTML_TEMPLATE_LOOP);
  testBlock->GetBlock("counter")->SetNLoops(5);
  for (int i=0;i<testBlock->GetBlock("counter")->GetNLoops();i++)
  {
    testBlock->GetBlock("counter")->PushBlock("oddEven",MED_HTML_TEMPLATE_IF);
    testBlock->GetBlock("counter")->PushVar("count",i+1);
    testBlock->GetBlock("counter")->GetNthBlock("oddEven")->SetIfCondition((i+1)%2);
  }
  testBlock->Parse();
  OutputString=testBlock->GetOutputString();
  cmpValue=rightOutput.Cmp(OutputString);
  CPPUNIT_ASSERT(cmpValue==0);
  cppDEL(testBlock);

}

