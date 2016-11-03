/*=========================================================================

 Program: MAF2
 Module: mafHTMLTemplateParserTest
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

#include "mafHTMLTemplateParserTest.h"
#include "mafHTMLTemplateParserBlock.h"
#include "mafHTMLTemplateParser.h"

//----------------------------------------------------------------------------
void mafHTMLTemplateParserTest::TestConstructorDestuctor()
{
	mafHTMLTemplateParser *testBlock;

	testBlock = new mafHTMLTemplateParser();

	cppDEL(testBlock);
}

//----------------------------------------------------------------------------
void mafHTMLTemplateParserTest::TestSetWriteTemplateFile()
{
	mafHTMLTemplateParser *testBlock;
	wxString root = MAF_DATA_ROOT;
	wxString OutputFileString, OutputString;
	wxString RightString = "MAF TEMPLATE OUTPUT TEST";
	int cmpValue;

	testBlock = new mafHTMLTemplateParser();

	testBlock->SetTemplateFromFile(root + "\\Test_HTMLTemplateParser\\TestTemplate.html");
	testBlock->Parse();

	mafString outputDir = GET_TEST_DATA_DIR();
	testBlock->WriteOutputFile(outputDir + "\\TestResult.html");
	OutputFileString = testBlock->GetOutputString();

	//Use mafHTMLTemplateParser for reading from the output of the test :-)
	mafHTMLTemplateParser *readerBlock;

	readerBlock = new mafHTMLTemplateParser();

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
void mafHTMLTemplateParserTest::TestSetGetTemplateString()
{
	wxString RightString = "MAF TEMPLATE STRING TEST";
	mafHTMLTemplateParser *testBlock;
	wxString OutputString;
	int cmpValue;

	testBlock = new mafHTMLTemplateParser();

	testBlock->SetTemplateFromString(RightString);
	testBlock->Parse();
	OutputString = testBlock->GetOutputString();

	//testing template reading
	cmpValue = RightString.Cmp(OutputString);
	CPPUNIT_ASSERT(cmpValue == 0);

	cppDEL(testBlock);
}

//----------------------------------------------------------------------------
void mafHTMLTemplateParserTest::TestParse()
{
	mafHTMLTemplateParser *testBlock;
	wxString OutputString;
	int cmpValue;

	//Variable Substitution test
	wxString Template = "Output: [MAFVariable Pippo]";
	wxString VariableString = "My name is Pippo";
	wxString rightOutput = "Output: My name is Pippo";
	testBlock = new mafHTMLTemplateParser();
	testBlock->SetTemplateFromString(Template);
	testBlock->AddVar("Pippo", "My name is Pippo");
	testBlock->Parse();
	OutputString = testBlock->GetOutputString();
	cmpValue = rightOutput.Cmp(OutputString);
	CPPUNIT_ASSERT(cmpValue == 0);
	cppDEL(testBlock);

	//If Substitution test (true)
	Template = "Output: [MAFIf condition] true [MAFElse condition] false [/MAFIf condition]";
	rightOutput = "Output:  true ";
	testBlock = new mafHTMLTemplateParser();
	testBlock->SetTemplateFromString(Template);
	testBlock->AddBlock("condition", maf_HTML_TEMPLATE_IF);
	testBlock->GetBlock("condition")->SetIfCondition(true);
	testBlock->Parse();
	OutputString = testBlock->GetOutputString();
	cmpValue = rightOutput.Cmp(OutputString);
	CPPUNIT_ASSERT(cmpValue == 0);
	cppDEL(testBlock);

	//If Substitution test (false)
	rightOutput = "Output:  false ";
	testBlock = new mafHTMLTemplateParser();
	testBlock->SetTemplateFromString(Template);
	testBlock->AddBlock("condition", maf_HTML_TEMPLATE_IF);
	testBlock->GetBlock("condition")->SetIfCondition(false);
	testBlock->Parse();
	OutputString = testBlock->GetOutputString();
	cmpValue = rightOutput.Cmp(OutputString);
	CPPUNIT_ASSERT(cmpValue == 0);
	cppDEL(testBlock);

	//Loop Substitution test 
	Template = "Output: [MAFLoop counter] [MAFVariable count] [/MAFLoop counter]";
	rightOutput = "Output:  0  1  2  3  4 ";
	testBlock = new mafHTMLTemplateParser();
	testBlock->SetTemplateFromString(Template);
	testBlock->AddBlock("counter", maf_HTML_TEMPLATE_LOOP);
	testBlock->GetBlock("counter")->SetNLoops(5);
	for (int i = 0; i < testBlock->GetBlock("counter")->GetNLoops(); i++)
		testBlock->GetBlock("counter")->PushVar("count", i);

	testBlock->Parse();
	OutputString = testBlock->GetOutputString();
	cmpValue = rightOutput.Cmp(OutputString);
	CPPUNIT_ASSERT(cmpValue == 0);
	cppDEL(testBlock);

	//Complex Substitution test
	Template = "Output: [MAFLoop counter] [MAFVariable count] is [MAFIf oddEven]Odd[MAFElse oddEven]Even[/MAFIf oddEven] - [/MAFLoop counter]";
	rightOutput = "Output:  1 is Odd -  2 is Even -  3 is Odd -  4 is Even -  5 is Odd - ";
	testBlock = new mafHTMLTemplateParser();
	testBlock->SetTemplateFromString(Template);
	testBlock->AddBlock("counter", maf_HTML_TEMPLATE_LOOP);
	testBlock->GetBlock("counter")->SetNLoops(5);

	for (int i = 0; i < testBlock->GetBlock("counter")->GetNLoops(); i++)
	{
		testBlock->GetBlock("counter")->PushBlock("oddEven", maf_HTML_TEMPLATE_IF);
		testBlock->GetBlock("counter")->PushVar("count", i + 1);
		testBlock->GetBlock("counter")->GetNthBlock("oddEven")->SetIfCondition((i + 1) % 2);
	}

	testBlock->Parse();
	OutputString = testBlock->GetOutputString();
	cmpValue = rightOutput.Cmp(OutputString);
	CPPUNIT_ASSERT(cmpValue == 0);
	cppDEL(testBlock);
}

