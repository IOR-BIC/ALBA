/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaMSFImporterTest
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
#include "albaMSFImporterTest.h"
#include "albaMSFImporter.h"
#include "albaVMERoot.h"
#include "albaVME.h"

#define TEST_RESULT CPPUNIT_ASSERT(m_Result)

//----------------------------------------------------------------------------
void albaMSFImporterTest::TestFixture()
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
void albaMSFImporterTest::TestStaticAllocation()
//----------------------------------------------------------------------------
{
  albaMSFImporter importer;
}
//----------------------------------------------------------------------------
void albaMSFImporterTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  albaMSFImporter *importer = new albaMSFImporter();
  cppDEL(importer);
}
//----------------------------------------------------------------------------
void albaMSFImporterTest::TestSetGetRoot()
//----------------------------------------------------------------------------
{
  albaMSFImporter *importer = new albaMSFImporter();

  albaVMERoot *root;
  albaNEW(root);

  importer->SetRoot(root);
  
  m_Result = (root == importer->GetRoot());

  TEST_RESULT;

  albaDEL(root);
  cppDEL(importer);
}
//----------------------------------------------------------------------------
void albaMSFImporterTest::TestRestore() // test the utility class InternalRestore method (mmuMSF1xDocument::InternalRestore)
//----------------------------------------------------------------------------
{
  albaMSFImporter *importer = new albaMSFImporter();

  albaString filename = ALBA_DATA_ROOT;
  filename<<"/Test_MSFImporterTest/Placed1.msf"; // MSF 1.x

  albaVMERoot *root;
  albaNEW(root);

  importer->SetRoot(root);

  importer->SetURL(filename);

  m_Result = (importer->Restore() == ALBA_OK);
  TEST_RESULT;

  m_Result = (importer->GetRoot()->GetNumberOfChildren() == 2);
  TEST_RESULT;

  albaVME *node1 = importer->GetRoot()->GetChild(0);

  m_Result = (node1->IsA("albaVMEImage"));
  TEST_RESULT;

  m_Result = (albaString(node1->GetName()).Compare("FemurFront") == 0);
  TEST_RESULT;

  albaVME *node2 = importer->GetRoot()->GetChild(1);

  m_Result = (node2->IsA("albaVMEImage"));
  TEST_RESULT;

  m_Result = (albaString(node2->GetName()).Compare("FemurLeft") == 0);
  TEST_RESULT;

  albaDEL(node1);

  albaDEL(node2);

  albaDEL(root);

  cppDEL(importer);
}