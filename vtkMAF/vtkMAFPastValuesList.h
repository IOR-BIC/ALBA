/*=========================================================================

 Program: MAF2
 Module: vtkMAFPastValuesList
 Authors: Nigel McFarlane
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __vtkMAFPastValuesList_h
#define __vtkMAFPastValuesList_h

#include "mafConfigure.h"
#include <ostream>


/** 
class name: vtkMAFPastValuesList.
Used for saving the last n values of a variable.
It is a push-only stack with a maxiumum of n values.
Values are pushed onto the top of the list.  
Values which fall of the bottom of the list are lost 
*/
class MAF_EXPORT vtkMAFPastValuesList
{
public:
  /** construct class with max no. of past values */
  explicit vtkMAFPastValuesList(int NumberOfvtkMAFPastValues) ;
  /** destructor */
  ~vtkMAFPastValuesList() ;
  /** copy constructor */
  vtkMAFPastValuesList(const vtkMAFPastValuesList& stats) ;

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
