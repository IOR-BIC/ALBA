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


#ifndef __vtkALBATIFFReader_h
#define __vtkALBATIFFReader_h
#include "vtkTIFFReader.h"


class ALBA_EXPORT vtkALBATIFFReader : public vtkTIFFReader
{
public:
  static vtkALBATIFFReader *New();
  vtkTypeMacro(vtkALBATIFFReader, vtkTIFFReader);

protected:
  vtkALBATIFFReader();
  ~vtkALBATIFFReader();

	void ExecuteDataWithInformation(vtkDataObject *out, vtkInformation *outInfo) override;

  void LoadDataFromWX();

private:
  vtkALBATIFFReader(const vtkALBATIFFReader&);  // Not implemented.
  void operator=(const vtkALBATIFFReader&);  // Not implemented.
};
#endif


