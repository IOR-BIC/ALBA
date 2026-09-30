/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: vtkALBABridgeHoleFilter.h,v $
Language:  C++
Date:      $Date: 2009-03-20 16:39:48 $
Version:   $Revision: 1.1.2.2 $
Authors:   Nigel McFarlane
==========================================================================
Copyright (c) 2012
University of Bedfordshire
=========================================================================*/


#ifndef __vtkALBAAddScalarsFilter_h
#define __vtkALBAAddScalarsFilter_h

#include "albaConfigure.h"
#include "vtkPolyDataAlgorithm.h"
#include "vtkPolyData.h"
#include <ostream>
#include <vector>


//------------------------------------------------------------------------------
/// vtkALBAAddScalarsFilter. \n
/// Class which adds scalar attribute to polydata. \n
/// Scalar type is unsigned char. \n
//
// Version 18.2.14
//------------------------------------------------------------------------------
class ALBA_EXPORT vtkALBAAddScalarsFilter : public vtkPolyDataAlgorithm
{
public:
  vtkTypeMacro(vtkALBAAddScalarsFilter,vtkPolyDataAlgorithm);
  static vtkALBAAddScalarsFilter *New();
  void PrintSelf(ostream& os, vtkIndent indent) const {}

  void Clear() {m_UserColors.clear() ;} ///< clear filter

  void SetColor(double r, double g, double b) ; ///< set color, range 0-255
  void SetColor(double col[3]) ;  ///< set color, range 0-255

  void SetColor(double r, double g, double b, double alpha) ; ///< set color and alpha, range 0-255
  void SetColor(double col[3], double alpha) ;  ///< set color and alpha, range 0-255

  void SetName(char* name) ;
  
  void SetModeToPointScalars() {m_AttribMode = POINT_SCALARS ; this->Modified() ;} ///< Set mode to point scalars (default)
  void SetModeToCellScalars() {m_AttribMode = CELL_SCALARS ; this->Modified() ;} ///< Set mode to cell scalars

  /// Set color of single cell, scalar range 0-255. \n
  /// If the mode is set to point scalars, this colors all the points in the cell.
  void SetColorOfCell(int cellId, double r, double g, double b) ; 

  /// Set color of single cell, scalar range 0-255. \n
  /// If the mode is set to point scalars, this colors all the points in the cell.
  void SetColorOfCell(int cellId, double col[3]) ; 

  /// Set color of single cell, scalar range 0-255. \n
  /// If the mode is set to point scalars, this colors all the points in the cell.
  void SetColorOfCell(int cellId, double r, double g, double b, double alpha) ; 

  /// Set color of single cell, scalar range 0-255. \n
  /// If the mode is set to point scalars, this colors all the points in the cell.
  void SetColorOfCell(int cellId, double col[3], double alpha) ; 

  /// Set color of single point, scalar range 0-255. \n
  /// This does not work on cell scalar attributes.
  void SetColorOfPoint(int ptId, double r, double g, double b) ; 
  
  /// Set color of single point, scalar range 0-255. \n
  /// This does not work on cell scalar attributes.
  void SetColorOfPoint(int ptId, double col[3]) ; 
  
  /// Set color of single point, scalar range 0-255. \n
  /// This does not work on cell scalar attributes.
  void SetColorOfPoint(int ptId, double r, double g, double b, double alpha) ; 
  
  /// Set color of single point, scalar range 0-255. \n
  /// This does not work on cell scalar attributes.
  void SetColorOfPoint(int ptId, double col[3], double alpha) ; 

protected:
  vtkALBAAddScalarsFilter();
  ~vtkALBAAddScalarsFilter() {};

	/** Execute method */
	int RequestData( vtkInformation *vtkNotUsed(request), vtkInformationVector **inputVector, vtkInformationVector *outputVector);

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

  struct Color
  {
    Color(int idIn, double r, double g, double b) {id=idIn; col[0]=r; col[1]=g; col[2]=b; col[3]=255.0;}
    Color(int idIn, double colIn[3]) {id=idIn; col[0]=colIn[0]; col[1]=colIn[1]; col[2]=colIn[2]; col[3]=255.0;}
    Color(int idIn, double r, double g, double b, double alpha) {id=idIn; col[0]=r; col[1]=g; col[2]=b; col[3]=alpha;}
    Color(int idIn, double colIn[3], double alpha) {id=idIn; col[0]=colIn[0]; col[1]=colIn[1]; col[2]=colIn[2]; col[3]=alpha;}
    int id ;
    double col[4] ;
  };

  std::vector<Color> m_UserColors ; // remembers user-defined colors of individual points or cells
};

#endif


