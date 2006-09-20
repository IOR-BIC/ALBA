/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMEOutputPointSet.cpp,v $
  Language:  C++
  Date:      $Date: 2006-09-20 15:35:37 $
  Version:   $Revision: 1.7 $
  Authors:   Marco Petrone
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

#include "mafVMEOutputPointSet.h"
#include "mafVME.h"
#include "mmgGui.h"
#include "mafIndent.h"
#include "mmaMaterial.h"

#include "vtkPolyData.h"
#include "vtkDataSet.h"

#include <assert.h>

//-------------------------------------------------------------------------
mafCxxTypeMacro(mafVMEOutputPointSet)
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
mafVMEOutputPointSet::mafVMEOutputPointSet()
//-------------------------------------------------------------------------
{
  m_NumPoints = "0";
  m_Material = NULL;
}

//-------------------------------------------------------------------------
mafVMEOutputPointSet::~mafVMEOutputPointSet()
//-------------------------------------------------------------------------
{
}

//-------------------------------------------------------------------------
vtkPolyData *mafVMEOutputPointSet::GetPointSetData()
//-------------------------------------------------------------------------
{
  return (vtkPolyData *)GetVTKData();
}

//-------------------------------------------------------------------------
mmaMaterial *mafVMEOutputPointSet::GetMaterial()
//-------------------------------------------------------------------------
{
  // if the VME set the material directly in the output return it
  if (m_Material)
    return  m_Material;

  // search for a material attribute in the VME connected to this output
  return GetVME() ? mmaMaterial::SafeDownCast(GetVME()->GetAttribute("MaterialAttributes")) : NULL;
}

//-------------------------------------------------------------------------
void mafVMEOutputPointSet::SetMaterial(mmaMaterial *material)
//-------------------------------------------------------------------------
{
  m_Material = material;
}

//-------------------------------------------------------------------------
mmgGui* mafVMEOutputPointSet::CreateGui()
//-------------------------------------------------------------------------
{
  assert(m_Gui == NULL);
  m_Gui = mafVMEOutput::CreateGui();
  if (GetPointSetData())
  {
    this->Update();
    int num = GetPointSetData()->GetNumberOfVerts();
    m_NumPoints = num;
  }
  m_Gui->Label(_("points: "), &m_NumPoints, true);
  return m_Gui;
}
//-------------------------------------------------------------------------
void mafVMEOutputPointSet::Update()
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
