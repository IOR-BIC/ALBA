/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMEOutputImage.cpp,v $
  Language:  C++
  Date:      $Date: 2010-05-03 15:07:21 $
  Version:   $Revision: 1.1.24.4 $
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


#include "mafVMEOutputImage.h"
#include "mafVME.h"
#include "mmaMaterial.h"
#include "mafDataPipe.h"

#include "vtkImageData.h"
#include "vtkWindowLevelLookupTable.h"

#include <assert.h>

//-------------------------------------------------------------------------
mafCxxTypeMacro(mafVMEOutputImage)
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
mafVMEOutputImage::mafVMEOutputImage()
//-------------------------------------------------------------------------
{
  m_Material = NULL;
}

//-------------------------------------------------------------------------
mafVMEOutputImage::~mafVMEOutputImage()
//-------------------------------------------------------------------------
{
  if(m_Material)
  {
    vtkDEL(m_Material->m_ColorLut);
    delete m_Material;
  }
}

//-------------------------------------------------------------------------
vtkImageData *mafVMEOutputImage::GetImageData()
//-------------------------------------------------------------------------
{
  return (vtkImageData *)GetVTKData();
}

//-------------------------------------------------------------------------
mmaMaterial *mafVMEOutputImage::GetMaterial()
//-------------------------------------------------------------------------
{
  // if the VME set the material directly in the output return it
  if (m_Material)
    return  m_Material;

  m_Material = new mmaMaterial();

  vtkWindowLevelLookupTable* lut = vtkWindowLevelLookupTable::New();

  double range[2];
  ((vtkDataSet *)GetVTKData())->GetScalarRange(range);
  lut->SetTableRange(range[0], range[1]); // min and max of the data in tCoord
  lut->SetHueRange(0., 0.5);
  lut->SetSaturationRange(1., 1.);
  lut->SetValueRange(1., 1.);
  lut->Build();

  m_Material->m_ColorLut = lut;

  return m_Material;
}
//-------------------------------------------------------------------------
void mafVMEOutputImage::SetMaterial(mmaMaterial *material)
//-------------------------------------------------------------------------
{
  m_Material = material;
}

//-------------------------------------------------------------------------
mafGUI* mafVMEOutputImage::CreateGui()
//-------------------------------------------------------------------------
{
  //This method is used only to load data
  assert(m_Gui == NULL);
  m_Gui = mafVMEOutput::CreateGui();

  wxBusyCursor wait;

  if (m_VME && m_VME->GetDataPipe() && m_VME->GetDataPipe()->GetVTKData())
  {
    this->Update();
  }
   return m_Gui;
}
