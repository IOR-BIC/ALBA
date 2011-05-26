/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: vtkMEDPastValuesList.h,v $
Language:  C++
Date:      $Date: 2011-05-26 08:33:31 $
Version:   $Revision: 1.2.2.3 $
Authors:   Nigel McFarlane

================================================================================
Copyright (c) 2008 University of Bedfordshire, UK (www.beds.ac.uk)
All rights reserved.
===============================================================================*/

#ifndef __vtkMEDPastValuesList_h
#define __vtkMEDPastValuesList_h

#include "vtkMEDConfigure.h"
#include <ostream>


/** 
class name: vtkMEDPastValuesList.
Used for saving the last n values of a variable.
It is a push-only stack with a maxiumum of n values.
Values are pushed onto the top of the list.  
Values which fall of the bottom of the list are lost 
*/
class VTK_vtkMED_EXPORT vtkMEDPastValuesList
{
public:
  /** construct class with max no. of past values */
  explicit vtkMEDPastValuesList(int NumberOfvtkMEDPastValues) ;
  /** destructor */
  ~vtkMEDPastValuesList() ;
  /** copy constructor */
  vtkMEDPastValuesList(const vtkMEDPastValuesList& stats) ;

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
