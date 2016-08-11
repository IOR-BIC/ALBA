/*=========================================================================

 Program: MAF2
 Module: vtkMAFProfilingActorTest
 Authors: Alberto Losi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "mafDefines.h"
#include <cppunit/config/SourcePrefix.h>

#include "vtkMAFGridActor.h"
#include "vtkMAFProfilingActor.h"
#include "vtkMAFProfilingActorTest.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkMAFSmartPointer.h"
#include "vtkActor.h"
#include "vtkCamera.h"
#include "vtkPointData.h"
#include "vtkObjectFactory.h"

class vtkMAFProfilingActorDummy : public vtkMAFProfilingActor
{
  public:
    //vtkTypeRevisionMacro(vtkMAFProfilingActorDummy,vtkMAFProfilingActor);
    
    static vtkMAFProfilingActorDummy *New()
    {
      vtkObject* ret = vtkObjectFactory::CreateInstance("vtkMAFProfilingActorDummy");
      if(ret)
      { 
      return static_cast<vtkMAFProfilingActorDummy*>(ret); 
      } 
      return new vtkMAFProfilingActorDummy;
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
void vtkMAFProfilingActorTest::BeforeTest()
//----------------------------------------------------------------------------
{
	vtkNEW(m_Renderer);
	vtkNEW(m_RenderWindow);

	m_RenderWindow->AddRenderer(m_Renderer);
	m_RenderWindow->SetSize(640, 480);
	m_RenderWindow->SetPosition(100, 0);

	vtkCamera *camera = m_Renderer->GetActiveCamera();
	camera->ParallelProjectionOn();
	camera->Modified();

	m_Renderer->SetBackground(0.0, 0.0, 0.0);
}
//----------------------------------------------------------------------------
void vtkMAFProfilingActorTest::AfterTest()
//----------------------------------------------------------------------------
{
	vtkDEL(m_Renderer);
	vtkDEL(m_RenderWindow);
}

//------------------------------------------------------------
void vtkMAFProfilingActorTest::TestDynamicAllocation()
//------------------------------------------------------------
{
  vtkMAFProfilingActor *profActor = vtkMAFProfilingActor::New();
  profActor->Delete();
}
//--------------------------------------------
void vtkMAFProfilingActorTest::TestRenderOverlay()
//--------------------------------------------
{  
  vtkMAFProfilingActorDummy *profActor = vtkMAFProfilingActorDummy::New();
  
	m_Renderer->AddActor(profActor);
  m_RenderWindow->Render();

  CPPUNIT_ASSERT(profActor->RenderOverlay((vtkViewport*)m_Renderer) == 1);
	m_RenderWindow->Render();
  profActor->FPSUpdate(m_Renderer);

	COMPARE_IMAGES("TestRenderOverlay");

  profActor->Delete();
}
//--------------------------------------------
void vtkMAFProfilingActorTest::TestRenderOpaqueGeometry()
//--------------------------------------------
{
  vtkMAFProfilingActorDummy *profActor = vtkMAFProfilingActorDummy::New();

  m_Renderer->AddActor(profActor);
	m_RenderWindow->Render();

  CPPUNIT_ASSERT(profActor->RenderOpaqueGeometry((vtkViewport*)m_Renderer) == 0);
	m_RenderWindow->Render();
  profActor->FPSUpdate(m_Renderer);

	COMPARE_IMAGES("TestRenderOpaqueGeometry");

  profActor->Delete();
}
//--------------------------------------------
void vtkMAFProfilingActorTest::TestRenderTranslucentGeometry()
//--------------------------------------------
{
  vtkMAFProfilingActor *profActor = vtkMAFProfilingActor::New();

  m_Renderer->AddActor(profActor);

  CPPUNIT_ASSERT(profActor->RenderTranslucentGeometry((vtkViewport*)m_Renderer) == 0); //This method only returns 0

  m_RenderWindow->Render();
	COMPARE_IMAGES("TestRenderTranslucentGeometry");

	profActor->Delete();
}

//----------------------------------------------------------------------------
void vtkMAFProfilingActorTest::TestPrintSelf()
//----------------------------------------------------------------------------
{
  vtkMAFProfilingActor *actor;
  actor = vtkMAFProfilingActor::New();
  actor->PrintSelf(std::cout, 3);
  actor->Delete();
}