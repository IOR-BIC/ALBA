/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGizmoPathRuler
 Authors: Stefano Perticoni
 
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

#include "albaGizmoPath.h"
#include "albaGizmoPathRuler.h"
#include "albaDecl.h"
#include "albaInteractorGenericMouse.h"
#include "albaSmartPointer.h"
#include "albaVector.h"
#include "albaVMEGizmo.h"

#include "albaMatrix.h"
#include "albaVME.h"

//----------------------------------------------------------------------------
albaGizmoPathRuler::albaGizmoPathRuler(albaVME *input, albaObserver* listener, \
                 int ticksNumber, int originTickID, double ticksHeigth,double ticksDistance, bool enableShorterTicks)
{
  assert(input);
  m_InputVME = input;
  m_Listener = listener;
  m_TicksNumber = ticksNumber;
  m_OriginTickID = originTickID;
  m_TicksHeigth = ticksHeigth;
  m_TicksDistance = ticksDistance;
  m_CurvilinearAbscissa = 0;
  m_EnableShorterTicks = enableShorterTicks;
  // create gizmos and put their references in the vector 
  BuildGizmos();

  // create the gizmo gui
  // ...  
}
//----------------------------------------------------------------------------
albaGizmoPathRuler::~albaGizmoPathRuler() 
//----------------------------------------------------------------------------
{
  // destroy gizmos
  DestroyGizmos();

}
//----------------------------------------------------------------------------
void albaGizmoPathRuler::OnEvent(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{
  void *sender = alba_event->GetSender();
  
  // waiting for mouse interaction and gizmo gui...
  //if (...)
  //{
  //  OnEventGizmoComponents(alba_event); // process events from gizmo components
  //}
  //else if (sender == GuiGizmoTranslate)
  //{
  //  OnEventGizmoGui(alba_event); // process events from the gui
  //}
  //else
  //{
  //  albaEventMacro(*alba_event);
  //}
  
  // for the moment just forward up...
  albaEventMacro(*alba_event);
}
//----------------------------------------------------------------------------  
void albaGizmoPathRuler::Show(bool show)
//----------------------------------------------------------------------------
{
  for (int i = 0; i < m_GizmoPathVector.size();i++)
  {
    m_GizmoPathVector[i]->Show(show);
  }
}
//----------------------------------------------------------------------------
albaMatrix *albaGizmoPathRuler::GetAbsPose(int tickId)
//----------------------------------------------------------------------------
{
  assert(tickId < m_TicksNumber);
  return m_GizmoPathVector[tickId]->GetAbsPose();
}

void albaGizmoPathRuler::SetAbsPose( albaMatrix *absPose )
{

}

void albaGizmoPathRuler::OnEventGizmoGui( albaEventBase *alba_event )
{
	
}

void albaGizmoPathRuler::OnEventGizmoComponents( albaEventBase *alba_event )
{
	
}

void albaGizmoPathRuler::BuildGizmos()
{
  
  std::string VMEGizmoName = "gizmo path ";

  for (int gizmoID = 0; gizmoID < m_TicksNumber; gizmoID++)
  {
    VMEGizmoName += gizmoID;
   
    // create a gizmo for each tick
    int visibility = false;
    if(gizmoID == 0 || gizmoID == m_TicksNumber/2 || gizmoID == m_TicksNumber-1)
    {
      visibility = true;
    }
    albaGizmoPath *gp = new albaGizmoPath(m_InputVME, this, VMEGizmoName.c_str(), visibility );

    // set the constraint
    // ...

    if(m_EnableShorterTicks)
    {
      if((gizmoID == 0 || gizmoID == m_TicksNumber/2 || gizmoID == m_TicksNumber-1))
      {
        gp->SetLineLength(m_TicksHeigth);
      }
      else
      {
        gp->SetLineLength(m_TicksHeigth/2.);
      }
    }
    else
    {
      gp->SetLineLength(m_TicksHeigth);
    }
    
    

    // put pointer to gizmo in the vector;
    m_GizmoPathVector.push_back(gp);

    // show it
    gp->Show(true);

    gp = NULL;

  }
}
 
void albaGizmoPathRuler::DestroyGizmos()
{
  for (int i = 0; i < m_GizmoPathVector.size();i++)
  {
    cppDEL(m_GizmoPathVector[i]);
  }
}

void albaGizmoPathRuler::SetTicksHeigth( double height )
{
  for (int gizmoID = 0; gizmoID < m_GizmoPathVector.size();gizmoID++)
  {
    if(m_EnableShorterTicks)
    {
      if((gizmoID == 0 || gizmoID == m_TicksNumber/2 || gizmoID == m_TicksNumber-1))
      {
        m_GizmoPathVector[gizmoID]->SetLineLength(height);
      }
      else
      {
        m_GizmoPathVector[gizmoID]->SetLineLength(height/2.);
      }
    }
    else
    {
      m_GizmoPathVector[gizmoID]->SetLineLength(height);
    }
  }
}

void albaGizmoPathRuler::SetTicksDistance( double distance )
{
  m_TicksDistance = distance;
  SetCurvilinearAbscissa(m_CurvilinearAbscissa);
}

void albaGizmoPathRuler::SetCurvilinearAbscissa( double s )
{

  //// create the vector to send
  //albaVector<albaMatrix *> matrixVector;

  // set the abscissa for the ticks origin
  for (int i = 0; i < m_GizmoPathVector.size();i++)
  {
    m_GizmoPathVector[i]->SetCurvilinearAbscissa(s - m_TicksDistance*(m_OriginTickID - i)); 
  }
	m_CurvilinearAbscissa = s;

  // send the matrix vector... this is not supported for the moment...

  //albaEvent matrixVectorEvent;
  //// matrixVectorEvent.SetMafObject(&matrixVector);
  //matrixVectorEvent.SetSender(this);
  //matrixVectorEvent.SetId(ID_TRANSFORM);
  //matrixVectorEvent.SetArg(albaGizmoPath::ABS_POSE);
  //
  //albaEventMacro(matrixVectorEvent);

}

void albaGizmoPathRuler::SetConstraintPolyline( albaVME* constraintPolyline )
{
  for (int i = 0; i < m_GizmoPathVector.size();i++)
  {
    m_GizmoPathVector[i]->SetConstraintPolyline(constraintPolyline);
  }
}

void albaGizmoPathRuler::SetInput( albaVME *vme )
{
  // register the input vme
  m_InputVME = vme;
}

void albaGizmoPathRuler::SetColor( double col[3] )
{
  for (int i = 0; i < m_GizmoPathVector.size();i++)
  {
    m_GizmoPathVector[i]->SetColor(col);
  }
}
void albaGizmoPathRuler::SetColor( int idGizmo,double col[3] )
{
	m_GizmoPathVector[idGizmo]->SetColor(col);
}

void albaGizmoPathRuler::SetColor( double abscissa,double col[3] )
{
  double centerAbscissa = m_CurvilinearAbscissa;
  double difference = abscissa - m_CurvilinearAbscissa;
  int gizmoID;
  int middle = m_GizmoPathVector.size() / 2;
  gizmoID = middle + difference / m_TicksDistance;

  SetColor(gizmoID, col);
}

void albaGizmoPathRuler::HighlightExtremes(double col[3], int bound1, int bound2, int center, bool inside)
{
  int size = m_GizmoPathVector.size();
  int minBound = center - bound1;
  if(minBound < 0)
  {
    minBound = 0;
  }

  int maxBound = center + bound2;
  if(maxBound > size - 1)
  {
    maxBound = size - 1;
  }

  

  if(inside == true)
  {
    for(int i = minBound ; i<= maxBound; i++)
    {
      m_GizmoPathVector[i]->SetColor(col);
    }
  }
  else
  {
    m_GizmoPathVector[minBound]->SetColor(col);
    m_GizmoPathVector[maxBound]->SetColor(col);
  }

  

}
void albaGizmoPathRuler::HighlightExtremes(double col[3], double bound1, double bound2, double center, bool inside)
{ 
  double centerAbscissa = m_CurvilinearAbscissa;
  double difference = center - m_CurvilinearAbscissa;
  int gizmoID;
  int middle = m_GizmoPathVector.size() / 2;
  gizmoID = middle + difference / m_TicksDistance;

  int bound1Int = bound1 /  m_TicksDistance;
  int bound2Int = bound2 /  m_TicksDistance;
  HighlightExtremes(col, bound1Int, bound2Int, gizmoID, inside);
}


void albaGizmoPathRuler::SetGizmoLabelsVisibility(bool value)
{
  for (int gizmoID = 0; gizmoID < m_GizmoPathVector.size();gizmoID++)
  {
    if(m_GizmoPathVector[gizmoID]->GetOutput() == NULL) continue;
    if(gizmoID != 0 && gizmoID != m_TicksNumber/2 && gizmoID != m_TicksNumber-1)
    {
      m_GizmoPathVector[gizmoID]->GetOutput()->SetTextVisibility(false);
    }
    else
      m_GizmoPathVector[gizmoID]->GetOutput()->SetTextVisibility(value);
    
  }
}
void albaGizmoPathRuler::ResetLabelsVisibility()
{
  for (int gizmoID = 0; gizmoID < m_GizmoPathVector.size();gizmoID++)
  {
    m_GizmoPathVector[gizmoID]->GetOutput()->SetTextVisibility(false);
  }
}

void albaGizmoPathRuler::CustomIndexLabelVisibility(int index, int flag)
{
  if(index >= 0 && index <=m_GizmoPathVector.size()-1)
    m_GizmoPathVector[index]->GetOutput()->SetTextVisibility(flag);
}
