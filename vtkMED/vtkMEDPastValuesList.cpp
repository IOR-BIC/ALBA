/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: vtkMEDPastValuesList.cpp,v $
Language:  C++
Date:      $Date: 2008-11-06 09:27:27 $
Version:   $Revision: 1.2.2.1 $
Authors:   Nigel McFarlane

================================================================================
Copyright (c) 2008 University of Bedfordshire, UK (www.beds.ac.uk)
All rights reserved.
===============================================================================*/




#include "mafDefines.h"
#include "vtkMEDPastValuesList.h"
#include <algorithm>
#include <iostream>
#include <ostream>
#include <assert.h>



//------------------------------------------------------------------------------
// Constructor
vtkMEDPastValuesList::vtkMEDPastValuesList(int NumberOfvtkMEDPastValues) : MaxNumberOfValues(NumberOfvtkMEDPastValues), CurrentNumberOfValues(0)
//------------------------------------------------------------------------------
{
  Values = new double[NumberOfvtkMEDPastValues] ;
}


//------------------------------------------------------------------------------
// Deconstructor
vtkMEDPastValuesList::~vtkMEDPastValuesList()
//------------------------------------------------------------------------------
{
  delete [] Values ;
}


//------------------------------------------------------------------------------
// Copy constructor
vtkMEDPastValuesList::vtkMEDPastValuesList(const vtkMEDPastValuesList& stats)
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
void vtkMEDPastValuesList::Clear()
//------------------------------------------------------------------------------
{
  CurrentNumberOfValues = 0 ;
}


//------------------------------------------------------------------------------
// Set new value
void vtkMEDPastValuesList::AddNewValue(double newValue)
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
double vtkMEDPastValuesList::GetValue(int i) const
//------------------------------------------------------------------------------
{
  if ((i < 0) || (i >= CurrentNumberOfValues)){
    std::cout << "vtkMEDPastValuesList::GetValue() - index " << i << " out of range" << std::endl ;
    assert(false) ;
  }

  return Values[i] ;
}



//------------------------------------------------------------------------------
// Get minimum value
double vtkMEDPastValuesList::GetMin() const
//------------------------------------------------------------------------------
{
  if (CurrentNumberOfValues == 0){
    std::cout << "vtkMEDPastValuesList::GetMin() - no values found" << std::endl ;
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
double vtkMEDPastValuesList::GetMax() const
//------------------------------------------------------------------------------
{
  if (CurrentNumberOfValues == 0){
    std::cout << "vtkMEDPastValuesList::GetMax() - no values found" << std::endl ;
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
double vtkMEDPastValuesList::GetMean() const
//------------------------------------------------------------------------------
{
  if (CurrentNumberOfValues == 0){
    std::cout << "vtkMEDPastValuesList::GetMean() - no values found" << std::endl ;
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
double vtkMEDPastValuesList::GetMedian() const
//------------------------------------------------------------------------------
{
  if (CurrentNumberOfValues == 0){
    std::cout << "vtkMEDPastValuesList::GetMedian() - no values found" << std::endl ;
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
void vtkMEDPastValuesList::PrintSelf(std::ostream& os) const
//------------------------------------------------------------------------------
{
  os << "vtkMEDPastValuesList: no. of values = " << CurrentNumberOfValues << " max no. of values = " << MaxNumberOfValues << std::endl ;
  for (int i = 0 ;  i < CurrentNumberOfValues ;  i++)
    os << i << " " << Values[i] << std::endl ;
  os << std::endl ;
}


