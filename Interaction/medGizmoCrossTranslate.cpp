/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: medGizmoCrossTranslate.cpp,v $
  Language:  C++
  Date:      $Date: 2010-09-15 16:11:32 $
  Version:   $Revision: 1.1.2.1 $
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


#include "medGizmoCrossTranslate.h"
#include "mafDecl.h"
#include "mafInteractorGenericMouse.h"
#include "medGizmoCrossTranslateAxis.h"
#include "medGizmoCrossTranslatePlane.h"
#include "mafGUIGizmoTranslate.h"
#include "mafSmartPointer.h"

#include "mafInteractorGenericMouse.h"

#include "mafMatrix.h"
#include "mafTransform.h"
#include "mafVME.h"
#include "mafVMEOutput.h"

#include "vtkTransform.h"
#include "vtkMatrix4x4.h"

//----------------------------------------------------------------------------
medGizmoCrossTranslate::medGizmoCrossTranslate(mafVME* input, mafObserver *listener, bool buildGUI, int axis)
//----------------------------------------------------------------------------
{
  assert(input);
  m_InputVME = input;
  m_Listener = listener;
  m_BuildGUI = buildGUI;
  m_GTAxis0 = NULL;
  m_GTAxis1 = NULL;
  m_GTPlane = NULL;
  m_GuiGizmoTranslate = NULL;

  m_Axis = axis;

  m_ConstraintModality = mafInteractorConstraint::FREE;

  m_Step = 1;
  
  //no gizmo component is active at construction
  this->m_ActiveGizmoComponent = -1;
  this->SetModalityToLocal();

  mafMatrix *absInputMatrix = m_InputVME->GetOutput()->GetAbsMatrix();
  mafNEW(m_PivotPose);
  m_PivotPose->DeepCopy(absInputMatrix);


  // Create mafGizmoTranslateAxis and send events to this

  m_GTAxis0 = new medGizmoCrossTranslateAxis(input, this);
  m_GTAxis1 = new medGizmoCrossTranslateAxis(input, this);

  int axis0 = -1;
  int axis1 = -1;

  int plane = -1;

  if (m_Axis == X)
  {
	axis0 = X;
	axis1 = Y;
	plane = medGizmoCrossTranslatePlane::XY;
  }

  // td
  else if (m_Axis == Y)
  {
	  axis0 = X;
	  axis1 = Y;
	  plane = medGizmoCrossTranslatePlane::XY;
  }
  else if (m_Axis == Z)
  {
	  axis0 = X;
	  axis1 = Y;
	  plane = medGizmoCrossTranslatePlane::XY;
  }

  m_GTAxis0->SetAxis(axis0);
  //m_GTAxis0->SetColor(0, 0, 1, 0, 0, 1);

  m_GTAxis1->SetAxis(axis1);
  //m_GTAxis1->SetColor(0, 0, 1, 0, 0, 1);
  
  // Create mafGTranslateAPlane 
  m_GTPlane = new medGizmoCrossTranslatePlane(input, this);
  m_GTPlane->SetPlane(plane);
  m_GTPlane->SetColor(medGizmoCrossTranslatePlane::S1, 0,0,1 );
  m_GTPlane->SetColor(medGizmoCrossTranslatePlane::S2, 0,0,1 );

  if (m_BuildGUI)
  {
    // create the gizmo gui
    // gui is sending events to this
    m_GuiGizmoTranslate = new mafGUIGizmoTranslate(this);
    // initialize gizmo gui
    m_GuiGizmoTranslate->SetAbsPosition(absInputMatrix);
  }
}
//----------------------------------------------------------------------------
medGizmoCrossTranslate::~medGizmoCrossTranslate() 
//----------------------------------------------------------------------------
{
  //Destroy:
  //3 mafGizmoTranslateAxis 
  //3 mafGizmoTranslatePlane
  
  cppDEL(m_GTAxis0);
  cppDEL(m_GTAxis1);
  cppDEL(m_GTPlane);

  mafDEL(m_PivotPose);
  cppDEL(m_GuiGizmoTranslate);
}
//----------------------------------------------------------------------------
void medGizmoCrossTranslate::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  void *sender = maf_event->GetSender();

  if (sender == m_GTAxis0 || sender == m_GTAxis1 || sender == m_GTPlane )
  {
    OnEventGizmoComponents(maf_event); // process events from gizmo components
  }
  else if (sender == m_GuiGizmoTranslate)
  {
    OnEventGizmoGui(maf_event); // process events from the gui
  }
  else
  {
    mafEventMacro(*maf_event);
  }
}
//----------------------------------------------------------------------------
void medGizmoCrossTranslate::OnEventGizmoComponents(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    void *sender = e->GetSender();
    long arg = e->GetArg();

    switch (e->GetId())
    {
      case ID_TRANSFORM:
      {
        // if a gizmo has been picked register the active component; the sender is the component
        // to be activated
        if (arg == mafInteractorGenericMouse::MOUSE_DOWN)
        {
          if (sender == m_GTAxis0)
          {
            this->Highlight(AXIS_0);
            m_ActiveGizmoComponent = AXIS_0;
          }
		  else if (sender == m_GTAxis1)
		  {
			  this->Highlight(AXIS_1);
			  m_ActiveGizmoComponent = AXIS_1;
		  }
          else if (sender == m_GTPlane)
          {
            this->Highlight(PLANE);
            m_ActiveGizmoComponent = PLANE;
          }
          // Store pivot position
          m_PivotPose->DeepCopy(m_GTAxis0->GetAbsPose());
        }
        else if (arg == mafInteractorGenericMouse::MOUSE_MOVE)
        {     
          // matrix holding abs pose after mouse move event
          mafSmartPointer<mafMatrix> newAbsMatr;
          if (this->m_Modality == G_LOCAL) // gizmo working in local mode; all its components are moving
                                         // in a single mouse move event
          {
            // local mode
            // forward to all gizmo
            vtkTransform *currTr = vtkTransform::New();
            currTr->PostMultiply();
            currTr->SetMatrix(GetAbsPose()->GetVTKMatrix());
            currTr->Concatenate(e->GetMatrix()->GetVTKMatrix());
            currTr->Update();

            newAbsMatr->DeepCopy(currTr->GetMatrix());
            newAbsMatr->SetTimeStamp(GetAbsPose()->GetTimeStamp());

            // set the new pose to the gizmo
            SetAbsPose(newAbsMatr);
            currTr->Delete();
          }          
          else
          {
            // gizmo working in global mode; only one axis/plane is moving in a single mouse move event
            if (m_ActiveGizmoComponent == AXIS_0)
            {
              // forward to active axis gizmo              
              vtkTransform *currTr = vtkTransform::New();
              currTr->PostMultiply();
              currTr->SetMatrix(m_GTAxis0->GetAbsPose()->GetVTKMatrix());
              currTr->Concatenate(e->GetMatrix()->GetVTKMatrix());
              currTr->Update();

              newAbsMatr->DeepCopy(currTr->GetMatrix());
              newAbsMatr->SetTimeStamp(GetAbsPose()->GetTimeStamp());

              // set the new pose to the gizmo
              m_GTAxis0->SetAbsPose(newAbsMatr);
              currTr->Delete();
            }
			else if (m_ActiveGizmoComponent == AXIS_1)
			{
				// forward to active axis gizmo              
				vtkTransform *currTr = vtkTransform::New();
				currTr->PostMultiply();
				currTr->SetMatrix(m_GTAxis1->GetAbsPose()->GetVTKMatrix());
				currTr->Concatenate(e->GetMatrix()->GetVTKMatrix());
				currTr->Update();

				newAbsMatr->DeepCopy(currTr->GetMatrix());
				newAbsMatr->SetTimeStamp(GetAbsPose()->GetTimeStamp());

				// set the new pose to the gizmo
				m_GTAxis1->SetAbsPose(newAbsMatr);
				currTr->Delete();
			}
            else if (m_ActiveGizmoComponent == PLANE)
            {
              // forward to active plane gizmo
              vtkTransform *currTr = vtkTransform::New();
              currTr->PostMultiply();
              currTr->SetMatrix(m_GTPlane->GetAbsPose()->GetVTKMatrix());
              currTr->Concatenate(e->GetMatrix()->GetVTKMatrix());
              currTr->Update();

              newAbsMatr->DeepCopy(currTr->GetMatrix());
              newAbsMatr->SetTimeStamp(GetAbsPose()->GetTimeStamp());

              // set the new pose to the gizmo
              m_GTPlane->SetAbsPose(newAbsMatr);
              currTr->Delete();
            }     
          }
          // update only gui with gizmo abs position
          if (m_BuildGUI) m_GuiGizmoTranslate->SetAbsPosition(newAbsMatr);
        }
        else if (arg == mafInteractorGenericMouse::MOUSE_UP)
        {
			if (sender == m_GTAxis0 || sender == m_GTAxis1 || sender == m_GTPlane)
			{
				this->Highlight(NONE);
			}

			if (this->m_Modality == G_GLOBAL)
			{
				// put the gizmo back in the start position       
				SetAbsPose(m_PivotPose);
			}
        }
        // forward event to the listener ie the operation
        // instanciating the gizmo; the sender is changed to "this" so that the operation can check for
        // gizmo sending events
        e->SetSender(this);
        mafEventMacro(*e);
      }
      break;

      default:
      {
        mafEventMacro(*e);
      }
      break;
    }
  }
}
//----------------------------------------------------------------------------
void medGizmoCrossTranslate::OnEventGizmoGui(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  switch(maf_event->GetId())
  {
    // process events from the gui   
    case (mafGUIGizmoTranslate::ID_TRANSLATE_X):
    {
      // receiving abs position from gui
      SendTransformMatrixFromGui(maf_event);     
    }
    break;
    case (mafGUIGizmoTranslate::ID_TRANSLATE_Y):
    {
      SendTransformMatrixFromGui(maf_event);     
    }
    break;
    case (mafGUIGizmoTranslate::ID_TRANSLATE_Z):
    {
      SendTransformMatrixFromGui(maf_event);     
    }
    break;
    default:
    {
      mafEventMacro(*maf_event);
    }
    break;
  }
}
//----------------------------------------------------------------------------
void medGizmoCrossTranslate::Highlight (int component) 
//----------------------------------------------------------------------------
{
	if (component == AXIS_0)
	{
		m_GTAxis0->Highlight(true);
		m_GTAxis1->Highlight(false);
		m_GTPlane->Highlight(false);
	} 
	else if (component == AXIS_1)
	{
		m_GTAxis0->Highlight(false);
		m_GTAxis1->Highlight(true);
		m_GTPlane->Highlight(false);
	}
	else if (component == PLANE)
	{
		m_GTAxis0->Highlight(false);
		m_GTAxis1->Highlight(false);
		m_GTPlane->Highlight(true);
	}
	else if (component == NONE)
	{
		// DeHighlight everything;
		m_GTAxis0->Highlight(false);
		m_GTAxis1->Highlight(false);
		m_GTPlane->Highlight(false);
	}
}
//----------------------------------------------------------------------------  
void medGizmoCrossTranslate::Show(bool show)
//----------------------------------------------------------------------------
{
  // set visibility ivar
  m_Visibility = show;

  m_GTAxis0->Show(show);
  m_GTAxis1->Show(show);
  m_GTPlane->Show(show);

  if (m_BuildGUI)
  {
    // if auxiliary ref sys is different from vme its orientation cannot be changed
    // so gui must not be keyable. Otherwise set gui keyability to show.
    if (m_RefSysVME == m_InputVME)
    {
      m_GuiGizmoTranslate->EnableWidgets(show);
    }
    else
    {
      m_GuiGizmoTranslate->EnableWidgets(false);
    }
  }
}
//----------------------------------------------------------------------------
mafMatrix *medGizmoCrossTranslate::GetAbsPose()
//----------------------------------------------------------------------------
{
  return m_GTAxis0->GetAbsPose();
}

//----------------------------------------------------------------------------  
void medGizmoCrossTranslate::SetInput(mafVME *input)
//----------------------------------------------------------------------------
{
  this->m_InputVME = input;
  
  m_GTAxis0->SetInput(input);
  m_GTAxis1->SetInput(input);
  m_GTPlane->SetInput(input);

}
//----------------------------------------------------------------------------
void medGizmoCrossTranslate::SendTransformMatrixFromGui(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    // send matrix to be postmultiplied to listener
    // [NewAbsPose] = [M]*[OldAbsPose] => [M] = [NewAbsPose][OldAbsPose]

    // build objects
    mafSmartPointer<mafMatrix> M;
    mafMatrix invOldAbsPose;
    mafSmartPointer<mafMatrix> newAbsPose;

    // incoming matrix is a translation matrix
    newAbsPose->DeepCopy(e->GetMatrix()); // abs position from gui
    // copy rotation part from OldAbsPose into NewAbsPose
    mafTransform::CopyRotation(*this->GetAbsPose(), *newAbsPose.GetPointer()); // abs orientation from old pose
    invOldAbsPose.DeepCopy(this->GetAbsPose());
    invOldAbsPose.Invert();
    mafMatrix::Multiply4x4(*newAbsPose.GetPointer(), invOldAbsPose, *M.GetPointer());
    // update gizmo abs pose
    this->SetAbsPose(newAbsPose, m_InputVME->GetTimeStamp());
    // send transfrom to postmultiply to the listener. Events is sent as a transform event
    SendTransformMatrix(M, ID_TRANSFORM, mafInteractorGenericMouse::MOUSE_MOVE);
  }
}
//----------------------------------------------------------------------------  
void medGizmoCrossTranslate::SetAbsPose(mafMatrix *absPose, mafTimeStamp ts)
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafMatrix> tmpMatr;
  tmpMatr->DeepCopy(absPose);
  tmpMatr->SetTimeStamp(ts);
  
  m_GTPlane->SetAbsPose(tmpMatr);
  m_GTAxis0->SetAbsPose(tmpMatr);
  m_GTAxis1->SetAbsPose(tmpMatr);

  if (m_BuildGUI) m_GuiGizmoTranslate->SetAbsPosition(tmpMatr);

}
//----------------------------------------------------------------------------
void medGizmoCrossTranslate::SetRefSys(mafVME *refSys)
//----------------------------------------------------------------------------
{
  assert(m_InputVME);  
  if (m_BuildGUI) assert(m_GuiGizmoTranslate);

  m_RefSysVME = refSys;
  SetAbsPose(m_RefSysVME->GetOutput()->GetAbsMatrix());
  if (m_RefSysVME == m_InputVME)
  {
    SetModalityToLocal();
    // if the gizmo is visible set the widgets visibility to true
    // if the ref-sys is local
    if (m_Visibility == true)
    {
      if (m_BuildGUI) m_GuiGizmoTranslate->EnableWidgets(true);
    }
  }
  else
  {
    SetModalityToGlobal();

    // if the gizmo is visible set the widgets visibility to false
    // if the ref-sys is global since this ref-sys cannot be changed
    if (m_Visibility == true)
    {
      if (m_BuildGUI) m_GuiGizmoTranslate->EnableWidgets(false);
    }
  }
}
//----------------------------------------------------------------------------
void medGizmoCrossTranslate::SetConstraintModality(int constrainModality)
//----------------------------------------------------------------------------
{
	
  if (m_Axis == X)
  {
	  m_GTAxis0->SetConstraintModality(Y,constrainModality);
	  m_GTAxis1->SetConstraintModality(Z, constrainModality);
	  m_GTPlane->SetConstraintModality(m_Axis,constrainModality);
  }
  else if (m_Axis == Y)
  {
	  m_GTAxis0->SetConstraintModality(X,constrainModality);
	  m_GTAxis1->SetConstraintModality(Z, constrainModality);
	  m_GTPlane->SetConstraintModality(m_Axis,constrainModality);
  }
  else if (m_Axis == Z)
  {
	  m_GTAxis0->SetConstraintModality(X,constrainModality);
	  m_GTAxis1->SetConstraintModality(Y, constrainModality);
	  m_GTPlane->SetConstraintModality(m_Axis,constrainModality);
  }

  m_ConstraintModality = constrainModality;
}
//----------------------------------------------------------------------------
void medGizmoCrossTranslate::SetStep(double step)
//----------------------------------------------------------------------------
{
  if (m_Axis == X)
  {
	  m_GTAxis0->SetStep(Y,step);
	  m_GTAxis1->SetStep(Z,step);
	  m_GTPlane->SetStep(m_Axis,step);
  }
  else if (m_Axis == Y)
  {
	  m_GTAxis0->SetStep(X,step);
	  m_GTAxis1->SetStep(Z,step);
	  m_GTPlane->SetStep(m_Axis,step);
  }
  else if (m_Axis == Z)
  {
	  m_GTAxis0->SetStep(X,step);
	  m_GTAxis1->SetStep(Y,step);
	  m_GTPlane->SetStep(m_Axis,step);
  }

  m_Step = step;
}

int medGizmoCrossTranslate::GetConstraintModality()
{
  return m_ConstraintModality;
}

int medGizmoCrossTranslate::GetStep()
{
  return m_Step;
}

mafVME* medGizmoCrossTranslate::GetRefSys()
{
  return m_RefSysVME;
}
