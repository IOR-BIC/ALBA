/*=========================================================================
Program: ALBA
Module: albaOpImporterDicomSliceAccHelper
Authors: Gianluigi Crimi
==========================================================================
Copyright (c) BIC-IOR 2018 (https://github.com/IOR-BIC)

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE. See the above copyright notice for more information.
=========================================================================*/


#include "albaDefines.h"
#include "albaOpImporterDicomSliceAccHelper.h"
#include "vtkDoubleArray.h"
#include "vtkUnsignedShortArray.h"
#include "vtkUnsignedCharArray.h"
#include "vtkUnsignedIntArray.h"
#include "vtkCharArray.h"
#include "vtkIntArray.h"
#include "vtkShortArray.h"
#include "vtkFloatArray.h"
#include "vtkObjectFactory.h"
#include "vtkPointData.h"
#include "vtkDataArray.h"

#define EPISLON 0.0001

//--------------------------------------------------------------------------------------
albaOpImporterDicomSliceAccHelper::albaOpImporterDicomSliceAccHelper()
{
  m_NumOfSlices = 0;
  m_Allocated = 0;
}
//--------------------------------------------------------------------------------------
albaOpImporterDicomSliceAccHelper::~albaOpImporterDicomSliceAccHelper()
{
	vtkDEL(m_Scalars);
	delete[] m_Zcoord;
}
void albaOpImporterDicomSliceAccHelper::SetSlice(int slice_num,vtkImageData * slice,double *unRotatedOrigin)
//--------------------------------------------------------------------------------------
{
	int dimensions[3];
	int scalar_type = slice->GetPointData()->GetScalars()->GetDataType();

	slice->GetDimensions(dimensions);
		
	if (slice_num >=0 && slice_num < m_NumOfSlices)
	{
		if (!m_Allocated)
		{
			int extent[6];
			double spacing[3];
			
			slice->GetSpacing(spacing);
			slice->GetExtent(extent);
			double origin[3];
			SetSpacing(spacing); 
			origin[0] = unRotatedOrigin[0] + spacing[0] * extent[0];
			origin[1] = unRotatedOrigin[1] + spacing[1] * extent[2];
			origin[2] = unRotatedOrigin[2];
			SetOrigin(origin);
			SetDims(dimensions);
			m_DataType = scalar_type;
			Allocate();
		}
		if (dimensions[0] != m_Dims[0] || dimensions[1] != m_Dims[1])
		{
			albaLogMessage("Bad dimensions of input slice");
			return;
		}

		m_Zcoord[slice_num]=unRotatedOrigin[2];
					
		int start;
		void *out_dataPointer		= m_Scalars->GetVoidPointer(0);
		void *input_dataPointer = slice->GetPointData()->GetScalars()->GetVoidPointer(0);
		int numscalars = slice->GetPointData()->GetScalars()->GetNumberOfTuples();
		
    start = numscalars * slice_num;
		
		switch (scalar_type) 
		{
			case VTK_CHAR:
			case VTK_UNSIGNED_CHAR:
				out_dataPointer = ((char *)out_dataPointer) + start;
				memmove(out_dataPointer,input_dataPointer, sizeof(char) * numscalars);
			break;
			case VTK_SHORT:
			case VTK_UNSIGNED_SHORT:
				out_dataPointer = ((short *)out_dataPointer) + start;
				memmove((short *)out_dataPointer,(short *)input_dataPointer, sizeof(short) * numscalars);
			break;
			case VTK_INT:
			case VTK_UNSIGNED_INT:
				out_dataPointer = ((int *)out_dataPointer) + start;
				memmove((int *)out_dataPointer, (int *)input_dataPointer, sizeof(int) * numscalars);
			break;	
			case VTK_FLOAT:
				out_dataPointer = ((float *)out_dataPointer) + start;
				memmove((float *)out_dataPointer,(float *)input_dataPointer, sizeof(float) * numscalars);
			break;
			case VTK_DOUBLE:
				out_dataPointer = ((double *)out_dataPointer) + start;
				memmove((double *)out_dataPointer, (double *)input_dataPointer, sizeof(double) * numscalars);
		  break;

			default:
				return;
		}
	} 
}

//----------------------------------------------------------------------------
vtkDataSet* albaOpImporterDicomSliceAccHelper::GetNewOutput()
{
	bool useRG = false;
	double dzGlo = m_Zcoord[1] - m_Zcoord[0];
	for (int i = 1; i < m_Dims[2]-1; i++)
	{
		double dzLoc = m_Zcoord[i + 1] - m_Zcoord[i];
		if (abs(dzLoc - dzGlo) > EPISLON)
		{
			useRG = true;
			break;
		}
	}

	if (useRG) 
	{
		vtkRectilinearGrid *rg;
		vtkNEW(rg);

		vtkDoubleArray *vx = vtkDoubleArray::New();
		vtkDoubleArray *vy = vtkDoubleArray::New();
		vtkDoubleArray *vz = vtkDoubleArray::New();
		vx->SetNumberOfValues(m_Dims[0]);
		vy->SetNumberOfValues(m_Dims[1]);
		vz->SetNumberOfValues(m_Dims[2]);

		
		for (int i = 0; i < m_Dims[0]; i++) 
			vx->SetValue(i, m_Origin[0] + ((double)(i))*m_Spacing[0]);
		
		for (int i=0; i < m_Dims[1]; i++)
			vy->SetValue(i, m_Origin[1] + (double)(i)*m_Spacing[1]);
		
		for (int i = 0; i < m_Dims[2]; i++)
			vz->SetValue(i, m_Zcoord[i]);

		rg->SetDimensions(m_Dims);
		rg->SetXCoordinates(vx);
		rg->SetYCoordinates(vy);
		rg->SetZCoordinates(vz);
		rg->GetPointData()->SetScalars(m_Scalars);

		vtkDEL(vx);
		vtkDEL(vy);
		vtkDEL(vz);

		return rg;
	}
	else
	{
		vtkImageData *id;
		vtkNEW(id);

		m_Spacing[2] = dzGlo;

		id->SetDimensions(m_Dims);
		id->SetOrigin(m_Origin);
		id->SetSpacing(m_Spacing);
		id->GetPointData()->SetScalars(m_Scalars);

		return id;
	}
}

//----------------------------------------------------------------------------
void albaOpImporterDicomSliceAccHelper::SetDims(int *dims)
{
	m_Dims[0] = dims[0];
	m_Dims[1] = dims[1];
	m_Dims[2] = dims[2];
}

//----------------------------------------------------------------------------
void albaOpImporterDicomSliceAccHelper::SetOrigin(double *origin)
{
	m_Origin[0] = origin[0];
	m_Origin[1] = origin[1];
	m_Origin[2] = origin[2];
}

//----------------------------------------------------------------------------
void albaOpImporterDicomSliceAccHelper::SetSpacing(double *spacing)
{
	m_Spacing[0] = spacing[0];
	m_Spacing[1] = spacing[1];
	m_Spacing[2] = spacing[2];
}

//--------------------------------------------------------------------------------------
void albaOpImporterDicomSliceAccHelper::Allocate()
{
	if (m_NumOfSlices == 0)
		return;
	
	m_Allocated = 1;
	m_Dims[2]=m_NumOfSlices;

	m_Zcoord = new double[m_Dims[2]];
	memset(m_Zcoord, 0, sizeof(double)*m_Dims[2]);

	// data array should consistent with scalar type:
	switch (m_DataType) 
		{
			case VTK_CHAR:
				m_Scalars = vtkCharArray::New();
			break;
			case VTK_UNSIGNED_CHAR:
				m_Scalars = vtkUnsignedCharArray::New();
			break;
			case VTK_SHORT:
				m_Scalars = vtkShortArray::New();
			break;
			case VTK_INT:
				m_Scalars = vtkIntArray::New();
			break;
			case VTK_UNSIGNED_INT:
				m_Scalars = vtkUnsignedIntArray::New();
			break;
			case VTK_UNSIGNED_SHORT:
				m_Scalars = vtkUnsignedShortArray::New();
			break;
			case VTK_FLOAT:
				m_Scalars = vtkFloatArray::New();
			break;
			case VTK_DOUBLE:
				m_Scalars = vtkDoubleArray::New();
			break;
			default:
				return;
		}
	
	m_Scalars->SetNumberOfTuples(m_Dims[0] * m_Dims[1] * m_Dims[2]);
}
