/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpImporterASCIITest
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
#include <iostream>

#include "albaOpImporterASCIITest.h"
#include "albaOpImporterASCII.h"
#include "albaVMEScalarMatrix.h"
#include "albaString.h"

#define TEST_RESULT CPPUNIT_ASSERT(result)

class albaOpImporterASCIIDummy : public albaOpImporterASCII
{
public:
  albaOpImporterASCIIDummy(const wxString &label = "ASCIIImporter"){Superclass;};
  ~albaOpImporterASCIIDummy(){Superclass;}; 
  albaTypeMacro(albaOpImporterASCIIDummy,albaOpImporterASCII);
  albaVME *GetOutput(){return m_ScalarData;}; // Redefined to directly get m_ScalarData without use OpRun that creates a GUI
};

albaCxxTypeMacro(albaOpImporterASCIIDummy);

//----------------------------------------------------------------------------
void albaOpImporterASCIITest::TestConstructor()
//----------------------------------------------------------------------------
{
	albaOpImporterASCII *opImporterASCII = new albaOpImporterASCII("Importer ASCII");
	opImporterASCII->TestModeOn();
	
  cppDEL(opImporterASCII);
}
//----------------------------------------------------------------------------
void albaOpImporterASCIITest::TestCopy()
//----------------------------------------------------------------------------
{
  albaOpImporterASCII *opImporterASCII1 = new albaOpImporterASCII("Importer ASCII");
  albaOpImporterASCII *opImporterASCII2;
  opImporterASCII2 = albaOpImporterASCII::SafeDownCast(opImporterASCII1->Copy());
  result = (opImporterASCII2 != NULL); // Can only test if is not NULL
  TEST_RESULT;  
  cppDEL(opImporterASCII1);
  cppDEL(opImporterASCII2);
}
//----------------------------------------------------------------------------
void albaOpImporterASCIITest::TestAccept()
//----------------------------------------------------------------------------
{
  albaOpImporterASCII *opImporterASCII = new albaOpImporterASCII("Importer ASCII");
  opImporterASCII->TestModeOn();
  // Always returns true
  result = opImporterASCII->Accept(NULL);
  TEST_RESULT;
  cppDEL(opImporterASCII);
}
//----------------------------------------------------------------------------
void albaOpImporterASCIITest::TestImportSingleASCIIFile()
//----------------------------------------------------------------------------
{
  albaOpImporterASCIIDummy *opImporterASCII = new albaOpImporterASCIIDummy("Importer ASCII");
  albaString filename = ALBA_DATA_ROOT;
  opImporterASCII->TestModeOn();

  albaVMEScalarMatrix *matrix;
  albaNEW(matrix);
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
  result = opImporterASCII->ImportASCII() == ALBA_OK;
  TEST_RESULT;

  albaVMEScalarMatrix *output = albaVMEScalarMatrix::SafeDownCast(opImporterASCII->GetOutput());
  
  // Set the same attributes otherwise the test will fail
  matrix->SetName(output->GetName());
  matrix->SetActiveScalarOnGeometry(output->GetActiveScalarOnGeometry());
  matrix->SetScalarArrayOrientation(output->GetScalarArrayOrientation());

  result = output->Equals(matrix);
  TEST_RESULT;

  albaDEL(matrix);
  cppDEL(opImporterASCII);
}
//----------------------------------------------------------------------------
void albaOpImporterASCIITest::TestImportMultipleASCIIFiles()
//----------------------------------------------------------------------------
{
  albaOpImporterASCIIDummy *opImporterASCII = new albaOpImporterASCIIDummy("Importer ASCII");
  albaString filename = ALBA_DATA_ROOT;
  opImporterASCII->TestModeOn();

  albaVMEScalarMatrix *matrix;
  albaNEW(matrix);
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

  filename = ALBA_DATA_ROOT;
  filename<<"/Test_ImporterASCII/matrix_02.txt";
  opImporterASCII->AddFileName(std::string(filename.GetCStr()));

  // opImporterASCII->OpRun();
  // Cannot use OpRun method because it creates a GUI
  result = opImporterASCII->ImportASCII() == ALBA_OK;
  TEST_RESULT;

  albaVMEScalarMatrix *output = albaVMEScalarMatrix::SafeDownCast(opImporterASCII->GetOutput());

  // Set the same attributes otherwise the test will fail
  matrix->SetName(output->GetName());
  matrix->SetActiveScalarOnGeometry(output->GetActiveScalarOnGeometry());
  matrix->SetScalarArrayOrientation(output->GetScalarArrayOrientation());

  result = output->Equals(matrix);
  TEST_RESULT;

  albaDEL(matrix);
  cppDEL(opImporterASCII);
}
