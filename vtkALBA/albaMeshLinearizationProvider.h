/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaMeshLinearizationProvider
 Authors: Gianluigi Crimi

 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaMeshLinearizationProvider_h
#define __albaMeshLinearizationProvider_h

class vtkALBAMeshLinearizationFilter;
class vtkAlgorithmOutput;

/**Provides shared access to mesh linearization filters.

This class allows multiple visualization pipelines to reuse the same
vtkALBAMeshLinearizationFilter for a given mesh input, avoiding redundant
linearization operations and reducing both computational cost and memory
usage.

It is particularly useful in visualization pipelines where the same mesh
is processed by multiple views or filters, such as when generating slices
of a mesh in several sub-views.

The linearization filter is automatically created when first requested and
released when no longer referenced.
*/
class albaMeshLinearizationProvider
{

public:
	albaMeshLinearizationProvider();
	~albaMeshLinearizationProvider();

	/** Get a linearizzation filter for the specific output port, creates a new filter or return an existing one */
	vtkALBAMeshLinearizationFilter *GetFilter(vtkAlgorithmOutput *port);

	/** Release the linearizzation filter for the specific output port, if the reference count is 0, delete it */
	void ReleaseFilter();

protected:
	/** Get the index of the filter for the specific output port, returns -1 if not found */
	int GetFilterIndex(vtkAlgorithmOutput *port);

	vtkAlgorithmOutput *m_Port;
};

#endif