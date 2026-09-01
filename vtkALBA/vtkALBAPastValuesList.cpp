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

#include "vtkALBAPastValuesList.h"
#include <algorithm>
#include <iostream>
#include <ostream>
#include <assert.h>



//------------------------------------------------------------------------------
// Constructor
vtkALBAPastValuesList::vtkALBAPastValuesList(int NumberOfvtkALBAPastValues) : MaxNumberOfValues(NumberOfvtkALBAPastValues), CurrentNumberOfValues(0)
//------------------------------------------------------------------------------
{
  Values = new double[NumberOfvtkALBAPastValues] ;
}


//------------------------------------------------------------------------------
// Deconstructor
vtkALBAPastValuesList::~vtkALBAPastValuesList()
//------------------------------------------------------------------------------
{
  delete [] Values ;
}


//------------------------------------------------------------------------------
// Copy constructor
vtkALBAPastValuesList::vtkALBAPastValuesList(const vtkALBAPastValuesList& stats)
//------------------------------------------------------------------------------
{
  this->CurrentNumberOfValues = stats.CurrentNumberOfValues ;
  this->MaxNumberOfValues = stats.MaxNumberOfValues ;

  Values = new double[MaxNumberOfValues] ;
  for (int i = 0 ;  i < CurrentNumberOfValues ;  i++)
    this->Values[i] = stats.Values[i] ;
}



//------------------------------------------------------------------------------
// Clear to empty
void vtkALBAPastValuesList::Clear()
//------------------------------------------------------------------------------
{
  CurrentNumberOfValues = 0 ;
}


//------------------------------------------------------------------------------
// Set new value
void vtkALBAPastValuesList::AddNewValue(double newValue)
//------------------------------------------------------------------------------
{
  if (CurrentNumberOfValues < MaxNumberOfValues)
    CurrentNumberOfValues++ ;

  for (int i = CurrentNumberOfValues-1 ;  i > 0 ;  i--)
    Values[i] = Values[i-1] ;

  Values[0] = newValue ;
}


//------------------------------------------------------------------------------
// Get ith value
double vtkALBAPastValuesList::GetValue(int i) const
//------------------------------------------------------------------------------
{
  if ((i < 0) || (i >= CurrentNumberOfValues)){
    std::cout << "vtkALBAPastValuesList::GetValue() - index " << i << " out of range" << std::endl ;
    assert(false) ;
  }

  return Values[i] ;
}



//------------------------------------------------------------------------------
// Get minimum value
double vtkALBAPastValuesList::GetMin() const
//------------------------------------------------------------------------------
{
  if (CurrentNumberOfValues == 0){
    std::cout << "vtkALBAPastValuesList::GetMin() - no values found" << std::endl ;
    assert(false) ;
  }

  double minval = Values[0] ;
  for (int i = 1 ;  i < CurrentNumberOfValues ;  i++){
    if (Values[i] < minval)
      minval = Values[i] ;
  }
  return minval ;
}


//------------------------------------------------------------------------------
// Get maximum value
double vtkALBAPastValuesList::GetMax() const
//------------------------------------------------------------------------------
{
  if (CurrentNumberOfValues == 0){
    std::cout << "vtkALBAPastValuesList::GetMax() - no values found" << std::endl ;
    assert(false) ;
  }

  double maxval = Values[0] ;
  for (int i = 1 ;  i < CurrentNumberOfValues ;  i++){
    if (Values[i] > maxval)
      maxval = Values[i] ;
  }
  return maxval ;
}


//------------------------------------------------------------------------------
// Get mean value
double vtkALBAPastValuesList::GetMean() const
//------------------------------------------------------------------------------
{
  if (CurrentNumberOfValues == 0){
    std::cout << "vtkALBAPastValuesList::GetMean() - no values found" << std::endl ;
    assert(false) ;
  }

  double sum = 0.0 ;
  for (int i = 0 ;  i < CurrentNumberOfValues ;  i++)
    sum += Values[i] ;
  sum /= (double)CurrentNumberOfValues ;
  return sum ;
}



//------------------------------------------------------------------------------
// Get median value
double vtkALBAPastValuesList::GetMedian() const
//------------------------------------------------------------------------------
{
  if (CurrentNumberOfValues == 0){
    std::cout << "vtkALBAPastValuesList::GetMedian() - no values found" << std::endl ;
    assert(false) ;
  }

  // copy to array and sort
  double *temp = new double[CurrentNumberOfValues] ;

  for (int i = 0 ;  i < CurrentNumberOfValues ;  i++)
    temp[i] = Values[i] ;
  std::sort<double*>(temp, temp+CurrentNumberOfValues) ;

  double median ;
  int odd = CurrentNumberOfValues - 2*int(CurrentNumberOfValues / 2) ;
  if (odd)
    median = temp[(CurrentNumberOfValues-1) / 2] ;
  else{
    int idx = CurrentNumberOfValues / 2 ;
    median = (temp[idx] + temp[idx-1]) / 2.0 ;
  }

  delete [] temp ;

  return median ;

}



//------------------------------------------------------------------------------
// Print self
void vtkALBAPastValuesList::PrintSelf(std::ostream& os) const
//------------------------------------------------------------------------------
{
  os << "vtkALBAPastValuesList: no. of values = " << CurrentNumberOfValues << " max no. of values = " << MaxNumberOfValues << std::endl ;
  for (int i = 0 ;  i < CurrentNumberOfValues ;  i++)
    os << i << " " << Values[i] << std::endl ;
  os << std::endl ;
}
