/*=========================================================================

 Program: MAF2
 Module: mafMSFImporterTest
 Authors: Alberto Losi
 
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
#include "mafMSFImporterTest.h"
#include "mafMSFImporter.h"
#include "mafVMERoot.h"
#include "mafNode.h"

#define TEST_RESULT CPPUNIT_ASSERT(m_Result)

//----------------------------------------------------------------------------
void mafMSFImporterTest::TestFixture()
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
void mafMSFImporterTest::TestStaticAllocation()
//----------------------------------------------------------------------------
{
  mafMSFImporter importer;
}
//----------------------------------------------------------------------------
void mafMSFImporterTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  mafMSFImporter *importer = new mafMSFImporter();
  cppDEL(importer);
}
//----------------------------------------------------------------------------
void mafMSFImporterTest::TestSetGetRoot()
//----------------------------------------------------------------------------
{
  mafMSFImporter *importer = new mafMSFImporter();

  mafVMERoot *root;
  mafNEW(root);

  importer->SetRoot(root);
  
  m_Result = (root == importer->GetRoot());

  TEST_RESULT;

  mafDEL(root);
  cppDEL(importer);
}
//----------------------------------------------------------------------------
void mafMSFImporterTest::TestRestore() // test the utility class InternalRestore method (mmuMSF1xDocument::InternalRestore)
//----------------------------------------------------------------------------
{
  mafMSFImporter *importer = new mafMSFImporter();

  mafString filename = MAF_DATA_ROOT;
  filename<<"/Test_MSFImporterTest/Placed1.msf"; // MSF 1.x

  mafVMERoot *root;
  mafNEW(root);

  importer->SetRoot(root);

  importer->SetURL(filename);

  m_Result = (importer->Restore() == MAF_OK);
  TEST_RESULT;

  m_Result = (importer->GetRoot()->GetNumberOfChildren() == 2);
  TEST_RESULT;

  mafNode *node1 = importer->GetRoot()->GetChild(0);

  m_Result = (node1->IsA("mafVMEImage"));
  TEST_RESULT;

  m_Result = (mafString(node1->GetName()).Compare("FemurFront") == 0);
  TEST_RESULT;

  mafNode *node2 = importer->GetRoot()->GetChild(1);

  m_Result = (node2->IsA("mafVMEImage"));
  TEST_RESULT;

  m_Result = (mafString(node2->GetName()).Compare("FemurLeft") == 0);
  TEST_RESULT;

  mafDEL(node1);

  mafDEL(node2);

  mafDEL(root);

  cppDEL(importer);
}