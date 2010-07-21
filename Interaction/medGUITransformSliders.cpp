/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medGUITransformSliders.cpp,v $
Language:  C++
Date:      $Date: 2010-07-21 13:04:14 $
Version:   $Revision: 1.1.2.4 $
Authors:   Eleonora Mambrini
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


#include "medGUITransformSliders.h"
#include "mafDecl.h"
#include "mafSmartPointer.h"
#include "mafTransformFrame.h"

#include "mafGUI.h"
#include "mafGUIButton.h"
#include "mafGUIFloatSlider.h"

#include "mafInteractorGenericMouse.h"
#include "mafInteractorCompositorMouse.h"

#include "mafMatrix.h"
#include "mafTransform.h"
#include "mafVME.h"
#include "mafVMEOutput.h"

#include "vtkMatrix4x4.h"

//----------------------------------------------------------------------------
medGUITransformSliders::medGUITransformSliders(mafVME *input, double translationRange[6], mafObserver *listener /* = NULL */, bool enableScaling /* = true */,bool testMode /* = false */)
//----------------------------------------------------------------------------
{
  assert(input);

  m_EnableScaling = enableScaling;
  m_CurrentTime = -1;
  m_Listener = listener;
  m_InputVME = input;
  m_Gui = NULL;
  m_TestMode = testMode;

  m_RefSysVME = m_InputVME;

  m_Position[0] = m_Position[1] = m_Position[2] = 0;
  m_Orientation[0] = m_Orientation[1] = m_Orientation[2] = 0;
  m_Scaling[0] = m_Scaling[1] = m_Scaling[2] = 1; 
  
  for(int i=0;i<6;i++)
    m_TranslationRange[i] = translationRange[i];

  m_CurrentTime = m_InputVME->GetTimeStamp();

  if (!m_TestMode)
  {
  	CreateGui();
  }
  SetAbsPose(m_InputVME->GetOutput()->GetAbsMatrix());

  mafNEW(m_OldAbsMatrix);
  m_OldAbsMatrix->DeepCopy(m_InputVME->GetOutput()->GetAbsMatrix());
}
//----------------------------------------------------------------------------
medGUITransformSliders::~medGUITransformSliders() 
//----------------------------------------------------------------------------
{
  mafDEL(m_OldAbsMatrix);
}

//----------------------------------------------------------------------------
void medGUITransformSliders::CreateGui()
//----------------------------------------------------------------------------
{
  m_Gui = new mafGUI(this);
  m_Gui->SetListener(this);

  m_Gui->Divider(2);
  m_Gui->Label("vme pose", true);
 
  m_Gui->FloatSlider(ID_TRANSLATE_X, "Translate X", &m_Position[0], m_TranslationRange[0], m_TranslationRange[1]);
  m_Gui->FloatSlider(ID_TRANSLATE_Y, "Translate Y", &m_Position[1], m_TranslationRange[2], m_TranslationRange[3]);
  m_Gui->FloatSlider(ID_TRANSLATE_Z, "Translate Z", &m_Position[2], m_TranslationRange[4], m_TranslationRange[5]);
  m_Gui->FloatSlider(ID_ROTATE_X, "Rotate X", &m_Orientation[0], -90, 90);
  m_Gui->FloatSlider(ID_ROTATE_Y, "Rotate Y", &m_Orientation[1], -90, 90);
  m_Gui->FloatSlider(ID_ROTATE_Z, "Rotate Z", &m_Orientation[2], -90, 90);

  if (m_EnableScaling == true)
  {
    m_Gui->Double(ID_SCALE_X, "Scale X", &m_Scaling[0], 0);
    m_Gui->Double(ID_SCALE_Y, "Scale Y", &m_Scaling[1], 0);
    m_Gui->Double(ID_SCALE_Z, "Scale Z", &m_Scaling[2], 0);
  }
  else
  {
    mafLogMessage("scaling not enabled, not building scaling gui");
  }

  m_Gui->Divider();
  m_Gui->Button(ID_RESET, "Reset", "Reset");

  m_Gui->Divider();

  m_Gui->Update();
}

//----------------------------------------------------------------------------
void medGUITransformSliders::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
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
        SlidersValuesChanged();
      }
      break;
    case ID_RESET:
      {
        Reset();
        SlidersValuesChanged();
      }
      break;

    default:
      mafEventMacro(*e);
      break;
    }
  }
}

//----------------------------------------------------------------------------
void medGUITransformSliders::EnableWidgets(bool enable)
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
void medGUITransformSliders::Reset()
//----------------------------------------------------------------------------
{
  m_InputVME->SetAbsMatrix(*m_OldAbsMatrix);
  SetAbsPose(m_InputVME->GetOutput()->GetAbsMatrix());
  m_Scaling[0] = m_Scaling[1] = m_Scaling[2] = 1;
  if (!m_TestMode)
  {
  	m_Gui->Update();
  }
}

//----------------------------------------------------------------------------
void medGUITransformSliders::RefSysVmeChanged()
//----------------------------------------------------------------------------
{
  this->SetAbsPose(m_InputVME->GetOutput()->GetAbsMatrix());
}

//----------------------------------------------------------------------------
void medGUITransformSliders::SlidersValuesChanged()
//----------------------------------------------------------------------------
{
  // build the matrix to be applied to vme:

  /*
  - scale
  - rotate around Y, than X, then Z
  - translate 
  */

  mafSmartPointer<mafTransform> tran;
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
  mafEvent e2s;
  e2s.SetSender(this);
  e2s.SetMatrix(tran->GetMatrixPointer());
  e2s.SetId(ID_TRANSFORM);
  mafEventMacro(e2s);
}
//----------------------------------------------------------------------------
void medGUITransformSliders::SetAbsPose(mafMatrix* absPose, mafTimeStamp timeStamp)
//----------------------------------------------------------------------------
{
  // express absPose in RefSysVME refsys
  mafTransformFrame *mflTr = mafTransformFrame::New();
  mflTr->SetInput(absPose);
  mflTr->SetTargetFrame(m_RefSysVME->GetOutput()->GetAbsMatrix());
  mflTr->Update();


  // update gui with new pose: Position, Orientation, Scale
  mafTransform::GetPosition(mflTr->GetMatrix(), m_Position);
  mafTransform::GetOrientation(mflTr->GetMatrix(), m_Orientation);
  mafTransform::GetScale(mflTr->GetMatrix(), m_Scaling);

  if (!m_TestMode)
  {
  	m_Gui->Update();
  }

  mafDEL(mflTr);
}
//----------------------------------------------------------------------------
void medGUITransformSliders::GetPosition(double pos[3])
//----------------------------------------------------------------------------
{
  pos[0] = m_Position[0];
  pos[1] = m_Position[1];
  pos[2] = m_Position[2];
}
//----------------------------------------------------------------------------
void medGUITransformSliders::GetOrientation(double orientation[3])
//----------------------------------------------------------------------------
{
  orientation[0] = m_Orientation[0];
  orientation[1] = m_Orientation[1];
  orientation[2] = m_Orientation[2];
}
//----------------------------------------------------------------------------
void medGUITransformSliders::GetScaling(double scaling[3])
//----------------------------------------------------------------------------
{
  scaling[0] = m_Scaling[0];
  scaling[1] = m_Scaling[1];
  scaling[2] = m_Scaling[2];
}
