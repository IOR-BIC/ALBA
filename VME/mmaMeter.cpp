/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: mmaMeter
 Authors: Marco Petrone, Paolo Quadrani
 
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

#include "mmaMeter.h"
#include "albaVMEMeter.h"
#include "albaStorageElement.h"
#include "albaIndent.h"

#include <assert.h>

//-------------------------------------------------------------------------
albaCxxTypeMacro(mmaMeter)
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
mmaMeter::mmaMeter()
//-------------------------------------------------------------------------
{
  m_Name           = "MeterAttributes";
  m_LabelVisibility= 1;
  m_ColorMode      = albaVMEMeter::ONE_COLOR;
  m_Representation = albaVMEMeter::LINE_REPRESENTATION;
  m_Capping        = 0;
  m_MeterMode      = albaVMEMeter::POINT_DISTANCE;
  m_TubeRadius     = 1.0;
  m_InitMeasure    = 0.0;
  m_GenerateEvent  = 0;
  m_ThresholdEvent = 0;
  m_DeltaPercent   = 10.0;
  m_MeasureType    = albaVMEMeter::ABSOLUTE_MEASURE;

  m_DistanceRange[0] = 0.0;
  m_DistanceRange[1] = 1.0;
}
//-------------------------------------------------------------------------
mmaMeter::~mmaMeter()
//-------------------------------------------------------------------------
{
}
//-------------------------------------------------------------------------
void mmaMeter::DeepCopy(const albaAttribute *a)
//-------------------------------------------------------------------------
{ 
  Superclass::DeepCopy(a);
  m_MeterMode       = ((mmaMeter *)a)->m_MeterMode;
  m_ColorMode       = ((mmaMeter *)a)->m_ColorMode;
  m_Representation  = ((mmaMeter *)a)->m_Representation;
  m_Capping         = ((mmaMeter *)a)->m_Capping;
  m_MeasureType     = ((mmaMeter *)a)->m_MeasureType;
  m_GenerateEvent   = ((mmaMeter *)a)->m_GenerateEvent;
  m_DeltaPercent    = ((mmaMeter *)a)->m_DeltaPercent;
  m_ThresholdEvent  = ((mmaMeter *)a)->m_ThresholdEvent;
  m_InitMeasure     = ((mmaMeter *)a)->m_InitMeasure;
  m_TubeRadius      = ((mmaMeter *)a)->m_TubeRadius;
  m_DistanceRange[0]= ((mmaMeter *)a)->m_DistanceRange[0];
  m_DistanceRange[1]= ((mmaMeter *)a)->m_DistanceRange[1];
  m_LabelVisibility = ((mmaMeter *)a)->m_LabelVisibility;
}
//-------------------------------------------------------------------------
bool mmaMeter::Equals(const albaAttribute *a)
//-------------------------------------------------------------------------
{
  if (Superclass::Equals(a))
  {
    return (m_MeterMode       == ((mmaMeter *)a)->m_MeterMode       &&
            m_ColorMode       == ((mmaMeter *)a)->m_ColorMode       &&
            m_Representation  == ((mmaMeter *)a)->m_Representation  &&
            m_Capping         == ((mmaMeter *)a)->m_Capping         &&
            m_MeasureType     == ((mmaMeter *)a)->m_MeasureType     &&
            m_GenerateEvent   == ((mmaMeter *)a)->m_GenerateEvent   &&
            m_DeltaPercent    == ((mmaMeter *)a)->m_DeltaPercent    &&
            m_ThresholdEvent  == ((mmaMeter *)a)->m_ThresholdEvent  &&
            m_InitMeasure     == ((mmaMeter *)a)->m_InitMeasure     &&
            m_TubeRadius      == ((mmaMeter *)a)->m_TubeRadius      &&
            m_DistanceRange[0]== ((mmaMeter *)a)->m_DistanceRange[0]&&
            m_DistanceRange[1]== ((mmaMeter *)a)->m_DistanceRange[1]&&
            m_LabelVisibility == ((mmaMeter *)a)->m_LabelVisibility);
  }
  return false;
}
//-----------------------------------------------------------------------
int mmaMeter::InternalStore(albaStorageElement *parent)
//-----------------------------------------------------------------------
{  
  if (Superclass::InternalStore(parent)==ALBA_OK)
  {
    parent->StoreInteger("MeterMode", m_MeterMode);
    parent->StoreInteger("ColorMode", m_ColorMode);
    parent->StoreInteger("Representation", m_Representation);
    parent->StoreInteger("Capping", m_Capping);
    parent->StoreInteger("MeasureType", m_MeasureType);
    parent->StoreInteger("GenerateEvent", m_GenerateEvent);
    parent->StoreDouble("DeltaPercent", m_DeltaPercent);
    parent->StoreInteger("ThresholdEvent", m_ThresholdEvent);
    parent->StoreInteger("LabelVisibility", m_LabelVisibility);
    parent->StoreDouble("InitMeasure",m_InitMeasure);
    parent->StoreDouble("TubeRadius",m_TubeRadius);
    parent->StoreVectorN("DistanceRange",m_DistanceRange,2);
    return ALBA_OK;
  }
  return ALBA_ERROR;
}
//-----------------------------------------------------------------------
int mmaMeter::InternalRestore(albaStorageElement *node)
//-----------------------------------------------------------------------
{
  if (Superclass::InternalRestore(node)==ALBA_OK)
  {
    //albaMatrix matrix;  //modified by Marco. 29-9-2005 It seems this field is not stored...
    ///if (node->RestoreMatrix("Transform",&matrix)==ALBA_OK)
    //{
      node->RestoreInteger("MeterMode", m_MeterMode);
      node->RestoreInteger("ColorMode", m_ColorMode);
      node->RestoreInteger("Representation", m_Representation);
      node->RestoreInteger("Capping", m_Capping);
      node->RestoreInteger("MeasureType", m_MeasureType);
      node->RestoreInteger("GenerateEvent", m_GenerateEvent);
      node->RestoreDouble("DeltaPercent", m_DeltaPercent);
      node->RestoreInteger("ThresholdEvent", m_ThresholdEvent);
      node->RestoreInteger("LabelVisibility", m_LabelVisibility);
      node->RestoreDouble("InitMeasure",m_InitMeasure);
      node->RestoreDouble("TubeRadius",m_TubeRadius);
      node->RestoreVectorN("DistanceRange",m_DistanceRange,2);
      return ALBA_OK;
    //}
  }
  return ALBA_ERROR;
}
//-----------------------------------------------------------------------
void mmaMeter::Print(std::ostream& os, const int tabs) const
//-----------------------------------------------------------------------
{
  Superclass::Print(os,tabs);
  albaIndent indent(tabs);
}
