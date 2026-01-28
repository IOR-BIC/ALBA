/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpClassicICPRegistrationTest
 Authors: Simone Brazzale
 
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

#include "albaOpClassicICPRegistrationTest.h"
#include "albaOpClassicICPRegistration.h"

#include "albaVME.h"
#include "albaVMESurface.h"
#include "albaVMESurfaceParametric.h"
#include "albaVMELandmarkCloud.h"
#include "albaSmartPointer.h"
#include "albaAbsMatrixPipe.h"
#include "vtkALBASmartPointer.h"
#include "albaOpImporterVTK.h"

#include <string>
#include <assert.h>
#include "albaVMEStorage.h"
#include "albaVMERoot.h"

//-----------------------------------------------------------
void albaOpClassicICPRegistrationTest::TestDynamicAllocation() 
//-----------------------------------------------------------
{
  albaOpClassicICPRegistration *create = new albaOpClassicICPRegistration("icp");
  albaDEL(create);
}
//-----------------------------------------------------------
void albaOpClassicICPRegistrationTest::TestCopy() 
//-----------------------------------------------------------
{
  albaOpClassicICPRegistration *create = new albaOpClassicICPRegistration("icp");
  albaOpClassicICPRegistration *create2 = (albaOpClassicICPRegistration*)(create->Copy());

  CPPUNIT_ASSERT(create2 != NULL);

  albaDEL(create2);
  albaDEL(create);
}
//-----------------------------------------------------------
void albaOpClassicICPRegistrationTest::TestAccept() 
//-----------------------------------------------------------
{
  albaSmartPointer<albaVMESurface> surface;
  albaSmartPointer<albaVMESurfaceParametric> psurface;  
  albaSmartPointer<albaVMELandmarkCloud> lc;
  
  albaOpClassicICPRegistration *create=new albaOpClassicICPRegistration("icp");
  
  CPPUNIT_ASSERT(create->Accept(surface));
  CPPUNIT_ASSERT(create->Accept(psurface));

  // not rigid lc
  CPPUNIT_ASSERT(!create->Accept(lc));

  CPPUNIT_ASSERT(!create->Accept(NULL));

  albaDEL(create);
}
//-----------------------------------------------------------
void albaOpClassicICPRegistrationTest::TestOpDo() 
//-----------------------------------------------------------
{
	//Storage Creation
	albaVMEStorage *storage = albaVMEStorage::New();
	albaVMERoot *root = storage->GetRoot();
	root->SetName("root");
	root->GetRoot()->Initialize();

  // import VTK  
  albaOpImporterVTK *importer=new albaOpImporterVTK("importerVTK");
  importer->TestModeOn();
  albaString fileName=ALBA_DATA_ROOT;
  fileName<<"/Surface/sphere.vtk";
  importer->SetFileName(fileName);
	importer->SetInput(root);
  importer->ImportFile();
  
  albaVMESurface *surface = albaVMESurface::SafeDownCast(importer->GetOutput());
  
  CPPUNIT_ASSERT(surface!=NULL);
  CPPUNIT_ASSERT(surface->GetOutput()->GetVTKData()!=NULL);

  albaOpImporterVTK *importer2=new albaOpImporterVTK("importerVTK");
  importer2->TestModeOn();
  albaString fileName2=ALBA_DATA_ROOT;
  fileName2<<"/VTK_Surfaces/surface.vtk";
  importer2->SetFileName(fileName2);
	importer2->SetInput(root);
  importer2->ImportFile();
  
  albaVME *vme = importer2->GetOutput();
  
  CPPUNIT_ASSERT(vme!=NULL);
  CPPUNIT_ASSERT(vme->GetOutput()->GetVTKData()!=NULL);

  albaOpClassicICPRegistration *op=new albaOpClassicICPRegistration("icp");
  op->TestModeOn();
  op->SetInput(surface);
  op->SetTarget(vme);
  op->OpDo();

  // test attributes
	CPPUNIT_ASSERT(surface->GetParent()->GetNumberOfChildren() == 3);
  albaVME* registered = surface->GetByPath("next");
  CPPUNIT_ASSERT(registered->GetAbsMatrixPipe()->GetMatrix().GetVTKMatrix()!=NULL);

  albaDEL(op);
  albaDEL(importer);
  albaDEL(importer2);
	albaDEL(storage);
}