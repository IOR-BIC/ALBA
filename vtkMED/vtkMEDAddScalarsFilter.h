/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: vtkMAFBridgeHoleFilter.h,v $
Language:  C++
Date:      $Date: 2009-03-20 16:39:48 $
Version:   $Revision: 1.1.2.2 $
Authors:   Nigel McFarlane
==========================================================================
Copyright (c) 2012
University of Bedfordshire
=========================================================================*/


#ifndef __vtkMEDAddScalarsFilter_h
#define __vtkMEDAddScalarsFilter_h

#include "vtkMEDConfigure.h"
#include "vtkPolyDataToPolyDataFilter.h"
#include "vtkPolyData.h"
#include <ostream>


//------------------------------------------------------------------------------
/// vtkMEDAddScalarsFilter. \n
/// Class which adds scalar attribute to polydata. \n
/// Scalar type is unsigned char.
//
// Version 26.11.13
//------------------------------------------------------------------------------
class VTK_vtkMED_EXPORT vtkMEDAddScalarsFilter : public vtkPolyDataToPolyDataFilter
{
public:
  vtkTypeRevisionMacro(vtkMEDAddScalarsFilter,vtkPolyDataToPolyDataFilter);
  static vtkMEDAddScalarsFilter *New();
  void PrintSelf(ostream& os, vtkIndent indent) const {}

  void SetColor(double r, double g, double b) ; ///< set color, range 0-255
  void SetColor(double col[3]) ;  ///< set color, range 0-255

  void SetColor(double r, double g, double b, double alpha) ; ///< set color and alpha, range 0-255
  void SetColor(double col[3], double alpha) ;  ///< set color and alpha, range 0-255

  void SetName(char* name) ;

  void SetModeToPointScalars() {m_AttribMode = POINT_SCALARS ;}
  void SetModeToCellScalars() {m_AttribMode = CELL_SCALARS ;}

protected:
  vtkMEDAddScalarsFilter();
  ~vtkMEDAddScalarsFilter() {};

  void Execute();

  float m_Color[4] ;
  char m_ScalarName[256] ;

  vtkPolyData *m_Input ;
  vtkPolyData *m_Output ;

  enum{
    POINT_SCALARS,
    CELL_SCALARS
  };

  int m_AttribMode ; ///< flag indicating point or cell scalars
  int m_NumberOfComponents ;

};

#endif

