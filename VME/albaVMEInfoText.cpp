/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEInfoText
 Authors: Fedor Moiseev
 
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

#include "albaVMEInfoText.h"
#include "albaGUI.h"

#include "albaStorageElement.h"
#include "albaIndent.h"

#include "vtkALBASmartPointer.h"

#ifdef _MSC_FULL_VER
#pragma warning (disable: 4786)
#endif

//-------------------------------------------------------------------------
albaCxxTypeMacro(albaVMEInfoText)
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
albaVMEInfoText::albaVMEInfoText()
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
albaVMEInfoText::~albaVMEInfoText()
//-------------------------------------------------------------------------
{
}
//-------------------------------------------------------------------------
int albaVMEInfoText::DeepCopy(albaVME *a)
//-------------------------------------------------------------------------
{ 
  if (Superclass::DeepCopy(a) == ALBA_OK)
  {
    albaVMEInfoText *it = albaVMEInfoText::SafeDownCast(a);
    if(it == NULL)
      return ALBA_ERROR;
    m_Strings = it->m_Strings;
    return ALBA_OK;
  }  
  return ALBA_ERROR;
}
//-------------------------------------------------------------------------
bool albaVMEInfoText::Equals(albaVME *vme)
//-------------------------------------------------------------------------
{
  if (Superclass::Equals(vme))
  {
    albaVMEInfoText *it = albaVMEInfoText::SafeDownCast(vme);
    if(it == NULL)
      return false;
    return m_Strings == it->m_Strings;
  }
  return false;
}

//-----------------------------------------------------------------------
int albaVMEInfoText::InternalStore(albaStorageElement *parent)
//-----------------------------------------------------------------------
{  
  if (Superclass::InternalStore(parent)==ALBA_OK)
  {
    for(int i = 0; i < 3; i++)
    {
      char txtname[256];
      sprintf(txtname, "Label%ld", i);
      if(parent->StoreText(txtname, m_PosLabels[i]) != ALBA_OK)
        return ALBA_ERROR;
    }
    for(int i = 0; i < 3; i++)
    {
      char txtname[256];
      sprintf(txtname, "ShowLabel%ld", i);
      if(parent->StoreInteger(txtname, m_PosShow[i] ? 1 : 0) != ALBA_OK)
        return ALBA_ERROR;
    }
    int strSz = m_Strings.size();
    parent->StoreInteger("NumberOfStrings", strSz);
    for(int i = 0; i < m_Strings.size(); i++)
    {
      char txtname[256];
      sprintf(txtname, "String%ld", i);
      if(parent->StoreText(txtname, m_Strings[i]) != ALBA_OK)
        return ALBA_ERROR;
    }
    return ALBA_OK;
  }
  return ALBA_ERROR;
}

//-----------------------------------------------------------------------
int albaVMEInfoText::InternalRestore(albaStorageElement *node)
//-----------------------------------------------------------------------
{
  if (Superclass::InternalRestore(node)==ALBA_OK)
  {
    char txtname[256];
    for(int j = 0; j < 3; j++)
    {
      sprintf(txtname, "Label%ld", j);
      if(node->RestoreText(txtname, m_PosLabels[j]) != ALBA_OK)
        return ALBA_ERROR;
    }
    for(int j = 0; j < 3; j++)
    {
      sprintf(txtname, "ShowLabel%ld", j);
      int val;
      if(node->RestoreInteger(txtname, val) != ALBA_OK)
        return ALBA_ERROR;
      m_PosShow[j] = (val != 0);
    }

    albaString ReadStr;
    int       i = 0;
    sprintf(txtname, "String%ld", i);
    m_Strings.clear();

    int strSz = m_Strings.size();
    if(node->RestoreInteger("NumberOfStrings", strSz) == ALBA_OK)
    {
      for(int k = 0; k < strSz; k++) 
      {
        if(node->RestoreText(txtname,ReadStr) == ALBA_OK)
          m_Strings.push_back(ReadStr);
        i++;
        sprintf(txtname, "String%ld", i);
      }
    }
    else
    {
      while (node->RestoreText(txtname,ReadStr) == ALBA_OK)
      {
        m_Strings.push_back(ReadStr);
        i++;
        sprintf(txtname, "String%ld", i);
      }
    }
    return ALBA_OK;
  }
  return ALBA_ERROR;
}

//-------------------------------------------------------------------------
albaGUI* albaVMEInfoText::CreateGui()
//-------------------------------------------------------------------------
{

  albaID sub_id = -1;

  m_Gui = albaVME::CreateGui(); // Called to show info about vmes' type and name
  m_Gui->SetListener(this);
  m_Gui->Divider();
  for(int i = 0; i < m_Strings.size(); i++)
  {
    m_Gui->Label(&m_Strings[i]);
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
    m_Gui->Label("", &m_PositionText[i]);
  }

  m_Gui->Update();

  return m_Gui;
}
//-----------------------------------------------------------------------
void albaVMEInfoText::Print(std::ostream& os, const int tabs)
//-----------------------------------------------------------------------
{
  Superclass::Print(os,tabs);
  albaIndent indent(tabs);

  for(int i = 0; i < m_Strings.size(); i++)
    os << m_Strings[i] << endl;
}

//-------------------------------------------------------------------------
void albaVMEInfoText::SetTimeStamp(albaTimeStamp t)
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
albaVMEOutput *albaVMEInfoText::GetOutput()
//-------------------------------------------------------------------------
{
  // allocate the right type of output on demand
  if (m_Output==NULL)
  {
    SetOutput(albaVMEOutputNULL::New()); // create the output
  }
  return m_Output;
}

//-------------------------------------------------------------------------
void albaVMEInfoText::SetPosShow(bool show, int index)
//-------------------------------------------------------------------------
{
  assert(index >= 0 && index <= 2); 
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
//-------------------------------------------------------------------------
bool albaVMEInfoText::GetPosShow(int index)
//-------------------------------------------------------------------------
{
  assert(index >= 0 && index <= 2); 
  if(index < 0 || index > 2)
    return false; 
  return m_PosShow[index];
}

//-------------------------------------------------------------------------
void albaVMEInfoText::SetPosLabel(const albaString& label, int index)
//-------------------------------------------------------------------------
{
  assert(index >= 0 && index <= 2); 
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
//-------------------------------------------------------------------------
const char *albaVMEInfoText::GetPosLabel(int index)
//-------------------------------------------------------------------------
{
  assert(index >= 0 && index <= 2); 
  if(index < 0 || index > 2)
    return NULL; 
  return m_PosLabels[index].GetCStr();
}
//-------------------------------------------------------------------------
const char *albaVMEInfoText::GetPosText(int index)
//-------------------------------------------------------------------------
{
  assert(index >= 0 && index <= 2); 
  if(index < 0 || index > 2)
    return NULL; 
  return m_PositionText[index].GetCStr();
}
//-------------------------------------------------------------------------
char** albaVMEInfoText::GetIcon() 
//-------------------------------------------------------------------------
{
#include "albaVMEText.xpm"
  return albaVMEText_xpm;
}
