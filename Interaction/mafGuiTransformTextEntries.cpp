/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGuiTransformTextEntries.cpp,v $
  Language:  C++
  Date:      $Date: 2006-03-17 11:17:40 $
  Version:   $Revision: 1.4 $
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


#include "mafGuiTransformTextEntries.h"
#include "mafDecl.h"
#include "mafSmartPointer.h"
#include "mafTransformFrame.h"

#include "mmgGui.h"
#include "mmgButton.h"

#include "mmiGenericMouse.h"
#include "mmiCompositorMouse.h"

#include "mafMatrix.h"
#include "mafTransform.h"
#include "mafVME.h"
#include "mafVMEOutput.h"

#include "vtkMatrix4x4.h"

//----------------------------------------------------------------------------
mafGuiTransformTextEntries::mafGuiTransformTextEntries(mafVME *input, mafObserver *listener)
//----------------------------------------------------------------------------
{
  assert(input);

  CurrentTime = -1;
  m_Listener = listener;
  InputVME = input;
  m_Gui = NULL;
  
  RefSysVME = InputVME;

  Position[0] = Position[1] = Position[2] = 0;
  Orientation[0] = Orientation[1] = Orientation[2] = 0;
  Scaling[0] = Scaling[1] = Scaling[2] = 1; 

  CurrentTime = InputVME->GetTimeStamp();

  CreateGui();
  SetAbsPose(InputVME->GetOutput()->GetAbsMatrix());
}
//----------------------------------------------------------------------------
mafGuiTransformTextEntries::~mafGuiTransformTextEntries() 
//----------------------------------------------------------------------------
{   
  
}

//----------------------------------------------------------------------------
void mafGuiTransformTextEntries::CreateGui()
//----------------------------------------------------------------------------
{
  m_Gui = new mmgGui(this);
  m_Gui->SetListener(this);

  m_Gui->Divider(2);
  m_Gui->Label("vme pose", true);
  m_Gui->Double(ID_TRANSLATE_X, "Translate X", &Position[0]);
  m_Gui->Double(ID_TRANSLATE_Y, "Translate Y", &Position[1]);
  m_Gui->Double(ID_TRANSLATE_Z, "Translate Z", &Position[2]);
  m_Gui->Double(ID_ROTATE_X, "Rotate X", &Orientation[0]);
  m_Gui->Double(ID_ROTATE_Y, "Rotate Y", &Orientation[1]);
  m_Gui->Double(ID_ROTATE_Z, "Rotate Z", &Orientation[2]);
  m_Gui->Double(ID_SCALE_X, "Scale X", &Scaling[0], 0);
  m_Gui->Double(ID_SCALE_Y, "Scale Y", &Scaling[1], 0);
  m_Gui->Double(ID_SCALE_Z, "Scale Z", &Scaling[2], 0);

  m_Gui->Update();
}

//----------------------------------------------------------------------------
void mafGuiTransformTextEntries::OnEvent(mafEventBase *maf_event)
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
        TextEntriesChanged();
      }
      break;

      default:
        mafEventMacro(*e);
      break;
    }
  }
}

//----------------------------------------------------------------------------
void mafGuiTransformTextEntries::EnableWidgets(bool enable)
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
void mafGuiTransformTextEntries::Reset()
//----------------------------------------------------------------------------
{
  SetRefSys(InputVME);
  SetAbsPose(InputVME->GetOutput()->GetAbsMatrix());
  Scaling[0] = Scaling[1] = Scaling[2] = 1;
  m_Gui->Update();
}

//----------------------------------------------------------------------------
void mafGuiTransformTextEntries::RefSysVmeChanged()
//----------------------------------------------------------------------------
{
  this->SetAbsPose(InputVME->GetOutput()->GetAbsMatrix());
}

//----------------------------------------------------------------------------
void mafGuiTransformTextEntries::TextEntriesChanged()
//----------------------------------------------------------------------------
{
  // build the matrix to be applied to vme:

  /*
  - scale
  - rotate around Y, than X, then Z
  - translate 
  */

  mafSmartPointer<mafTransform> tran;
  tran->Scale(Scaling[0], Scaling[1], Scaling[2],POST_MULTIPLY);
  tran->RotateY(Orientation[1], POST_MULTIPLY);
  tran->RotateX(Orientation[0], POST_MULTIPLY);
  tran->RotateZ(Orientation[2], POST_MULTIPLY);
  tran->SetPosition(Position);

  // premultiply to ref sys abs matrix
  tran->Concatenate(RefSysVME->GetOutput()->GetAbsTransform(), POST_MULTIPLY);
  InputVME->SetAbsMatrix(tran->GetMatrix(), CurrentTime);
  
  this->SetAbsPose(InputVME->GetOutput()->GetAbsMatrix());
  
  // notify the listener about the new abs pose
  mafEvent e2s;
  e2s.SetSender(this);
  e2s.SetMatrix(tran->GetMatrixPointer());
  e2s.SetId(ID_TRANSFORM);
  mafEventMacro(e2s);
}

//----------------------------------------------------------------------------
void mafGuiTransformTextEntries::SetAbsPose(mafMatrix* absPose, mafTimeStamp timeStamp)
//----------------------------------------------------------------------------
{
  // express absPose in RefSysVME refsys
  mafTransformFrame *mflTr = mafTransformFrame::New();
  mflTr->SetInput(absPose);
  mflTr->SetTargetFrame(RefSysVME->GetOutput()->GetAbsMatrix());
  mflTr->Update();

  
  // update gui with new pose: Position, Orientation, Scale
  mafTransform::GetPosition(mflTr->GetMatrix(), Position);
  mafTransform::GetOrientation(mflTr->GetMatrix(), Orientation);
  mafTransform::GetScale(mflTr->GetMatrix(), Scaling);

  m_Gui->Update();
}
