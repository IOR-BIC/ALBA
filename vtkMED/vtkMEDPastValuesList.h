/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: vtkMEDPastValuesList.h,v $
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
  int m_CurrentNumberOfValues ;
  int m_MaxNumberOfValues ;
  double *m_Values ;
};


#endif
