/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: vtkMAFSubdividePolylines.h,v $
Language:  C++
Date:      $Date: 2009-03-20 16:39:48 $
Version:   $Revision: 1.1.2.2 $
Authors:   Nigel McFarlane
==========================================================================
Copyright (c) 2012
University of Bedfordshire
=========================================================================*/


#ifndef __vtkMAFSubdividePolylines_h
#define __vtkMAFSubdividePolylines_h

#include "mafConfigure.h"
#include "vtkPolyDataToPolyDataFilter.h"
#include "vtkPolyData.h"
#include "vtkMAFPolyDataNavigator.h"

#include <ostream>


//------------------------------------------------------------------------------
/// vtkMAFSubdividePolylines. \n
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
class MAF_EXPORT vtkMAFSubdividePolylines : public vtkPolyDataToPolyDataFilter
{
public:
  vtkTypeRevisionMacro(vtkMAFSubdividePolylines,vtkPolyDataToPolyDataFilter);
  static vtkMAFSubdividePolylines *New();
  void PrintSelf(ostream& os, vtkIndent indent) const ;

  /// Set nominal size of subdivided segments. \n
  /// Argument is absolute value.
  void SetSegmentSizeAbsolute(double size) ;

  /// Set no. of subdivisions.
  void SetNumberOfSubdivisions(int n) ;


protected:
  vtkMAFSubdividePolylines();
  ~vtkMAFSubdividePolylines();

  void Execute();

  vtkMAFPolyDataNavigator* m_Nav ;

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

