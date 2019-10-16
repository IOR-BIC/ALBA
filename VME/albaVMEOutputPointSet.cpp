/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEOutputPointSet
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

#include "albaVMEOutputPointSet.h"
#include "albaVME.h"
#include "albaGUI.h"
#include "albaIndent.h"
#include "mmaMaterial.h"

#include "vtkPolyData.h"
#include "vtkDataSet.h"

#include <assert.h>

//-------------------------------------------------------------------------
albaCxxTypeMacro(albaVMEOutputPointSet)
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
albaVMEOutputPointSet::albaVMEOutputPointSet()
//-------------------------------------------------------------------------
{
  m_NumPoints = "0";
  m_Material = NULL;
}

//-------------------------------------------------------------------------
albaVMEOutputPointSet::~albaVMEOutputPointSet()
//-------------------------------------------------------------------------
{
}

//-------------------------------------------------------------------------
vtkPolyData *albaVMEOutputPointSet::GetPointSetData()
//-------------------------------------------------------------------------
{
  return (vtkPolyData *)GetVTKData();
}

//-------------------------------------------------------------------------
mmaMaterial *albaVMEOutputPointSet::GetMaterial()
//-------------------------------------------------------------------------
{
  // if the VME set the material directly in the output return it
  if (m_Material)
    return  m_Material;

  // search for a material attribute in the VME connected to this output
  return GetVME() ? mmaMaterial::SafeDownCast(GetVME()->GetAttribute("MaterialAttributes")) : NULL;
}

//-------------------------------------------------------------------------
void albaVMEOutputPointSet::SetMaterial(mmaMaterial *material)
//-------------------------------------------------------------------------
{
  m_Material = material;
}

//-------------------------------------------------------------------------
albaGUI* albaVMEOutputPointSet::CreateGui()
//-------------------------------------------------------------------------
{
  assert(m_Gui == NULL);
  m_Gui = albaVMEOutput::CreateGui();
  if (GetPointSetData())
  {
    this->Update();
    int num = GetPointSetData()->GetNumberOfVerts();
    m_NumPoints = num;
  }
  m_Gui->Label(_("Points:"), &m_NumPoints, true);
	m_Gui->Divider();

  return m_Gui;
}
//-------------------------------------------------------------------------
void albaVMEOutputPointSet::Update()
//-------------------------------------------------------------------------
{
  assert(m_VME);
  m_VME->Update();
  if (GetPointSetData())
  {
    int num = GetPointSetData()->GetNumberOfVerts();
    m_NumPoints = num;
  }
  else
  {
    m_NumPoints = "0";
  }
  if (m_Gui)
  {
    m_Gui->Update();
  }
}
