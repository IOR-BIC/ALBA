/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafOpEditMetadataTest.cpp,v $
Language:  C++
Date:      $Date: 2010-01-13 16:12:54 $
Version:   $Revision: 1.1.2.1 $
Authors:   Roberto Mucci
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

#include <cppunit/config/SourcePrefix.h>
#include "mafOpEditMetadataTest.h"

#include "mafOpEditMetadata.h"

#include "mafVMESurface.h"
#include "mafVMEGenericAbstract.h"
#include "mafVMERoot.h"
#include "mafVMELandmark.h"
#include "mafVMELandmarkCloud.h"
#include "mafMatrix.h"
#include "mafMatrixVector.h"
#include "mafTagArray.h"

void mafOpEditMetadataTest::TestFixture()
{
}

void mafOpEditMetadataTest::setUp()
{
}

void mafOpEditMetadataTest::tearDown()
{
}

//-----------------------------------------------------------
void mafOpEditMetadataTest::TestConstructor()
//-----------------------------------------------------------
{
  mafOpEditMetadata *editor = new mafOpEditMetadata("editor");
  mafDEL(editor);
}

//-----------------------------------------------------------
void mafOpEditMetadataTest::TestAddNewTag()
//-----------------------------------------------------------
{
  mafSmartPointer<mafVMESurface> surface;
  mafString tagName = "newTagTest";
  

  mafOpEditMetadata *editor = new mafOpEditMetadata("editor");
  editor->TestModeOn();
  editor->SetInput(surface);
  editor->OpRun();
  editor->AddNewTag(tagName);

  mafTagArray *tag = surface->GetTagArray();
  CPPUNIT_ASSERT(tag->GetNumberOfTags() == 1);

  CPPUNIT_ASSERT(tag->IsTagPresent(tagName));

  mafDEL(editor);
}

//-----------------------------------------------------------
void mafOpEditMetadataTest::TestRemoveTag()
//-----------------------------------------------------------
{
  mafSmartPointer<mafVMESurface> surface;
  mafString tagName = "newTagTest";


  mafOpEditMetadata *editor = new mafOpEditMetadata("editor");
  editor->TestModeOn();
  editor->SetInput(surface);
  editor->OpRun();
  editor->AddNewTag(tagName);

  mafTagArray *tag = surface->GetTagArray();
  CPPUNIT_ASSERT(tag->GetNumberOfTags() == 1);
  CPPUNIT_ASSERT(tag->IsTagPresent(tagName));

  editor->SelectTag(tagName);
  editor->RemoveTag();

  CPPUNIT_ASSERT(tag->GetNumberOfTags() == 0);
  CPPUNIT_ASSERT(!tag->IsTagPresent(tagName));

  mafDEL(editor);
}

//-----------------------------------------------------------
void mafOpEditMetadataTest::TestSetTagName()
//-----------------------------------------------------------
{
  mafSmartPointer<mafVMESurface> surface;
  mafString tagName = "newTagTest";

  mafOpEditMetadata *editor = new mafOpEditMetadata("editor");
  editor->TestModeOn();
  editor->SetInput(surface);
  editor->OpRun();
  editor->AddNewTag(tagName);

  mafTagArray *tag = surface->GetTagArray();
  CPPUNIT_ASSERT(tag->GetNumberOfTags() == 1);
  CPPUNIT_ASSERT(tag->IsTagPresent(tagName));

  mafString newTagName = "newTagNameTest";
  editor->SelectTag(tagName);
  editor->SetTagName(newTagName.GetCStr());

  CPPUNIT_ASSERT(tag->GetNumberOfTags() == 1);
  CPPUNIT_ASSERT(tag->IsTagPresent(newTagName));
  CPPUNIT_ASSERT(!tag->IsTagPresent(tagName));

   mafDEL(editor);
}
