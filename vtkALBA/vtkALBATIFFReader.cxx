/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkALBATIFFReader
 Authors: Gianluigi Crimi

 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "albaConfigure.h"
#include "albaDefines.h"

#include "vtkALBATIFFReader.h"
#include "vtkObjectFactory.h"
#include "vtkImageData.h"

#include <wx/image.h>


vtkStandardNewMacro(vtkALBATIFFReader);

//----------------------------------------------------------------------------
// This function reads data from a TIFF file using wxImage when the standard
// VTK reader does not provide valid scalar data.
void vtkALBATIFFReader::ExecuteDataWithInformation(
	vtkDataObject *out,
	vtkInformation *outInfo)
{
	Superclass::ExecuteDataWithInformation(out, outInfo);

	vtkImageData *outputImg = GetOutput();
	if (outputImg == NULL)
	{
		return;
	}

	double scalarRange[2];
	outputImg->GetScalarRange(scalarRange);

	if (scalarRange[0] != 0.0 || scalarRange[1] != 0.0)
	{
		return;
	}

	if (wxImage::FindHandler(wxBITMAP_TYPE_TIF) == NULL)
	{
		wxImage::AddHandler(new wxTIFFHandler);
	}

	wxString fileName(FileName, wxConvUTF8);
	wxImage image;

	if (!image.LoadFile(fileName, wxBITMAP_TYPE_TIF))
	{
		albaErrorMacro("Cannot read TIFF file with wxImage:" << FileName);
		return;
	}

	if (!image.IsOk() || image.GetData() == NULL)
	{
		albaErrorMacro("Invalid TIFF image: " << FileName);
		return;
	}

	const int width = image.GetWidth();
	const int height = image.GetHeight();
	const unsigned char *source = image.GetData();

	if (width <= 0 || height <= 0)
	{
		albaErrorMacro("TIFF image has invalid dimensions: " << FileName);
		return;
	}

	outputImg->SetExtent(0, width - 1, 0, height - 1, 0, 0);
	outputImg->AllocateScalars(VTK_FLOAT, 1);

	const unsigned char *sourcePixel = source;
	for (int sourceY = 0; sourceY < height; ++sourceY)
	{
		// wxImage uses a top-left origin, while VTK uses a bottom-left origin.
		const int destinationY = height - sourceY - 1;

		for (int x = 0; x < width; ++x)
		{
			float *destinationPixel = static_cast<float *>(outputImg->GetScalarPointer(x, destinationY, 0));
			destinationPixel[0] = (sourcePixel[0] + sourcePixel[1] + sourcePixel[2]) / 3.0f;
			sourcePixel += 3; // Move to the next pixel (RGB)
		}
	}
}

//----------------------------------------------------------------------------
vtkALBATIFFReader::vtkALBATIFFReader()
{
}

//----------------------------------------------------------------------------
vtkALBATIFFReader::~vtkALBATIFFReader()
{
}