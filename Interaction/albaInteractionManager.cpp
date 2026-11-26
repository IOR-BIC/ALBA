/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaInteractionManager
 Authors: Marco Petrone
 
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

#include "albaInteractionManager.h"
#include "albaDecl.h"
#include "albaEventBase.h"
#include "albaView.h"
#include "albaPositionalEventRouter.h"
#include "albaToolFlyTo.h"
#include "albaServiceLocator.h"

//------------------------------------------------------------------------------
albaCxxTypeMacro(albaInteractionManager);

//------------------------------------------------------------------------------
albaInteractionManager::albaInteractionManager()
{
  m_FlyToTool = NULL;

  albaNEW(m_PositionalEventRouter);
  m_PositionalEventRouter->SetName("PositionalEventRouter");
  m_PositionalEventRouter->SetListener(this);
  m_PositionalEventRouter->Initialize();

  // the render windows send the mouse events to the registered router
  albaServiceLocator::SetPositionalEventRouter(m_PositionalEventRouter);
}

//------------------------------------------------------------------------------
albaInteractionManager::~albaInteractionManager()
{
  if (m_FlyToTool)
    m_PositionalEventRouter->PopTool(m_FlyToTool);
  albaDEL(m_FlyToTool);
  if (albaServiceLocator::GetPositionalEventRouter() == m_PositionalEventRouter)
    albaServiceLocator::SetPositionalEventRouter(NULL);
  albaDEL(m_PositionalEventRouter);
}

//----------------------------------------------------------------------------
void albaInteractionManager::OnEvent(albaEventBase *event)
{
  InvokeEvent(event);
}

//----------------------------------------------------------------------------
bool albaInteractionManager::OnKey(int keyCode, bool pressed)
{
  return m_PositionalEventRouter->OnKey(keyCode, pressed);
}

//----------------------------------------------------------------------------
void albaInteractionManager::SetEventTracing(bool enable)
{
  m_PositionalEventRouter->SetEventTracing(enable);
}

//----------------------------------------------------------------------------
void albaInteractionManager::CameraFlyToMode()
{
  if (m_FlyToTool == NULL)
    albaNEW(m_FlyToTool);
  m_PositionalEventRouter->PushTool(m_FlyToTool);
}

//------------------------------------------------------------------------------
void albaInteractionManager::EnableSelect(bool enable)
{
  m_PositionalEventRouter->EnableSelect(enable);
}