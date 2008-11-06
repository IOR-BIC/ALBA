/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: vtkMEDPastValuesList.h,v $
Language:  C++
Date:      $Date: 2008-11-06 09:27:28 $
Version:   $Revision: 1.2.2.1 $
Authors:   Nigel McFarlane

================================================================================
Copyright (c) 2008 University of Bedfordshire, UK (www.beds.ac.uk)
All rights reserved.
===============================================================================*/

#ifndef __vtkMEDPastValuesList_h
#define __vtkMEDPastValuesList_h

#include "vtkMAFConfigure.h"


//------------------------------------------------------------------------------
/** vtkMEDPastValuesList.
Used for saving the last n values of a variable.
It is a push-only stack with a maxiumum of n values.
Values are pushed onto the top of the list.  
Values which fall of the bottom of the list are lost */
//------------------------------------------------------------------------------
class VTK_vtkMAF_EXPORT vtkMEDPastValuesList
{
public:
  explicit vtkMEDPastValuesList(int NumberOfvtkMEDPastValues) ;   ///< construct class with max no. of past values
  ~vtkMEDPastValuesList() ;
  vtkMEDPastValuesList(const vtkMEDPastValuesList& stats) ;

  void Clear() ;                                    ///< reset to empty
  void AddNewValue(double newValue) ;               ///< add new value to saved list
  double GetValue(int i) const ;                    ///< get ith value
  double GetMin() const ;                           ///< get min of saved values
  double GetMax() const ;                           ///< get max of saved values
  double GetMean() const ;                          ///< get mean of saved values
  double GetMedian() const ;                        ///< get median of saved values

  void PrintSelf(std::ostream& os) const ;

private:
  int CurrentNumberOfValues ;
  int MaxNumberOfValues ;
  double *Values ;
};


#endif
