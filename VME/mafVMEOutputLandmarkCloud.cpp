/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMEOutputLandmarkCloud.cpp,v $
  Language:  C++
  Date:      $Date: 2005-07-22 13:45:24 $
  Version:   $Revision: 1.1 $
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

#include "mafVMEOutputLandmarkCloud.h"
#include "mmgGui.h"
#include "mafDataPipe.h"

#include "mafVME.h"
#include "mafVMELandmarkCloud.h"

#include "vtkDataSet.h"

#include <assert.h>

//-------------------------------------------------------------------------
mafCxxTypeMacro(mafVMEOutputLandmarkCloud)
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
mafVMEOutputLandmarkCloud::mafVMEOutputLandmarkCloud()
//-------------------------------------------------------------------------
{
  m_NumLandmarks = "0";
}

//-------------------------------------------------------------------------
mafVMEOutputLandmarkCloud::~mafVMEOutputLandmarkCloud()
//-------------------------------------------------------------------------
{
}

//-------------------------------------------------------------------------
vtkDataSet *mafVMEOutputLandmarkCloud::GetVTKData()
//-------------------------------------------------------------------------
{
  assert(m_VME);
  assert(mafVMELandmarkCloud::SafeDownCast(m_VME));
  if (mafVMELandmarkCloud::SafeDownCast(m_VME)->IsOpen())
  {
    return NULL;
  }
  else
  {
    return Superclass::GetVTKData();
  }
}

//-------------------------------------------------------------------------
mmgGui* mafVMEOutputLandmarkCloud::CreateGui()
//-------------------------------------------------------------------------
{
  assert(m_Gui == NULL);
  m_Gui = mafVMEOutput::CreateGui();
  int num_lm = mafVMELandmarkCloud::SafeDownCast(m_VME)->GetNumberOfLandmarks();
  m_NumLandmarks = num_lm;
  m_Gui->Label("points: ", m_NumLandmarks, true);
  return m_Gui;
}
