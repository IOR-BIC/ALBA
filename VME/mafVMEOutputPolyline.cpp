/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMEOutputPolyline.cpp,v $
  Language:  C++
  Date:      $Date: 2005-12-19 14:55:27 $
  Version:   $Revision: 1.2 $
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

#include "mafVMEOutputPolyline.h"
#include "mafVME.h"
#include "mafIndent.h"
#include "mmaMaterial.h"

#include "vtkPolyData.h"

#include <assert.h>

//-------------------------------------------------------------------------
mafCxxTypeMacro(mafVMEOutputPolyline)
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
mafVMEOutputPolyline::mafVMEOutputPolyline()
//-------------------------------------------------------------------------
{
  m_Material = NULL;
}

//-------------------------------------------------------------------------
mafVMEOutputPolyline::~mafVMEOutputPolyline()
//-------------------------------------------------------------------------
{
}

//-------------------------------------------------------------------------
vtkPolyData *mafVMEOutputPolyline::GetPolylineData()
//-------------------------------------------------------------------------
{
  return (vtkPolyData *)GetVTKData();
}
//-------------------------------------------------------------------------
void mafVMEOutputPolyline::SetMaterial(mmaMaterial *material)
//-------------------------------------------------------------------------
{
  m_Material = material;
}
//-------------------------------------------------------------------------
mmaMaterial *mafVMEOutputPolyline::GetMaterial()
//-------------------------------------------------------------------------
{
  // if the VME set the material directly in the output return it
  if (m_Material)
    return  m_Material;

  // search for a material attribute in the VME connected to this output
  return GetVME() ? mmaMaterial::SafeDownCast(GetVME()->GetAttribute("MaterialAttributes")) : NULL;
}
