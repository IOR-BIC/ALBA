/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafOpImporterASCIITest.cpp,v $
Language:  C++
Date:      $Date: 2011-05-25 11:58:32 $
Version:   $Revision: 1.1.2.2 $
Authors:   Alberto Losi
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
#include <iostream>

#include "mafOpImporterASCIITest.h"
#include "mafOpImporterASCII.h"
#include "mafVMEScalarMatrix.h"
#include "mafString.h"

#define TEST_RESULT CPPUNIT_ASSERT(result)

class mafOpImporterASCIIDummy : public mafOpImporterASCII
{
public:
  mafOpImporterASCIIDummy(const wxString &label = "ASCIIImporter"){Superclass;};
  ~mafOpImporterASCIIDummy(){Superclass;}; 
  mafTypeMacro(mafOpImporterASCIIDummy,mafOpImporterASCII);
  mafNode *GetOutput(){return m_ScalarData;}; // Redefined to directly get m_ScalarData without use OpRun that creates a GUI
};

mafCxxTypeMacro(mafOpImporterASCIIDummy);

//----------------------------------------------------------------------------
void mafOpImporterASCIITest::setUp()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafOpImporterASCIITest::tearDown()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafOpImporterASCIITest::TestConstructor()
//----------------------------------------------------------------------------
{
	mafOpImporterASCII *opImporterASCII = new mafOpImporterASCII("Importer ASCII");
	opImporterASCII->TestModeOn();
	
  cppDEL(opImporterASCII);
}
//----------------------------------------------------------------------------
void mafOpImporterASCIITest::TestCopy()
//----------------------------------------------------------------------------
{
  mafOpImporterASCII *opImporterASCII1 = new mafOpImporterASCII("Importer ASCII");
  mafOpImporterASCII *opImporterASCII2;
  opImporterASCII2 = mafOpImporterASCII::SafeDownCast(opImporterASCII1->Copy());
  result = (opImporterASCII2 != NULL); // Can only test if is not NULL
  TEST_RESULT;  
  cppDEL(opImporterASCII1);
  cppDEL(opImporterASCII2);
}
//----------------------------------------------------------------------------
void mafOpImporterASCIITest::TestAccept()
//----------------------------------------------------------------------------
{
  mafOpImporterASCII *opImporterASCII = new mafOpImporterASCII("Importer ASCII");
  opImporterASCII->TestModeOn();
  // Always returns true
  result = opImporterASCII->Accept(NULL);
  TEST_RESULT;
  cppDEL(opImporterASCII);
}
//----------------------------------------------------------------------------
void mafOpImporterASCIITest::TestImportSingleASCIIFile()
//----------------------------------------------------------------------------
{
  mafOpImporterASCIIDummy *opImporterASCII = new mafOpImporterASCIIDummy("Importer ASCII");
  mafString filename = MAF_DATA_ROOT;
  opImporterASCII->TestModeOn();

  mafVMEScalarMatrix *matrix;
  mafNEW(matrix);
  vnl_matrix<double> m;

  /* Create a Matrix equals to the one stored in the ASCII file;
  1.0 2.0 3.0
  4.0 5.0 6.0
  7.0 8.0 9.0
  */

  m.set_size(3,3);
  m.put(0,0,1.0);
  m.put(0,1,2.0);
  m.put(0,2,3.0);
  m.put(1,0,4.0);
  m.put(1,1,5.0);
  m.put(1,2,6.0);
  m.put(2,0,7.0);
  m.put(2,1,8.0);
  m.put(2,2,9.0);

  matrix->SetData(m,0);

  filename<<"/Test_ImporterASCII/matrix_01.txt";
  opImporterASCII->SetFileName(std::string(filename.GetCStr()));
  
  // opImporterASCII->OpRun();
  // Cannot use OpRun method because it creates a GUI
  result = opImporterASCII->ImportASCII() == MAF_OK;
  TEST_RESULT;

  mafVMEScalarMatrix *output = mafVMEScalarMatrix::SafeDownCast(opImporterASCII->GetOutput());
  
  // Set the same attributes otherwise the test will fail
  matrix->SetName(output->GetName());
  matrix->SetActiveScalarOnGeometry(output->GetActiveScalarOnGeometry());
  matrix->SetScalarArrayOrientation(output->GetScalarArrayOrientation());

  result = output->Equals(matrix);
  TEST_RESULT;

  mafDEL(matrix);
  cppDEL(opImporterASCII);
}
//----------------------------------------------------------------------------
void mafOpImporterASCIITest::TestImportMultipleASCIIFiles()
//----------------------------------------------------------------------------
{
  mafOpImporterASCIIDummy *opImporterASCII = new mafOpImporterASCIIDummy("Importer ASCII");
  mafString filename = MAF_DATA_ROOT;
  opImporterASCII->TestModeOn();

  mafVMEScalarMatrix *matrix;
  mafNEW(matrix);
  vnl_matrix<double> m;

  /* Create a Matrix equals to the one stored in the ASCII file;
  1.0 2.0 3.0
  4.0 5.0 6.0
  7.0 8.0 9.0
  */

  m.set_size(3,3);
  m.put(0,0,1.0);
  m.put(0,1,2.0);
  m.put(0,2,3.0);
  m.put(1,0,4.0);
  m.put(1,1,5.0);
  m.put(1,2,6.0);
  m.put(2,0,7.0);
  m.put(2,1,8.0);
  m.put(2,2,9.0);

  matrix->SetData(m,0);

  /* Create a Matrix equals to the one stored in the ASCII file;
  1.0 2.0 3.0
  4.0 5.0 6.0
  7.0 8.0 9.0
  */

  m.set_size(3,3);
  m.put(0,0,10.0);
  m.put(0,1,11.0);
  m.put(0,2,12.0);
  m.put(1,0,13.0);
  m.put(1,1,14.0);
  m.put(1,2,15.0);
  m.put(2,0,16.0);
  m.put(2,1,17.0);
  m.put(2,2,18.0);

  matrix->SetData(m,1);

  filename<<"/Test_ImporterASCII/matrix_01.txt";
  opImporterASCII->SetFileName(std::string(filename.GetCStr()));

  filename = MAF_DATA_ROOT;
  filename<<"/Test_ImporterASCII/matrix_02.txt";
  opImporterASCII->AddFileName(std::string(filename.GetCStr()));

  // opImporterASCII->OpRun();
  // Cannot use OpRun method because it creates a GUI
  result = opImporterASCII->ImportASCII() == MAF_OK;
  TEST_RESULT;

  mafVMEScalarMatrix *output = mafVMEScalarMatrix::SafeDownCast(opImporterASCII->GetOutput());

  // Set the same attributes otherwise the test will fail
  matrix->SetName(output->GetName());
  matrix->SetActiveScalarOnGeometry(output->GetActiveScalarOnGeometry());
  matrix->SetScalarArrayOrientation(output->GetScalarArrayOrientation());

  result = output->Equals(matrix);
  TEST_RESULT;

  mafDEL(matrix);
  cppDEL(opImporterASCII);
}
