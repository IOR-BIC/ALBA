/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: vtkALBABridgeHoleFilter.cxx,v $
Language:  C++
Date:      $Date: 2009-10-08 13:44:29 $
Version:   $Revision: 1.1.2.5 $
Authors:   Nigel McFarlane
==========================================================================
Copyright (c) 2012
University of Bedfordshire
=========================================================================*/

#include "vtkALBAAddScalarsFilter.h"
#include "vtkObjectFactory.h"
#include "vtkPolyData.h"
#include "vtkDataArray.h"
#include "vtkUnsignedCharArray.h"
#include "vtkPointData.h"
#include "vtkCellData.h"
#include "vtkIdList.h"
#include "assert.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"




vtkStandardNewMacro(vtkALBAAddScalarsFilter);



//------------------------------------------------------------------------------
// Constructor 
//------------------------------------------------------------------------------
vtkALBAAddScalarsFilter::vtkALBAAddScalarsFilter()
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
void vtkALBAAddScalarsFilter::SetColor(double r, double g, double b)
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
void vtkALBAAddScalarsFilter::SetColor(double col[3])
{
  SetColor(col[0], col[1], col[2]) ;
}



//------------------------------------------------------------------------------
// Set the color
//------------------------------------------------------------------------------
void vtkALBAAddScalarsFilter::SetColor(double r, double g, double b, double alpha)
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
void vtkALBAAddScalarsFilter::SetColor(double col[3], double alpha)
{
  SetColor(col[0], col[1], col[2], alpha) ;
}




//------------------------------------------------------------------------------
// set color of cell, range 0-255
//------------------------------------------------------------------------------
void vtkALBAAddScalarsFilter::SetColorOfCell(int cellId, double r, double g, double b)
{
  SetColorOfCell(cellId, r, g, b, 255) ;
}



//------------------------------------------------------------------------------
// set color of cell, range 0-255
//------------------------------------------------------------------------------
void vtkALBAAddScalarsFilter::SetColorOfCell(int cellId, double col[3])
{
  SetColorOfCell(cellId, col[0], col[1], col[2]) ;
}



//------------------------------------------------------------------------------
// set color of cell, range 0-255
//------------------------------------------------------------------------------
void vtkALBAAddScalarsFilter::SetColorOfCell(int cellId, double r, double g, double b, double alpha)
{
  if (m_AttribMode == CELL_SCALARS){
    Color newcol(cellId, r, g, b, alpha) ;
    m_UserColors.push_back(newcol) ;
  }
  else{
    m_Input = vtkPolyData::SafeDownCast(this->GetInput());
    vtkIdList* idlist = vtkIdList::New() ;
    m_Input->GetCellPoints(cellId, idlist) ;
    for (int j = 0 ;  j < idlist->GetNumberOfIds() ;  j++){
      int ptId = idlist->GetId(j) ;
      Color newcol(ptId, r, g, b, alpha) ;
      m_UserColors.push_back(newcol) ;
    }
  }

  this->Modified() ;
}



//------------------------------------------------------------------------------
// set color of cell, range 0-255
//------------------------------------------------------------------------------
void vtkALBAAddScalarsFilter::SetColorOfCell(int cellId, double col[3], double alpha)
{
  SetColorOfCell(cellId, col[0], col[1], col[2], alpha) ;
}



//------------------------------------------------------------------------------
// set color of point, range 0-255
//------------------------------------------------------------------------------
void vtkALBAAddScalarsFilter::SetColorOfPoint(int ptId, double r, double g, double b)
{
  SetColorOfPoint(ptId, r, g, b, 255) ;
}



//------------------------------------------------------------------------------
// set color of point, range 0-255
//------------------------------------------------------------------------------
void vtkALBAAddScalarsFilter::SetColorOfPoint(int ptId, double col[3])
{
  SetColorOfPoint(ptId, col[0], col[1], col[2]) ;
}



//------------------------------------------------------------------------------
// set color of point, range 0-255
//------------------------------------------------------------------------------
void vtkALBAAddScalarsFilter::SetColorOfPoint(int ptId, double r, double g, double b, double alpha)
{
  assert(m_AttribMode == POINT_SCALARS) ;
  Color newcol(ptId, r, g, b, alpha) ;
  m_UserColors.push_back(newcol) ;
  this->Modified() ;
}



//------------------------------------------------------------------------------
// set color of point, range 0-255
//------------------------------------------------------------------------------
void vtkALBAAddScalarsFilter::SetColorOfPoint(int ptId, double col[3], double alpha)
{
  SetColorOfPoint(ptId, col[0], col[1], col[2], alpha) ;
}



//------------------------------------------------------------------------------
// Set the name
//------------------------------------------------------------------------------
void vtkALBAAddScalarsFilter::SetName(char* name)
{
  bool istop = false ;
  for (int i = 0 ;  !istop && i < 256 ;   i++){
    m_ScalarName[i] = name[i] ;
    istop = (name[i] == '\0') ;
  }

  this->Modified() ;
}



//------------------------------------------------------------------------------
int vtkALBAAddScalarsFilter::RequestData( vtkInformation *vtkNotUsed(request), vtkInformationVector **inputVector, vtkInformationVector *outputVector)
{
	// get the info objects
	vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
	vtkInformation *outInfo = outputVector->GetInformationObject(0);

	// Initialize some frequently used values.
	vtkPolyData  *input = vtkPolyData::SafeDownCast(inInfo->Get(vtkDataObject::DATA_OBJECT()));
	vtkPolyData *output = vtkPolyData::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT()));

  vtkDebugMacro(<< "Executing vtkALBAAddScalarsFilter") ;

  // pointers to input and output
  m_Input = input ;
  m_Output = output;

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
  }
  else if (m_AttribMode == CELL_SCALARS){
    n = m_Output->GetNumberOfCells() ;
    scalars->SetNumberOfTuples(n) ;
    m_Output->GetCellData()->SetScalars(scalars) ;
  }
  else{
    // error: unknown mode
    scalars->Delete() ;
    return 1;
  }


  // set scalars to current color
  int m = scalars->GetNumberOfComponents() ;
  assert(m == m_NumberOfComponents) ;
  for (int i = 0 ;  i < n ;  i++)
    scalars->SetTuple(i, m_Color) ;



  // add user-defined colors of individual points or cells
  if (m_AttribMode == POINT_SCALARS){
    for (int i = 0 ;  i < (int)m_UserColors.size() ;  i++){
      int id = m_UserColors[i].id ;
      assert(id < scalars->GetNumberOfTuples()) ;
      scalars->SetTuple(id, m_UserColors[i].col) ;
    }
  }
  else if (m_AttribMode == CELL_SCALARS){
    for (int i = 0 ;  i < (int)m_UserColors.size() ;  i++){
      int id = m_UserColors[i].id ;
      assert(id < scalars->GetNumberOfTuples()) ;
      scalars->SetTuple(id, m_UserColors[i].col) ;
    }
  }
  else{
    // error: unknown mode
    scalars->Delete() ;
    return 1;
  }


  // clean up and exit
  scalars->Delete() ;

	return 1;
}
