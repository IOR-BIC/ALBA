/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: mmaApplicationLayout
 Authors: Paolo Quadrani
 
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

#include "mmaApplicationLayout.h"
#include "albaDecl.h"
#include "albaEvent.h"
#include "albaView.h"

#include "albaStorageElement.h"
#include "albaIndent.h"
#include "albaVME.h"
#include "albaSceneGraph.h"
#include "albaSceneNode.h"
#include "albaRWIBase.h"

#include "vtkCamera.h"
#include <iterator>

//----------------------------------------------------------------------------
albaCxxTypeMacro(mmaApplicationLayout)
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mmaApplicationLayout::mmaApplicationLayout()
//----------------------------------------------------------------------------
{  
  m_Name = "ApplicationLayout";
  m_AppMaximized = 0;
  m_AppPosition[0] = 0;
  m_AppPosition[1] = 0;
  m_AppSize[0] = 800;
  m_AppSize[1] = 600;
}
//----------------------------------------------------------------------------
mmaApplicationLayout::~mmaApplicationLayout()
//----------------------------------------------------------------------------
{
}
//-------------------------------------------------------------------------
void mmaApplicationLayout::DeepCopy(const albaAttribute *a)
//-------------------------------------------------------------------------
{ 
  Superclass::DeepCopy(a);
  mmaApplicationLayout *src_layout = (mmaApplicationLayout *)a;
}
//----------------------------------------------------------------------------
bool mmaApplicationLayout::Equals(const albaAttribute *a)
//----------------------------------------------------------------------------
{
  if (Superclass::Equals(a))
	{
    return true;
  }
  return false;
}

//-----------------------------------------------------------------------
void mmaApplicationLayout::GetApplicationInfo(int &maximized, int pos[2], int size[2])
//-----------------------------------------------------------------------
{
  maximized = m_AppMaximized;
  pos[0] = m_AppPosition[0];
  pos[1] = m_AppPosition[1];
  size[0] = m_AppSize[0];
  size[1] = m_AppSize[1];
}
//-----------------------------------------------------------------------
void mmaApplicationLayout::SetApplicationInfo(int maximized, int pos[2], int size[2])
//-----------------------------------------------------------------------
{
  m_AppMaximized = maximized;
  m_AppPosition[0] = pos[0];
  m_AppPosition[1] = pos[1];
  m_AppSize[0] = size[0];
  m_AppSize[1] = size[1];
}
//-----------------------------------------------------------------------
void mmaApplicationLayout::SetInterfaceElementVisibility(albaString panel_name, int visibility)
//-----------------------------------------------------------------------
{
  if (panel_name.Equals("toolbar"))
  {
    m_ToolBarVisibility = visibility;
  }
  else if (panel_name.Equals("sidebar"))
  {
    m_SideBarVisibility = visibility;
  }
  else if (panel_name.Equals("logbar"))
  {
    m_LogBarVisibility = visibility;
  }
}
//-----------------------------------------------------------------------
int mmaApplicationLayout::InternalStore(albaStorageElement *parent)
//-----------------------------------------------------------------------
{  
  if (Superclass::InternalStore(parent)==ALBA_OK)
  {
    parent->StoreInteger("APPLICATION_MAXIMIZED", m_AppMaximized);
    parent->StoreVectorN("APPLICATION_SIZE",m_AppSize,2);
    parent->StoreVectorN("APPLICATION_POSITION",m_AppPosition,2);

    parent->StoreInteger("TOOLBAR_VISIBILITY", m_ToolBarVisibility);
    parent->StoreInteger("SIDEBAR_VISIBILITY", m_SideBarVisibility);
    parent->StoreInteger("LOGBAR_VISIBILITY", m_LogBarVisibility);

    return ALBA_OK;
  }
  return ALBA_ERROR;
}
//----------------------------------------------------------------------------
int mmaApplicationLayout::InternalRestore(albaStorageElement *node)
//----------------------------------------------------------------------------
{
  if (Superclass::InternalRestore(node) == ALBA_OK)
  {
    node->RestoreInteger("APPLICATION_MAXIMIZED", m_AppMaximized);
    node->RestoreVectorN("APPLICATION_SIZE",m_AppSize,2);
    node->RestoreVectorN("APPLICATION_POSITION",m_AppPosition,2);

    node->RestoreInteger("TOOLBAR_VISIBILITY", m_ToolBarVisibility);
    node->RestoreInteger("SIDEBAR_VISIBILITY", m_SideBarVisibility);
    node->RestoreInteger("LOGBAR_VISIBILITY", m_LogBarVisibility);

    return ALBA_OK;
  }
  return ALBA_ERROR;
}
