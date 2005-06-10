/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmaMeter.cpp,v $
  Language:  C++
  Date:      $Date: 2005-06-10 15:27:07 $
  Version:   $Revision: 1.5 $
  Authors:   Marco Petrone, Paolo Quadrani
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

#include "mmaMeter.h"
#include "mafVMEMeter.h"
#include "mafStorageElement.h"
#include "mafIndent.h"

#include <assert.h>

//-------------------------------------------------------------------------
mafCxxTypeMacro(mmaMeter)
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
mmaMeter::mmaMeter()
//-------------------------------------------------------------------------
{
  m_Name           = "MeterAttributes";
  m_LabelVisibility= 1;
  m_ColorMode      = mafVMEMeter::ONE_COLOR;
  m_Representation = mafVMEMeter::LINE_REPRESENTATION;
  m_Capping        = 0;
  m_MeterMode      = mafVMEMeter::POINT_DISTANCE;
  m_TubeRadius     = 1.0;
  m_InitMeasure    = 0.0;
  m_GenerateEvent  = 0;
  m_ThresholdEvent = 0;
  m_DeltaPercent   = 10.0;
  m_MeasureType    = mafVMEMeter::ABSOLUTE_MEASURE;

  m_DistanceRange[0] = 0.0;
  m_DistanceRange[1] = 1.0;
}
//-------------------------------------------------------------------------
mmaMeter::~mmaMeter()
//-------------------------------------------------------------------------
{
}
//-------------------------------------------------------------------------
void mmaMeter::DeepCopy(const mafAttribute *a)
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
bool mmaMeter::Equals(const mafAttribute *a)
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
int mmaMeter::InternalStore(mafStorageElement *parent)
//-----------------------------------------------------------------------
{  
  if (Superclass::InternalStore(parent)==MAF_OK)
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
    return MAF_OK;
  }
  return MAF_ERROR;
}
//-----------------------------------------------------------------------
int mmaMeter::InternalRestore(mafStorageElement *node)
//-----------------------------------------------------------------------
{
  if (Superclass::InternalRestore(node)==MAF_OK)
  {
    mafMatrix matrix;
    if (node->RestoreMatrix("Transform",&matrix)==MAF_OK)
    {
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
      return MAF_OK;
    }
  }
  return MAF_ERROR;
}
//-----------------------------------------------------------------------
void mmaMeter::Print(std::ostream& os, const int tabs)
//-----------------------------------------------------------------------
{
  Superclass::Print(os,tabs);
  mafIndent indent(tabs);
}