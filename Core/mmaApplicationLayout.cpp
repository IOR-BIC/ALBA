/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmaApplicationLayout.cpp,v $
  Language:  C++
  Date:      $Date: 2006-11-24 16:05:25 $
  Version:   $Revision: 1.1 $
  Authors:   Paolo Quadrani
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
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

#include <iterator>

//----------------------------------------------------------------------------
mafCxxTypeMacro(mmaApplicationLayout)
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mmaApplicationLayout::mmaApplicationLayout()
//----------------------------------------------------------------------------
{  
  m_Name = "ApplicationLayout";
  m_LayoutName = "Default Name";
  m_AppMaximized = 0;
  m_AppPosition[0] = 0;
  m_AppPosition[1] = 0;
  m_AppSize[0] = 800;
  m_AppSize[1] = 600;
  ClearLayout();
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
  m_LayoutName = src_layout->m_LayoutName;
  m_LayoutViewList.clear();
  for (int i = 0; i < src_layout->m_LayoutViewList.size(); i++)
  {
    m_LayoutViewList.push_back(src_layout->m_LayoutViewList[i]);
  }
}
//----------------------------------------------------------------------------
bool mmaApplicationLayout::Equals(const mafAttribute *a)
//----------------------------------------------------------------------------
{
  if (Superclass::Equals(a))
  {
    mmaApplicationLayout *src_layout = (mmaApplicationLayout *)a;
    if (m_LayoutViewList.size() != src_layout->m_LayoutViewList.size())
      return false;
    if (m_LayoutName != src_layout->m_LayoutName)
      return false;
    std::vector<ViewLayoutInfo>::iterator it1 = m_LayoutViewList.begin();
    std::vector<ViewLayoutInfo>::iterator it2 = src_layout->m_LayoutViewList.begin();
    for (;it1 != m_LayoutViewList.end();it1++,it2++)
    {
      if ((*it1).m_Id != (*it2).m_Id                    ||
          (*it1).m_Mult != (*it2).m_Mult                ||
          (*it1).m_Label != (*it2).m_Label              ||
          (*it1).m_Maximized != (*it2).m_Maximized      ||
          (*it1).m_Position[0] != (*it2).m_Position[0]  ||
          (*it1).m_Position[1] != (*it2).m_Position[1]  ||
          (*it1).m_Size[0] != (*it2).m_Size[0]          ||
          (*it1).m_Size[1] != (*it2).m_Size[1])
      {
        return false;
      }
    }
    return true;
  }
  return false;
}
//-----------------------------------------------------------------------
void mmaApplicationLayout::SetLayoutName(const char *name)
//-----------------------------------------------------------------------
{
  m_LayoutName = name;
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
int mmaApplicationLayout::InternalStore(mafStorageElement *parent)
//-----------------------------------------------------------------------
{  
  if (Superclass::InternalStore(parent)==MAF_OK)
  {
    parent->StoreInteger("APPLICATION_MAXIMIZED", m_AppMaximized);
    parent->StoreVectorN("APPLICATION_SIZE",m_AppSize,2);
    parent->StoreVectorN("APPLICATION_POSITION",m_AppPosition,2);

    parent->StoreText("LAYOUT_NAME",m_LayoutName.GetCStr());
    mafString view_id;
    mafString view_mult;
    mafString view_label;
    mafString view_max;
    mafString view_size;
    mafString view_pos;
    ViewLayoutInfo info;
    int n = m_LayoutViewList.size();
    parent->StoreInteger("NUMBER_OF_VIEW", n);
    for (int i = 0; i < n; i++)
    {
      info = m_LayoutViewList[i];
      view_id = "VIEW_ID_";
      view_id << i;
      parent->StoreInteger(view_id, info.m_Id);
      view_mult = "VIEW_MULT_";
      view_mult << i;
      parent->StoreInteger(view_mult, info.m_Mult);
      view_label = "VIEW_LABEL_";
      view_label << i;
      parent->StoreText(view_label.GetCStr(), info.m_Label.GetCStr());
      view_max = "VIEW_MAXIMIZED_";
      view_max << i;
      parent->StoreInteger(view_max.GetCStr(),info.m_Maximized);
      view_size = "VIEW_SIZE_";
      view_size << i;
      view_pos = "VIEW_POS_";
      view_pos << i;
      parent->StoreVectorN(view_size.GetCStr(),info.m_Size,2);
      parent->StoreVectorN(view_pos.GetCStr(),info.m_Position,2);
    }
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

    node->RestoreText("LAYOUT_NAME",m_LayoutName);
    mafString view_id;
    mafString view_mult;
    mafString view_label;
    mafString view_max;
    mafString view_size;
    mafString view_pos;
    ViewLayoutInfo info;
    int n;
    node->RestoreInteger("NUMBER_OF_VIEW", n);
    for (int i = 0; i < n; i++)
    {
      view_id = "VIEW_ID_";
      view_id << i;
      node->RestoreInteger(view_id, info.m_Id);
      view_mult = "VIEW_MULT_";
      view_mult << i;
      node->RestoreInteger(view_mult, info.m_Mult);
      view_label = "VIEW_LABEL_";
      view_label << i;
      node->RestoreText(view_label.GetCStr(), info.m_Label);
      view_max = "VIEW_MAXIMIZED_";
      view_max << i;
      node->RestoreInteger(view_max, info.m_Maximized);
      view_size = "VIEW_SIZE_";
      view_size << i;
      view_pos = "VIEW_POS_";
      view_pos << i;
      node->RestoreVectorN(view_size.GetCStr(),info.m_Size,2);
      node->RestoreVectorN(view_pos.GetCStr(),info.m_Position,2);
      m_LayoutViewList.push_back(info);
    }
    return MAF_OK;
  }
  return MAF_ERROR;
}
//-----------------------------------------------------------------------
void mmaApplicationLayout::AddView(mafView *v)
//-----------------------------------------------------------------------
{
  ViewLayoutInfo info;
  info.m_Id = v->m_Id;
  info.m_Mult = v->m_Mult;
  info.m_Label = v->GetLabel().c_str();
  wxFrame *frame = v->GetFrame();
  info.m_Maximized = frame->IsMaximized();
  wxRect r = frame->GetRect();
  info.m_Position[0] = r.GetPosition().x;
  info.m_Position[1] = r.GetPosition().y;
  info.m_Size[0] = r.GetSize().GetWidth();
  info.m_Size[1] = r.GetSize().GetHeight();
  
  m_LayoutViewList.push_back(info);
}
//-----------------------------------------------------------------------
void mmaApplicationLayout::RemoveView(mafView *v)
//-----------------------------------------------------------------------
{
  for (int i = 0; i < m_LayoutViewList.size(); i++)
  {
    if (m_LayoutViewList[i].m_Id == v->m_Id && m_LayoutViewList[i].m_Mult == v->m_Mult)
    {
      m_LayoutViewList.erase(m_LayoutViewList.begin()+i);
      return;
    }
  }
}
//-----------------------------------------------------------------------
void mmaApplicationLayout::RemoveView(int view_id)
//-----------------------------------------------------------------------
{
  m_LayoutViewList.erase(m_LayoutViewList.begin()+view_id);
}
//-----------------------------------------------------------------------
void mmaApplicationLayout::ClearLayout()
//-----------------------------------------------------------------------
{
  m_LayoutViewList.clear();
  m_LayoutName = "Null Layout";
}
//-----------------------------------------------------------------------
void mmaApplicationLayout::Print(std::ostream& os, const int tabs) const
//-----------------------------------------------------------------------
{
  Superclass::Print(os,tabs);
  mafIndent indent(tabs);

  os << indent << "Number of stored views: " << m_LayoutViewList.size() << "\n";
  os << std::endl;
}
