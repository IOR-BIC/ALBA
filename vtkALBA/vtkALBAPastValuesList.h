/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkALBAPastValuesList
 Authors: Nigel McFarlane
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __vtkALBAPastValuesList_h
#define __vtkALBAPastValuesList_h

#include "albaConfigure.h"
#include <ostream>


/** 
class name: vtkALBAPastValuesList.
Used for saving the last n values of a variable.
It is a push-only stack with a maxiumum of n values.
Values are pushed onto the top of the list.  
Values which fall of the bottom of the list are lost 
*/
class ALBA_EXPORT vtkALBAPastValuesList
{
public:
  /** construct class with max no. of past values */
  explicit vtkALBAPastValuesList(int NumberOfvtkALBAPastValues) ;
  /** destructor */
  ~vtkALBAPastValuesList() ;
  /** copy constructor */
  vtkALBAPastValuesList(const vtkALBAPastValuesList& stats) ;

  /** reset to empty */
  void Clear() ;
  /** add new value to saved list */
  void AddNewValue(double newValue) ;              
  /** get ith value */
  double GetValue(int i) const ;                    
  /** get min of saved values */
  double GetMin() const ;                          
  /** get max of saved values */
  double GetMax() const ;                          
  /** get mean of saved values */
  double GetMean() const ;                         
  /** get median of saved values */
  double GetMedian() const ;                       
  /** print object information */
  void PrintSelf(std::ostream& os) const ;

private:
  int CurrentNumberOfValues ;
  int MaxNumberOfValues ;
  double *Values ;
};


#endif
