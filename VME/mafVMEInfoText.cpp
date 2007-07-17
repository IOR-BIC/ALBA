/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMEInfoText.cpp,v $
  Language:  C++
  Date:      $Date: 2007-07-17 14:24:49 $
  Version:   $Revision: 1.2 $
  Authors:   Fedor Moiseev
==========================================================================
  Copyright (c) 2001/2007 
  ULB - Universite Libre de Bruxelles (www.ulb.ac.be)
=========================================================================*/

#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "mafVMEInfoText.h"
#include "mmgGui.h"

#include "mafStorageElement.h"
#include "mafIndent.h"

#include "vtkMAFSmartPointer.h"

#ifdef _MSC_FULL_VER
#pragma warning (disable: 4786)
#endif

//-------------------------------------------------------------------------
mafCxxTypeMacro(mafVMEInfoText)
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
mafVMEInfoText::mafVMEInfoText()
//-------------------------------------------------------------------------
{
  m_PosShow[0]   = false;
  m_PosShow[1]   = false;
  m_PosShow[2]   = false;
  m_PosLabels[0] = "";
  m_PosLabels[1] = "";
  m_PosLabels[2] = "";
}

//-------------------------------------------------------------------------
mafVMEInfoText::~mafVMEInfoText()
//-------------------------------------------------------------------------
{
}
//-------------------------------------------------------------------------
int mafVMEInfoText::DeepCopy(mafNode *a)
//-------------------------------------------------------------------------
{ 
  if (Superclass::DeepCopy(a) == MAF_OK)
  {
    mafVMEInfoText *it = mafVMEInfoText::SafeDownCast(a);
    if(it == NULL)
      return MAF_ERROR;
    m_strings = it->m_strings;
    return MAF_OK;
  }  
  return MAF_ERROR;
}
//-------------------------------------------------------------------------
bool mafVMEInfoText::Equals(mafVME *vme)
//-------------------------------------------------------------------------
{
  if (Superclass::Equals(vme))
  {
    mafVMEInfoText *it = mafVMEInfoText::SafeDownCast(vme);
    if(it == NULL)
      return false;
    return m_strings == it->m_strings;
  }
  return false;
}

//-----------------------------------------------------------------------
int mafVMEInfoText::InternalStore(mafStorageElement *parent)
//-----------------------------------------------------------------------
{  
  if (Superclass::InternalStore(parent)==MAF_OK)
  {
    for(int i = 0; i < 3; i++)
    {
      char txtname[256];
      sprintf(txtname, "Label%ld", i);
      if(parent->StoreText(txtname, m_PosLabels[i]) != MAF_OK)
        return MAF_ERROR;
    }
    for(int i = 0; i < 3; i++)
    {
      char txtname[256];
      sprintf(txtname, "ShowLabel%ld", i);
      if(parent->StoreInteger(txtname, m_PosShow[i] ? 1 : 0) != MAF_OK)
        return MAF_ERROR;
    }
    for(int i = 0; i < m_strings.size(); i++)
    {
      char txtname[256];
      sprintf(txtname, "String%ld", i);
      if(parent->StoreText(txtname, m_strings[i]) != MAF_OK)
        return MAF_ERROR;
    }
    return MAF_OK;
  }
  return MAF_ERROR;
}

//-----------------------------------------------------------------------
int mafVMEInfoText::InternalRestore(mafStorageElement *node)
//-----------------------------------------------------------------------
{
  if (Superclass::InternalRestore(node)==MAF_OK)
  {
    char txtname[256];
    for(int j = 0; j < 3; j++)
    {
      sprintf(txtname, "Label%ld", j);
      if(node->RestoreText(txtname, m_PosLabels[j]) != MAF_OK)
        return MAF_ERROR;
    }
    for(int j = 0; j < 3; j++)
    {
      sprintf(txtname, "ShowLabel%ld", j);
      int val;
      if(node->RestoreInteger(txtname, val) != MAF_OK)
        return MAF_ERROR;
      m_PosShow[j] = (val != 0);
    }

    mafString ReadStr;
    int       i = 0;
    sprintf(txtname, "String%ld", i);
    m_strings.clear();
    while (node->RestoreText("Transform",ReadStr) == MAF_OK)
    {
      m_strings.push_back(ReadStr);
      i++;
      sprintf(txtname, "String%ld", i);
    }
    return MAF_OK;
  }
  return MAF_ERROR;
}

//-------------------------------------------------------------------------
mmgGui* mafVMEInfoText::CreateGui()
//-------------------------------------------------------------------------
{

  mafID sub_id = -1;

  m_Gui = mafNode::CreateGui(); // Called to show info about vmes' type and name
  m_Gui->SetListener(this);
  m_Gui->Divider();
  for(int i = 0; i < m_strings.size(); i++)
  {
    m_Gui->Label(&m_strings[i]);
    m_Gui->Divider();
  }

  double xyz[3],rxyz[3];
  this->GetOutput()->GetAbsPose(xyz,rxyz);
  for(int i = 0; i < 3; i++)
  {
    if(m_PosShow[i])
      m_PositionText[i] = wxString::Format("%s%f",m_PosLabels[i].GetCStr(), xyz[i]);
    else
      m_PositionText[i] = "";
    m_Gui->Label(_(""), &m_PositionText[i]);
  }

  m_Gui->Update();

  return m_Gui;
}
//-----------------------------------------------------------------------
void mafVMEInfoText::Print(std::ostream& os, const int tabs)
//-----------------------------------------------------------------------
{
  Superclass::Print(os,tabs);
  mafIndent indent(tabs);

  for(int i = 0; i < m_strings.size(); i++)
    os << m_strings[i] << endl;
}

//-------------------------------------------------------------------------
void mafVMEInfoText::SetTimeStamp(mafTimeStamp t)
//-------------------------------------------------------------------------
{
  Superclass::SetTimeStamp(t);
  this->GetOutput()->Update();
  double xyz[3],rxyz[3];
  this->GetOutput()->GetAbsPose(xyz,rxyz);  
  for(int i = 0; i < 3; i++)
  {
    if(m_PosShow[i])
      m_PositionText[i] = wxString::Format("%s%f",m_PosLabels[i].GetCStr(), xyz[i]);
    else
      m_PositionText[i] = "";
  }
  if(m_Gui)
    m_Gui->Update();
}

//-------------------------------------------------------------------------
mafVMEOutput *mafVMEInfoText::GetOutput()
//-------------------------------------------------------------------------
{
  // allocate the right type of output on demand
  if (m_Output==NULL)
  {
    SetOutput(mafVMEOutputNULL::New()); // create the output
  }
  return m_Output;
}

void mafVMEInfoText::SetPosShow(bool show, int index)
{
  wxASSERT(index >= 0 && index <= 2); 
  if(index < 0 || index > 2)
    return; 
  m_PosShow[index] = show; 

  this->GetOutput()->Update();
  double xyz[3],rxyz[3];
  this->GetOutput()->GetAbsPose(xyz,rxyz);  
  if(m_PosShow[index])
    m_PositionText[index] = wxString::Format("%s%f",m_PosLabels[index].GetCStr(), xyz[index]);
  else
    m_PositionText[index] = "";

  if(m_Gui) 
    m_Gui->Update();
}
bool mafVMEInfoText::GetPosShow(int index)
{
  wxASSERT(index >= 0 && index <= 2); 
  if(index < 0 || index > 2)
    return false; 
  return m_PosShow[index];
}

void mafVMEInfoText::SetPosLabel(const mafString& label, int index)
{
  wxASSERT(index >= 0 && index <= 2); 
  if(index < 0 || index > 2)
    return; 
  m_PosLabels[index] = label; 

  this->GetOutput()->Update();
  double xyz[3],rxyz[3];
  this->GetOutput()->GetAbsPose(xyz,rxyz);  
  if(m_PosShow[index])
    m_PositionText[index] = wxString::Format("%s%f",m_PosLabels[index].GetCStr(), xyz[index]);
  else
    m_PositionText[index] = "";

  if(m_Gui) 
    m_Gui->Update();
}
const char *mafVMEInfoText::GetPosLabel(int index)
{
  wxASSERT(index >= 0 && index <= 2); 
  if(index < 0 || index > 2)
    return NULL; 
  return m_PosLabels[index].GetCStr();
}

//-------------------------------------------------------------------------
char** mafVMEInfoText::GetIcon() 
//-------------------------------------------------------------------------
{
#include "mafVMELandmark.xpm"
  return mafVMELandmark_xpm;
}
