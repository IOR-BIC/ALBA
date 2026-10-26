/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkALBATicksGenerator
 Authors: Gianluigi Crimi

 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "albaMeshLinearizationProvider.h"
#include "vtkALBAMeshLinearizationFilter.h"


struct MeshLinearizationFilterList
{
	vtkAlgorithmOutput *m_Port;
	vtkALBAMeshLinearizationFilter *m_Filter;    //<GPU provider for GPU computation
	int m_ReferenceCount;
};

//Global vector with all current registered 3d texture 
std::vector<MeshLinearizationFilterList> glo_Filters;

//----------------------------------------------------------------------------
albaMeshLinearizationProvider::albaMeshLinearizationProvider()
{
	m_Port = NULL;
}

//----------------------------------------------------------------------------
albaMeshLinearizationProvider::~albaMeshLinearizationProvider()
{
	ReleaseFilter();
}


//----------------------------------------------------------------------------
vtkALBAMeshLinearizationFilter *albaMeshLinearizationProvider::GetFilter(vtkAlgorithmOutput *port)
{

	/**If a filter already exists release it */
	ReleaseFilter();

	int pos = GetFilterIndex(port);
	if(pos == -1)
	{
		MeshLinearizationFilterList newFilter;
		newFilter.m_Port = port;
		newFilter.m_Filter = vtkALBAMeshLinearizationFilter::New();
		newFilter.m_Filter->SetInputConnection(port);
		newFilter.m_ReferenceCount = 1;
		glo_Filters.push_back(newFilter);

		m_Port = port;
		return newFilter.m_Filter;
	}
	else
	{
		glo_Filters[pos].m_ReferenceCount++;
		m_Port = glo_Filters[pos].m_Port;
		return glo_Filters[pos].m_Filter;
	}
}

//----------------------------------------------------------------------------
void albaMeshLinearizationProvider::ReleaseFilter()
{
	int pos = GetFilterIndex(m_Port);

	if (pos != -1)
	{
		glo_Filters[pos].m_ReferenceCount--;
		if (glo_Filters[pos].m_ReferenceCount == 0)
		{
			glo_Filters[pos].m_Filter->Delete();
			glo_Filters.erase(glo_Filters.begin() + pos);
		}
	}

	m_Port = NULL;
}

//----------------------------------------------------------------------------
int albaMeshLinearizationProvider::GetFilterIndex(vtkAlgorithmOutput *port)
{
	for (int i = 0; i < glo_Filters.size(); i++)
	{
		if (glo_Filters[i].m_Port == port)
			return i;
	}
	return -1;
}

