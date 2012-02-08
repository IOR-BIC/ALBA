/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medHTMLTemplateParserTest.cpp,v $
Language:  C++
Date:      $Date: 2012-02-08 16:57:11 $
Version:   $Revision: 1.1.2.1 $
Authors:   Gianluigi Crimi
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

#include "medHTMLTemplateParserTest.h"
#include "medHTMLTemplateParserBlock.h"
#include "medHTMLTemplateParser.h"



//----------------------------------------------------------------------------
void medHTMLTemplateParserTest::setUp()
//----------------------------------------------------------------------------
{
  CreateTestData();
}

//----------------------------------------------------------------------------
void medHTMLTemplateParserTest::tearDown()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void medHTMLTemplateParserTest::CreateTestData()
//----------------------------------------------------------------------------
{
  
  CPPUNIT_ASSERT("" != NULL);


  
}
//----------------------------------------------------------------------------
void medHTMLTemplateParserTest::TestConstructor()
//----------------------------------------------------------------------------
{
  //medHTMLTemplateParser *gizmoSlice = new medHTMLTemplateParser();
  //cppDEL(gizmoSlice);
}



