/*========================================================================= 
  Program: Multimod Application Framework RELOADED 
  Module: $RCSfile: mmiVTKPicker.cpp,v $ 
  Language: C++ 
  Date: $Date: 2010-11-17 16:19:42 $ 
  Version: $Revision: 1.1.2.5 $ 
  Authors: Josef Kohout (Josef.Kohout *AT* beds.ac.uk)
  ========================================================================== 
  Copyright (c) 2009 University of Bedfordshire (www.beds.ac.uk)
  See the COPYINGS file for license details 
  =========================================================================
*/

#include "albaDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "mmiVTKPicker.h"
#include "albaEventInteraction.h"

#include "vtkALBASmartPointer.h"
#include "vtkCamera.h"
#include "vtkPointPicker.h"
#include "vtkCellPicker.h"
#include "vtkRenderer.h"
#include "vtkMath.h"
#include "vtkPoints.h"

#include "albaMemDbg.h"
//#include "../albaDbg.h"


//------------------------------------------------------------------------------
albaCxxTypeMacro(mmiVTKPicker)
//------------------------------------------------------------------------------

mmiVTKPicker::~mmiVTKPicker()
{
  vtkDEL(m_Picker);
}

//----------------------------------------------------------------------------
void mmiVTKPicker::OnLeftButtonDown(albaEventInteraction *e) 
//----------------------------------------------------------------------------
{
  // if the event is NULL return
  if (e == NULL)
  {
	  return;
  }

  //if we debug OnLeftButtonDown, it happens usually that OnLeftButtonUp
  //is not executed and, therefore, we have still some picker
  //we will fix it here
  if (m_Picker != NULL)
  {
    albaEventMacro(albaEvent(this, VME_PICKED, m_Picker));
    vtkDEL(m_Picker);
  }

  // Check if CTRL modifier is pressed
  if (e->GetModifier(ALBA_CTRL_KEY)) 
  {    
    // perform picking
    int x = m_LastMousePose[0];
    int y = m_LastMousePose[1];

    if (m_Renderer != NULL)
    {
      vtkNEW(m_Picker);
      m_Picker->SetTolerance(0.001); 

      if (m_Picker->Pick(x,y,0,m_Renderer) == 0) 
	  {
	    //nothing was picked
        vtkDEL(m_Picker); 
      }
      else
      {   
        //picking successful               
        albaEvent ev(this, VME_PICKING, m_Picker);
        ev.SetBool(false);
        albaEventMacro(ev);
      }
    }
  }
  else
  {   
    // call superclass
    Superclass::OnLeftButtonDown(e);
  }
}
//----------------------------------------------------------------------------
void mmiVTKPicker::OnLeftButtonUp()
//----------------------------------------------------------------------------
{
  if (m_Picker != NULL)
  {
    albaEventMacro(albaEvent(this, VME_PICKED, m_Picker));
    vtkDEL(m_Picker);
  }

  Superclass::OnLeftButtonUp();
}

//----------------------------------------------------------------------------
void mmiVTKPicker::OnMouseMove() 
//----------------------------------------------------------------------------
{  
  // if something has been picked do not move the camera
  if (m_Picker != NULL)
  {    
    //if the continuous picking is allowed, compute the new position of picked 
    //point (stored in m_Picker)
    if (m_ContinuousPicking)
    {
      double cameraPos[3], pointOrigPos[3], pointNewPos[3];
      vtkCamera* camera = m_Renderer->GetActiveCamera();
      camera->GetPosition(cameraPos);

      //convert the current 2d selection point into world coordinates
      double cameraFP[4]; cameraFP[3] = 1.0;
      camera->GetFocalPoint(cameraFP);
      m_Renderer->SetWorldPoint(cameraFP);
      m_Renderer->WorldToDisplay(); //project focal point

      double selPoint[3];
      selPoint[0] = m_MousePose[0];
      selPoint[1] = m_MousePose[1];
      selPoint[2] = m_Renderer->GetDisplayPoint()[2];
      m_Renderer->SetDisplayPoint(selPoint);
      m_Renderer->DisplayToWorld();

      //and get the result
      double* wcoords = m_Renderer->GetWorldPoint();
      for (int i = 0; i < 3; i++) 
      {
        //convert coordinates from homogeneous coordinates to Cartesian
        //and create a vector from cameraPos to this point
        pointNewPos[i] = (wcoords[i] / wcoords[3]) - cameraPos[i];
      }

      //normalize the vector
      vtkMath::Normalize(pointNewPos);      

      //get the original picked position and its distance from the origin of projection
      m_Picker->GetPickedPositions()->GetPoint(0, pointOrigPos);
      double dblDist = sqrt(vtkMath::Distance2BetweenPoints(cameraPos, pointOrigPos));           

      //and compute the new coordinates
      for (int i = 0; i < 3; i++) {
        pointNewPos[i] = cameraPos[i] + pointNewPos[i]*dblDist;        
      }

      vtkALBASmartPointer< vtkPoints > points;      
      points->InsertNextPoint(pointNewPos);

      albaEvent ev(this, VME_PICKING, points);
      ev.SetBool(true);        //continuous picking      
      albaEventMacro(ev);
    }    
  }
  else
  {
    // call superclass
    Superclass::OnMouseMove();
  }
}