/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkALBAMeshLinearizationFilter
 Authors: Gianluigi Crimi

 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __vtkALBAMeshLinearizationFilter_h
#define __vtkALBAMeshLinearizationFilter_h

#include "vtkUnstructuredGridAlgorithm.h"
#include "albaConfigure.h"

/**
 * Converts the non-linear cells of an unstructured grid into the
 * corresponding linear cells by discarding every point that is not a
 * corner of the cell (mid-edge, mid-face and mid-volume points).
 *
 * The cell order is preserved, so cell data remains valid. Linear cells
 * are copied unchanged. The geometry of curved elements is replaced by
 * its linear control mesh.
 *
 * Supported non-linear types: quadratic, biquadratic, triquadratic,
 * cubic line, Lagrange and Bezier cells. Grids containing polyhedral
 * cells are rejected.
 */
class ALBA_EXPORT vtkALBAMeshLinearizationFilter : public vtkUnstructuredGridAlgorithm
{
public:
  static vtkALBAMeshLinearizationFilter *New();
  vtkTypeMacro(vtkALBAMeshLinearizationFilter, vtkUnstructuredGridAlgorithm);
  void PrintSelf(ostream &os, vtkIndent indent) override;

  //@{
  /**
   * When on, points that are no longer referenced by any cell are removed
   * from the output and point data is remapped accordingly.
   * When off, the input point array is shared with the output.
   * Default is off.
   */
  vtkSetMacro(RemoveUnusedPoints, vtkTypeBool);
  vtkGetMacro(RemoveUnusedPoints, vtkTypeBool);
  vtkBooleanMacro(RemoveUnusedPoints, vtkTypeBool);
  //@}

protected:
  vtkALBAMeshLinearizationFilter();
  ~vtkALBAMeshLinearizationFilter() override = default;

  int RequestData(vtkInformation *request,
    vtkInformationVector **inputVector,
    vtkInformationVector *outputVector) override;

	/* Maps a non - linear cell type to its linear counterpart.
  Returns false if the cell type is not a supported non-linear type.
  On success, linearType and numCornerPoints are set.*/
	static bool GetLinearCellInfo(int cellType, int &linearType, vtkIdType &numCornerPoints);

  vtkTypeBool RemoveUnusedPoints;

private:
  vtkALBAMeshLinearizationFilter(const vtkALBAMeshLinearizationFilter &) = delete;
  void operator=(const vtkALBAMeshLinearizationFilter &) = delete;
};

#endif