/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGizmoPathRuler.cpp,v $
  Language:  C++
  Date:      $Date: 2008-11-03 14:46:57 $
  Version:   $Revision: 1.6.2.2 $
  Authors:   Stefano Perticoni
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "mafGizmoPath.h"
#include "mafGizmoPathRuler.h"
#include "mafDecl.h"
#include "mmiGenericMouse.h"
#include "mafSmartPointer.h"
#include "mafVector.h"
#include "mafVMEGizmo.h"

#include "mafMatrix.h"
#include "mafVME.h"

//----------------------------------------------------------------------------
mafGizmoPathRuler::mafGizmoPathRuler(mafVME *input, mafObserver* listener, \
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
mafGizmoPathRuler::~mafGizmoPathRuler() 
//----------------------------------------------------------------------------
{
  // destroy gizmos
  DestroyGizmos();

}
//----------------------------------------------------------------------------
void mafGizmoPathRuler::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  void *sender = maf_event->GetSender();
  
  // waiting for mouse interaction and gizmo gui...
  //if (...)
  //{
  //  OnEventGizmoComponents(maf_event); // process events from gizmo components
  //}
  //else if (sender == GuiGizmoTranslate)
  //{
  //  OnEventGizmoGui(maf_event); // process events from the gui
  //}
  //else
  //{
  //  mafEventMacro(*maf_event);
  //}
  
  // for the moment just forward up...
  mafEventMacro(*maf_event);
}
//----------------------------------------------------------------------------  
void mafGizmoPathRuler::Show(bool show)
//----------------------------------------------------------------------------
{
  for (int i = 0; i < m_GizmoPathVector.size();i++)
  {
    m_GizmoPathVector[i]->Show(show);
  }
}
//----------------------------------------------------------------------------
mafMatrix *mafGizmoPathRuler::GetAbsPose(int tickId)
//----------------------------------------------------------------------------
{
  assert(tickId < m_TicksNumber);
  return m_GizmoPathVector[tickId]->GetAbsPose();
}

void mafGizmoPathRuler::SetAbsPose( mafMatrix *absPose )
{

}

void mafGizmoPathRuler::OnEventGizmoGui( mafEventBase *maf_event )
{
	
}

void mafGizmoPathRuler::OnEventGizmoComponents( mafEventBase *maf_event )
{
	
}

void mafGizmoPathRuler::BuildGizmos()
{
  
  std::string VMEGizmoName = "gizmo path ";

  for (int gizmoID = 0; gizmoID < m_TicksNumber; gizmoID++)
  {
    VMEGizmoName += gizmoID;
   
    // create a gizmo for each tick
    int visibility = FALSE;
    if(gizmoID == 0 || gizmoID == m_TicksNumber/2 || gizmoID == m_TicksNumber-1)
    {
      visibility = TRUE;
    }
    mafGizmoPath *gp = new mafGizmoPath(m_InputVME, this, VMEGizmoName.c_str(), visibility );

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
 
void mafGizmoPathRuler::DestroyGizmos()
{
  for (int i = 0; i < m_GizmoPathVector.size();i++)
  {
    cppDEL(m_GizmoPathVector[i]);
  }
}

void mafGizmoPathRuler::SetTicksHeigth( double height )
{
  for (int gizmoID = 0; gizmoID < m_GizmoPathVector.size();gizmoID++)
  {
    if(m_EnableShorterTicks)
    {
      if((gizmoID == 0 || gizmoID == m_TicksNumber/2 || gizmoID == m_TicksNumber-1))
      {
        m_GizmoPathVector[gizmoID]->SetLineLength(m_TicksHeigth);
      }
      else
      {
        m_GizmoPathVector[gizmoID]->SetLineLength(m_TicksHeigth/2.);
      }
    }
    else
    {
      m_GizmoPathVector[gizmoID]->SetLineLength(m_TicksHeigth);
    }
  }
}

void mafGizmoPathRuler::SetTicksDistance( double distance )
{
  m_TicksDistance = distance;
  SetCurvilinearAbscissa(m_CurvilinearAbscissa);
}

void mafGizmoPathRuler::SetCurvilinearAbscissa( double s )
{

  //// create the vector to send
  //mafVector<mafMatrix *> matrixVector;

  // set the abscissa for the ticks origin
  for (int i = 0; i < m_GizmoPathVector.size();i++)
  {
    m_GizmoPathVector[i]->SetCurvilinearAbscissa(s - m_TicksDistance*(m_OriginTickID - i)); 
  }
	m_CurvilinearAbscissa = s;

  // send the matrix vector... this is not supported for the moment...

  //mafEvent matrixVectorEvent;
  //// matrixVectorEvent.SetMafObject(&matrixVector);
  //matrixVectorEvent.SetSender(this);
  //matrixVectorEvent.SetId(ID_TRANSFORM);
  //matrixVectorEvent.SetArg(mafGizmoPath::ABS_POSE);
  //
  //mafEventMacro(matrixVectorEvent);

}

void mafGizmoPathRuler::SetConstraintPolyline( mafVME* constraintPolyline )
{
  for (int i = 0; i < m_GizmoPathVector.size();i++)
  {
    m_GizmoPathVector[i]->SetConstraintPolyline(constraintPolyline);
  }
}

void mafGizmoPathRuler::SetInput( mafVME *vme )
{
  // register the input vme
  m_InputVME = mafVME::SafeDownCast(vme);
}

void mafGizmoPathRuler::SetColor( double col[3] )
{
  for (int i = 0; i < m_GizmoPathVector.size();i++)
  {
    m_GizmoPathVector[i]->SetColor(col);
  }
}
void mafGizmoPathRuler::SetColor( int idGizmo,double col[3] )
{
	m_GizmoPathVector[idGizmo]->SetColor(col);
}

void mafGizmoPathRuler::SetColor( double abscissa,double col[3] )
{
  double centerAbscissa = m_CurvilinearAbscissa;
  double difference = abscissa - m_CurvilinearAbscissa;
  int gizmoID;
  int middle = m_GizmoPathVector.size() / 2;
  gizmoID = middle + difference / m_TicksDistance;

  SetColor(gizmoID, col);
}

void mafGizmoPathRuler::HighlightExtremes(double col[3], int bound1, int bound2, int center, bool inside)
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
void mafGizmoPathRuler::HighlightExtremes(double col[3], double bound1, double bound2, double center, bool inside)
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


void mafGizmoPathRuler::SetGizmoLabelsVisibility(bool value)
{
  for (int gizmoID = 0; gizmoID < m_GizmoPathVector.size();gizmoID++)
  {
    if(m_GizmoPathVector[gizmoID]->GetOutput() == NULL) continue;
    if(gizmoID != 0 && gizmoID != m_TicksNumber/2 && gizmoID != m_TicksNumber-1)
    {
      m_GizmoPathVector[gizmoID]->GetOutput()->SetTextVisibility(FALSE);
    }
    else
      m_GizmoPathVector[gizmoID]->GetOutput()->SetTextVisibility(value);
    
  }
}
void mafGizmoPathRuler::ResetLabelsVisibility()
{
  for (int gizmoID = 0; gizmoID < m_GizmoPathVector.size();gizmoID++)
  {
    m_GizmoPathVector[gizmoID]->GetOutput()->SetTextVisibility(FALSE);
  }
}

void mafGizmoPathRuler::CustomIndexLabelVisibility(int index, int flag)
{
  if(index >= 0 || index <=m_GizmoPathVector.size()-1)
    m_GizmoPathVector[index]->GetOutput()->SetTextVisibility(flag);
}