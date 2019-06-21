/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpEditMetadataTest
 Authors: Roberto Mucci
 
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
#include "albaOpEditMetadataTest.h"

#include "albaOpEditMetadata.h"

#include "albaVMESurface.h"
#include "albaVMEGenericAbstract.h"
#include "albaVMERoot.h"
#include "albaVMELandmark.h"
#include "albaVMELandmarkCloud.h"
#include "albaMatrix.h"
#include "albaMatrixVector.h"
#include "albaTagArray.h"

void albaOpEditMetadataTest::TestFixture()
{
}

//-----------------------------------------------------------
void albaOpEditMetadataTest::TestConstructor()
//-----------------------------------------------------------
{
  albaOpEditMetadata *editor = new albaOpEditMetadata("editor");
  albaDEL(editor);
}

//-----------------------------------------------------------
void albaOpEditMetadataTest::TestAddNewTag()
//-----------------------------------------------------------
{
  albaSmartPointer<albaVMESurface> surface;
  albaString tagName = "newTagTest";
  

  albaOpEditMetadata *editor = new albaOpEditMetadata("editor");
  editor->TestModeOn();
  editor->SetInput(surface);
  editor->OpRun();
  editor->AddNewTag(tagName);

  albaTagArray *tag = surface->GetTagArray();
  CPPUNIT_ASSERT(tag->GetNumberOfTags() == 1);

  CPPUNIT_ASSERT(tag->IsTagPresent(tagName));

  albaDEL(editor);
}

//-----------------------------------------------------------
void albaOpEditMetadataTest::TestRemoveTag()
//-----------------------------------------------------------
{
  albaSmartPointer<albaVMESurface> surface;
  albaString tagName = "newTagTest";


  albaOpEditMetadata *editor = new albaOpEditMetadata("editor");
  editor->TestModeOn();
  editor->SetInput(surface);
  editor->OpRun();
  editor->AddNewTag(tagName);

  albaTagArray *tag = surface->GetTagArray();
  CPPUNIT_ASSERT(tag->GetNumberOfTags() == 1);
  CPPUNIT_ASSERT(tag->IsTagPresent(tagName));

  editor->SelectTag(tagName);
  editor->RemoveTag();

  CPPUNIT_ASSERT(tag->GetNumberOfTags() == 0);
  CPPUNIT_ASSERT(!tag->IsTagPresent(tagName));

  albaDEL(editor);
}

//-----------------------------------------------------------
void albaOpEditMetadataTest::TestSetTagName()
//-----------------------------------------------------------
{
  albaSmartPointer<albaVMESurface> surface;
  albaString tagName = "newTagTest";

  albaOpEditMetadata *editor = new albaOpEditMetadata("editor");
  editor->TestModeOn();
  editor->SetInput(surface);
  editor->OpRun();
  editor->AddNewTag(tagName);

  albaTagArray *tag = surface->GetTagArray();
  CPPUNIT_ASSERT(tag->GetNumberOfTags() == 1);
  CPPUNIT_ASSERT(tag->IsTagPresent(tagName));

  albaString newTagName = "newTagNameTest";
  editor->SelectTag(tagName);
  editor->SetTagName(newTagName.GetCStr());

  CPPUNIT_ASSERT(tag->GetNumberOfTags() == 1);
  CPPUNIT_ASSERT(tag->IsTagPresent(newTagName));
  CPPUNIT_ASSERT(!tag->IsTagPresent(tagName));

   albaDEL(editor);
}
