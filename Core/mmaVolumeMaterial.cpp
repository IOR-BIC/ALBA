/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: mmaVolumeMaterial
 Authors: Paolo Quadrani
 
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

#include "mmaVolumeMaterial.h"
#include "albaDecl.h"
#include "albaEvent.h"
#include "albaGUILutPreset.h"

#include "albaStorageElement.h"
#include "albaIndent.h"

#include "albaVME.h"

#include "vtkALBASmartPointer.h"
#include "vtkLookupTable.h"
#include "vtkPiecewiseFunction.h"
#include "vtkColorTransferFunction.h"
#include "vtkVolumeProperty.h"
#include "vtkALBATransferFunction2D.h"

//----------------------------------------------------------------------------
albaCxxTypeMacro(mmaVolumeMaterial)
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mmaVolumeMaterial::mmaVolumeMaterial()
//----------------------------------------------------------------------------
{  
  m_Name        = "VolumeMaterialAttributes";
  m_MaterialName= "new material";
  vtkNEW(m_ColorLut);

  vtkNEW(m_OpacityTransferFunction);
  vtkNEW(m_GradientTransferFunction);
  vtkNEW(m_ColorTransferFunction);
  vtkNEW(m_VolumeProperty);
  vtkNEW(m_VolumeProperty2);

  m_Level_LUT           = 0.5;
  m_Window_LUT          = 1.0;
  m_HueRange[0]         = 0.0;
  m_HueRange[1]         = 0.6667;
  m_SaturationRange[0]  = 0;
  m_SaturationRange[1]  = 1;
  m_TableRange[0]       = 0.0;
  m_TableRange[1]       = -1.0; // this is an invalid range, needed to be checked by visual pipe, 
                                // that if them found it invalid, initialize with data range
  m_GammaCorrection     = 1.0;
  m_NumValues           = 128;
  m_InterpolationType   = 0;
  m_Shade               = 0;
  m_NumOpacityValues    = 0;
  m_NumGradientValues   = 0;
  m_NumColorValues      = 0;

  lutPreset(4,m_ColorLut);
  m_NumValues = m_ColorLut->GetNumberOfTableValues();
}
//----------------------------------------------------------------------------
mmaVolumeMaterial::~mmaVolumeMaterial()
//----------------------------------------------------------------------------
{
  vtkDEL(m_ColorLut);
  vtkDEL(m_VolumeProperty2); 

  vtkDEL(m_OpacityTransferFunction);
  vtkDEL(m_GradientTransferFunction);
  vtkDEL(m_ColorTransferFunction);
  vtkDEL(m_VolumeProperty);
}
//-------------------------------------------------------------------------
void mmaVolumeMaterial::DeepCopy(const albaAttribute *a)
//-------------------------------------------------------------------------
{ 
  Superclass::DeepCopy(a);
  // property
  m_MaterialName        = ((mmaVolumeMaterial *)a)->m_MaterialName;
  // lut
  m_Level_LUT           = ((mmaVolumeMaterial *)a)->m_Level_LUT;
  m_Window_LUT          = ((mmaVolumeMaterial *)a)->m_Window_LUT;
  m_HueRange[0]         = ((mmaVolumeMaterial *)a)->m_HueRange[0];
  m_HueRange[1]         = ((mmaVolumeMaterial *)a)->m_HueRange[1];
  m_SaturationRange[0]  = ((mmaVolumeMaterial *)a)->m_SaturationRange[0];
  m_SaturationRange[1]  = ((mmaVolumeMaterial *)a)->m_SaturationRange[1];
  m_TableRange[0]       = ((mmaVolumeMaterial *)a)->m_TableRange[0];
  m_TableRange[1]       = ((mmaVolumeMaterial *)a)->m_TableRange[1];
  m_GammaCorrection     = ((mmaVolumeMaterial *)a)->m_GammaCorrection;
  m_NumValues           = ((mmaVolumeMaterial *)a)->m_NumValues;
  m_InterpolationType   = ((mmaVolumeMaterial *)a)->m_InterpolationType;
  m_Shade               = ((mmaVolumeMaterial *)a)->m_Shade;
  UpdateProp();
}
//----------------------------------------------------------------------------
bool mmaVolumeMaterial::Equals(const albaAttribute *a)
//----------------------------------------------------------------------------
{
  if (Superclass::Equals(a))
  {
    return (m_MaterialName  == ((mmaVolumeMaterial *)a)->m_MaterialName       &&
      m_Level_LUT           == ((mmaVolumeMaterial *)a)->m_Level_LUT          &&
      m_Window_LUT          == ((mmaVolumeMaterial *)a)->m_Window_LUT         &&
      m_HueRange[0]         == ((mmaVolumeMaterial *)a)->m_HueRange[0]        &&
      m_HueRange[1]         == ((mmaVolumeMaterial *)a)->m_HueRange[1]        &&
      m_SaturationRange[0]  == ((mmaVolumeMaterial *)a)->m_SaturationRange[0] &&
      m_SaturationRange[1]  == ((mmaVolumeMaterial *)a)->m_SaturationRange[1] &&
      m_TableRange[0]       == ((mmaVolumeMaterial *)a)->m_TableRange[0]      &&
      m_TableRange[1]       == ((mmaVolumeMaterial *)a)->m_TableRange[1]      &&
      m_GammaCorrection     == ((mmaVolumeMaterial *)a)->m_GammaCorrection    &&
      m_InterpolationType   == ((mmaVolumeMaterial *)a)->m_InterpolationType  &&
      m_Shade               == ((mmaVolumeMaterial *)a)->m_Shade              &&
      m_NumValues           == ((mmaVolumeMaterial *)a)->m_NumValues);
  }
  return false;
}
//-----------------------------------------------------------------------
int mmaVolumeMaterial::InternalStore(albaStorageElement *parent)
//-----------------------------------------------------------------------
{  
  if (Superclass::InternalStore(parent)==ALBA_OK)
  {
    UpdateFromTables();
    // property
    parent->StoreText("MaterialName",m_MaterialName);
    // lut
    parent->StoreDouble("Level_LUT", m_Level_LUT);
    parent->StoreDouble("Window_LUT", m_Window_LUT);
    parent->StoreDouble("HueRange0", m_HueRange[0]);
    parent->StoreDouble("HueRange1", m_HueRange[1]);
    parent->StoreDouble("SaturationRange0", m_SaturationRange[0]);
    parent->StoreDouble("SaturationRange1", m_SaturationRange[1]);
    parent->StoreDouble("TableRange0", m_TableRange[0]);
    parent->StoreDouble("TableRange1", m_TableRange[1]);
    parent->StoreDouble("GammaCorrection", m_GammaCorrection);
    parent->StoreInteger("NumValues", m_NumValues);
    albaString lutvalues;
    double *rgba;
    for (int v = 0; v < m_NumValues; v++)
    {
      lutvalues = "LUT_VALUE_";
      lutvalues << v;
      rgba = m_ColorLut->GetTableValue(v);
      parent->StoreVectorN(lutvalues,rgba,4);
    }
    parent->StoreInteger("InterpolationType", m_InterpolationType);
    parent->StoreInteger("Shade", m_Shade);
    m_NumOpacityValues = m_OpacityTransferFunction->GetSize();
    parent->StoreInteger("NumOpacityValues", m_NumOpacityValues);
    double *data_values = m_OpacityTransferFunction->GetDataPointer();
    double point[2];
    int p;
    for (p = 0; p < m_NumOpacityValues; p++)
    {
      lutvalues = "OPACITY_VALUE_";
      lutvalues << p;
      point[0] = data_values[2*p];
      point[1] = data_values[2*p + 1];
      parent->StoreVectorN(lutvalues,point,2);
    }
    m_NumGradientValues = m_GradientTransferFunction->GetSize();
    parent->StoreInteger("NumGradientValues", m_NumGradientValues);
    data_values = m_GradientTransferFunction->GetDataPointer();
    for (p = 0; p < m_NumGradientValues; p++)
    {
      lutvalues = "GRADIENT_VALUE_";
      lutvalues << p;
      point[0] = data_values[2*p];
      point[1] = data_values[2*p + 1];
      parent->StoreVectorN(lutvalues,point,2);
    }
    return ALBA_OK;
  }
  return ALBA_ERROR;
}
//----------------------------------------------------------------------------
int mmaVolumeMaterial::InternalRestore(albaStorageElement *node)
//----------------------------------------------------------------------------
{
  if (Superclass::InternalRestore(node) == ALBA_OK)
  {
    // property
    node->RestoreText("MaterialName",m_MaterialName);
    // lut
    node->RestoreDouble("Level_LUT", m_Level_LUT);
    node->RestoreDouble("Window_LUT", m_Window_LUT);
    node->RestoreDouble("HueRange0", m_HueRange[0]);
    node->RestoreDouble("HueRange1", m_HueRange[1]);
    node->RestoreDouble("SaturationRange0", m_SaturationRange[0]);
    node->RestoreDouble("SaturationRange1", m_SaturationRange[1]);
    node->RestoreDouble("TableRange0", m_TableRange[0]);
    node->RestoreDouble("TableRange1", m_TableRange[1]);
    node->RestoreDouble("GammaCorrection", m_GammaCorrection);
    node->RestoreInteger("NumValues", m_NumValues);
    m_ColorLut->SetNumberOfTableValues(m_NumValues);
    albaString lutvalues;
    double rgba[4];
    for (int v = 0; v < m_NumValues; v++)
    {
      lutvalues = "LUT_VALUE_";
      lutvalues << v;
      node->RestoreVectorN(lutvalues,rgba,4);
      m_ColorLut->SetTableValue(v,rgba);
    }
    m_ColorLut->SetTableRange(m_TableRange);
    m_ColorLut->SetRange(m_TableRange);
    m_ColorLut->Build();
    node->RestoreInteger("InterpolationType", m_InterpolationType);
    node->RestoreInteger("Shade", m_Shade);
    node->RestoreInteger("NumOpacityValues", m_NumOpacityValues);

    double point[2];
    int p;
    for (p = 0; p < m_NumOpacityValues; p++)
    {
      lutvalues = "OPACITY_VALUE_";
      lutvalues << p;
      node->RestoreVectorN(lutvalues,point,2);
      m_OpacityTransferFunction->AddPoint(point[0],point[1]);
    }
    node->RestoreInteger("NumGradientValues", m_NumGradientValues);
    for (p = 0; p < m_NumGradientValues; p++)
    {
      lutvalues = "GRADIENT_VALUE_";
      lutvalues << p;
      node->RestoreVectorN(lutvalues,point,2);
      m_GradientTransferFunction->AddPoint(point[0],point[1]);
    }
    return ALBA_OK;
  }
  return ALBA_ERROR;
}
//-----------------------------------------------------------------------
void mmaVolumeMaterial::UpdateProp()
//-----------------------------------------------------------------------
{
  m_VolumeProperty->SetScalarOpacity(m_OpacityTransferFunction);
  m_VolumeProperty->SetGradientOpacity(m_GradientTransferFunction);
  m_VolumeProperty->SetColor(m_ColorTransferFunction);
  m_VolumeProperty->SetInterpolationType(m_InterpolationType);
  m_VolumeProperty->SetShade(m_Shade);

  m_ColorLut->SetHueRange(m_HueRange);
  m_ColorLut->SetSaturationRange(m_SaturationRange);
  m_ColorLut->SetNumberOfTableValues(m_NumValues);
  m_ColorLut->SetTableRange(m_TableRange);
  m_ColorLut->SetRange(m_TableRange);
  m_ColorLut->Build();

  m_Window_LUT = m_TableRange[1] - m_TableRange[0];
  m_Level_LUT  = (m_TableRange[1] + m_TableRange[0])* .5;
}
//-----------------------------------------------------------------------
void mmaVolumeMaterial::UpdateFromTables()
//-----------------------------------------------------------------------
{
  m_ColorLut->GetHueRange(m_HueRange);
  m_ColorLut->GetSaturationRange(m_SaturationRange);
  m_NumValues = m_ColorLut->GetNumberOfTableValues();

  // Added by Losi 09.23.09
  // BUG 1809 Fix
  // When storing the material if m_TableRange attribute represents the invalid range [0, -1] (set in the constructor)
  // it mustn't be updated from m_ColorLut's GetTableRange method that returns a valid range [0, 1]
  // This avoids storing a valid range when the invalid range is set

  if(!(m_TableRange[0] == 0 && m_TableRange[1] == -1))
  {
	  m_ColorLut->GetTableRange(m_TableRange);
  }
  m_Window_LUT = m_TableRange[1] - m_TableRange[0];
  m_Level_LUT  = (m_TableRange[1] + m_TableRange[0])* .5;
}
//-----------------------------------------------------------------------
void mmaVolumeMaterial::ApplyGammaCorrection(const int preset)
//-----------------------------------------------------------------------
{
  if (-1 != preset)
  {
    lutPreset(preset, m_ColorLut);
  }
  
  if(1.0 == m_GammaCorrection)
  {
    //useless calculation
    return;
  }
  for(int i=0; i<m_NumValues;i++)
  {
    double rgba[4];
    m_ColorLut->GetTableValue(i,rgba);
    rgba[0] = pow(rgba[0], 1./m_GammaCorrection);
    rgba[0] = rgba[0] < 0. ?  0.: rgba[0];
    rgba[0] = rgba[0] > 1. ?  1.: rgba[0];
    rgba[1] = pow(rgba[1], 1./m_GammaCorrection);
    rgba[1] = rgba[1] < 0. ?  0.: rgba[1];
    rgba[1] = rgba[1] > 1. ?  1.: rgba[1];
    rgba[2] = pow(rgba[2], 1./m_GammaCorrection);
    rgba[2] = rgba[2] < 0. ?  0.: rgba[2];
    rgba[2] = rgba[2] > 1. ?  1.: rgba[2];
    m_ColorLut->SetTableValue(i,rgba);
  }
}
//-----------------------------------------------------------------------
void mmaVolumeMaterial::Print(std::ostream& os, const int tabs) const
//-----------------------------------------------------------------------
{
  Superclass::Print(os,tabs);
  albaIndent indent(tabs);
}
