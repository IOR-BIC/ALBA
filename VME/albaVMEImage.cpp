/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEImage
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



#include "albaVMEImage.h"
#include "albaMatrixInterpolator.h"
#include "albaDataVector.h"
#include "albaDataPipeInterpolatorVTK.h"
#include "albaVMEItemVTK.h"
#include "albaAbsMatrixPipe.h"
#include "vtkDataSet.h"
#include "vtkImageData.h"
#include "mmaMaterial.h"
#include "wx\bitmap.h"
#include "wx\image.h"
#include "vtkPointData.h"
#include "vtkDataArray.h"
//-------------------------------------------------------------------------
albaCxxTypeMacro(albaVMEImage)
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
albaVMEImage::albaVMEImage()
//-------------------------------------------------------------------------
{
}

//-------------------------------------------------------------------------
albaVMEImage::~albaVMEImage()
//-------------------------------------------------------------------------
{
  // data pipe destroyed in albaVME
  // data vector destroyed in albaVMEGeneric
}


//-------------------------------------------------------------------------
albaVMEOutput *albaVMEImage::GetOutput()
//-------------------------------------------------------------------------
{
  // allocate the right type of output on demand
  if (m_Output==NULL)
  {
    SetOutput(albaVMEOutputImage::New()); // create the output
  }
  return m_Output;
}

//-------------------------------------------------------------------------
int albaVMEImage::SetData(vtkImageData *data, albaTimeStamp t, int mode)
//-------------------------------------------------------------------------
{
  return Superclass::SetData(data,t,mode);
}
//-------------------------------------------------------------------------
int albaVMEImage::SetData(vtkDataSet *data, albaTimeStamp t, int mode)
//-------------------------------------------------------------------------
{
  assert(data);
  if (data->IsA("vtkImageData"))
  {
    int dimensions[3];
    vtkImageData::SafeDownCast(data)->GetDimensions(dimensions);
	if( dimensions[0] != 1 && dimensions[1] != 1 && dimensions[2] != 1 ) {
		return ALBA_ERROR;
	}
    return Superclass::SetData(data,t,mode);
  }
  
  return ALBA_ERROR;
}

//----------------------------------------------------------------------------
wxBitmap albaVMEImage::GetImageAsBitmap()
{

	vtkImageData *imgData= vtkImageData::SafeDownCast(GetOutput()->GetVTKData());
	if (imgData)
	{

		int *dims = imgData->GetDimensions();

		if (dims[0] > 0 && dims[1] > 0 && dims[2] == 1)
		{

			wxImage img(dims[0], dims[1]);
			unsigned char *data = img.GetData();

			vtkDataArray* scalars = imgData->GetPointData()->GetScalars();
			int numberOfComponents = scalars->GetNumberOfComponents();

			for (int i = 0; i < dims[0] * dims[1]; i++)
			{
				if (numberOfComponents == 3)
				{
					double *values;
					values = scalars->GetTuple3(i);
					data[i * 3] = values[0];
					data[i * 3 + 1] = values[1];
					data[i * 3 + 2] = values[2];
				}
				else
				{
					double value;
					value = scalars->GetTuple1(i);
					data[i * 3] = data[i * 3 + 1] = data[i * 3 + 2] = value;
				}
			}

			wxBitmap bpm(img);

			return bpm;
		}
	}

	//return empty bitmap
	return wxBitmap();
}

//-------------------------------------------------------------------------
char** albaVMEImage::GetIcon() 
//-------------------------------------------------------------------------
{
  #include "albaVMEImage.xpm"
  return albaVMEImage_xpm;
}

//-------------------------------------------------------------------------
int albaVMEImage::InternalInitialize()
//-------------------------------------------------------------------------
{
	if (Superclass::InternalInitialize()==ALBA_OK)
	{
		// force material allocation
		GetMaterial();
		return ALBA_OK;
	}
	return ALBA_ERROR;
}

//-------------------------------------------------------------------------
mmaMaterial *albaVMEImage::GetMaterial()
//-------------------------------------------------------------------------
{
  mmaMaterial *material = (mmaMaterial *)GetAttribute("MaterialAttributes");
  if (material == NULL)
  {
    material = mmaMaterial::New();
		material->m_MaterialType=mmaMaterial::USE_LOOKUPTABLE;
		    
    SetAttribute("MaterialAttributes", material);
		GetOutput();
    
		((albaVMEOutputImage *)m_Output)->SetMaterial(material);
  }
  return material;
}