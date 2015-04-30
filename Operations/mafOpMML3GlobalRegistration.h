/*=========================================================================

 Program: MAF2
 Module: mafOpMML3GlobalRegistration
 Authors: Mel Krokos, Nigel McFarlane
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafOpMML3GlobalRegistration_H__
#define __mafOpMML3GlobalRegistration_H__

#include "mafDefines.h"

#include "vtkMatrix4x4.h"
#include "vtkPolyData.h"
#include "vtkTransform.h"
#include "vtkTransformPolyDataFilter.h"


//------------------------------------------------------------------------------
/// mafOpMML3GlobalRegistration. \n
/// Helper class for mafOpMML3. \n
/// This class performs global registration given 0, 3 or 4 pairs of landmarks. \n
/// If there are not 3 or 4 pairs, the transform defaults to the identity. \n
/// The user sets pairs of landmarks 1 - 3 from atlas and patient, and optionally pair 4. \n
/// The class calculates the transform required to move the atlas landmarks to their positions in the patient. \n
/// The transform may be applied to polydata or points.
//------------------------------------------------------------------------------
class mafOpMML3GlobalRegistration
{
public:
  /// constructor
  mafOpMML3GlobalRegistration() ;

  /// destructor
  ~mafOpMML3GlobalRegistration() ;


  // patient landmarks functions
  void SetLandmark1OfPatient(double *xyz) ;
  void SetLandmark2OfPatient(double *xyz) ;
  void SetLandmark3OfPatient(double *xyz) ;
  void SetLandmark4OfPatient(double *xyz) ;

  // atlas landmarks functions
  void SetLandmark1OfAtlas(double *xyz) ;
  void SetLandmark2OfAtlas(double *xyz) ;
  void SetLandmark3OfAtlas(double *xyz) ;
  void SetLandmark4OfAtlas(double *xyz) ;

  /// Transform point from atlas to patient
  void TransformPoint(double point_atlas[3], double point_patient[3]) ;

  /// Transform polydata from atlas to patient
  vtkPolyData* TransformPolydata(vtkPolyData* polydata_atlas) ;

  /// Update (calculate transform matrix without having to use it)
  void Update() ;

  /// Get Transform matrix
  vtkMatrix4x4 *GetMatrix() {return m_Transform->GetMatrix() ;}

private:
  /// Calculate transform matrix
  void CalculateTransformMatrix() ;

  // input atlas landmarks
  bool m_AtlasLandmark1Defined ;
  bool m_AtlasLandmark2Defined ;
  bool m_AtlasLandmark3Defined ;
  bool m_AtlasLandmark4Defined ;
  double m_AtlasLandmark1[3];
  double m_AtlasLandmark2[3];
  double m_AtlasLandmark3[3];
  double m_AtlasLandmark4[3];

  // input patient landmarks
  bool m_PatientLandmark1Defined ;
  bool m_PatientLandmark2Defined ;
  bool m_PatientLandmark3Defined ;
  bool m_PatientLandmark4Defined ;
  double m_PatientLandmark1[3];
  double m_PatientLandmark2[3];
  double m_PatientLandmark3[3];
  double m_PatientLandmark4[3];

  // modified flag - indicates whether matrix needs recalculating
  bool m_Modified ;

  // transformation
  vtkTransform *m_Transform ;
  vtkTransformPolyDataFilter *m_PolydataTransform ;
};


#endif