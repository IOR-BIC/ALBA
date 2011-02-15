/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medOpClassicICPRegistrationTest.cpp,v $
Language:  C++
Date:      $Date: 2011-02-15 10:49:30 $
Version:   $Revision: 1.1.2.1 $
Authors:   Simone Brazzale
==========================================================================
Copyright (c) 2007
CINECA - Interuniversity Consortium (www.cineca.it)
SCS s.r.l. - BioComputing Competence Centre (www.scsolutions.it - www.b3c.it)

MafMedical Library use license agreement

The software named MafMedical Library and any accompanying documentation, 
manuals or data (hereafter collectively "SOFTWARE") is property of the SCS s.r.l.
This is an open-source copyright as follows:
Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:
* Redistributions of source code must retain the above copyright notice, 
this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice, 
this list of conditions and the following disclaimer in the documentation and/or 
other materials provided with the distribution.
* Modified source versions must be plainly marked as such, and must not be misrepresented 
as being the original software.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 'AS IS' 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
ARE DISCLAIMED. IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE FOR 
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND 
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

MafMedical is partially based on OpenMAF.
=========================================================================*/

#include "medDefines.h"
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "medOpClassicICPRegistrationTest.h"
#include "medOpClassicICPRegistration.h"

#include "mafVME.h"
#include "mafVMESurface.h"
#include "mafVMESurfaceParametric.h"
#include "mafVMELandmarkCloud.h"
#include "mafSmartPointer.h"
#include "mafAbsMatrixPipe.h"
#include "vtkMAFSmartPointer.h"
#include "medOpImporterVTK.h"

#include <string>
#include <assert.h>

//-----------------------------------------------------------
void medOpClassicICPRegistrationTest::TestDynamicAllocation() 
//-----------------------------------------------------------
{
  medOpClassicICPRegistration *create = new medOpClassicICPRegistration("icp");
  mafDEL(create);
}
//-----------------------------------------------------------
void medOpClassicICPRegistrationTest::TestCopy() 
//-----------------------------------------------------------
{
  medOpClassicICPRegistration *create = new medOpClassicICPRegistration("icp");
  medOpClassicICPRegistration *create2 = (medOpClassicICPRegistration*)(create->Copy());

  CPPUNIT_ASSERT(create2 != NULL);

  mafDEL(create2);
  mafDEL(create);
}
//-----------------------------------------------------------
void medOpClassicICPRegistrationTest::TestAccept() 
//-----------------------------------------------------------
{
  mafSmartPointer<mafVMESurface> surface;
  mafSmartPointer<mafVMESurfaceParametric> psurface;  
  mafSmartPointer<mafVMELandmarkCloud> lc;
  
  medOpClassicICPRegistration *create=new medOpClassicICPRegistration("icp");
  
  CPPUNIT_ASSERT(create->Accept(surface));
  CPPUNIT_ASSERT(create->Accept(psurface));

  // not rigid lc
  CPPUNIT_ASSERT(!create->Accept(lc));

  CPPUNIT_ASSERT(!create->Accept(NULL));

  mafDEL(create);
}
//-----------------------------------------------------------
void medOpClassicICPRegistrationTest::TestOpDo() 
//-----------------------------------------------------------
{
  // import VTK  
  medOpImporterVTK *importer=new medOpImporterVTK("importerVTK");
  importer->TestModeOn();
  mafString fileName=MED_DATA_ROOT;
  fileName<<"/Surface/sphere.vtk";
  importer->SetFileName(fileName);
  importer->ImportVTK();
  
  mafVMESurface *surface = mafVMESurface::SafeDownCast(importer->GetOutput());
  
  CPPUNIT_ASSERT(surface!=NULL);
  CPPUNIT_ASSERT(surface->GetOutput()->GetVTKData()!=NULL);

  medOpImporterVTK *importer2=new medOpImporterVTK("importerVTK");
  importer2->TestModeOn();
  mafString fileName2=MED_DATA_ROOT;
  fileName2<<"/VTK_Volumes/volume.vtk";
  importer2->SetFileName(fileName2);
  importer2->ImportVTK();
  
  mafVME *vme = mafVME::SafeDownCast(importer2->GetOutput());
  
  CPPUNIT_ASSERT(vme!=NULL);
  CPPUNIT_ASSERT(vme->GetOutput()->GetVTKData()!=NULL);

  medOpClassicICPRegistration *op=new medOpClassicICPRegistration("icp");
  op->TestModeOn();
  op->SetInput(surface);
  op->SetTarget(vme);
  op->OpDo();

  // test attributes
  CPPUNIT_ASSERT(surface->GetNumberOfChildren()==1);
  mafVME* registered = mafVME::SafeDownCast(surface->GetChild(0));
  CPPUNIT_ASSERT(registered->GetAbsMatrixPipe()->GetMatrix().GetVTKMatrix()!=NULL);

  mafDEL(op);
  mafDEL(importer);
  mafDEL(importer2);

  delete wxLog::SetActiveTarget(NULL);
}