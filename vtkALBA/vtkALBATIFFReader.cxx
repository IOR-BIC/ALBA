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

#include "itkimage.h"
#include "itkImageFileReader.h"
#include "itkTIFFImageIO.h"

#include "itkImageToVTKImageFilter.h"

#include <algorithm>
#include "albaProgressBarHelper.h"
#include "itkImageToVTKImageFilter.h"
#include "vtkImageToStructuredPoints.h"
#include "itkFlipImageFilter.h"


const unsigned int Dimension = 2;

typedef float InputPixelTypeFloat;
typedef itk::Image< InputPixelTypeFloat, Dimension > ImageType;

typedef itk::ImageToVTKImageFilter< ImageType > ConverteritkTOvtk;



vtkStandardNewMacro(vtkALBATIFFReader);

//----------------------------------------------------------------------------
// This function reads a data from a file.  The datas extent/axes
// are assumed to be the same as the file extent/order.
void vtkALBATIFFReader::ExecuteData(vtkDataObject *output)
{
	Superclass::ExecuteData(output);

	vtkImageData *outputImg = GetOutput();
	if (outputImg->GetScalarRange()[0] != 0 || outputImg->GetScalarRange()[1] != 0)
	{
		return;
	}
	else
	{
		using ReaderType = itk::ImageFileReader<ImageType>;
		ReaderType::Pointer reader = ReaderType::New();

		itk::TIFFImageIO::Pointer tiffIO = itk::TIFFImageIO::New();
		reader->SetImageIO(tiffIO);

		reader->SetFileName(FileName);

		try
		{
			reader->Update();
		}
		catch (itk::ExceptionObject& ex)
		{
			vtkErrorMacro("Cannot Read %s \n %s", m_Files[i].c_str(), ex.GetDescription());
			return;
		}

		using FlipFilterType = itk::FlipImageFilter<ImageType>;
		FlipFilterType::Pointer flipFilter = FlipFilterType::New();

		FlipFilterType::FlipAxesArrayType flipAxes;
		flipAxes[0] = false;
		flipAxes[1] = true;
		flipFilter->SetFlipAxes(flipAxes);

		flipFilter->SetInput(reader->GetOutput());
		flipFilter->Update();

		ConverteritkTOvtk::Pointer itkTOvtk = ConverteritkTOvtk::New();
		itkTOvtk->SetInput(flipFilter->GetOutput());
		itkTOvtk->Update();

		outputImg->DeepCopy(itkTOvtk->GetOutput());
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
