/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpComputeInertialTensorTest
 Authors: Simone Brazzale , Stefano Perticoni
 
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

#include "albaOpComputeInertialTensorTest.h"
#include "albaOpComputeInertialTensor.h"

#include "albaVMESurface.h"
#include "albaVMEGroup.h"
#include "vtkALBASmartPointer.h"
#include "albaSmartPointer.h"
#include "albaOpImporterVTK.h"
#include "albaTagItem.h"
#include "albaTagArray.h"

#include <string>
#include <assert.h>

//-----------------------------------------------------------
void albaOpComputeInertialTensorTest::TestDynamicAllocation() 
//-----------------------------------------------------------
{
  albaOpComputeInertialTensor *create=new albaOpComputeInertialTensor();
  albaDEL(create);
}
//-----------------------------------------------------------
void albaOpComputeInertialTensorTest::TestCopy() 
//-----------------------------------------------------------
{
  albaOpComputeInertialTensor *create=new albaOpComputeInertialTensor();
  albaOpComputeInertialTensor *create2 = albaOpComputeInertialTensor::SafeDownCast(create->Copy());

  CPPUNIT_ASSERT(create2 != NULL);

  albaDEL(create2);
  albaDEL(create);
}
//-----------------------------------------------------------
void albaOpComputeInertialTensorTest::TestAccept() 
//-----------------------------------------------------------
{
  albaSmartPointer<albaVMESurface> volume;
  albaSmartPointer<albaVMEGroup> group;

  albaOpComputeInertialTensor *create=new albaOpComputeInertialTensor();
  CPPUNIT_ASSERT(create->Accept(volume));
  CPPUNIT_ASSERT(create->Accept(group));
  CPPUNIT_ASSERT(!create->Accept(NULL));

  albaDEL(create);
}
//-----------------------------------------------------------
void albaOpComputeInertialTensorTest::TestAddAttributes() 
//-----------------------------------------------------------
{
  // import VTK  
  albaOpImporterVTK *importer=new albaOpImporterVTK("importerVTK");
  importer->TestModeOn();
  albaString fileName=ALBA_DATA_ROOT;
  fileName<<"/Surface/sphere.vtk";
  importer->SetFileName(fileName);
  importer->ImportFile();
  albaVMESurface *surface=albaVMESurface::SafeDownCast(importer->GetOutput());
  
  CPPUNIT_ASSERT(surface!=NULL);
  CPPUNIT_ASSERT(surface->GetOutput()->GetVTKData()!=NULL);

  albaOpComputeInertialTensor *op=new albaOpComputeInertialTensor();
  op->TestModeOn();
  op->SetInput(surface);
  op->ComputeInertialTensor(surface);
  op->AddAttributes();

  // test attributes
  CPPUNIT_ASSERT(surface->GetTagArray()->IsTagPresent("PRINCIPAL_INERTIAL_TENSOR_COMPONENTS"));
  CPPUNIT_ASSERT(surface->GetTagArray()->IsTagPresent("SURFACE_MASS"));

  albaDEL(op);
  albaDEL(importer);
}
//-----------------------------------------------------------
void albaOpComputeInertialTensorTest::TestComputeInertialTensorFromDefaultValue() 
//-----------------------------------------------------------
{
  // import VTK  
  albaOpImporterVTK *importer=new albaOpImporterVTK("importerVTK");
  importer->TestModeOn();
  albaString fileName=ALBA_DATA_ROOT;
  fileName<<"/Surface/sphere.vtk";
  importer->SetFileName(fileName);
  importer->ImportFile();
  albaVMESurface *surface=albaVMESurface::SafeDownCast(importer->GetOutput());
  
  CPPUNIT_ASSERT(surface!=NULL);
  CPPUNIT_ASSERT(surface->GetOutput()->GetVTKData()!=NULL);

  // test on surface
  albaOpComputeInertialTensor *op=new albaOpComputeInertialTensor();
  op->TestModeOn();
  op->SetInput(surface);
  op->ComputeInertialTensor(surface);
  op->AddAttributes();

  double tagValue = -1;

  albaTagItem tag;
  surface->GetTagArray()->GetTag("PRINCIPAL_INERTIAL_TENSOR_COMPONENTS",tag);

  tagValue = tag.GetValueAsDouble(0);

  CPPUNIT_ASSERT(tag.GetNumberOfComponents()==3);
  
  tagValue = tag.GetValueAsDouble(0);
  CPPUNIT_ASSERT(46.147349667471083 == tagValue);

  tagValue = tag.GetValueAsDouble(1);
  CPPUNIT_ASSERT(46.147348793016427 == tagValue);

  tagValue = tag.GetValueAsDouble(2);
  CPPUNIT_ASSERT(44.637590473208157 == tagValue);

  surface->GetTagArray()->GetTag("SURFACE_MASS",tag);

  CPPUNIT_ASSERT(tag.GetNumberOfComponents()==1);
  
  tagValue = tag.GetValueAsDouble(0);

  CPPUNIT_ASSERT(30.403272694462050 == tagValue);

  albaDEL(op);
  albaDEL(importer);
}
//-----------------------------------------------------------
void albaOpComputeInertialTensorTest::TestComputeInertialTensorFromGroupFromDefaultValue() 
//-----------------------------------------------------------
{
  // import VTK  
  albaOpImporterVTK *importer=new albaOpImporterVTK("importerVTK");
  importer->TestModeOn();
  albaString fileName=ALBA_DATA_ROOT;
  fileName<<"/Surface/sphere.vtk";
  importer->SetFileName(fileName);
  importer->ImportFile();
  albaVMESurface *surface=albaVMESurface::SafeDownCast(importer->GetOutput());
  
  CPPUNIT_ASSERT(surface!=NULL);
  CPPUNIT_ASSERT(surface->GetOutput()->GetVTKData()!=NULL);

  // create surface copy
  albaVMESurface* copy;
  albaNEW(copy);
  copy->DeepCopy(surface);

  // create group
  albaVMEGroup* group;
  albaNEW(group);
  group->AddChild(surface);
  group->AddChild(copy);

  // test on group
  albaOpComputeInertialTensor *op=new albaOpComputeInertialTensor();
  op->TestModeOn();
  op->SetInput(group);
  op->ComputeInertialTensorFromGroup();
  op->AddAttributes();

  albaTagItem tag;
  group->GetTagArray()->GetTag("PRINCIPAL_INERTIAL_TENSOR_COMPONENTS",tag);
  CPPUNIT_ASSERT(tag.GetNumberOfComponents()==3);
  group->GetTagArray()->GetTag("SURFACE_MASS",tag);
  CPPUNIT_ASSERT(tag.GetNumberOfComponents()==1);

  double tagValue = -1;

  group->GetTagArray()->GetTag("PRINCIPAL_INERTIAL_TENSOR_COMPONENTS",tag);

  tagValue = tag.GetValueAsDouble(0);

  CPPUNIT_ASSERT(tag.GetNumberOfComponents()==3);

  tagValue = tag.GetValueAsDouble(0);
  CPPUNIT_ASSERT(92.294699334942152 == tagValue);

  tagValue = tag.GetValueAsDouble(1);
  CPPUNIT_ASSERT(92.294697586032868 == tagValue);

  tagValue = tag.GetValueAsDouble(2);
  CPPUNIT_ASSERT(89.275180946416327 == tagValue);

  group->GetTagArray()->GetTag("SURFACE_MASS",tag);

  CPPUNIT_ASSERT(tag.GetNumberOfComponents()==1);

  tagValue = tag.GetValueAsDouble(0);

  CPPUNIT_ASSERT(60.806545388924100 == tagValue);

  albaVME *child0 = NULL;
  child0 = group->GetChild(0);
  CPPUNIT_ASSERT(child0 != NULL);

  CPPUNIT_ASSERT( child0->GetTagArray()->GetTag("SURFACE_MASS") != NULL);

  child0->GetTagArray()->GetTag("SURFACE_MASS",tag);
  CPPUNIT_ASSERT(tag.GetNumberOfComponents()==1);

  CPPUNIT_ASSERT( child0->GetTagArray()->GetTag("PRINCIPAL_INERTIAL_TENSOR_COMPONENTS") == NULL);

  albaVME *child1 = NULL;
  child1 = group->GetChild(1);
  CPPUNIT_ASSERT(child1 != NULL);

  tagValue = tag.GetValueAsDouble(0);
  CPPUNIT_ASSERT(30.403272694462050 == tagValue);

  CPPUNIT_ASSERT( child1->GetTagArray()->GetTag("SURFACE_MASS") != NULL);

  child1->GetTagArray()->GetTag("SURFACE_MASS",tag);
  CPPUNIT_ASSERT(tag.GetNumberOfComponents()==1);

  tagValue = tag.GetValueAsDouble(0);
  CPPUNIT_ASSERT(30.403272694462050 == tagValue);

  CPPUNIT_ASSERT( child1->GetTagArray()->GetTag("PRINCIPAL_INERTIAL_TENSOR_COMPONENTS") == NULL);
  
  albaDEL(op);
  albaDEL(group);
  albaDEL(copy);
  albaDEL(importer);
}
//-----------------------------------------------------------
void albaOpComputeInertialTensorTest::TestOpDoUndo() 
//-----------------------------------------------------------
{ 
  // import VTK  
  albaOpImporterVTK *importer=new albaOpImporterVTK("importerVTK");
  importer->TestModeOn();
  albaString fileName=ALBA_DATA_ROOT;
  fileName<<"/Surface/sphere.vtk";
  importer->SetFileName(fileName);
  importer->ImportFile();
  albaVMESurface *surface=albaVMESurface::SafeDownCast(importer->GetOutput());
  
  CPPUNIT_ASSERT(surface!=NULL);
  CPPUNIT_ASSERT(surface->GetOutput()->GetVTKData()!=NULL);

  albaOpComputeInertialTensor *op=new albaOpComputeInertialTensor();
  op->TestModeOn();
  op->SetInput(surface);
  op->ComputeInertialTensor(surface);
  op->AddAttributes();

  op->OpUndo();

  // test undo 
  CPPUNIT_ASSERT(!surface->GetTagArray()->IsTagPresent("PRINCIPAL_INERTIAL_TENSOR_COMPONENTS"));
  CPPUNIT_ASSERT(!surface->GetTagArray()->IsTagPresent("INERTIAL_TENSOR_COMPONENTS"));
  CPPUNIT_ASSERT(!surface->GetTagArray()->IsTagPresent("SURFACE_MASS"));

  op->OpDo();

  // test do
  CPPUNIT_ASSERT(surface->GetTagArray()->IsTagPresent("PRINCIPAL_INERTIAL_TENSOR_COMPONENTS"));
  CPPUNIT_ASSERT(surface->GetTagArray()->IsTagPresent("INERTIAL_TENSOR_COMPONENTS"));
  CPPUNIT_ASSERT(surface->GetTagArray()->IsTagPresent("SURFACE_MASS"));

  albaDEL(op);
  albaDEL(importer);
}

//-----------------------------------------------------------
void albaOpComputeInertialTensorTest::TestComputeInertialTensorFromDENSITYTag() 
	//-----------------------------------------------------------
{
	// import VTK  
	albaOpImporterVTK *importer=new albaOpImporterVTK("importerVTK");
	importer->TestModeOn();
	albaString fileName=ALBA_DATA_ROOT;
	fileName<<"/Surface/sphere.vtk";
	importer->SetFileName(fileName);
	importer->ImportFile();
	albaVMESurface *surface=albaVMESurface::SafeDownCast(importer->GetOutput());

	surface->GetTagArray()->SetTag("DENSITY", "1.0");
	double density = surface->GetTagArray()->GetTag("DENSITY")->GetComponentAsDouble(0);

	CPPUNIT_ASSERT(surface!=NULL);
	CPPUNIT_ASSERT(surface->GetOutput()->GetVTKData()!=NULL);
	CPPUNIT_ASSERT(surface->GetTagArray()->GetTag("DENSITY") != NULL);
	CPPUNIT_ASSERT(density == 1.0);

	// test on surface
	albaOpComputeInertialTensor *op=new albaOpComputeInertialTensor();
	op->TestModeOn();
	op->SetDefaultDensity(5.0);
	op->SetInput(surface);
	op->ComputeInertialTensor(surface);
	op->AddAttributes();

	double tagValue = -1;

	albaTagItem tag;
	surface->GetTagArray()->GetTag("PRINCIPAL_INERTIAL_TENSOR_COMPONENTS",tag);

	tagValue = tag.GetValueAsDouble(0);

	CPPUNIT_ASSERT(tag.GetNumberOfComponents()==3);

	tagValue = tag.GetValueAsDouble(0);
	CPPUNIT_ASSERT(46.147349667471083 == tagValue);

	tagValue = tag.GetValueAsDouble(1);
	CPPUNIT_ASSERT(46.147348793016427 == tagValue);

	tagValue = tag.GetValueAsDouble(2);
	CPPUNIT_ASSERT(44.637590473208157 == tagValue);

	surface->GetTagArray()->GetTag("SURFACE_MASS",tag);

	CPPUNIT_ASSERT(tag.GetNumberOfComponents()==1);

	tagValue = tag.GetValueAsDouble(0);

	CPPUNIT_ASSERT(30.403272694462050 == tagValue);

	albaDEL(op);
	albaDEL(importer);
}

//-----------------------------------------------------------
void albaOpComputeInertialTensorTest::TestComputeInertialTensorFromGroupFromDENSITYTag() 
//-----------------------------------------------------------
{
	// import VTK  
	albaOpImporterVTK *importer=new albaOpImporterVTK("importerVTK");
	importer->TestModeOn();
	albaString fileName=ALBA_DATA_ROOT;
	fileName<<"/Surface/sphere.vtk";
	importer->SetFileName(fileName);
	importer->ImportFile();
	albaVMESurface *surface=albaVMESurface::SafeDownCast(importer->GetOutput());

	surface->GetTagArray()->SetTag("DENSITY", "1.0");

	CPPUNIT_ASSERT(surface!=NULL);
	CPPUNIT_ASSERT(surface->GetOutput()->GetVTKData()!=NULL);

	// create surface copy
	albaVMESurface* copy;
	albaNEW(copy);
	copy->DeepCopy(surface);

	// create group
	albaVMEGroup* group;
	albaNEW(group);
	group->AddChild(surface);
	group->AddChild(copy);

	// test on group
	albaOpComputeInertialTensor *op=new albaOpComputeInertialTensor();
	op->TestModeOn();
	op->SetInput(group);
	op->SetDefaultDensity(5.0);
	op->ComputeInertialTensorFromGroup();
	op->AddAttributes();

	albaTagItem tag;
	group->GetTagArray()->GetTag("PRINCIPAL_INERTIAL_TENSOR_COMPONENTS",tag);
	CPPUNIT_ASSERT(tag.GetNumberOfComponents()==3);
	group->GetTagArray()->GetTag("SURFACE_MASS",tag);
	CPPUNIT_ASSERT(tag.GetNumberOfComponents()==1);

	double tagValue = -1;

	group->GetTagArray()->GetTag("PRINCIPAL_INERTIAL_TENSOR_COMPONENTS",tag);

	tagValue = tag.GetValueAsDouble(0);

	CPPUNIT_ASSERT(tag.GetNumberOfComponents()==3);

	tagValue = tag.GetValueAsDouble(0);
	CPPUNIT_ASSERT(92.294699334942152 == tagValue);

	tagValue = tag.GetValueAsDouble(1);
	CPPUNIT_ASSERT(92.294697586032868 == tagValue);

	tagValue = tag.GetValueAsDouble(2);
	CPPUNIT_ASSERT(89.275180946416327 == tagValue);

	group->GetTagArray()->GetTag("SURFACE_MASS",tag);

	CPPUNIT_ASSERT(tag.GetNumberOfComponents()==1);

	tagValue = tag.GetValueAsDouble(0);

	CPPUNIT_ASSERT(60.806545388924100 == tagValue);

	albaDEL(op);
	albaDEL(group);
	albaDEL(copy);
	albaDEL(importer);
}