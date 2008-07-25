/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmaVolumeMaterial.cpp,v $
  Language:  C++
  Date:      $Date: 2008-07-25 06:56:05 $
  Version:   $Revision: 1.8 $
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

#include "mmaVolumeMaterial.h"
#include "mafDecl.h"
#include "mafEvent.h"
#include "mafGUILutPreset.h"

#include "mafStorageElement.h"
#include "mafIndent.h"

#include "mafNode.h"

#include "vtkMAFSmartPointer.h"
#include "vtkLookupTable.h"
#include "vtkPiecewiseFunction.h"
#include "vtkColorTransferFunction.h"
#include "vtkVolumeProperty.h"
#include "vtkMAFTransferFunction2D.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(mmaVolumeMaterial)
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
void mmaVolumeMaterial::DeepCopy(const mafAttribute *a)
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
  m_NumValues           = ((mmaVolumeMaterial *)a)->m_NumValues;
  m_InterpolationType   = ((mmaVolumeMaterial *)a)->m_InterpolationType;
  m_Shade               = ((mmaVolumeMaterial *)a)->m_Shade;
}
//----------------------------------------------------------------------------
bool mmaVolumeMaterial::Equals(const mafAttribute *a)
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
      m_InterpolationType   == ((mmaVolumeMaterial *)a)->m_InterpolationType  &&
      m_Shade               == ((mmaVolumeMaterial *)a)->m_Shade              &&
      m_NumValues           == ((mmaVolumeMaterial *)a)->m_NumValues);
  }
  return false;
}
//-----------------------------------------------------------------------
int mmaVolumeMaterial::InternalStore(mafStorageElement *parent)
//-----------------------------------------------------------------------
{  
  if (Superclass::InternalStore(parent)==MAF_OK)
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
    parent->StoreInteger("NumValues", m_NumValues);
    mafString lutvalues;
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
    return MAF_OK;
  }
  return MAF_ERROR;
}
//----------------------------------------------------------------------------
int mmaVolumeMaterial::InternalRestore(mafStorageElement *node)
//----------------------------------------------------------------------------
{
  if (Superclass::InternalRestore(node) == MAF_OK)
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
    node->RestoreInteger("NumValues", m_NumValues);
    m_ColorLut->SetNumberOfTableValues(m_NumValues);
    mafString lutvalues;
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
    return MAF_OK;
  }
  return MAF_ERROR;
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
  m_ColorLut->GetTableRange(m_TableRange);
  m_Window_LUT = m_TableRange[1] - m_TableRange[0];
  m_Level_LUT  = (m_TableRange[1] + m_TableRange[0])* .5;
}
//-----------------------------------------------------------------------
void mmaVolumeMaterial::Print(std::ostream& os, const int tabs) const
//-----------------------------------------------------------------------
{
  Superclass::Print(os,tabs);
  mafIndent indent(tabs);
}
