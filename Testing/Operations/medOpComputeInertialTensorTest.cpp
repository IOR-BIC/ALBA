/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medOpComputeInertialTensorTest.cpp,v $
Language:  C++
Date:      $Date: 2011-02-16 22:45:49 $
Version:   $Revision: 1.1.2.2 $
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

#include "medOpComputeInertialTensorTest.h"
#include "medOpComputeInertialTensor.h"

#include "mafVMESurface.h"
#include "mafVMEGroup.h"
#include "vtkMAFSmartPointer.h"
#include "mafSmartPointer.h"
#include "medOpImporterVTK.h"
#include "mafTagItem.h"
#include "mafTagArray.h"

#include <string>
#include <assert.h>

//-----------------------------------------------------------
void medOpComputeInertialTensorTest::TestDynamicAllocation() 
//-----------------------------------------------------------
{
  medOpComputeInertialTensor *create=new medOpComputeInertialTensor();
  mafDEL(create);
}
//-----------------------------------------------------------
void medOpComputeInertialTensorTest::TestCopy() 
//-----------------------------------------------------------
{
  medOpComputeInertialTensor *create=new medOpComputeInertialTensor();
  medOpComputeInertialTensor *create2 = medOpComputeInertialTensor::SafeDownCast(create->Copy());

  CPPUNIT_ASSERT(create2 != NULL);

  mafDEL(create2);
  mafDEL(create);
}
//-----------------------------------------------------------
void medOpComputeInertialTensorTest::TestAccept() 
//-----------------------------------------------------------
{
  mafSmartPointer<mafVMESurface> volume;
  mafSmartPointer<mafVMEGroup> group;

  medOpComputeInertialTensor *create=new medOpComputeInertialTensor();
  CPPUNIT_ASSERT(create->Accept(volume));
  CPPUNIT_ASSERT(create->Accept(group));
  CPPUNIT_ASSERT(!create->Accept(NULL));

  mafDEL(create);
}
//-----------------------------------------------------------
void medOpComputeInertialTensorTest::TestAddAttributes() 
//-----------------------------------------------------------
{
  // import VTK  
  medOpImporterVTK *importer=new medOpImporterVTK("importerVTK");
  importer->TestModeOn();
  mafString fileName=MED_DATA_ROOT;
  fileName<<"/Surface/sphere.vtk";
  importer->SetFileName(fileName);
  importer->ImportVTK();
  mafVMESurface *surface=mafVMESurface::SafeDownCast(importer->GetOutput());
  
  CPPUNIT_ASSERT(surface!=NULL);
  CPPUNIT_ASSERT(surface->GetOutput()->GetVTKData()!=NULL);

  medOpComputeInertialTensor *op=new medOpComputeInertialTensor();
  op->TestModeOn();
  op->SetInput(surface);
  op->ComputeInertialTensor(surface);
  op->AddAttributes();

  // test attributes
  CPPUNIT_ASSERT(surface->GetTagArray()->IsTagPresent("INERTIAL_TENSOR_COMPONENTS_[Ixx,Iyy,Izz]"));
  CPPUNIT_ASSERT(surface->GetTagArray()->IsTagPresent("SURFACE_MASS"));

  mafDEL(op);
  mafDEL(importer);

  delete wxLog::SetActiveTarget(NULL);
}
//-----------------------------------------------------------
void medOpComputeInertialTensorTest::TestComputeInertialTensor() 
//-----------------------------------------------------------
{
  // import VTK  
  medOpImporterVTK *importer=new medOpImporterVTK("importerVTK");
  importer->TestModeOn();
  mafString fileName=MED_DATA_ROOT;
  fileName<<"/Surface/sphere.vtk";
  importer->SetFileName(fileName);
  importer->ImportVTK();
  mafVMESurface *surface=mafVMESurface::SafeDownCast(importer->GetOutput());
  
  CPPUNIT_ASSERT(surface!=NULL);
  CPPUNIT_ASSERT(surface->GetOutput()->GetVTKData()!=NULL);

  // test on surface
  medOpComputeInertialTensor *op=new medOpComputeInertialTensor();
  op->TestModeOn();
  op->SetInput(surface);
  op->ComputeInertialTensor(surface);
  op->AddAttributes();

  mafTagItem tag;
  surface->GetTagArray()->GetTag("INERTIAL_TENSOR_COMPONENTS_[Ixx,Iyy,Izz]",tag);
  CPPUNIT_ASSERT(tag.GetNumberOfComponents()==3);
  surface->GetTagArray()->GetTag("SURFACE_MASS",tag);
  CPPUNIT_ASSERT(tag.GetNumberOfComponents()==1);

  mafDEL(op);
  mafDEL(importer);

  delete wxLog::SetActiveTarget(NULL);
}
//-----------------------------------------------------------
void medOpComputeInertialTensorTest::TestComputeInertialTensorFromGroup() 
//-----------------------------------------------------------
{
  // import VTK  
  medOpImporterVTK *importer=new medOpImporterVTK("importerVTK");
  importer->TestModeOn();
  mafString fileName=MED_DATA_ROOT;
  fileName<<"/Surface/sphere.vtk";
  importer->SetFileName(fileName);
  importer->ImportVTK();
  mafVMESurface *surface=mafVMESurface::SafeDownCast(importer->GetOutput());
  
  CPPUNIT_ASSERT(surface!=NULL);
  CPPUNIT_ASSERT(surface->GetOutput()->GetVTKData()!=NULL);

  // create surface copy
  mafVMESurface* copy;
  mafNEW(copy);
  copy->DeepCopy(surface);

  // create group
  mafVMEGroup* group;
  mafNEW(group);
  group->AddChild(surface);
  group->AddChild(copy);

  // test on group
  medOpComputeInertialTensor *op=new medOpComputeInertialTensor();
  op->TestModeOn();
  op->SetInput(group);
  op->ComputeInertialTensorFromGroup();
  op->AddAttributes();

  mafTagItem tag;
  group->GetTagArray()->GetTag("INERTIAL_TENSOR_COMPONENTS_[Ixx,Iyy,Izz]",tag);
  CPPUNIT_ASSERT(tag.GetNumberOfComponents()==3);
  group->GetTagArray()->GetTag("SURFACE_MASS",tag);
  CPPUNIT_ASSERT(tag.GetNumberOfComponents()==1);

  mafDEL(op);
  mafDEL(group);
  mafDEL(copy);
  mafDEL(importer);

  delete wxLog::SetActiveTarget(NULL);
}
//-----------------------------------------------------------
void medOpComputeInertialTensorTest::TestOpDoUndo() 
//-----------------------------------------------------------
{ 
  // import VTK  
  medOpImporterVTK *importer=new medOpImporterVTK("importerVTK");
  importer->TestModeOn();
  mafString fileName=MED_DATA_ROOT;
  fileName<<"/Surface/sphere.vtk";
  importer->SetFileName(fileName);
  importer->ImportVTK();
  mafVMESurface *surface=mafVMESurface::SafeDownCast(importer->GetOutput());
  
  CPPUNIT_ASSERT(surface!=NULL);
  CPPUNIT_ASSERT(surface->GetOutput()->GetVTKData()!=NULL);

  medOpComputeInertialTensor *op=new medOpComputeInertialTensor();
  op->TestModeOn();
  op->SetInput(surface);
  op->ComputeInertialTensor(surface);
  op->AddAttributes();

  op->OpUndo();

  // test undo 
  CPPUNIT_ASSERT(!surface->GetTagArray()->IsTagPresent("INERTIAL_TENSOR_COMPONENTS_[Ixx,Iyy,Izz]"));
  CPPUNIT_ASSERT(!surface->GetTagArray()->IsTagPresent("SURFACE_MASS"));

  op->OpDo();

  // test do
  CPPUNIT_ASSERT(surface->GetTagArray()->IsTagPresent("INERTIAL_TENSOR_COMPONENTS_[Ixx,Iyy,Izz]"));
  CPPUNIT_ASSERT(surface->GetTagArray()->IsTagPresent("SURFACE_MASS"));

  mafDEL(op);
  mafDEL(importer);

  delete wxLog::SetActiveTarget(NULL);
}