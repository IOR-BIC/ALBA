/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUITransformTextEntries
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


#include "albaGUITransformTextEntries.h"
#include "albaDecl.h"
#include "albaSmartPointer.h"
#include "albaTransformFrame.h"

#include "albaGUI.h"
#include "albaGUIButton.h"

#include "albaInteractorGenericMouse.h"
#include "albaInteractorCompositorMouse.h"

#include "albaMatrix.h"
#include "albaTransform.h"
#include "albaVME.h"
#include "albaVMEOutput.h"

#include "vtkMatrix4x4.h"

//----------------------------------------------------------------------------
albaGUITransformTextEntries::albaGUITransformTextEntries(albaVME *input, albaObserver *listener, bool enableScaling, bool testMode)
//----------------------------------------------------------------------------
{
  assert(input);

  m_EnableScaling = enableScaling;
  m_CurrentTime = -1;
  m_Listener = listener;
  m_InputVME = input;
  m_Gui = NULL;
  
  m_RefSysVME = m_InputVME;

  m_Position[0] = m_Position[1] = m_Position[2] = 0;
  m_Orientation[0] = m_Orientation[1] = m_Orientation[2] = 0;
  m_Scaling[0] = m_Scaling[1] = m_Scaling[2] = 1; 

  m_CurrentTime = m_InputVME->GetTimeStamp();

  m_TestMode = testMode;

  if (m_TestMode == false)
  {
    CreateGui();
  }

  SetAbsPose(m_InputVME->GetOutput()->GetAbsMatrix());
}
//----------------------------------------------------------------------------
albaGUITransformTextEntries::~albaGUITransformTextEntries() 
//----------------------------------------------------------------------------
{   
  
}

//----------------------------------------------------------------------------
void albaGUITransformTextEntries::CreateGui()
//----------------------------------------------------------------------------
{
  m_Gui = new albaGUI(this);
  m_Gui->SetListener(this);

  m_Gui->Divider(2);
  m_Gui->Label("vme pose", true);
  m_Gui->Double(ID_TRANSLATE_X, "Translate X", &m_Position[0]);
  m_Gui->Double(ID_TRANSLATE_Y, "Translate Y", &m_Position[1]);
  m_Gui->Double(ID_TRANSLATE_Z, "Translate Z", &m_Position[2]);
  m_Gui->Double(ID_ROTATE_X, "Rotate X", &m_Orientation[0]);
  m_Gui->Double(ID_ROTATE_Y, "Rotate Y", &m_Orientation[1]);
  m_Gui->Double(ID_ROTATE_Z, "Rotate Z", &m_Orientation[2]);

  if (m_EnableScaling == true)
  {
    m_Gui->Double(ID_SCALE_X, "Scale X", &m_Scaling[0], 0);
    m_Gui->Double(ID_SCALE_Y, "Scale Y", &m_Scaling[1], 0);
    m_Gui->Double(ID_SCALE_Z, "Scale Z", &m_Scaling[2], 0);
  }
  else
  {
    albaLogMessage("scaling not enabled, not building scaling gui");
  }

  m_Gui->Divider();

  m_Gui->Update();
}

//----------------------------------------------------------------------------
void albaGUITransformTextEntries::OnEvent(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{
  if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
  {
    switch(e->GetId())
    {
      case ID_TRANSLATE_X:
      case ID_TRANSLATE_Y:
      case ID_TRANSLATE_Z:
      case ID_ROTATE_X:
      case ID_ROTATE_Y:
      case ID_ROTATE_Z:
      case ID_SCALE_X:
      case ID_SCALE_Y:
      case ID_SCALE_Z:
      {
        TextEntriesChanged();
      }
      break;

      default:
        albaEventMacro(*e);
      break;
    }
  }
}

//----------------------------------------------------------------------------
void albaGUITransformTextEntries::EnableWidgets(bool enable)
//----------------------------------------------------------------------------
{
  m_Gui->Enable(ID_TRANSLATE_X, enable);
  m_Gui->Enable(ID_TRANSLATE_Y, enable);
  m_Gui->Enable(ID_TRANSLATE_Z, enable);
  m_Gui->Enable(ID_ROTATE_X, enable);
  m_Gui->Enable(ID_ROTATE_Y, enable);
  m_Gui->Enable(ID_ROTATE_Z, enable);
  m_Gui->Enable(ID_SCALE_X, enable);
  m_Gui->Enable(ID_SCALE_Y, enable);
  m_Gui->Enable(ID_SCALE_Z, enable);
}

//----------------------------------------------------------------------------
void albaGUITransformTextEntries::Reset()
//----------------------------------------------------------------------------
{
  SetRefSys(m_InputVME);
  SetAbsPose(m_InputVME->GetOutput()->GetAbsMatrix());
  m_Scaling[0] = m_Scaling[1] = m_Scaling[2] = 1;
  m_Gui->Update();
}

//----------------------------------------------------------------------------
void albaGUITransformTextEntries::RefSysVmeChanged()
//----------------------------------------------------------------------------
{
  this->SetAbsPose(m_InputVME->GetOutput()->GetAbsMatrix());
}

//----------------------------------------------------------------------------
void albaGUITransformTextEntries::TextEntriesChanged()
//----------------------------------------------------------------------------
{
  // build the matrix to be applied to vme:

  /*
  - scale
  - rotate around Y, than X, then Z
  - translate 
  */

  albaSmartPointer<albaTransform> tran;
  tran->Scale(m_Scaling[0], m_Scaling[1], m_Scaling[2],POST_MULTIPLY);
  tran->RotateY(m_Orientation[1], POST_MULTIPLY);
  tran->RotateX(m_Orientation[0], POST_MULTIPLY);
  tran->RotateZ(m_Orientation[2], POST_MULTIPLY);
  tran->SetPosition(m_Position);

  // premultiply to ref sys abs matrix
  tran->Concatenate(m_RefSysVME->GetOutput()->GetAbsTransform(), POST_MULTIPLY);
  m_InputVME->SetAbsMatrix(tran->GetMatrix(), m_CurrentTime);
  
  this->SetAbsPose(m_InputVME->GetOutput()->GetAbsMatrix());
  
  // notify the listener about the new abs pose
  albaEvent e2s;
  e2s.SetSender(this);
  e2s.SetMatrix(tran->GetMatrixPointer());
  e2s.SetId(ID_TRANSFORM);
  albaEventMacro(e2s);
}

#define TOLERANCE 1.0e-02
//----------------------------------------------------------------------------
void albaGUITransformTextEntries::SetAbsPose(albaMatrix* absPose, albaTimeStamp timeStamp)
//----------------------------------------------------------------------------
{
  // express absPose in RefSysVME refsys
  albaTransformFrame *mflTr = albaTransformFrame::New();
  mflTr->SetInput(absPose);
  mflTr->SetTargetFrame(m_RefSysVME->GetOutput()->GetAbsMatrix());
  mflTr->Update();
  
  // update gui with new pose: Position, Orientation, Scale
  albaTransform::GetPosition(mflTr->GetMatrix(), m_Position);
  albaTransform::GetOrientation(mflTr->GetMatrix(), m_Orientation);
  albaTransform::GetScale(mflTr->GetMatrix(), m_Scaling);

  // bug #2754: Text boxes show a bad approxiamation of the inserted value 
  // if the Position or/and Orientation is near to zero but not zero, 
  // we approximate to zero the new Position or/and Orientation
  for (int i=0; i<3; i++)
  {
    if (m_Position[i]!=0.0 && (m_Position[i] < TOLERANCE && m_Position[i] > - TOLERANCE))
      m_Position[i] = 0.0;
    
    if (m_Orientation[i]!=0.0 && (m_Orientation[i] < TOLERANCE && m_Orientation[i] > - TOLERANCE))
      m_Orientation[i] = 0.0;  
  }

  if (m_TestMode == false)
  {
    assert(m_Gui);
    m_Gui->Update();
  }

  albaDEL(mflTr);
}
