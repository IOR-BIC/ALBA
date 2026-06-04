/*=========================================================================

Program: ALBA
Module: vtkALBAEllipseSource
Authors: Gianluigi Crimi

Copyright (c) BIC
All rights reserved. See Copyright.txt or
http://www.scsitaly.com/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __vtkALBAConnectedRegionsContourTriangulator_h
#define __vtkALBAConnectedRegionsContourTriangulator_h

#include <vtkPolyDataAlgorithm.h>

class vtkALBAConnectedRegionsContourTriangulator : public vtkPolyDataAlgorithm
{
public:
	static vtkALBAConnectedRegionsContourTriangulator *New();
	vtkTypeMacro(vtkALBAConnectedRegionsContourTriangulator, vtkPolyDataAlgorithm);

protected:
	vtkALBAConnectedRegionsContourTriangulator() = default;
	~vtkALBAConnectedRegionsContourTriangulator() override = default;

	int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

private:
	vtkALBAConnectedRegionsContourTriangulator(const vtkALBAConnectedRegionsContourTriangulator &) = delete;
	void operator=(const vtkALBAConnectedRegionsContourTriangulator &) = delete;
};

#endif