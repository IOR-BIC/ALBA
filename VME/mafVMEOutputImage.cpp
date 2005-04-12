/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMEOutputImage.cpp,v $
  Language:  C++
  Date:      $Date: 2005-04-12 19:31:16 $
  Version:   $Revision: 1.1 $
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

#include "vtkImageData.h"

#include <assert.h>

//-------------------------------------------------------------------------
mafCxxTypeMacro(mafVMEOutputImage)
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
mafVMEOutputImage::mafVMEOutputImage()
//-------------------------------------------------------------------------
{
}

//-------------------------------------------------------------------------
mafVMEOutputImage::~mafVMEOutputImage()
//-------------------------------------------------------------------------
{
}

//-------------------------------------------------------------------------
vtkImageData *mafVMEOutputImage::GetImageData()
//-------------------------------------------------------------------------
{
  return (vtkImageData *)GetVTKData();
}
