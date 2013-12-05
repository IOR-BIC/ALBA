/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: vtkMAFBridgeHoleFilter.cxx,v $
Language:  C++
Date:      $Date: 2009-10-08 13:44:29 $
Version:   $Revision: 1.1.2.5 $
Authors:   Nigel McFarlane
==========================================================================
Copyright (c) 2012
University of Bedfordshire
=========================================================================*/

#include "vtkMEDAddScalarsFilter.h"
#include "vtkObjectFactory.h"
#include "vtkPolyData.h"
#include "vtkDataArray.h"
#include "vtkUnsignedCharArray.h"
#include "vtkPointData.h"
#include "vtkCellData.h"
#include "assert.h"




vtkCxxRevisionMacro(vtkMEDAddScalarsFilter, "$Revision: 1.61 $");
vtkStandardNewMacro(vtkMEDAddScalarsFilter);



//------------------------------------------------------------------------------
// Constructor 
//------------------------------------------------------------------------------
vtkMEDAddScalarsFilter::vtkMEDAddScalarsFilter()
  : m_AttribMode(POINT_SCALARS), m_NumberOfComponents(3)
{
  m_Color[0] = 255 ;
  m_Color[1] = 255 ;
  m_Color[2] = 0 ;
  m_Color[3] = 255 ;

  char defaultName[256] = "NewScalars" ;
  bool istop = false ;
  for (int i = 0 ;  !istop && i < 256 ;   i++){
    m_ScalarName[i] = defaultName[i] ;
    istop = (defaultName[i] == '\0') ;
  }
}



//------------------------------------------------------------------------------
// Set the color
//------------------------------------------------------------------------------
void vtkMEDAddScalarsFilter::SetColor(double r, double g, double b)
{
  m_Color[0] = r ;
  m_Color[1] = g ;
  m_Color[2] = b ;
  m_Color[3] = 255.0 ;
  m_NumberOfComponents = 3 ;

  this->Modified() ;
}



//------------------------------------------------------------------------------
// Set the color
//------------------------------------------------------------------------------
void vtkMEDAddScalarsFilter::SetColor(double col[3])
{
  SetColor(col[0], col[1], col[2]) ;
}



//------------------------------------------------------------------------------
// Set the color
//------------------------------------------------------------------------------
void vtkMEDAddScalarsFilter::SetColor(double r, double g, double b, double alpha)
{
  m_Color[0] = r ;
  m_Color[1] = g ;
  m_Color[2] = b ;
  m_Color[3] = alpha ;
  m_NumberOfComponents = 4 ;

  this->Modified() ;
}



//------------------------------------------------------------------------------
// Set the color
//------------------------------------------------------------------------------
void vtkMEDAddScalarsFilter::SetColor(double col[3], double alpha)
{
  SetColor(col[0], col[1], col[2], alpha) ;
}



//------------------------------------------------------------------------------
// Set the name
//------------------------------------------------------------------------------
void vtkMEDAddScalarsFilter::SetName(char* name)
{
  bool istop = false ;
  for (int i = 0 ;  !istop && i < 256 ;   i++){
    m_ScalarName[i] = name[i] ;
    istop = (name[i] == '\0') ;
  }

  this->Modified() ;
}



//------------------------------------------------------------------------------
// Execute method
//------------------------------------------------------------------------------
void vtkMEDAddScalarsFilter::Execute()
{
  vtkDebugMacro(<< "Executing nmcfAreaWeightedNormals Filter") ;

  // pointers to input and output
  m_Input = this->GetInput() ;
  m_Output = this->GetOutput() ;

  m_Output->DeepCopy(m_Input) ;

  // create attribute array for scalars
  vtkUnsignedCharArray* scalars = vtkUnsignedCharArray::New() ;
  scalars->SetNumberOfComponents(m_NumberOfComponents) ;
  scalars->SetName(m_ScalarName) ;

  int n ;
  if (m_AttribMode == POINT_SCALARS){
    n = m_Output->GetPoints()->GetNumberOfPoints() ;
    scalars->SetNumberOfTuples(n) ;
    m_Output->GetPointData()->SetScalars(scalars) ;
    scalars->Delete() ;
  }
  else if (m_AttribMode == CELL_SCALARS){
    n = m_Output->GetNumberOfCells() ;
    scalars->SetNumberOfTuples(n) ;
    m_Output->GetCellData()->SetScalars(scalars) ;
    scalars->Delete() ;
  }
  else{
    // error: unknown mode
    scalars->Delete() ;
    return ;
  }


  // set scalars to current color
  int m = scalars->GetNumberOfComponents() ;
  assert(m == m_NumberOfComponents) ;
  for (int i = 0 ;  i < n ;  i++)
    scalars->SetTuple(i, m_Color) ;

}



