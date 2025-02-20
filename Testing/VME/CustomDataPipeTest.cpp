/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: multiThreaderTest
 Authors: Gianluigi Crimi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// Test albaVMEGeneric class
#include "albaDefines.h"

#include "CustomDataPipeTest.h"
#include "albaVME.h"
#include "albaDataPipeCustom.h"
#include "albaVMEOutputSurface.h"
#include "albaTransform.h"

#include "vtkALBASmartPointer.h"
#include "vtkALBADataPipe.h"
#include "vtkActor.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkPolyDataMapper.h"
#include "vtkPolyData.h"
#include "vtkCamera.h"
#include "vtkProperty.h"
#include "vtkSphereSource.h"
#include "vtkConeSource.h"
#include <iostream>

//-------------------------------------------------------------------------
class albaTestVME: public albaVME
//-------------------------------------------------------------------------
{
public:
  albaTypeMacro(albaTestVME,albaVME);
  albaTestVME();
  ~albaTestVME();
  void SetMatrix(const albaMatrix &mat) {m_Transform->SetMatrix(mat);}
  void SetTypeToCone(){m_Type=0;Modified();}
  void SetTypeToSphere(){m_Type=1;Modified();}
  void SetRadius(double r) {m_Radius=r;Modified();}
  void GetLocalTimeStamps(std::vector<double> &vect) {vect.clear();}
  virtual void InternalPreUpdate();
  virtual void InternalUpdate();
  albaTransform *m_Transform;
  vtkSphereSource *m_Sphere;
  vtkConeSource *m_Cone;
  double m_Radius;
  int m_Type;
  int m_PreUpdateConunter;
  int m_UpdateConunter;
};

//-------------------------------------------------------------------------
albaCxxTypeMacro(albaTestVME)
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
albaTestVME::albaTestVME():m_PreUpdateConunter(0),m_UpdateConunter(0),m_Type(0),m_Radius(1)
//-------------------------------------------------------------------------
{
  albaNEW(m_Transform);
  vtkNEW(m_Sphere);
	m_Sphere->Update();
  vtkNEW(m_Cone);
	m_Cone->Update();
  SetDataPipe(albaDataPipeCustom::New());
  SetOutput(albaVMEOutputSurface::New());
  GetOutput()->SetTransform(m_Transform);
}
//-------------------------------------------------------------------------
albaTestVME::~albaTestVME()
//-------------------------------------------------------------------------
{
  albaDEL(m_Transform);
  vtkDEL(m_Sphere);
  vtkDEL(m_Cone);
  // pipe and output deleted by VME
}

//-------------------------------------------------------------------------
void albaTestVME::InternalPreUpdate()
//-------------------------------------------------------------------------
{
  m_PreUpdateConunter++;
  albaDataPipeCustom *dpipe=(albaDataPipeCustom *)GetDataPipe();
  switch (m_Type)
  {
    case 0: 
    {
      dpipe->GetVTKDataPipe()->SetInputConnection(m_Cone->GetOutputPort());
    }
    break;
    case 1: 
    {
      dpipe->GetVTKDataPipe()->SetInputConnection(m_Sphere->GetOutputPort());
    }
    break;
  }
  
}

//-------------------------------------------------------------------------
void albaTestVME::InternalUpdate()
//-------------------------------------------------------------------------
{
  m_UpdateConunter++; // increment the counter
  // set vtk filters radius before of their execution
  m_Sphere->SetRadius(m_Radius);
	m_Sphere->Update();
  m_Cone->SetRadius(m_Radius);
	m_Cone->Update();
}
//-------------------------------------------------------------------------
void CustomDataPipeTest::CustomDataPipeMainTest()
//-------------------------------------------------------------------------
{
  // create a small tree with a root, a volume and a slicer 
  albaSmartPointer<albaTestVME> vme;
  vme->SetName("test vme");

  vtkDataSet *data=vme->GetOutput()->GetVTKData();

  CPPUNIT_ASSERT(data->IsA("vtkPolyData"));

  vtkPolyData *surface=(vtkPolyData *)data;

  CPPUNIT_ASSERT(vme->m_PreUpdateConunter==1);
  CPPUNIT_ASSERT(vme->m_UpdateConunter==1);


  CPPUNIT_ASSERT(vme->m_Sphere->GetRadius()==1);
  CPPUNIT_ASSERT(vme->m_Cone->GetRadius()==1);

	//Update after set Radius, the VME has changed and the output should be updated.
  vme->SetRadius(2);
	vme->GetOutput()->Update();
  CPPUNIT_ASSERT(vme->m_PreUpdateConunter==2);
  CPPUNIT_ASSERT(vme->m_UpdateConunter==2);

	//Update after set type to Sphere, the VME has changed and the output should be updated.
	vme->SetTypeToSphere();
  CPPUNIT_ASSERT(vme->GetOutput()->GetVTKData()==surface);
  CPPUNIT_ASSERT(vme->m_PreUpdateConunter==3);
  CPPUNIT_ASSERT(vme->m_UpdateConunter==3);
  
	//Update whiteout VME changes, the VME should not be updated to avoid resource consuming tasks
	vme->GetOutput()->Update();
  
  CPPUNIT_ASSERT(vme->m_PreUpdateConunter==3);
  CPPUNIT_ASSERT(vme->m_UpdateConunter==3);
  
  std::cerr<<"Test completed successfully!"<<std::endl;

}
