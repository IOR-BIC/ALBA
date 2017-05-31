/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: vtkMAFSubdividePolylines.cxx,v $
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
#include "vtkMAFPolyDataNavigator.h"
#include "vtkMAFSubdividePolylines.h"

#include <ostream>
#include <cmath>
#include "vtkInformation.h"
#include "vtkInformationVector.h"



vtkStandardNewMacro(vtkMAFSubdividePolylines);



//------------------------------------------------------------------------------
// Constructor 
//------------------------------------------------------------------------------
vtkMAFSubdividePolylines::vtkMAFSubdividePolylines()
  : m_SizeMode(USE_NUMBER_OF_SUBDIVS), m_NumberOfSubdivisions(2)
{
  m_Nav = vtkMAFPolyDataNavigator::New() ;
}



//------------------------------------------------------------------------------
// Destructor 
//------------------------------------------------------------------------------
vtkMAFSubdividePolylines::~vtkMAFSubdividePolylines()
{
  m_Nav->Delete() ;
}




//------------------------------------------------------------------------------
int vtkMAFSubdividePolylines::RequestData( vtkInformation *vtkNotUsed(request), vtkInformationVector **inputVector, vtkInformationVector *outputVector)
{
	// get the info objects
	vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
	vtkInformation *outInfo = outputVector->GetInformationObject(0);

	// Initialize some frequently used values.
	vtkPolyData  *input = vtkPolyData::SafeDownCast(inInfo->Get(vtkDataObject::DATA_OBJECT()));
	vtkPolyData *output = vtkPolyData::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT()));

	vtkDebugMacro(<< "Executing vtkMAFSubdividePolylines Filter") ;

  // pointers to input and output
  m_Input = input;
  m_Output = output;

  m_Output->DeepCopy(m_Input) ;

  // get edges 
  vtkMAFPolyDataNavigator::EdgeVector edges ;
  m_Nav->GetAllEdges(m_Output, edges) ;

  // get subdivisions required for each edge
  std::vector<std::vector<double> > lambda ;
  for (int i = 0 ;  i < (int)edges.size() ;  i++){
    vtkMAFPolyDataNavigator::Edge edge = edges[i] ;

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

	return 1;
}



//------------------------------------------------------------------------------
// Set nominal size of subdivided segments. \n
// Argument is absolute value.
//------------------------------------------------------------------------------
void vtkMAFSubdividePolylines::SetSegmentSizeAbsolute(double size)
{
  m_SegmentSizeAbs = size ;
  m_SizeMode = USE_ABSOLUTE_VALUE ;
  this->Modified() ;
}



//------------------------------------------------------------------------------
// Set no. of subdivisions.
//------------------------------------------------------------------------------
void vtkMAFSubdividePolylines::SetNumberOfSubdivisions(int n)
{
  m_NumberOfSubdivisions = n ;
  m_SizeMode = USE_NUMBER_OF_SUBDIVS ;
  this->Modified() ;
}



//------------------------------------------------------------------------------
// Print self
//------------------------------------------------------------------------------
void vtkMAFSubdividePolylines::PrintSelf(ostream& os, vtkIndent indent) const
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
