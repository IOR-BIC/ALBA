/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkALBAProfilingActorTest
 Authors: Alberto Losi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "albaDefines.h"
#include <cppunit/config/SourcePrefix.h>

#include "vtkALBAGridActor.h"
#include "vtkALBAProfilingActor.h"
#include "vtkALBAProfilingActorTest.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkALBASmartPointer.h"
#include "vtkActor.h"
#include "vtkCamera.h"
#include "vtkPointData.h"
#include "vtkObjectFactory.h"

#include "vtkPropCollection.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"


class vtkALBAProfilingActorDummy : public vtkALBAProfilingActor
{
  public:
    
    static vtkALBAProfilingActorDummy *New()
    {
      vtkObject* ret = vtkObjectFactory::CreateInstance("vtkALBAProfilingActorDummy");
      if(ret)
      { 
      return static_cast<vtkALBAProfilingActorDummy*>(ret); 
      } 
      return new vtkALBAProfilingActorDummy;
    };

    void FPSUpdate(vtkRenderer *ren)
    {
      int *size = ren->GetSize();
      TextFPS->SetPosition(10,size[1]-40);
      TextFPS->Modified();
      sprintf(TextBuff,"fps: %.1f \nrender time: %.3f s",0, 0);
      TextFPS->SetInput(this->TextBuff);
    };
};

//----------------------------------------------------------------------------
void vtkALBAProfilingActorTest::BeforeTest()
//----------------------------------------------------------------------------
{
  InitializeRenderWindow();
}
//----------------------------------------------------------------------------
void vtkALBAProfilingActorTest::AfterTest()
//----------------------------------------------------------------------------
{
}

//------------------------------------------------------------
void vtkALBAProfilingActorTest::TestDynamicAllocation()
//------------------------------------------------------------
{
  vtkALBAProfilingActor *profActor = vtkALBAProfilingActor::New();
  profActor->Delete();
}
//--------------------------------------------
void vtkALBAProfilingActorTest::TestRenderOverlay()
//--------------------------------------------
{  
  vtkALBAProfilingActorDummy *profActor = vtkALBAProfilingActorDummy::New();
  
	m_Renderer->AddActor(profActor);

	m_Renderer->ResetCamera();
  CPPUNIT_ASSERT(profActor->RenderOverlay((vtkViewport*)m_Renderer) == 1);
	m_RenderWindow->Render();
  profActor->FPSUpdate(m_Renderer);

	COMPARE_IMAGES("TestRenderOverlay");

  profActor->Delete();
}
//--------------------------------------------
void vtkALBAProfilingActorTest::TestRenderOpaqueGeometry()
//--------------------------------------------
{
  vtkALBAProfilingActorDummy *profActor = vtkALBAProfilingActorDummy::New();

  m_Renderer->AddActor(profActor);

	m_Renderer->ResetCamera();
  CPPUNIT_ASSERT(profActor->RenderOpaqueGeometry((vtkViewport*)m_Renderer) == 0);
	m_RenderWindow->Render();
  profActor->FPSUpdate(m_Renderer);

	COMPARE_IMAGES("TestRenderOpaqueGeometry");

  profActor->Delete();
}
//--------------------------------------------
void vtkALBAProfilingActorTest::TestRenderTranslucentGeometry()
//--------------------------------------------
{
  vtkALBAProfilingActor *profActor = vtkALBAProfilingActor::New();

  m_Renderer->AddActor(profActor);


	m_Renderer->ResetCamera();
  CPPUNIT_ASSERT(profActor->RenderTranslucentGeometry((vtkViewport*)m_Renderer) == 0); //This method only returns 0
  m_RenderWindow->Render();
	COMPARE_IMAGES("TestRenderTranslucentGeometry");

	profActor->Delete();
}

//----------------------------------------------------------------------------
void vtkALBAProfilingActorTest::TestPrintSelf()
//----------------------------------------------------------------------------
{
  vtkALBAProfilingActor *actor;
  actor = vtkALBAProfilingActor::New();
  actor->PrintSelf(std::cout, vtkIndent(2));
  actor->Delete();
}