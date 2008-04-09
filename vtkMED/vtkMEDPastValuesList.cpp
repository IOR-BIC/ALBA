/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: vtkMEDPastValuesList.cpp,v $
Language:  C++
Date:      $Date: 2008-04-09 09:14:17 $
Version:   $Revision: 1.1 $
Authors:   Nigel McFarlane
==========================================================================
Copyright (c) 2002/2007
SCS s.r.l. - BioComputing Competence Centre (www.scsolutions.it - www.b3c.it)

MafMedical Library use license agreement

The software named MafMedical Library and any accompanying documentation, 
manuals or data (hereafter collectively "SOFTWARE") is property of the SCS s.r.l.
This is an open-source copyright as follows:
Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:
* Redistributions of source code must retain the above copyright notice, 
this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice, 
this list of conditions and the following disclaimer in the documentation and/or 
other materials provided with the distribution.
* Modified source versions must be plainly marked as such, and must not be misrepresented 
as being the original software.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 'AS IS' 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
ARE DISCLAIMED. IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE FOR 
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND 
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

MafMedical is partially based on OpenMAF.
=========================================================================*/



#include "mafDefines.h"
#include "vtkMEDPastValuesList.h"
#include <algorithm>
#include <iostream>
#include <ostream>
#include <assert.h>



//------------------------------------------------------------------------------
// Constructor
vtkMEDPastValuesList::vtkMEDPastValuesList(int NumberOfvtkMEDPastValues) : m_MaxNumberOfValues(NumberOfvtkMEDPastValues), m_CurrentNumberOfValues(0)
//------------------------------------------------------------------------------
{
  m_Values = new double[NumberOfvtkMEDPastValues] ;
}


//------------------------------------------------------------------------------
// Deconstructor
vtkMEDPastValuesList::~vtkMEDPastValuesList()
//------------------------------------------------------------------------------
{
  delete [] m_Values ;
}


//------------------------------------------------------------------------------
// Copy constructor
vtkMEDPastValuesList::vtkMEDPastValuesList(const vtkMEDPastValuesList& stats)
//------------------------------------------------------------------------------
{
  this->m_CurrentNumberOfValues = stats.m_CurrentNumberOfValues ;
  this->m_MaxNumberOfValues = stats.m_MaxNumberOfValues ;

  m_Values = new double[m_MaxNumberOfValues] ;
  for (int i = 0 ;  i < m_CurrentNumberOfValues ;  i++)
    this->m_Values[i] = stats.m_Values[i] ;
}



//------------------------------------------------------------------------------
// Clear to empty
void vtkMEDPastValuesList::Clear()
//------------------------------------------------------------------------------
{
  m_CurrentNumberOfValues = 0 ;
}


//------------------------------------------------------------------------------
// Set new value
void vtkMEDPastValuesList::AddNewValue(double newValue)
//------------------------------------------------------------------------------
{
  if (m_CurrentNumberOfValues < m_MaxNumberOfValues)
    m_CurrentNumberOfValues++ ;

  for (int i = m_CurrentNumberOfValues-1 ;  i > 0 ;  i--)
    m_Values[i] = m_Values[i-1] ;

  m_Values[0] = newValue ;
}


//------------------------------------------------------------------------------
// Get ith value
double vtkMEDPastValuesList::GetValue(int i) const
//------------------------------------------------------------------------------
{
  if ((i < 0) || (i >= m_CurrentNumberOfValues)){
    std::cout << "vtkMEDPastValuesList::GetValue() - index " << i << " out of range" << std::endl ;
    assert(false) ;
  }

  return m_Values[i] ;
}



//------------------------------------------------------------------------------
// Get minimum value
double vtkMEDPastValuesList::GetMin() const
//------------------------------------------------------------------------------
{
  if (m_CurrentNumberOfValues == 0){
    std::cout << "vtkMEDPastValuesList::GetMin() - no values found" << std::endl ;
    assert(false) ;
  }

  double minval = m_Values[0] ;
  for (int i = 1 ;  i < m_CurrentNumberOfValues ;  i++){
    if (m_Values[i] < minval)
      minval = m_Values[i] ;
  }
  return minval ;
}


//------------------------------------------------------------------------------
// Get maximum value
double vtkMEDPastValuesList::GetMax() const
//------------------------------------------------------------------------------
{
  if (m_CurrentNumberOfValues == 0){
    std::cout << "vtkMEDPastValuesList::GetMax() - no values found" << std::endl ;
    assert(false) ;
  }

  double maxval = m_Values[0] ;
  for (int i = 1 ;  i < m_CurrentNumberOfValues ;  i++){
    if (m_Values[i] > maxval)
      maxval = m_Values[i] ;
  }
  return maxval ;
}


//------------------------------------------------------------------------------
// Get mean value
double vtkMEDPastValuesList::GetMean() const
//------------------------------------------------------------------------------
{
  if (m_CurrentNumberOfValues == 0){
    std::cout << "vtkMEDPastValuesList::GetMean() - no values found" << std::endl ;
    assert(false) ;
  }

  double sum = 0.0 ;
  for (int i = 0 ;  i < m_CurrentNumberOfValues ;  i++)
    sum += m_Values[i] ;
  sum /= (double)m_CurrentNumberOfValues ;
  return sum ;
}



//------------------------------------------------------------------------------
// Get median value
double vtkMEDPastValuesList::GetMedian() const
//------------------------------------------------------------------------------
{
  if (m_CurrentNumberOfValues == 0){
    std::cout << "vtkMEDPastValuesList::GetMedian() - no values found" << std::endl ;
    assert(false) ;
  }

  // copy to array and sort
  double *temp = new double[m_CurrentNumberOfValues] ;

  for (int i = 0 ;  i < m_CurrentNumberOfValues ;  i++)
    temp[i] = m_Values[i] ;
  std::sort<double*>(temp, temp+m_CurrentNumberOfValues) ;

  double median ;
  int odd = m_CurrentNumberOfValues - 2*int(m_CurrentNumberOfValues / 2) ;
  if (odd)
    median = temp[(m_CurrentNumberOfValues-1) / 2] ;
  else{
    int idx = m_CurrentNumberOfValues / 2 ;
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
  os << "vtkMEDPastValuesList: no. of values = " << m_CurrentNumberOfValues << " max no. of values = " << m_MaxNumberOfValues << std::endl ;
  for (int i = 0 ;  i < m_CurrentNumberOfValues ;  i++)
    os << i << " " << m_Values[i] << std::endl ;
  os << std::endl ;
}


