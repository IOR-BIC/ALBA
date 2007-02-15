/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGizmoPathRuler.cpp,v $
  Language:  C++
  Date:      $Date: 2007-02-15 18:01:22 $
  Version:   $Revision: 1.1 $
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

#include "mafMatrix.h"
#include "mafVME.h"

//----------------------------------------------------------------------------
mafGizmoPathRuler::mafGizmoPathRuler(mafVME *input, mafObserver* listener, \
                 int ticksNumber, int originTickID, double ticksHeigth,double ticksDistance)
{
  assert(input);
  InputVME = input;
  m_Listener = listener;
  m_TicksNumber = ticksNumber;
  m_OriginTickID = originTickID;
  m_TicksHeigth = ticksHeigth;
  m_TicksDistance = ticksDistance;
  m_CurvilinearAbscissa = 0;

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
    mafGizmoPath *gp = new mafGizmoPath(InputVME, this, VMEGizmoName.c_str() );

    // set the constraint
    // ...

    gp->SetLineLength(m_TicksHeigth);

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
  for (int i = 0; i < m_GizmoPathVector.size();i++)
  {
    m_GizmoPathVector[i]->SetLineLength(height);
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
    int valid = m_GizmoPathVector[i]->SetCurvilinearAbscissa(s - m_TicksDistance*(m_OriginTickID - i));
    if (valid == MAF_OK)
    {
      m_CurvilinearAbscissa = s;
      m_GizmoPathVector[i]->Show(true);
        // matrixVector.Push(m_GizmoPathVector[i]->GetAbsPose());
    } 
    else
    {
      m_GizmoPathVector[i]->Show(false);
      // matrixVector.Push(invalid);
    } 
  }

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
  InputVME = mafVME::SafeDownCast(vme);
}

void mafGizmoPathRuler::SetColor( double col[3] )
{
  for (int i = 0; i < m_GizmoPathVector.size();i++)
  {
    m_GizmoPathVector[i]->SetColor(col);
  }
}