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
albaVMEInfoText::albaVMEInfoText()
{
}

//-------------------------------------------------------------------------
albaVMEInfoText::~albaVMEInfoText()
{
}

//-------------------------------------------------------------------------
int albaVMEInfoText::DeepCopy(albaVME *a)
{ 
  if (Superclass::DeepCopy(a) == ALBA_OK)
  {
    albaVMEInfoText *it = albaVMEInfoText::SafeDownCast(a);
    if(it == NULL)
      return ALBA_ERROR;
    m_Label = it->m_Label;
    return ALBA_OK;
  }  
  return ALBA_ERROR;
}
//-------------------------------------------------------------------------
bool albaVMEInfoText::Equals(albaVME *vme)
{
  if (Superclass::Equals(vme))
  {
    albaVMEInfoText *it = albaVMEInfoText::SafeDownCast(vme);
    if(it == NULL)
      return false;
    return m_Label == it->m_Label;
  }
  return false;
}

//-----------------------------------------------------------------------
int albaVMEInfoText::InternalStore(albaStorageElement *parent)
{
	if (Superclass::InternalStore(parent) == ALBA_OK)
	{
		parent->StoreText("Label0", m_Label);
		parent->SetAttribute("Version", "2");
		return ALBA_OK;
	}
	else
		return ALBA_ERROR;
}

//-----------------------------------------------------------------------
int albaVMEInfoText::InternalRestore(albaStorageElement *node)
{
	if (Superclass::InternalRestore(node) == ALBA_OK)
	{
		albaString label,vers;

		node->RestoreText("Label0", label);
		node->GetAttribute("Version",vers);
		
		//Retro-compatibility with old register cluster op
		if (vers != "2")
		{
			double xyz[3], rxyz[3];
			this->GetOutput()->GetAbsPose(xyz, rxyz);
			m_Label.Printf("%s%f", label.GetCStr(), xyz[0]);
		}
		else
			m_Label = label;

		return ALBA_OK;
	}
	else
		return ALBA_ERROR;
}

//-------------------------------------------------------------------------
albaGUI* albaVMEInfoText::CreateGui()
{

	albaID sub_id = -1;

	m_Gui = albaVME::CreateGui(); // Called to show info about vmes' type and name
	m_Gui->SetListener(this);
	m_Gui->Divider();
	m_Gui->Label("Text:");
	m_Gui->String(ID_LABEL, "", &m_Label, "Info Text", true);
	m_Gui->Divider();

	m_Gui->Update();

	return m_Gui;
}

//-------------------------------------------------------------------------
albaVMEOutput *albaVMEInfoText::GetOutput()
{
  // allocate the right type of output on demand
  if (m_Output==NULL)
  {
    SetOutput(albaVMEOutputNULL::New()); // create the output
  }
  return m_Output;
}
//-------------------------------------------------------------------------
void albaVMEInfoText::SetLabel(const albaString& label)
{
	m_Label = label;

  if(m_Gui) 
    m_Gui->Update();
}
//-------------------------------------------------------------------------
const char *albaVMEInfoText::GetLabel()
{
  return m_Label.GetCStr();
}
//-------------------------------------------------------------------------
char** albaVMEInfoText::GetIcon() 
{
#include "albaVMEText.xpm"
  return albaVMEText_xpm;
}
