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

#include "vtkMAFPastValuesList.h"
#include <algorithm>
#include <iostream>
#include <ostream>
#include <assert.h>



//------------------------------------------------------------------------------
// Constructor
vtkMAFPastValuesList::vtkMAFPastValuesList(int NumberOfvtkMAFPastValues) : MaxNumberOfValues(NumberOfvtkMAFPastValues), CurrentNumberOfValues(0)
//------------------------------------------------------------------------------
{
  Values = new double[NumberOfvtkMAFPastValues] ;
}


//------------------------------------------------------------------------------
// Deconstructor
vtkMAFPastValuesList::~vtkMAFPastValuesList()
//------------------------------------------------------------------------------
{
  delete [] Values ;
}


//------------------------------------------------------------------------------
// Copy constructor
vtkMAFPastValuesList::vtkMAFPastValuesList(const vtkMAFPastValuesList& stats)
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
void vtkMAFPastValuesList::Clear()
//------------------------------------------------------------------------------
{
  CurrentNumberOfValues = 0 ;
}


//------------------------------------------------------------------------------
// Set new value
void vtkMAFPastValuesList::AddNewValue(double newValue)
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
double vtkMAFPastValuesList::GetValue(int i) const
//------------------------------------------------------------------------------
{
  if ((i < 0) || (i >= CurrentNumberOfValues)){
    std::cout << "vtkMAFPastValuesList::GetValue() - index " << i << " out of range" << std::endl ;
    assert(false) ;
  }

  return Values[i] ;
}



//------------------------------------------------------------------------------
// Get minimum value
double vtkMAFPastValuesList::GetMin() const
//------------------------------------------------------------------------------
{
  if (CurrentNumberOfValues == 0){
    std::cout << "vtkMAFPastValuesList::GetMin() - no values found" << std::endl ;
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
double vtkMAFPastValuesList::GetMax() const
//------------------------------------------------------------------------------
{
  if (CurrentNumberOfValues == 0){
    std::cout << "vtkMAFPastValuesList::GetMax() - no values found" << std::endl ;
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
double vtkMAFPastValuesList::GetMean() const
//------------------------------------------------------------------------------
{
  if (CurrentNumberOfValues == 0){
    std::cout << "vtkMAFPastValuesList::GetMean() - no values found" << std::endl ;
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
double vtkMAFPastValuesList::GetMedian() const
//------------------------------------------------------------------------------
{
  if (CurrentNumberOfValues == 0){
    std::cout << "vtkMAFPastValuesList::GetMedian() - no values found" << std::endl ;
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
void vtkMAFPastValuesList::PrintSelf(std::ostream& os) const
//------------------------------------------------------------------------------
{
  os << "vtkMAFPastValuesList: no. of values = " << CurrentNumberOfValues << " max no. of values = " << MaxNumberOfValues << std::endl ;
  for (int i = 0 ;  i < CurrentNumberOfValues ;  i++)
    os << i << " " << Values[i] << std::endl ;
  os << std::endl ;
}


