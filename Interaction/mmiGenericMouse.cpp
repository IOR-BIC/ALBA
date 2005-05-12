/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmiGenericMouse.cpp,v $
  Language:  C++
  Date:      $Date: 2005-05-12 16:26:45 $
  Version:   $Revision: 1.2 $
  Authors:   Stefano Perticoni
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "wx/wxprec.h" 
#pragma hdrstop

#include "mafDecl.h"
#include "mafDevice.h"

#include "mmdMouse.h"

#include "mmiGenericMouse.h"

#include "mflDefines.h"
#include "mflEventInteraction.h"
#include "mflMatrixPipeDirectCinematic.h"
#include "mflVME.h"

#include "vtkObjectFactory.h"
#include "vtkCommand.h"
#include "vtkDoubleArray.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkCellPicker.h"
#include "vtkMath.h"
#include "vtkTransform.h"
#include "vtkCamera.h"
#include "vtkRenderer.h"
#include "vtkTimerLog.h"

vtkStandardNewMacro(mmiGenericMouse);

//----------------------------------------------------------------------------
mmiGenericMouse::mmiGenericMouse() 
//----------------------------------------------------------------------------
{
   ResultMatrixConcatenation = true;

   this->SetResultMatrixConcatenationSemanticToPostMultiply();
  
   // Picking stuff
   this->Picker = vtkCellPicker::New();
   this->Picker->SetTolerance(0.005);

   // Projection Accumulator
   ProjAcc = 0;

   this->LastX = this->LastY = 0;

   // default is to translation mode
   this->EnableTranslation(true);

   // set default constraint axes
   GetRotationConstraint()->SetConstraintModality(mmiConstraint::FREE, mmiConstraint::FREE, mmiConstraint::LOCK);
   GetTranslationConstraint()->SetConstraintModality(mmiConstraint::FREE, mmiConstraint::FREE, mmiConstraint::LOCK);

   CurrentCamera = NULL;
   MousePose[0] = MousePose[1] = 0;
   LastMousePose[0] = LastMousePose[1] = 0;
   ConstrainRefSys = NULL;
}
//----------------------------------------------------------------------------
mmiGenericMouse::~mmiGenericMouse() 
//----------------------------------------------------------------------------
{
   this->Picker->Delete();
}

//----------------------------------------------------------------------------
void mmiGenericMouse::Translate(double *p1, double *p2)
//----------------------------------------------------------------------------
{
  // create matrix to send
  vtkMatrix4x4 *matr = vtkMatrix4x4::New();

  // motion vector
  double motionVec[3];

  this->BuildVector(p1, p2, motionVec);

  // get the ref sys type  
  int refSysType = GetTranslationConstraint()->GetRefSys()->GetType();

  // set the constraint refsys to translation constraint, the matrix is referenced
  ConstrainRefSys = GetTranslationConstraint()->GetRefSys()->GetMatrix();

  if (this->GetTranslationConstraint()->GetNumberOfDOF() == 1)
  { 

    // result point position
    double rp[3];

    // result point projected on constrain axis
    double pos_rp;

    // projection of motion vector on constrain axis 
    double motVecProj[3];

    // projection value along constrain axis
    double projVal;

    //current axis versor
    double constrainVers[3];

    assert(VME);

    //get the result point pos
    mflTransform::GetPosition(VME->GetMatrixPipe()->GetMatrix(), rp); 

    //get the constrain axis
    int axis = GetTranslationConstraint()->GetConstraintAxis();

    //project the motion vector on constrain axis    
    mflTransform::GetVersor(axis, this->ConstrainRefSys, constrainVers);  
    projVal = ProjectVectorOnAxis(motionVec, constrainVers, motVecProj);

   // project rp on constrain axis; if target refsys is local the projection 
   // is the value along the constrain axis
    if (refSysType == mafRefSys::GLOBAL)
    {
      pos_rp = ProjectVectorOnAxis(rp, constrainVers);
    }
    else
    {
      pos_rp = rp[axis];
    }

    switch(GetTranslationConstraint()->GetConstraintModality(axis)) 
    {
      case (mmiConstraint::FREE):
      { 
        //set the matrix to be send            
        mflTransform::SetPosition(matr, motVecProj);
         
        //send the transform matrix
        SendTransformMatrix(matr);
      }
      break;

      case (mmiConstraint::BOUNDS):
      {
    
  /*

                            
      ------------proj-------->  
      
      0-------minb-------------rp------------------maxb------->      
                              pos_rp
      ^  
      |
      C-> constrainVers  

      // C:  constrain ref sys center
      // rp: result point
  */
  

  /*

                                          >
                                        /
                                      /
                                motionVec    
                                  /    
                                /
                                -projVal-->  
      
      0-------minb-------------------|-------------maxb------->      
                                   pos_rp
      ^  
      |
      C-> constrainVers              rp

      ------------proj-------------->|
                                   projVal      
                                
      // C:  constrain ref sys center
      // rp: result point

    
    
  */
      double minb, maxb;
      double tmpVec[3] = {0, 0, 0};
    
      //get the interval bounds
      minb = GetTranslationConstraint()->GetLowerBound(axis);
      maxb = GetTranslationConstraint()->GetUpperBound(axis);
    
      mflTransform::GetPosition(VME->GetMatrixPipe()->GetMatrix(), rp);  
    
      /*                         projVal  
                             -->
          -----|------------|-----------|------|---------------|----------->
               O          pos_rp      minb   pos_rp           maxb

          ------------------------>ProjAcc
      */ 

          //update projection accumulator
          ProjAcc += projVal;

          if (pos_rp < minb)
          {
            if (pos_rp + ProjAcc > minb)
            {
              BuildVector(minb - pos_rp, constrainVers, tmpVec, refSysType, axis);
              mflTransform::SetPosition(matr, tmpVec);                             
              ProjAcc = 0;
            }
        }
          else if (pos_rp > maxb)
          { 
            if (pos_rp + ProjAcc < maxb)
            {
              BuildVector(maxb - pos_rp, constrainVers, tmpVec, refSysType, axis);
              mflTransform::SetPosition(matr, tmpVec);            
              ProjAcc = 0;
            }
        }
          else if (minb < pos_rp && pos_rp < maxb)
          {
            if (pos_rp + projVal < minb)
            {              
              BuildVector(minb - pos_rp, constrainVers, tmpVec, refSysType, axis);
              mflTransform::SetPosition(matr, tmpVec);                           
              ProjAcc = pos_rp + projVal - minb;
            }
            else if (pos_rp + projVal > maxb)
            {            
              BuildVector(maxb - pos_rp, constrainVers, tmpVec, refSysType, axis);
              mflTransform::SetPosition(matr, tmpVec);
              ProjAcc = pos_rp + projVal - maxb;
            }
            else
            {
              if (refSysType == mafRefSys::GLOBAL)
              {
                mflTransform::SetPosition(matr, motVecProj);             
              }
              else
              {
                BuildVector(projVal, NULL, tmpVec, refSysType, axis);
                mflTransform::SetPosition(matr, tmpVec);
              }
            }
          }
          else if (pos_rp == minb) 
          {
                                                                 
      /*                                              
                                                       projVal
                                                    <-------- 
                                                       ProjAcc
                                                    <----|
                                    2   |    1           |  3
          -----|------------------------|----------------|----------->
               O                       minb             maxb
                                      pos_rp 
                                
      */ 


            if (pos_rp + ProjAcc >= minb)
            {
              if (pos_rp + ProjAcc <= maxb)
              {
                BuildVector(ProjAcc, constrainVers, tmpVec, refSysType, axis);
                mflTransform::SetPosition(matr, tmpVec);                         
                ProjAcc = 0;
              }
              else if (pos_rp + ProjAcc > maxb)
              {
                BuildVector(maxb - pos_rp, constrainVers, tmpVec, refSysType, axis);
                mflTransform::SetPosition(matr, tmpVec);                         
                ProjAcc -= maxb - pos_rp;
              }
            }
          }
          else if (pos_rp == maxb)
          {
            if (pos_rp + ProjAcc <= maxb)
            {
              if (pos_rp + ProjAcc >= minb)
              {
                BuildVector(ProjAcc, constrainVers, tmpVec, refSysType, axis);
                mflTransform::SetPosition(matr, tmpVec);     
                ProjAcc = 0;
              }
              else if (pos_rp + ProjAcc < minb)
              {
                BuildVector(minb - pos_rp, constrainVers, tmpVec, refSysType, axis);
                mflTransform::SetPosition(matr, tmpVec);     
                ProjAcc -= minb - pos_rp;
              }

            }          
          }
        
        //send the transform matrix
        SendTransformMatrix(matr);
           
      }
      case (mmiConstraint::SNAP_STEP):
      {
        
    

      }
      break;
  
      case (mmiConstraint::SNAP_ARRAY):
      {
        //get a pointer to the snap array
        vtkDoubleArray *darray = GetTranslationConstraint()->GetSnapArray(axis);
  
        double normalizedVectorToSend[3] = {0, 0, 0};

        if (refSysType == mafRefSys::GLOBAL)
        {
          normalizedVectorToSend[0] = constrainVers[0];
          normalizedVectorToSend[1] = constrainVers[1];
          normalizedVectorToSend[2] = constrainVers[2];
        }
        else if (refSysType == mafRefSys::LOCAL)
        {
          normalizedVectorToSend[axis] = 1;
        } 
        else
        {
          vtkGenericWarningMacro(<< "Ref sys type:" << refSysType << "is unsupported!");
          return;
        }


        ProjAcc += projVal;

        if (HelperPointStatus == ON_GRID_POINT)
        {

//              pos_rp             ProjAcc
//    ^------------------------->|-------------->        
//    |                             
//   -O----------0-------1-------i----------3------N-1---------->
//                           HelpPIndex    ind    
//                               |--------->|--->
//    HelpPIndex  = i;               trval                
//    HelperPointStatus = ON_GRID_POINT;
        
          int status;
          int ind = BinarySearch(pos_rp + ProjAcc, darray, status);    

          if (ProjAcc > 0)
          {
            if (ind != HelpPIndex)
            {
              double trval = darray->GetValue(ind) - darray->GetValue(HelpPIndex);

              //send translation vector from HelpPIndex to ind
              SendTransformMatrix(normalizedVectorToSend, trval);  

              //UpdateProjAcc  
              ProjAcc -= trval;

              //update HelpPIndex
              HelpPIndex = ind;
            }
          }
          else if (ProjAcc < 0)
          {
            if (ind != HelpPIndex - 1)
            {
              double trval = darray->GetValue(ind + 1) - darray->GetValue(HelpPIndex);
   
              //send translation vector from HelpPIndex to ind
              SendTransformMatrix(normalizedVectorToSend, trval);  
  
              //UpdateProjAcc  
              ProjAcc -= trval;
   
              //update HelpPIndex
              HelpPIndex = ind + 1;
            }
          }
        }
        else if (HelperPointStatus == NOT_ON_GRID_POINT) 
        {
//              pos_rp             ProjAcc
//    ^------------------------->|---------->        
//    |                          |   
//   -O----------0-------1-------|------2------N-1---------->
//                   HelpPIndex  |----->| 
//                                 trval
//    HelpPIndex  = i;                               
//    HelperPointStatus = NOT_ON_GRID_POINT;
      
          int status;
          int ind = BinarySearch(pos_rp + ProjAcc, darray, status); 
      
          if (ind != HelpPIndex)
          {
            double trval = darray->GetValue(ProjAcc > 0 ? ind : ind + 1) - pos_rp;

            //send translation vector from HelpPIndex to ind
            SendTransformMatrix(normalizedVectorToSend, trval); 

            //UpdateProjAcc  
            ProjAcc -= trval;

            //update HelpPIndex
            HelpPIndex = ind;
          }
        }   
      }
      break;
    }
  }
  else if (this->GetTranslationConstraint()->GetNumberOfDOF() == 2)
  {    
    // the constrain plane
    int constrainPlane;

    // get the translation plane
    constrainPlane = this->GetTranslationConstraint()->GetConstraintPlane();

    // get constraint axes
    int axis1, axis2;
    GetTranslationConstraint()->GetConstraintPlaneAxes(axis1, axis2);
      
    if (GetTranslationConstraint()->GetConstraintModality(axis1) == mmiConstraint::FREE &&
        GetTranslationConstraint()->GetConstraintModality(axis2) == mmiConstraint::FREE)
    {
      if (GetTranslationConstraint()->GetRefSys()->GetType() == mafRefSys::CUSTOM
       || GetTranslationConstraint()->GetRefSys()->GetType() == mafRefSys::GLOBAL
       || GetTranslationConstraint()->GetRefSys()->GetType() == mafRefSys::PARENT
       || GetTranslationConstraint()->GetRefSys()->GetType() == mafRefSys::LOCAL        
         )
      {    
        // translate on a plane

        // the projection of motion vector on the constrain plane
        double motVecProj[3];
        double planeNormal[3];

        switch  (constrainPlane) 
        {
          case (mmiConstraint::XY):
          {
            mflTransform::GetVersor(mmiConstraint::Z, ConstrainRefSys, planeNormal);
          }
  	      break;
          case (mmiConstraint::XZ):
          {
            mflTransform::GetVersor(mmiConstraint::Y, ConstrainRefSys, planeNormal);
          }
          break;
          case (mmiConstraint::YZ):
          {
            mflTransform::GetVersor(mmiConstraint::X, ConstrainRefSys, planeNormal);
          }
          break;
        }
    
        // project the motion vector on constrain plane    
        ProjectVectorOnPlane(motionVec, planeNormal, motVecProj);

       //set the matrix to be send
        mflTransform::SetPosition(matr, motVecProj);
          
        //send the transform matrix
        SendTransformMatrix(matr);
      }
      else if (GetTranslationConstraint()->GetRefSys()->GetType() == mafRefSys::VIEW)
      {
        if (constrainPlane == mmiConstraint::XY)
        {
          if (SurfaceSnap == false)
          {
            TrackballTranslate();
          }          
          else
          {
            SnapOnSurface();
          }
        }
        else
        {
          vtkGenericWarningMacro(<< "Transaltion on VIEW RefSys is not supported for " << constrainPlane << " plane");
        }
      }
      else
      {
        vtkGenericWarningMacro(<< "Refsys type" << GetTranslationConstraint()->GetRefSys()->GetType() << "is not supported!");
      }
    }
    else
    {
         vtkGenericWarningMacro(<< "Only supporting free movement on both plane axes!");
    }
  }
 
vtkDEL(matr);

}
//----------------------------------------------------------------------------
void mmiGenericMouse::Rotate(double *p1, double *p2, double *viewup)
//----------------------------------------------------------------------------
{
  // set the constraint refsys to rotation constraint
  ConstrainRefSys = GetRotationConstraint()->GetRefSys()->GetMatrix();

  //motion vector
  double motionVec[3];
  BuildVector(p1, p2, motionVec);

  double pa[3], pb[3];
  for (int i = 0;i<3; i++)
  {
    pa[i] = p1[i];
    pb[i] = p2[i];
  }

  //view up vector
  double *viewUp = viewup;

  //transform
  vtkTransform *transform = vtkTransform::New();

  int refSysType = GetRotationConstraint()->GetRefSys()->GetType();

  if (GetRotationConstraint()->GetNumberOfDOF() == 1)
  {    

    //get the constrain axis
    int axis = GetRotationConstraint()->GetConstraintAxis();
  
    switch(GetRotationConstraint()->GetConstraintModality(axis)) 
    {
      case (mmiConstraint::FREE):
      { 
        if (refSysType == mafRefSys::CUSTOM)
        {
          /**
          Build the rotation angle to be sent to listener by rotating around the circle
          */
          
          //current axis versor
          double constrainVers[3];

          //get the constrain axis
          int axis = GetRotationConstraint()->GetConstraintAxis();

          mflTransform::GetVersor(axis, ConstrainRefSys, constrainVers);
          
          // centre of rotation in abs coordinates
          double o[3] = {0,0,0};

          // get the centre of rotation from PivotRefSys
          mflTransform::GetPosition(GetPivotRefSys()->GetMatrix(), o);

          double op1Vec[3] = {0, 0, 0};
          double op2Vec[3] = {0, 0, 0};
          
          BuildVector(o, p1, op1Vec);
          BuildVector(o, p2, op2Vec);

          // op1 X op2
          double op1Vec_X_op2Vec[3] = {0, 0, 0};
          vtkMath::Cross(op1Vec, op2Vec, op1Vec_X_op2Vec);

          // rotation angle should be positive or negative?
          bool angleIsPositive = (vtkMath::Dot(op1Vec_X_op2Vec, constrainVers) >= 0 ? true : false);

          /*
          Cosine rule:

          p1        p2                   -1      2     2     2
          -----------     a = angle = cos  ((p1p2 - p2o - p1o )/(-2 * p2o * p1o)) 
          \        /
            \  a /
              \/
               o
          */
          
          double p1p2Squared = vtkMath::Distance2BetweenPoints(p1, p2);
          double p2oSquared = vtkMath::Distance2BetweenPoints(p2, o);
          double p1oSquared = vtkMath::Distance2BetweenPoints(p1, o);
          
          double angle = vtkMath::RadiansToDegrees() * acos((p1p2Squared - p2oSquared - p1oSquared) / (-2 * sqrt(p2oSquared) * sqrt(p1oSquared)));

          if (angleIsPositive) 
          {
            angle = fabs(angle);
          }
          else
          {
            angle = -fabs(angle);
          }

          transform->PostMultiply();
          transform->Translate(-o[0], -o[1], -o[2]);
          transform->RotateWXYZ(angle, constrainVers);
          transform->Translate(o);
     
          //send the transform matrix along with rotation angle
          SendTransformMatrix(transform->GetMatrix(), MOUSE_MOVE, angle);
          
        }
        else if (refSysType == mafRefSys::VIEW)
        {
          // only rotation around view look is supported
          if (axis == mmiConstraint::Z)
          {
            TrackballRoll();
          }
          else
          {
            vtkGenericWarningMacro(<< "Rotation in VIEW ref sys around" << axis << "axis is not supported yet!");
          }
        }
      }
      break;
  
      case (mmiConstraint::SNAP_STEP):
      {
    
      }
      break;
  
      case (mmiConstraint::SNAP_ARRAY):
      {
  

      }
      break;
    }//switch  
  }//if
  else if (GetRotationConstraint()->GetNumberOfDOF() == 2)
  {
     // the constrain plane
    int constrainPlane;

    // get the translation plane
    constrainPlane = this->GetRotationConstraint()->GetConstraintPlane();

    // get the constraint axes
    int axis1, axis2;
    GetRotationConstraint()->GetConstraintPlaneAxes(axis1, axis2);

    if (GetRotationConstraint()->GetConstraintModality(axis1) == mmiConstraint::FREE &&
        GetRotationConstraint()->GetConstraintModality(axis2) == mmiConstraint::FREE)
    {
      if (GetRotationConstraint()->GetRefSys()->GetType() == mafRefSys::VIEW)
      {
        if (constrainPlane == mmiConstraint::XY)
        {
          TrackballRotate();
        }      
        else
        {
          vtkGenericWarningMacro(<< "Rotation in VIEW ref sys on" << constrainPlane << "is not supported!");
        }
      }
      else
      {
        vtkGenericWarningMacro(<< "Only supporting VIEW refsys!");
      }
    }
    else
    {
      vtkGenericWarningMacro(<< "Only supporting FREE movement on both axes!");
    }
  }


  vtkDEL(transform);
}


//----------------------------------------------------------------------------
void mmiGenericMouse::Scale(double *p1, double *p2, double *viewup)
//----------------------------------------------------------------------------
{
  // NOT YET IMPLEMENTED
  //Produce a scale matrix based on current constrain

  /*
  //motion vector
  double motionVec[3];
  BuildVector(p1, p2, motionVec);

  //current axis versor
  double constrainVers[3];

  //view up vector
  double *viewUp = viewup;
  */
}

//----------------------------------------------------------------------------
int mmiGenericMouse::BinarySearch(double pos, vtkDoubleArray *array, int& result_point_status)
//----------------------------------------------------------------------------
{
  assert(array);
  
  int num_of_coord = array->GetNumberOfTuples();
  int sup = num_of_coord - 1;
  int inf = 0;
  int i;

  if (pos < array->GetValue(inf))
  {
    result_point_status = NOT_ON_GRID_POINT;
    return -1;
  }
  else if (pos > array->GetValue(sup))
  {
    result_point_status = NOT_ON_GRID_POINT;
    return sup;
  }
  else
  {
    while ((sup - inf) != 1)
    {    
      i = (inf + sup) / 2;
      if (pos == array->GetValue(i)) 
      {
        result_point_status = ON_GRID_POINT;
        return i;
      }
      if (pos > array->GetValue(i))
      {
        inf = i;
      } 
      else 
      {
        sup = i;
      }
    }

    result_point_status = NOT_ON_GRID_POINT;
    return inf;
  }
}

//----------------------------------------------------------------------------
double mmiGenericMouse::ProjectVectorOnAxis(const double *in_vector, const double *in_axis, double *out_projection)
//----------------------------------------------------------------------------
{

  double in_axis_norm[3];
  
  in_axis_norm[0] = in_axis[0];
  in_axis_norm[1] = in_axis[1];
  in_axis_norm[2] = in_axis[2];

  vtkMath::Normalize(in_axis_norm);

  double dot = vtkMath::Dot(in_vector, in_axis_norm);

  //if an output vector is provided
  if (out_projection)
  {
  out_projection[0] = dot * in_axis_norm[0];
  out_projection[1] = dot * in_axis_norm[1];
  out_projection[2] = dot * in_axis_norm[2];
  }

  return dot;
}

//----------------------------------------------------------------------------
double mmiGenericMouse::ProjectVectorOnPlane(const double *in_vector, const double *in_plane_normal, double *out_projection)
//----------------------------------------------------------------------------
{
  // normalise the plane normal
  double inPlaneNormalNormalised[3];

  inPlaneNormalNormalised[0] = in_plane_normal[0];  
  inPlaneNormalNormalised[1] = in_plane_normal[1];  
  inPlaneNormalNormalised[2] = in_plane_normal[2];
  
  vtkMath::Normalize(inPlaneNormalNormalised);

  // normalise the input vector
  double inVectorNormalised[3];
  inVectorNormalised[0] = in_vector[0];  
  inVectorNormalised[1] = in_vector[1];  
  inVectorNormalised[2] = in_vector[2];
  
  // get the input vector norm
  double norm = vtkMath::Normalize(inVectorNormalised);
  
  /*

   n: normalised plane normal;    
   A: vector to be projected
   a: normalised vector to be projected

   projection_vec = ||A||(n x (a x n))
  */

  double a_x_n[3];
  vtkMath::Cross(inVectorNormalised, inPlaneNormalNormalised, a_x_n);
 
  double n_x_a_x_n[3];
  vtkMath::Cross(inPlaneNormalNormalised, a_x_n, n_x_a_x_n);

  // the projection vector
  double projVec[3];
  projVec[0] = norm * n_x_a_x_n[0];
  projVec[1] = norm * n_x_a_x_n[1];
  projVec[2] = norm * n_x_a_x_n[2];

  //if an output vector is provided
  if (out_projection)
  {
  out_projection[0] = projVec[0];
  out_projection[1] = projVec[1];
  out_projection[2] = projVec[2];
  }

  // return the norm of the projection
  return vtkMath::Norm(projVec);
}

//----------------------------------------------------------------------------
void mmiGenericMouse::ConcatenateToResultMatrix(vtkMatrix4x4 *matrix)
//----------------------------------------------------------------------------
{
  if (GetResultMatrix() && matrix) 
  {
    vtkTransform *tmptr = vtkTransform::New();
    tmptr->SetMatrix(this->GetResultMatrix());

    if (this->GetResultMatrixConcatenationSemantic() == POSTMULTIPLY)
    {
      tmptr->PostMultiply();
    }
    else if (this->GetResultMatrixConcatenationSemantic() == PREMULTIPLY)
    {
      tmptr->PreMultiply();
    }

    tmptr->Concatenate(matrix);
    GetResultMatrix()->DeepCopy(tmptr->GetMatrix());
    vtkDEL(tmptr);
  }
}

//----------------------------------------------------------------------------
void mmiGenericMouse::SendTransformMatrix(double *vector, int mouseAction)
//----------------------------------------------------------------------------
{
  vtkMatrix4x4 *mat = vtkMatrix4x4::New();
  mflTransform::SetPosition(mat, vector);
  SendTransformMatrix(mat, mouseAction);
  vtkDEL(mat);
}

//----------------------------------------------------------------------------
void mmiGenericMouse::SendTransformMatrix(double *versor, double translation, int mouseAction)
//----------------------------------------------------------------------------
{
  double tempVec[3] = {
                         (translation) * versor[0], 
                         (translation) * versor[1], 
                         (translation) * versor[2]
                      };

  SendTransformMatrix(tempVec, mouseAction);
}

//----------------------------------------------------------------------------
void mmiGenericMouse::SendTransformMatrix(vtkMatrix4x4 *matrix, int mouseAction, float rotationAngle)
//----------------------------------------------------------------------------
{ 
  // matrix registering the pick position in world coordinates at MOUSE_DOWN
  vtkMatrix4x4 *pickPosMatrix = vtkMatrix4x4::New();

  if (this->GetResultMatrixConcatenation())
  {
    ConcatenateToResultMatrix(matrix);
  }

  mafEvent e;
  e.SetMatrix(matrix);
  e.SetId(ID_TRANSFORM);
  e.SetSender(this);
  e.SetArg(mouseAction);
  e.SetFloat(rotationAngle);

  // if mouseAction == MOUSE_DOWN notify the listener about the
  // picked position in world coordinates
  if (mouseAction == MOUSE_DOWN)
  {
    mflTransform::SetPosition(pickPosMatrix, (float) LastPickPosition[0],
      (float) LastPickPosition[1], (float) LastPickPosition[2]);
    e.SetVtkObj(pickPosMatrix);
  }  

  mafEventMacro(e);

  // send standard event... have to recreate it since original from device is not available :-(
  mflSmartPointer<mflEventInteraction> event;
  event->SetSender(this);
  // have to recreate the time stamp :-(
  mflSmartPointer<mflMatrix> tmatrix;
  tmatrix->DeepCopy(matrix);
  tmatrix->SetTimeStamp(vtkTimerLog::GetTimeStamp());
  event->SetMatrix(tmatrix);
  event->Set2DPosition(MousePose[0],MousePose[1]);
  if (mouseAction == MOUSE_DOWN)
  {
    event->SetID(ButtonDownEvent);
    event->SetButton(GetStartButton());
    event->SetModifiers(GetModifiers());
  }
  else if (mouseAction == MOUSE_UP)
  {
    event->SetID(ButtonUpEvent);
    event->SetButton(GetStartButton());
    event->SetModifiers(GetModifiers());
  }
  else
  {
    event->SetID(MoveActionEvent);
  }

  ForwardEvent(event);
  // clean up
  vtkDEL(pickPosMatrix); 
}

//----------------------------------------------------------------------------
void mmiGenericMouse::OnMouseMoveAction(int X, int Y)
//----------------------------------------------------------------------------
{
  // picked X and Y in screen coordinates
  MousePose[0] = X;
  MousePose[1] = Y;
    
   double focalPoint[4], pickPoint[4], prevPickPoint[4], viewUp[3];
   double z;

   assert(CurrentCamera);

   //get the view up vector
   CurrentCamera->OrthogonalizeViewUp();
   CurrentCamera->GetViewUp(viewUp);
   vtkMath::Normalize(viewUp);

   //get the focal point
   this->ComputeWorldToDisplay(this->LastPickPosition[0], this->LastPickPosition[1],
                               this->LastPickPosition[2], focalPoint);
   z = focalPoint[2];

   this->ComputeDisplayToWorld((double)LastMousePose[0], (double)LastMousePose[1],
                               z, prevPickPoint);
 
   this->ComputeDisplayToWorld(double(X), double(Y), z, pickPoint);

   // Now we have prevPickPoint, pickPoint and viewUp;

   // prevPickPoint and pickPoint are the world coordinates of picked points 
   // do different things depending on the state 

   if (RotationFlag == true)
   {
      // rotate
      this->Rotate(prevPickPoint, pickPoint, viewUp);
   }

   if (TranslationFlag == true)
   {
     // translate
     this->Translate(prevPickPoint, pickPoint);
   }

   if (ScalingFlag == true)
   {
     // scale
     // not yet implemented
   }

   if (UniformScalingFlag == true)
   {
     // uniform scale
     // not yet implemented
   }

  // Update LastMousePose
  LastMousePose[0] = MousePose[0];
  LastMousePose[1] = MousePose[1];

}

//----------------------------------------------------------------------------
void mmiGenericMouse::OnButtonDownAction(int X, int Y)
//----------------------------------------------------------------------------
{
  // debug
  // set the constraint refsys to translation constraint, the matrix is referenced
  ConstrainRefSys = GetTranslationConstraint()->GetRefSys()->GetMatrix();


  // register MousePose
  MousePose[0] = LastMousePose[0] = X;
  MousePose[1] = LastMousePose[1] = Y;

  // perform picking on current renderer
  this->Picker->Pick(X,Y,0.0,Renderer);
  this->Picker->GetPickPosition(this->LastPickPosition);
 
  //reset projection accumulator   
    ProjAcc = 0;

    /*
            ^
            |                   
     -------A->----0-------1-----i-----rp-----i+1------N-1-------rp-
                                      pos_rp
                                 |
            ^                HelpPIndex
            |
    --------C->  

    C:  constrain ref sys origin
    rp: result point refsys origin
    A: double array 

                     projVal
                      -->
    -----|------------|-----------|------|---------------|----------->
        O          pos_rp      minb   pos_rp           maxb

    ------------------------>ProjAcc
    */ 


  if (TranslationFlag == true)
  {
    // if translation is constrained on one axis:
    if (GetTranslationConstraint()->GetNumberOfDOF() == 1)
    {
      // constrain axis
      int axis; 

      // get the constrain axis
      axis = GetTranslationConstraint()->GetConstraintAxis();

      // set constrain refsys to translation constraint 
      ConstrainRefSys = GetTranslationConstraint()->GetRefSys()->GetMatrix();

      if (GetTranslationConstraint()->GetConstraintModality(axis) == mmiConstraint::SNAP_ARRAY)
      {         
        // current axis versor
        double constrainVers[3];

        // rp vector
        double rp[3] = {0, 0, 0}, proj[3] = {0, 0, 0};
        double pos_rp = 0;

        mflTransform::GetVersor(axis, ConstrainRefSys, constrainVers);
        mflTransform::GetPosition(VME->GetMatrixPipe()->GetMatrix(), rp);

        //project rp on constrain axis
        pos_rp = ProjectVectorOnAxis(rp, constrainVers, proj);

        //set HelpPIndex and HelperPointStatus
        HelpPIndex = BinarySearch(pos_rp, 
                                  GetTranslationConstraint()->GetSnapArray(axis),
                                  HelperPointStatus);      
      }
    }
    // else if translation is constrained on a plane
    else if (GetTranslationConstraint()->GetNumberOfDOF() == 2)
    {
      // currently supporting only free movement on a plane
      // TODO 
    }
  }

  LastX = MousePose[0];
  LastY = MousePose[1];

  // notify the listener about the mouse action that has been performed 
  vtkMatrix4x4 *identity = vtkMatrix4x4::New();
  SendTransformMatrix(identity, MOUSE_DOWN);
  vtkDEL(identity);
}

//----------------------------------------------------------------------------
void mmiGenericMouse::OnButtonUpAction()
//----------------------------------------------------------------------------
{
  // reset projection accumulator
  this->ProjAcc = 0;

  // register LastX and Last
  LastX = MousePose[0];
  LastY = MousePose[1];

  // notify the listener about the mouse action that has been performed 
  vtkMatrix4x4 *identity = vtkMatrix4x4::New();
  SendTransformMatrix(identity, MOUSE_UP);
  vtkDEL(identity);
}

//----------------------------------------------------------------------------
void mmiGenericMouse::TrackballRotate()
//----------------------------------------------------------------------------
{   
   vtkTransform *transform = vtkTransform::New();
   double v2[3];

   // set constraint refsys to rotation constraint
   ConstrainRefSys = GetRotationConstraint()->GetRefSys()->GetMatrix();

   // register last mouse position
   int x = MousePose[0];
   int y = MousePose[1];

   // the pivot center
   double pivotPoint[3];

   // get the centre of rotation from Pivot RefSys
   mflTransform::GetPosition(GetPivotRefSys()->GetMatrix(), pivotPoint);

   // translate to center of the constrain ref sys
   transform->Identity();
   transform->Translate(pivotPoint[0], pivotPoint[1], pivotPoint[2]);
   
   float dx = this->LastX - x;
   float dy = this->LastY - y;
   
   // azimuth
   CurrentCamera->OrthogonalizeViewUp();
   double *viewUp = CurrentCamera->GetViewUp();
   int *size = Renderer->GetSize();
   transform->RotateWXYZ(-360.0 * dx / size[0], viewUp[0], viewUp[1], viewUp[2]);
   
   // elevation
   vtkMath::Cross(CurrentCamera->GetDirectionOfProjection(), viewUp, v2);
   transform->RotateWXYZ(360.0 * dy / size[1], v2[0], v2[1], v2[2]);
   
   // translate back
   transform->Translate(-pivotPoint[0], -pivotPoint[1], -pivotPoint[2]);
   
   //send the transform matrix
   SendTransformMatrix(transform->GetMatrix());

   // clean up
   transform->Delete();
 
   // update LastX and LastY
   this->LastX = x;
   this->LastY = y;
}

//----------------------------------------------------------------------------
void mmiGenericMouse::TrackballTranslate()
//----------------------------------------------------------------------------
{  
  // Use initial center as the origin from which to pan

  float *obj_center = Prop->GetCenter();

  float disp_obj_center[3], new_pick_point[4], old_pick_point[4], motion_vector[3];
  
  this->ComputeWorldToDisplay(obj_center[0], obj_center[1], obj_center[2], 
                              disp_obj_center);

  this->ComputeDisplayToWorld((double)MousePose[0], (double)MousePose[1], disp_obj_center[2], new_pick_point);
  
  this->ComputeDisplayToWorld((double)LastMousePose[0], (double)LastMousePose[1], disp_obj_center[2], old_pick_point);
  
  motion_vector[0] = new_pick_point[0] - old_pick_point[0];
  motion_vector[1] = new_pick_point[1] - old_pick_point[1];
  motion_vector[2] = new_pick_point[2] - old_pick_point[2];
    
  vtkTransform *t = vtkTransform::New();
  t->Translate(motion_vector[0], motion_vector[1], motion_vector[2]);
  
  //send the transform matrix
  SendTransformMatrix(t->GetMatrix());
    
  // clean up
  t->Delete();

}

//----------------------------------------------------------------------------
void mmiGenericMouse::TrackballRoll()
//----------------------------------------------------------------------------
{
   // get the rotation refsys
   ConstrainRefSys = GetRotationConstraint()->GetRefSys()->GetMatrix();  

   // the refsys center
   double pivotPoint[3];

   // get the centre of rotation from PivotRefSys
   mflTransform::GetPosition(GetPivotRefSys()->GetMatrix(), pivotPoint); 
   
   // Get the axis to rotate around = vector from eye to origin
 
   float motion_vector[3];
   double view_point[3];
 
   if (CurrentCamera->GetParallelProjection())
   {
     // If parallel projection, want to get the view plane normal...
     CurrentCamera->ComputeViewPlaneNormal();
     CurrentCamera->GetViewPlaneNormal(motion_vector);
   }
   else
   {   
     // Perspective projection, get vector from eye to center of actor
     CurrentCamera->GetPosition(view_point);
     motion_vector[0] = view_point[0] - pivotPoint[0];
     motion_vector[1] = view_point[1] - pivotPoint[1];
     motion_vector[2] = view_point[2] - pivotPoint[2];
     vtkMath::Normalize(motion_vector);
   }
   
   float disp_refSysCenter[3];
   
   this->ComputeWorldToDisplay(pivotPoint[0], pivotPoint[1], pivotPoint[2], 
                               disp_refSysCenter);
   
   double newAngle = 
     atan2((double)MousePose[1] - (double)disp_refSysCenter[1],
           (double)MousePose[0] - (double)disp_refSysCenter[0]);
 
   double oldAngle = 
     atan2((double)LastMousePose[1] - (double)disp_refSysCenter[1],
           (double)LastMousePose[0] - (double)disp_refSysCenter[0]);
   
   newAngle *= vtkMath::RadiansToDegrees();
   oldAngle *= vtkMath::RadiansToDegrees();
   
   vtkTransform *t = vtkTransform::New();
   t->PostMultiply();
   t->Translate(-pivotPoint[0], -pivotPoint[1], -pivotPoint[2]);
   t->RotateWXYZ(newAngle - oldAngle, motion_vector);
   t->Translate(pivotPoint[0], pivotPoint[1], pivotPoint[2]);
     
   //send the transform matrix
   SendTransformMatrix(t->GetMatrix());

   // clean up
   t->Delete();
}

//----------------------------------------------------------------------------
void mmiGenericMouse::SnapOnSurface()
//----------------------------------------------------------------------------
{
  if (Renderer == NULL || Prop == NULL ) return;

  int x = MousePose[0];
  int y = MousePose[1];

	vtkPropCollection *pc = vtkPropCollection::New();
	Prop->GetActors(pc); 
	pc->InitTraversal();
	vtkProp* p = pc->GetNextProp();
	while(p)
	{
		p->PickableOff();
		p = pc->GetNextProp();
	}
  
  int picked = Picker->Pick(x,y,0,Renderer);

	pc->InitTraversal();
	p = pc->GetNextProp();
	while(p)
	{
		p->PickableOn();
		p = pc->GetNextProp();
	}

	if(picked==0)
	{
     TrackballTranslate();
		 return;
	}

  float newAbsPickPos[3];
  Picker->GetPickPosition(newAbsPickPos);
  
  double absPivotPos[3];
  assert(VME);
  mflTransform::GetPosition(VME->GetAbsMatrixPipe()->GetMatrix(), absPivotPos);

  double absMotionVec[3];
  absMotionVec[0] = newAbsPickPos[0] - absPivotPos[0];
  absMotionVec[1] = newAbsPickPos[1] - absPivotPos[1];
  absMotionVec[2] = newAbsPickPos[2] - absPivotPos[2];

  SendTransformMatrix(absMotionVec);
}
//----------------------------------------------------------------------------
void mmiGenericMouse::BuildVector(double coeff, const double *inVector, double *outVector, int refSysType, int localAxis)
//----------------------------------------------------------------------------
{
  if (outVector)
  {
    // default
    if (refSysType == mafRefSys::GLOBAL)
    {
      outVector[0] = coeff * inVector[0];
      outVector[1] = coeff * inVector[1];
      outVector[2] = coeff * inVector[2];
    }
    else if (refSysType == mafRefSys::LOCAL)
    {

      outVector[0] = outVector[1] = outVector[2];
      switch (localAxis)
      {
        case (X):
        {
          outVector[0] = coeff;
        }
        break;
        case (Y):
        {
          outVector[1] = coeff;
        }
        break;
        case (Z):
        {
          outVector[2] = coeff;
        }
        break;
      }
    }
    else
    {
      vtkWarningMacro(<< "Ref sys type: " << refSysType << "is not supported!");
    }
  }
}

//------------------------------------------------------------------------------
void mmiGenericMouse::EnableTranslationInternal(bool enable)
//------------------------------------------------------------------------------
{
  TranslationFlag = enable;
  RotationFlag = ScalingFlag = UniformScalingFlag = !enable;
}

//------------------------------------------------------------------------------
void mmiGenericMouse::EnableRotationInternal(bool enable)
//------------------------------------------------------------------------------
{
  RotationFlag = enable;
  TranslationFlag = ScalingFlag = UniformScalingFlag = !enable;
}

//------------------------------------------------------------------------------
void mmiGenericMouse::EnableScalingInternal(bool enable)
//------------------------------------------------------------------------------
{
  ScalingFlag = enable;
  TranslationFlag = RotationFlag = UniformScalingFlag = !enable;
}

//------------------------------------------------------------------------------
void mmiGenericMouse::EnableUniformScalingInternal(bool enable)
//------------------------------------------------------------------------------
{
  UniformScalingFlag = enable;
  TranslationFlag = RotationFlag = ScalingFlag = !enable;
}