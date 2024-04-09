/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpMML3GlobalRegistration
 Authors: Mel Krokos, Nigel McFarlane
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "albaDefines.h"

#include "albaOpMML3GlobalRegistration.h"

#include "vtkMatrix4x4.h"
#include "vtkPolyData.h"
#include "vtkTransform.h"
#include "vtkTransformPolyDataFilter.h"


//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
albaOpMML3GlobalRegistration::albaOpMML3GlobalRegistration()
: m_AtlasLandmark1Defined(false), m_AtlasLandmark2Defined(false),
m_AtlasLandmark3Defined(false), m_AtlasLandmark4Defined(false),
m_PatientLandmark1Defined(false), m_PatientLandmark2Defined(false),
m_PatientLandmark3Defined(false), m_PatientLandmark4Defined(false),
m_Modified(true)
{
  m_Transform = vtkTransform::New() ;
  m_PolydataTransform = vtkTransformPolyDataFilter::New() ;
  m_PolydataTransform->SetTransform(m_Transform) ;
}



//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
albaOpMML3GlobalRegistration::~albaOpMML3GlobalRegistration()
{
  m_Transform->Delete() ;
  m_PolydataTransform->Delete() ;
}



//------------------------------------------------------------------------------
void albaOpMML3GlobalRegistration::TransformPoint(double point_atlas[3], double point_patient[3])
//------------------------------------------------------------------------------
{
  // Transform point from atlas to patient

  if (m_Modified){
    CalculateTransformMatrix() ;
    m_Modified = false ;
  }

  double *tmpOut = m_Transform->TransformDoublePoint(point_atlas) ;
  point_patient[0] = tmpOut[0] ;
  point_patient[1] = tmpOut[1] ;
  point_patient[2] = tmpOut[2] ;
}



//------------------------------------------------------------------------------
vtkPolyData* albaOpMML3GlobalRegistration::TransformPolydata(vtkPolyData* polydata_atlas)
//------------------------------------------------------------------------------
{
  // Transform polydata from atlas to patient

  if (m_Modified){
    CalculateTransformMatrix() ;
    m_Modified = false ;
  }

  m_PolydataTransform->SetInputData(polydata_atlas) ;
  return m_PolydataTransform->GetOutput() ;
}


//------------------------------------------------------------------------------
void albaOpMML3GlobalRegistration::Update()
//------------------------------------------------------------------------------
{
  // Update

  if (m_Modified){
    CalculateTransformMatrix() ;
    m_Modified = false ;
  }
}




//------------------------------------------------------------------------------
void albaOpMML3GlobalRegistration::CalculateTransformMatrix()
//------------------------------------------------------------------------------
{
  // Calculate transform matrix

  if(m_AtlasLandmark1Defined && m_AtlasLandmark2Defined && m_AtlasLandmark3Defined
    && m_PatientLandmark1Defined && m_PatientLandmark2Defined && m_PatientLandmark3Defined){

      // shorthand notation
      double at1[3];
      double at2[3];
      double at3[3];
      double at4[3];
      double pt1[3];
      double pt2[3];
      double pt3[3];
      double pt4[3];
      for (int i = 0 ;  i < 3 ;  i++){
        at1[i] = m_AtlasLandmark1[i] ;
        at2[i] = m_AtlasLandmark2[i] ;
        at3[i] = m_AtlasLandmark3[i] ;
        at4[i] = m_AtlasLandmark4[i] ;
        pt1[i] = m_PatientLandmark1[i] ;
        pt2[i] = m_PatientLandmark2[i] ;
        pt3[i] = m_PatientLandmark3[i] ;
        pt4[i] = m_PatientLandmark4[i] ;
      }
      bool got4Landmarks = m_AtlasLandmark4Defined && m_PatientLandmark4Defined ;


      // create transformation matrix 1
      // translate origin to patient landmark 1
      // (0,0,0) -> pt1
      // pt2-pt1 -> pt2
      // pt3-pt1 -> pt3
      // pt4-pt1 -> pt4
      vtkMatrix4x4 *transm1 = vtkMatrix4x4::New();
      transm1->Identity();
      transm1->SetElement(0, 3, pt1[0]);
      transm1->SetElement(1, 3, pt1[1]);
      transm1->SetElement(2, 3, pt1[2]);


      // create transformation matrix 2
      // origin is pt1, x axis is pt2-pt1, y axis is pt3-pt1, z axis is pt4-pt1
      // rotate to new axes, so 
      // (1,0,0) -> pt2-pt1
      // (0,1,0) -> pt3-pt1
      // (0,0,1) -> pt4-pt1
      vtkMatrix4x4 *transm2 = vtkMatrix4x4::New();
      transm2->Identity();
      transm2->SetElement(0, 0, pt2[0] - pt1[0]);
      transm2->SetElement(0, 1, pt3[0] - pt1[0]);

      if (got4Landmarks)
        transm2->SetElement(0, 2, pt4[0] - pt1[0]);
      else{
        // no landmark 4, so use cross product for z axis
        transm2->SetElement(0, 2, (pt2[1]-pt1[1])*(pt3[2]-pt1[2])-(pt2[2]-pt1[2])*(pt3[1]-pt1[1]));
      }

      transm2->SetElement(1, 0, pt2[1] - pt1[1]);
      transm2->SetElement(1, 1, pt3[1] - pt1[1]);
      if (got4Landmarks)
        transm2->SetElement(1, 2, pt4[1] - pt1[1]);
      else
        transm2->SetElement(1, 2, (pt2[2]-pt1[2])*(pt3[0]-pt1[0])-(pt2[0]-pt1[0])*(pt3[2]-pt1[2]));

      transm2->SetElement(2, 0, pt2[2] - pt1[2]);
      transm2->SetElement(2, 1, pt3[2] - pt1[2]);
      if (got4Landmarks)
        transm2->SetElement(2, 2, pt4[2] - pt1[2]);
      else
        transm2->SetElement(2, 2, (pt2[0]-pt1[0])*(pt3[1]-pt1[1])-(pt2[1]-pt1[1])*(pt3[0]-pt1[0]));


      // create transformation matrix 3
      // same as matrix 2 but with atlas landmarks
      // origin is at1, x axis is at2-at1, y axis is at3-at1, z axis is at4-at1
      // rotate to new axes, so 
      // (1,0,0) -> at2-at1
      // (0,1,0) -> at3-at1
      // (0,0,1) -> at4-at1
      vtkMatrix4x4 *transm3 = vtkMatrix4x4::New();
      transm3->Identity();
      transm3->SetElement(0, 0, at2[0] - at1[0]);
      transm3->SetElement(0, 1, at3[0] - at1[0]);
      if (got4Landmarks)
        transm3->SetElement(0, 2, at4[0] - at1[0]);
      else
        transm3->SetElement(0, 2, (at2[1]-at1[1])*(at3[2]-at1[2])-(at2[2]-at1[2])*(at3[1]-at1[1]));
      transm3->SetElement(1, 0, at2[1] - at1[1]);
      transm3->SetElement(1, 1, at3[1] - at1[1]);
      if (got4Landmarks)
        transm3->SetElement(1, 2, at4[1] - at1[1]);
      else
        transm3->SetElement(1, 2, (at2[2]-at1[2])*(at3[0]-at1[0])-(at2[0]-at1[0])*(at3[2]-at1[2]));
      transm3->SetElement(2, 0, at2[2] - at1[2]);
      transm3->SetElement(2, 1, at3[2] - at1[2]);
      if (got4Landmarks)
        transm3->SetElement(2, 2, at4[2] - at1[2]);
      else
        transm3->SetElement(2, 2, (at2[0]-at1[0])*(at3[1]-at1[1])-(at2[1]-at1[1])*(at3[0]-at1[0]));


      // create inverse transformation matrix 3
      // The inverse matrix has the effect:
      // (at2-at1) -> (1,0,0)
      // (at3-at1) -> (0,1,0)
      // (at4-at1) -> (0,0,1)
      vtkMatrix4x4 *inversetransm3 = vtkMatrix4x4::New();
      inversetransm3->Identity();
      inversetransm3->Invert(transm3, inversetransm3);


      // create transformation matrix 4
      // translate atlas landmark 1 to the origin
      // at1 -> (0,0,0)
      // at2 -> at2-at1
      // at3 -> at3-at1
      // at4 -> at4-at1
      vtkMatrix4x4 *transm4 = vtkMatrix4x4::New();
      transm4->Identity();
      transm4->SetElement(0, 3, -1.0 * at1[0]);
      transm4->SetElement(1, 3, -1.0 * at1[1]);
      transm4->SetElement(2, 3, -1.0 * at1[2]);




      //----------------------------------------------------------------------------
      // calculate final transformation matrix
      // finalMat = transm1 * transm2 * inv(transm3) * transm4
      //----------------------------------------------------------------------------
      vtkMatrix4x4 *finalMat = m_Transform->GetMatrix() ;
      finalMat->Identity();
      finalMat->Multiply4x4(transm4, finalMat, finalMat) ;
      finalMat->Multiply4x4(inversetransm3, finalMat, finalMat) ;
      finalMat->Multiply4x4(transm2, finalMat, finalMat) ;
      finalMat->Multiply4x4(transm1, finalMat, finalMat) ;




      //----------------------------------------------------------------------------
      // test transform.
      // transformed positions of atlas landmarks should be same as patient landmarks
      //----------------------------------------------------------------------------

      double newat1[3];
      double newat2[3];
      double newat3[3];
      double newat4[3];

      m_Transform->TransformPoint(at1, newat1); // landmark 1
      m_Transform->TransformPoint(at2, newat2); // landmark 2
      m_Transform->TransformPoint(at3, newat3); // landmark 3
      if (got4Landmarks)
        m_Transform->TransformPoint(at4, newat4); // landmark 4

      double diff1[3];
      double diff2[3];
      double diff3[3];
      double diff4[3];
      for(int i = 0; i < 3; i++){
        diff1[i] = pt1[i] - newat1[i];
        diff2[i] = pt2[i] - newat2[i];
        diff3[i] = pt3[i] - newat3[i];
        if (got4Landmarks)
          diff4[i] = pt4[i] - newat4[i];
      }
      assert(sqrt(pow(diff1[0], 2.0) + pow(diff1[1], 2.0) + pow(diff1[2], 2.0)) < 0.001);
      assert(sqrt(pow(diff2[0], 2.0) + pow(diff2[1], 2.0) + pow(diff2[2], 2.0)) < 0.001);
      assert(sqrt(pow(diff3[0], 2.0) + pow(diff3[1], 2.0) + pow(diff3[2], 2.0)) < 0.001);
      if (got4Landmarks)
        assert(sqrt(pow(diff4[0], 2.0) + pow(diff4[1], 2.0) + pow(diff4[2], 2.0)) < 0.001);



      // clean up
      transm1->Delete();
      transm2->Delete();
      transm3->Delete();
      transm4->Delete();
      inversetransm3->Delete() ;
  }
  else{
    // not enough landmarks, so set transform to identity
    m_Transform->GetMatrix()->Identity() ;
  }
}



//----------------------------------------------------------------------------
void albaOpMML3GlobalRegistration::SetLandmark1OfAtlas(double *xyz)
//----------------------------------------------------------------------------
{
  m_AtlasLandmark1[0] = xyz[0];
  m_AtlasLandmark1[1] = xyz[1];
  m_AtlasLandmark1[2] = xyz[2];
  m_AtlasLandmark1Defined = true ;
  m_Modified = true ;
}

//----------------------------------------------------------------------------
void albaOpMML3GlobalRegistration::SetLandmark2OfAtlas(double *xyz)
//----------------------------------------------------------------------------
{
  m_AtlasLandmark2[0] = xyz[0];
  m_AtlasLandmark2[1] = xyz[1];
  m_AtlasLandmark2[2] = xyz[2];
  m_AtlasLandmark2Defined = true ;
  m_Modified = true ;
}

//----------------------------------------------------------------------------
void albaOpMML3GlobalRegistration::SetLandmark3OfAtlas(double *xyz)
//----------------------------------------------------------------------------
{
  m_AtlasLandmark3[0] = xyz[0];
  m_AtlasLandmark3[1] = xyz[1];
  m_AtlasLandmark3[2] = xyz[2];
  m_AtlasLandmark3Defined = true ;
  m_Modified = true ;
}

//----------------------------------------------------------------------------
void albaOpMML3GlobalRegistration::SetLandmark4OfAtlas(double *xyz)
//----------------------------------------------------------------------------
{
  m_AtlasLandmark4[0] = xyz[0];
  m_AtlasLandmark4[1] = xyz[1];
  m_AtlasLandmark4[2] = xyz[2];
  m_AtlasLandmark4Defined = true ;
  m_Modified = true ;
}


//----------------------------------------------------------------------------
void albaOpMML3GlobalRegistration::SetLandmark1OfPatient(double *xyz)
//----------------------------------------------------------------------------
{
  m_PatientLandmark1[0] = xyz[0];
  m_PatientLandmark1[1] = xyz[1];
  m_PatientLandmark1[2] = xyz[2];
  m_PatientLandmark1Defined = true ;
  m_Modified = true ;
}

//----------------------------------------------------------------------------
void albaOpMML3GlobalRegistration::SetLandmark2OfPatient(double *xyz)
//----------------------------------------------------------------------------
{
  m_PatientLandmark2[0] = xyz[0];
  m_PatientLandmark2[1] = xyz[1];
  m_PatientLandmark2[2] = xyz[2];
  m_PatientLandmark2Defined = true ;
  m_Modified = true ;
}

//----------------------------------------------------------------------------
void albaOpMML3GlobalRegistration::SetLandmark3OfPatient(double *xyz)
//----------------------------------------------------------------------------
{
  m_PatientLandmark3[0] = xyz[0];
  m_PatientLandmark3[1] = xyz[1];
  m_PatientLandmark3[2] = xyz[2];
  m_PatientLandmark3Defined = true ;
  m_Modified = true ;
}

//----------------------------------------------------------------------------
void albaOpMML3GlobalRegistration::SetLandmark4OfPatient(double *xyz)
//----------------------------------------------------------------------------
{
  m_PatientLandmark4[0] = xyz[0];
  m_PatientLandmark4[1] = xyz[1];
  m_PatientLandmark4[2] = xyz[2];
  m_PatientLandmark4Defined = true ;
  m_Modified = true ;
}
