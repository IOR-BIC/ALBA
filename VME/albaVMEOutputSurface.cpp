/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEOutputSurface
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



#include "albaVMEOutputSurface.h"
#include "albaVME.h"
#include "albaIndent.h"
#include "albaDataPipe.h"
#include "albaGUI.h"
#include "mmaMaterial.h"

#include "vtkPolyData.h"
#include "vtkImageData.h"

#include <assert.h>

//-------------------------------------------------------------------------
albaCxxTypeMacro(albaVMEOutputSurface)
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
albaVMEOutputSurface::albaVMEOutputSurface()
//-------------------------------------------------------------------------
{
  m_Texture       = NULL;
  m_Material      = NULL;
  m_NumTriangles  = "0";
}

//-------------------------------------------------------------------------
albaVMEOutputSurface::~albaVMEOutputSurface()
//-------------------------------------------------------------------------
{
  vtkDEL(m_Texture);
}

//-------------------------------------------------------------------------
vtkPolyData *albaVMEOutputSurface::GetSurfaceData()
//-------------------------------------------------------------------------
{
  return (vtkPolyData *)GetVTKData();
}

//-------------------------------------------------------------------------
void albaVMEOutputSurface::SetTexture(vtkImageData *tex)
//-------------------------------------------------------------------------
{
  if (m_Texture != tex)
  {
    m_Texture = tex;
    if(m_Texture)
    	m_Texture->Register(NULL);
  }
}
//-------------------------------------------------------------------------
vtkImageData *albaVMEOutputSurface::GetTexture()
//-------------------------------------------------------------------------
{
  return m_Texture;
}

//-------------------------------------------------------------------------
mmaMaterial *albaVMEOutputSurface::GetMaterial()
//-------------------------------------------------------------------------
{
  // if the VME set the material directly in the output return it
  if (m_Material)
    return  m_Material;

  // search for a material attribute in the VME connected to this output
  return GetVME() ? mmaMaterial::SafeDownCast(GetVME()->GetAttribute("MaterialAttributes")) : NULL;
}

//-------------------------------------------------------------------------
void albaVMEOutputSurface::SetMaterial(mmaMaterial *material)
//-------------------------------------------------------------------------
{
  m_Material = material;
}
//-------------------------------------------------------------------------
albaGUI* albaVMEOutputSurface::CreateGui()
//-------------------------------------------------------------------------
{
  assert(m_Gui == NULL);
  m_Gui = albaVMEOutput::CreateGui();
  this->Update();
  /*if (GetSurfaceData())
  {
    this->Update();
    int num = GetSurfaceData()->GetNumberOfPolys();
    m_NumTriangles = num;
  }*/
  m_Gui->Label(_("Triangles:"), &m_NumTriangles, true, false, false, 0.35);
  m_Gui->Divider();

	return m_Gui;
}
//-------------------------------------------------------------------------
void albaVMEOutputSurface::Update()
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
    m_NumTriangles = _("0");
  }
  if (m_Gui)
  {
    m_Gui->Update();
  }
}
