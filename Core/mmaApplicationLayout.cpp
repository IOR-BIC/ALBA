/*=========================================================================

 Program: MAF2
 Module: mmaApplicationLayout
 Authors: Paolo Quadrani
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "mmaApplicationLayout.h"
#include "mafDecl.h"
#include "mafEvent.h"
#include "mafView.h"

#include "mafStorageElement.h"
#include "mafIndent.h"
#include "mafNode.h"
#include "mafSceneGraph.h"
#include "mafSceneNode.h"
#include "mafRWIBase.h"

#include "vtkCamera.h"
#include <iterator>

//----------------------------------------------------------------------------
mafCxxTypeMacro(mmaApplicationLayout)
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
void mmaApplicationLayout::DeepCopy(const mafAttribute *a)
//-------------------------------------------------------------------------
{ 
  Superclass::DeepCopy(a);
  mmaApplicationLayout *src_layout = (mmaApplicationLayout *)a;
}
//----------------------------------------------------------------------------
bool mmaApplicationLayout::Equals(const mafAttribute *a)
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
void mmaApplicationLayout::SetInterfaceElementVisibility(mafString panel_name, int visibility)
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
int mmaApplicationLayout::InternalStore(mafStorageElement *parent)
//-----------------------------------------------------------------------
{  
  if (Superclass::InternalStore(parent)==MAF_OK)
  {
    parent->StoreInteger("APPLICATION_MAXIMIZED", m_AppMaximized);
    parent->StoreVectorN("APPLICATION_SIZE",m_AppSize,2);
    parent->StoreVectorN("APPLICATION_POSITION",m_AppPosition,2);

    parent->StoreInteger("TOOLBAR_VISIBILITY", m_ToolBarVisibility);
    parent->StoreInteger("SIDEBAR_VISIBILITY", m_SideBarVisibility);
    parent->StoreInteger("LOGBAR_VISIBILITY", m_LogBarVisibility);

    return MAF_OK;
  }
  return MAF_ERROR;
}
//----------------------------------------------------------------------------
int mmaApplicationLayout::InternalRestore(mafStorageElement *node)
//----------------------------------------------------------------------------
{
  if (Superclass::InternalRestore(node) == MAF_OK)
  {
    node->RestoreInteger("APPLICATION_MAXIMIZED", m_AppMaximized);
    node->RestoreVectorN("APPLICATION_SIZE",m_AppSize,2);
    node->RestoreVectorN("APPLICATION_POSITION",m_AppPosition,2);

    node->RestoreInteger("TOOLBAR_VISIBILITY", m_ToolBarVisibility);
    node->RestoreInteger("SIDEBAR_VISIBILITY", m_SideBarVisibility);
    node->RestoreInteger("LOGBAR_VISIBILITY", m_LogBarVisibility);

    return MAF_OK;
  }
  return MAF_ERROR;
}
