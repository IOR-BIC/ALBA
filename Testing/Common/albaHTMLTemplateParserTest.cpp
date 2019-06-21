/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaHTMLTemplateParserTest
 Authors: Gianluigi Crimi
 
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

#include "albaHTMLTemplateParserTest.h"
#include "albaHTMLTemplateParserBlock.h"
#include "albaHTMLTemplateParser.h"

//----------------------------------------------------------------------------
void albaHTMLTemplateParserTest::TestConstructorDestuctor()
{
	albaHTMLTemplateParser *testBlock;

	testBlock = new albaHTMLTemplateParser();

	cppDEL(testBlock);
}

//----------------------------------------------------------------------------
void albaHTMLTemplateParserTest::TestSetWriteTemplateFile()
{
	albaHTMLTemplateParser *testBlock;
	wxString root = ALBA_DATA_ROOT;
	wxString OutputFileString, OutputString;
	wxString RightString = "ALBA TEMPLATE OUTPUT TEST";
	int cmpValue;

	testBlock = new albaHTMLTemplateParser();

	testBlock->SetTemplateFromFile(root + "\\Test_HTMLTemplateParser\\TestTemplate.html");
	testBlock->Parse();

	albaString outputDir = GET_TEST_DATA_DIR();
	testBlock->WriteOutputFile(outputDir + "\\TestResult.html");
	OutputFileString = testBlock->GetOutputString();

	//Use albaHTMLTemplateParser for reading from the output of the test :-)
	albaHTMLTemplateParser *readerBlock;

	readerBlock = new albaHTMLTemplateParser();

	//this reader is reading from the output file
	readerBlock->SetTemplateFromFile(outputDir + "\\TestResult.html");
	readerBlock->Parse();
	OutputString = readerBlock->GetOutputString();

	//testing template reading
	cmpValue = RightString.Cmp(OutputFileString);
	CPPUNIT_ASSERT(cmpValue == 0);

	//testing template writing
	cmpValue = OutputString.Cmp(OutputFileString);
	CPPUNIT_ASSERT(cmpValue == 0);

	cppDEL(readerBlock);
	cppDEL(testBlock);
}

//----------------------------------------------------------------------------
void albaHTMLTemplateParserTest::TestSetGetTemplateString()
{
	wxString RightString = "ALBA TEMPLATE STRING TEST";
	albaHTMLTemplateParser *testBlock;
	wxString OutputString;
	int cmpValue;

	testBlock = new albaHTMLTemplateParser();

	testBlock->SetTemplateFromString(RightString);
	testBlock->Parse();
	OutputString = testBlock->GetOutputString();

	//testing template reading
	cmpValue = RightString.Cmp(OutputString);
	CPPUNIT_ASSERT(cmpValue == 0);

	cppDEL(testBlock);
}

//----------------------------------------------------------------------------
void albaHTMLTemplateParserTest::TestParse()
{
	albaHTMLTemplateParser *testBlock;
	wxString OutputString;
	int cmpValue;

	//Variable Substitution test
	wxString Template = "Output: [ALBAVariable Pippo]";
	wxString VariableString = "My name is Pippo";
	wxString rightOutput = "Output: My name is Pippo";
	testBlock = new albaHTMLTemplateParser();
	testBlock->SetTemplateFromString(Template);
	testBlock->AddVar("Pippo", "My name is Pippo");
	testBlock->Parse();
	OutputString = testBlock->GetOutputString();
	cmpValue = rightOutput.Cmp(OutputString);
	CPPUNIT_ASSERT(cmpValue == 0);
	cppDEL(testBlock);

	//If Substitution test (true)
	Template = "Output: [ALBAIf condition] true [ALBAElse condition] false [/ALBAIf condition]";
	rightOutput = "Output:  true ";
	testBlock = new albaHTMLTemplateParser();
	testBlock->SetTemplateFromString(Template);
	testBlock->AddBlock("condition", alba_HTML_TEMPLATE_IF);
	testBlock->GetBlock("condition")->SetIfCondition(true);
	testBlock->Parse();
	OutputString = testBlock->GetOutputString();
	cmpValue = rightOutput.Cmp(OutputString);
	CPPUNIT_ASSERT(cmpValue == 0);
	cppDEL(testBlock);

	//If Substitution test (false)
	rightOutput = "Output:  false ";
	testBlock = new albaHTMLTemplateParser();
	testBlock->SetTemplateFromString(Template);
	testBlock->AddBlock("condition", alba_HTML_TEMPLATE_IF);
	testBlock->GetBlock("condition")->SetIfCondition(false);
	testBlock->Parse();
	OutputString = testBlock->GetOutputString();
	cmpValue = rightOutput.Cmp(OutputString);
	CPPUNIT_ASSERT(cmpValue == 0);
	cppDEL(testBlock);

	//Loop Substitution test 
	Template = "Output: [ALBALoop counter] [ALBAVariable count] [/ALBALoop counter]";
	rightOutput = "Output:  0  1  2  3  4 ";
	testBlock = new albaHTMLTemplateParser();
	testBlock->SetTemplateFromString(Template);
	testBlock->AddBlock("counter", alba_HTML_TEMPLATE_LOOP);
	testBlock->GetBlock("counter")->SetNLoops(5);
	for (int i = 0; i < testBlock->GetBlock("counter")->GetNLoops(); i++)
		testBlock->GetBlock("counter")->PushVar("count", i);

	testBlock->Parse();
	OutputString = testBlock->GetOutputString();
	cmpValue = rightOutput.Cmp(OutputString);
	CPPUNIT_ASSERT(cmpValue == 0);
	cppDEL(testBlock);

	//Complex Substitution test
	Template = "Output: [ALBALoop counter] [ALBAVariable count] is [ALBAIf oddEven]Odd[ALBAElse oddEven]Even[/ALBAIf oddEven] - [/ALBALoop counter]";
	rightOutput = "Output:  1 is Odd -  2 is Even -  3 is Odd -  4 is Even -  5 is Odd - ";
	testBlock = new albaHTMLTemplateParser();
	testBlock->SetTemplateFromString(Template);
	testBlock->AddBlock("counter", alba_HTML_TEMPLATE_LOOP);
	testBlock->GetBlock("counter")->SetNLoops(5);

	for (int i = 0; i < testBlock->GetBlock("counter")->GetNLoops(); i++)
	{
		testBlock->GetBlock("counter")->PushBlock("oddEven", alba_HTML_TEMPLATE_IF);
		testBlock->GetBlock("counter")->PushVar("count", i + 1);
		testBlock->GetBlock("counter")->GetNthBlock("oddEven")->SetIfCondition((i + 1) % 2);
	}

	testBlock->Parse();
	OutputString = testBlock->GetOutputString();
	cmpValue = rightOutput.Cmp(OutputString);
	CPPUNIT_ASSERT(cmpValue == 0);
	cppDEL(testBlock);
}

