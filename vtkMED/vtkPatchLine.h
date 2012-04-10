#ifndef __vtkPatchLine_h
#define __vtkPatchLine_h

#include "vtkMEDConfigure.h"
#include "vtkPolyDataToPolyDataFilter.h"
#include <vtkSmartPointer.h>
#include <vtkPolyData.h>
#include <vtkCellArray.h>
#include <vtkMath.h>
#include <string>
#include <limits>
#include <cmath>
#include <vtkPolyLine.h>
#include <list>

class VTK_vtkMED_EXPORT vtkPatchLine : public vtkPolyDataToPolyDataFilter
{
	public:
		static vtkPatchLine *New();
		virtual void ExecuteData(vtkDataObject *output);
};

#endif