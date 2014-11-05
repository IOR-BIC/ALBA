/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: vtkMEDSubdividePolylines.cxx,v $
Language:  C++
Date:      $Date: 2009-10-08 13:44:29 $
Version:   $Revision: 1.1.2.5 $
Authors:   Nigel McFarlane
==========================================================================
Copyright (c) 2012
University of Bedfordshire
=========================================================================*/

#include "vtkObjectFactory.h"
#include "vtkPolyData.h"
#include "vtkIdList.h"
#include "vtkMEDPolyDataNavigator.h"
#include "vtkMEDSubdividePolylines.h"

#include <ostream>
#include <cmath>



vtkCxxRevisionMacro(vtkMEDSubdividePolylines, "$Revision: 1.61 $");
vtkStandardNewMacro(vtkMEDSubdividePolylines);



//------------------------------------------------------------------------------
// Constructor 
//------------------------------------------------------------------------------
vtkMEDSubdividePolylines::vtkMEDSubdividePolylines()
  : m_SizeMode(USE_NUMBER_OF_SUBDIVS), m_NumberOfSubdivisions(2)
{
  m_Nav = vtkMEDPolyDataNavigator::New() ;
}



//------------------------------------------------------------------------------
// Destructor 
//------------------------------------------------------------------------------
vtkMEDSubdividePolylines::~vtkMEDSubdividePolylines()
{
  m_Nav->Delete() ;
}




//------------------------------------------------------------------------------
// Execute method
//------------------------------------------------------------------------------
void vtkMEDSubdividePolylines::Execute()
{
  vtkDebugMacro(<< "Executing vtkMEDSubdividePolylines Filter") ;

  // pointers to input and output
  m_Input = this->GetInput() ;
  m_Output = this->GetOutput() ;

  m_Output->DeepCopy(m_Input) ;

  // get edges 
  vtkMEDPolyDataNavigator::EdgeVector edges ;
  m_Nav->GetAllEdges(m_Output, edges) ;

  // get subdivisions required for each edge
  std::vector<std::vector<double> > lambda ;
  for (int i = 0 ;  i < (int)edges.size() ;  i++){
    vtkMEDPolyDataNavigator::Edge edge = edges[i] ;

    if (m_SizeMode == USE_ABSOLUTE_VALUE){
      double length = m_Nav->GetLengthOfEdge(m_Output, edge) ;
      m_NumberOfSubdivisions = std::ceil(length / m_SegmentSizeAbs) ;
    }

    double lambda1 = 1.0/(double)m_NumberOfSubdivisions ;
    std::vector<double> thisvector ;
    for (int j = 1 ;  j < m_NumberOfSubdivisions ;  j++){
      double lamj = (double)j * lambda1 ;
      thisvector.push_back(lamj) ;
    }
    
    lambda.push_back(thisvector) ;
  }

  // do the subdivision
  std::vector<std::vector<int> > newPtIds ;
  m_Nav->AddPointsToEdges(m_Output, edges, lambda, newPtIds) ;
}



//------------------------------------------------------------------------------
// Set nominal size of subdivided segments. \n
// Argument is absolute value.
//------------------------------------------------------------------------------
void vtkMEDSubdividePolylines::SetSegmentSizeAbsolute(double size)
{
  m_SegmentSizeAbs = size ;
  m_SizeMode = USE_ABSOLUTE_VALUE ;
  this->Modified() ;
}



//------------------------------------------------------------------------------
// Set no. of subdivisions.
//------------------------------------------------------------------------------
void vtkMEDSubdividePolylines::SetNumberOfSubdivisions(int n)
{
  m_NumberOfSubdivisions = n ;
  m_SizeMode = USE_NUMBER_OF_SUBDIVS ;
  this->Modified() ;
}



//------------------------------------------------------------------------------
// Print self
//------------------------------------------------------------------------------
void vtkMEDSubdividePolylines::PrintSelf(ostream& os, vtkIndent indent) const
{
  switch(m_SizeMode){
  case USE_ABSOLUTE_VALUE:
    os << "size (abs) " << m_SegmentSizeAbs << "\n" ;
    break ;
  case USE_NUMBER_OF_SUBDIVS:
    os << "no. of subdivisions " << m_NumberOfSubdivisions << "\n" ;
    break ;
  default:
    assert(false) ;
  }
}
