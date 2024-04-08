/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkALBADataPipe
 Authors: Marco Petrone
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __vtkALBADataPipe_h
#define __vtkALBADataPipe_h
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaObserver.h"
#include "vtkDataSetAlgorithm.h"
//----------------------------------------------------------------------------
// forward declarations
//----------------------------------------------------------------------------
class albaDataPipe;
class vtkDataSet;

/** bridge class linking VTK pipeline to VME data pipe update mechanism.
  This object is a bridge between VTK pipeline update mechanism and ALBA
  VME.
  @sa mflInterpolator
 
  @todo
  -
*/
class ALBA_EXPORT vtkALBADataPipe : public vtkDataSetAlgorithm 
{
public:
  vtkTypeMacro(vtkALBADataPipe,vtkDataSetAlgorithm );

  static vtkALBADataPipe *New();

  /** Set the dataset to be reported as output of the VTK data pipe */
  virtual void SetNthInput(int num, vtkDataSet *input);


  /** A bit of magic making this filter to take into consideration VME data pipe MTime */
	vtkMTimeType GetMTime();

  /** return the modification time for internally stored information */
  virtual unsigned long GetInformationTime();

  /** the data pipe it is linked to */
  void SetDataPipe(albaDataPipe *dpipe);

  /** return the data pipe connected to this object */
  albaDataPipe *GetDataPipe() {return m_DataPipe;}

  /** 
    Overridden to attempt doing something before the pipeline is checked for the
    MTime (i.e. change the inputs) */
  virtual void UpdateInformation();

protected:
  vtkALBADataPipe();
  virtual ~vtkALBADataPipe();

  virtual int RequestInformation(vtkInformation *request, vtkInformationVector **inputVector, vtkInformationVector *outputVector);
  int RequestData(vtkInformation *vtkNotUsed(request),	vtkInformationVector **inputVector,	vtkInformationVector *outputVector);

  albaDataPipe *m_DataPipe; ///< the data pipe this object is linked to

private:
  vtkALBADataPipe(const vtkALBADataPipe&); // Not implemented
  void operator=(const vtkALBADataPipe&); // Not implemented
  
};

#endif /* __vtkALBADataPipe_h */
 
