/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMEOutputSurface.cpp,v $
  Language:  C++
  Date:      $Date: 2005-10-17 13:08:12 $
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



#include "mafVMEOutputSurface.h"
#include "mafVME.h"
#include "mafIndent.h"
#include "mafDataPipe.h"
#include "mmgGui.h"
#include "mmaMaterial.h"

#include "vtkPolyData.h"
#include "vtkImageData.h"

#include <assert.h>

//-------------------------------------------------------------------------
mafCxxTypeMacro(mafVMEOutputSurface)
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
mafVMEOutputSurface::mafVMEOutputSurface()
//-------------------------------------------------------------------------
{
  m_Texture       = NULL;
  m_Material      = NULL;
  m_NumTriangles  = "0";
}

//-------------------------------------------------------------------------
mafVMEOutputSurface::~mafVMEOutputSurface()
//-------------------------------------------------------------------------
{
  vtkDEL(m_Texture);
}

//-------------------------------------------------------------------------
vtkPolyData *mafVMEOutputSurface::GetSurfaceData()
//-------------------------------------------------------------------------
{
  return (vtkPolyData *)GetVTKData();
}

//-------------------------------------------------------------------------
void mafVMEOutputSurface::SetTexture(vtkImageData *tex)
//-------------------------------------------------------------------------
{
  if (m_Texture != tex)
  {
    vtkDEL(m_Texture);
    m_Texture = tex;
    m_Texture->Register(NULL);
  }
}
//-------------------------------------------------------------------------
vtkImageData *mafVMEOutputSurface::GetTexture()
//-------------------------------------------------------------------------
{
  if (m_VME && m_VME->GetDataPipe() && m_VME->GetDataPipe()->GetVTKData())
    m_VME->GetDataPipe()->GetVTKData()->UpdateInformation();
  return m_Texture;
}

//-------------------------------------------------------------------------
mmaMaterial *mafVMEOutputSurface::GetMaterial()
//-------------------------------------------------------------------------
{
  // if the VME set the material directly in the output return it
  if (m_Material)
    return  m_Material;

  // search for a material attribute in the VME connected to this output
  return GetVME() ? mmaMaterial::SafeDownCast(GetVME()->GetAttribute("MaterialAttributes")) : NULL;
}

//-------------------------------------------------------------------------
void mafVMEOutputSurface::SetMaterial(mmaMaterial *material)
//-------------------------------------------------------------------------
{
  m_Material = material;
}
//-------------------------------------------------------------------------
mmgGui* mafVMEOutputSurface::CreateGui()
//-------------------------------------------------------------------------
{
  assert(m_Gui == NULL);
  m_Gui = mafVMEOutput::CreateGui();
  if (GetSurfaceData())
  {
    this->Update();
    int num = GetSurfaceData()->GetNumberOfPolys();
    m_NumTriangles = num;
  }
  m_Gui->Label("triangles: ", &m_NumTriangles, true);
  return m_Gui;
}
//-------------------------------------------------------------------------
void mafVMEOutputSurface::Update()
//-------------------------------------------------------------------------
{
  assert(m_VME);
  m_VME->Update();
  if (GetSurfaceData())
  {
    int num = GetSurfaceData()->GetNumberOfPolys();
    m_NumTriangles = num;
  }
  else
  {
    m_NumTriangles = "0";
  }
  if (m_Gui)
  {
    m_Gui->Update();
  }
}
