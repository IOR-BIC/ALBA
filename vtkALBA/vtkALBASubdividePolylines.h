/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: vtkALBASubdividePolylines.h,v $
Language:  C++
Date:      $Date: 2009-03-20 16:39:48 $
Version:   $Revision: 1.1.2.2 $
Authors:   Nigel McFarlane
==========================================================================
Copyright (c) 2012
University of Bedfordshire
=========================================================================*/


#ifndef __vtkALBASubdividePolylines_h
#define __vtkALBASubdividePolylines_h

#include "albaConfigure.h"
#include "vtkPolyDataAlgorithm.h"
#include "vtkPolyData.h"
#include "vtkALBAPolyDataNavigator.h"

#include <ostream>


//------------------------------------------------------------------------------
/// vtkALBASubdividePolylines. \n
/// Filter which subdivides lines into smaller line segments. \n
/// Useful if you need to apply several colours to a long line. \n
/// The segment size can be set as an absolute value or as the \n
/// number of subdivisions. \n\n
///
/// NB This is meant to be applied to polyline data. If there are cells
/// with more than 2 vertices, their edges will also be subdivided.
//
// Version 19.2.14
//------------------------------------------------------------------------------
class ALBA_EXPORT vtkALBASubdividePolylines : public vtkPolyDataAlgorithm
{
public:
  vtkTypeMacro(vtkALBASubdividePolylines,vtkPolyDataAlgorithm);
  static vtkALBASubdividePolylines *New();
  void PrintSelf(ostream& os, vtkIndent indent) const ;

  /// Set nominal size of subdivided segments. \n
  /// Argument is absolute value.
  void SetSegmentSizeAbsolute(double size) ;

  /// Set no. of subdivisions.
  void SetNumberOfSubdivisions(int n) ;


protected:
  vtkALBASubdividePolylines();
  ~vtkALBASubdividePolylines();

	/** Execute method */
  int RequestData( vtkInformation *vtkNotUsed(request), vtkInformationVector **inputVector, vtkInformationVector *outputVector);

  vtkALBAPolyDataNavigator* m_Nav ;

  // Nominal size of subdivided segments.
  // Size can be absolute value or no. of subdivisions
  enum{
    USE_ABSOLUTE_VALUE,
    USE_NUMBER_OF_SUBDIVS
  };
  int m_SizeMode ;
  double m_SegmentSizeAbs ;
  int m_NumberOfSubdivisions ;

  vtkPolyData *m_Input ;
  vtkPolyData *m_Output ;

};

#endif

