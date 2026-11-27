/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaBehaviorTransform
 Authors: Stefano Perticoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "albaBehaviorTransform.h"
#include "albaRWIBase.h"

#include "albaDecl.h"
#include "albaRefSys.h"


#include "albaView.h"
#include "albaEvent.h"
#include "albaAbsMatrixPipe.h"
#include "albaVME.h"
#include "albaMatrix.h"
#include "albaTransform.h"
#include "albaVMESurface.h"
#include "albaSceneGraph.h"
#include "albaSceneNode.h"

#include "vtkDoubleArray.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkMath.h"
#include "vtkTransform.h"
#include "vtkCamera.h"
#include "vtkRenderer.h"
#include "vtkTimerLog.h"
#include "vtkPolyData.h"
#include "vtkPolyDataNormals.h"
#include "vtkPointData.h"
#include "vtkCellData.h"
#include "vtkPropCollection.h"
#include "vtkALBAAssembly.h"
#include "vtkALBASmartPointer.h"
#include "mmuIdFactory.h"

albaCxxTypeMacro(albaBehaviorTransform);

//----------------------------------------------------------------------------
albaBehaviorTransform::albaBehaviorTransform() 
//----------------------------------------------------------------------------
{
  m_TransformButton = -1;
  m_LastDragPos[0] = m_LastDragPos[1] = 0;
  m_TranslationFlag       = true;
  m_RotationFlag          = true;
  m_SurfaceSnap           = false;
	m_SurfaceNormal         = false;
  m_ResultTransform       = NULL;
  m_TargetRefSys          = new albaRefSys;
  m_TranslationConstraint = new albaTransformConstraint;
  m_RotationConstraint    = new albaTransformConstraint;
  m_ScaleConstraint       = new albaTransformConstraint;
  m_PivotRefSys           = new albaRefSys;
  m_CurrentRotationConstraint = m_RotationConstraint;
  m_RotationConstraint->GetRefSys()->SetTypeToView(); // the Renderer is set later!!!
  m_TranslationConstraint->GetRefSys()->SetTypeToView(); // the Renderer is set later!!!
  m_PivotRefSys->SetTypeToLocal(); // the VME is set later !!!

   m_ResultMatrixConcatenation = true;

   SetResultMatrixConcatenationSemanticToPostMultiply();
  
   // Projection Accumulator
   m_ProjectionAccumulator = 0;

   m_LastX = m_LastY = 0;

   // default is to translation mode
   EnableTranslation(true);

   // set default constraint axes
   GetRotationConstraint()->SetConstraintModality(albaTransformConstraint::FREE, albaTransformConstraint::FREE, albaTransformConstraint::LOCK);
   GetTranslationConstraint()->SetConstraintModality(albaTransformConstraint::FREE, albaTransformConstraint::FREE, albaTransformConstraint::LOCK);

   m_CurrentCamera = NULL;
   m_MousePointer2DPosition[0] = m_MousePointer2DPosition[1] = 0;
   m_LastMousePointer2DPosition[0] = m_LastMousePointer2DPosition[1] = 0;
   m_ConstrainRefSys = NULL;
}
//----------------------------------------------------------------------------
albaBehaviorTransform::~albaBehaviorTransform()
//----------------------------------------------------------------------------
{
  albaDEL(m_ResultTransform);
  delete m_TargetRefSys; m_TargetRefSys = NULL;
  delete m_PivotRefSys; m_PivotRefSys = NULL;
  delete m_TranslationConstraint; m_TranslationConstraint = NULL;
  delete m_RotationConstraint; m_RotationConstraint = NULL;
  delete m_ScaleConstraint; m_ScaleConstraint = NULL;
}

//------------------------------------------------------------------------------
void albaBehaviorTransform::SetResultTransform(albaTransform *result)
//------------------------------------------------------------------------------
{
  if (result!=m_ResultTransform)
  {
    albaDEL(m_ResultTransform);
    m_ResultTransform=result;
    m_ResultTransform->Register(this);
  }
}
//------------------------------------------------------------------------------
void albaBehaviorTransform::SetResultMatrix(albaMatrix *result)
//------------------------------------------------------------------------------
{
  albaSmartPointer<albaTransform> trans; // create a transform on the fly
  trans->SetMatrixPointer(result); // make it reference the result matrix
  SetResultTransform(trans);
}

//------------------------------------------------------------------------------
albaMatrix *albaBehaviorTransform::GetResultMatrix()
//------------------------------------------------------------------------------
{ 
  return (m_ResultTransform?m_ResultTransform->GetMatrixPointer():NULL);
}

//------------------------------------------------------------------------------
void albaBehaviorTransform::SetTargetRefSys(albaRefSys &ref_sys)
//------------------------------------------------------------------------------
{
  *m_TargetRefSys=ref_sys;
}
//------------------------------------------------------------------------------
void albaBehaviorTransform::SetTargetRefSys(albaRefSys *ref_sys)
//------------------------------------------------------------------------------
{
  m_TargetRefSys->DeepCopy(ref_sys);
}

//------------------------------------------------------------------------------
void albaBehaviorTransform::SetPivotRefSys(albaRefSys *pivot_frame )
//------------------------------------------------------------------------------
{
  m_PivotRefSys->DeepCopy(pivot_frame);
}

//------------------------------------------------------------------------------
void albaBehaviorTransform::SetPivotRefSys(albaRefSys &pivot_frame )
//------------------------------------------------------------------------------
{
  *m_PivotRefSys=pivot_frame;
}

//------------------------------------------------------------------------------
void albaBehaviorTransform::SetRendererAndView(vtkRenderer *ren, albaView *view)
//------------------------------------------------------------------------------
{
  m_TranslationConstraint->GetRefSys()->SetRenderer(ren);  
  m_RotationConstraint->GetRefSys()->SetRenderer(ren);  
  m_ScaleConstraint->GetRefSys()->SetRenderer(ren);  
  m_PivotRefSys->SetRenderer(ren);
  Superclass::SetRendererAndView(ren,view);
}

//------------------------------------------------------------------------------
bool albaBehaviorTransform::OnLeftButtonDownImpl(albaRWIBase *rwi, vtkRenderer *ren, double pos[2])
{
  return StartTransform(ALBA_LEFT_BUTTON, rwi, ren, pos);
}

//------------------------------------------------------------------------------
bool albaBehaviorTransform::OnMiddleButtonDownImpl(albaRWIBase *rwi, vtkRenderer *ren, double pos[2])
{
  return StartTransform(ALBA_MIDDLE_BUTTON, rwi, ren, pos);
}

//------------------------------------------------------------------------------
bool albaBehaviorTransform::PrepareTransform(int button)
{
  return button == ALBA_LEFT_BUTTON;
}

//------------------------------------------------------------------------------
bool albaBehaviorTransform::StartTransform(int button, albaRWIBase *rwi, vtkRenderer *ren, double pos[2])
{
  StopTransform(m_TransformButton);
  if (ren == NULL)
    return false;

  SetRendererAndView(ren, rwi->GetView());
  m_CurrentRotationConstraint = m_RotationConstraint;
  if (!PrepareTransform(button))
    return false;

  SetCurrentCamera(ren->GetActiveCamera());
  m_LastDragPos[0] = (int)pos[0];
  m_LastDragPos[1] = (int)pos[1];

  // set before the button down is sent: a listener reacting to it can already stop the transform
  m_TransformButton = button;
  OnButtonDownAction(m_LastDragPos[0], m_LastDragPos[1]);
  return true;
}

//------------------------------------------------------------------------------
bool albaBehaviorTransform::OnDragImpl(albaRWIBase *rwi, vtkRenderer *ren, double pos[2])
{
  if (m_TransformButton < 0)
    return false;

  if ((int)pos[0] != m_LastDragPos[0] || (int)pos[1] != m_LastDragPos[1])
  {
    m_LastDragPos[0] = (int)pos[0];
    m_LastDragPos[1] = (int)pos[1];
    OnMouseMoveAction(m_LastDragPos[0], m_LastDragPos[1]);
  }
  return true;
}

//------------------------------------------------------------------------------
bool albaBehaviorTransform::OnLeftButtonUpImpl(albaRWIBase *rwi, vtkRenderer *ren, double pos[2])
{
  return StopTransform(ALBA_LEFT_BUTTON);
}

//------------------------------------------------------------------------------
bool albaBehaviorTransform::OnMiddleButtonUpImpl(albaRWIBase *rwi, vtkRenderer *ren, double pos[2])
{
  return StopTransform(ALBA_MIDDLE_BUTTON);
}

//------------------------------------------------------------------------------
bool albaBehaviorTransform::StopTransform(int button)
{
  if (button < 0 || button != m_TransformButton)
    return false;

  m_TransformButton = -1;
  OnButtonUpAction();
  return true;
}

//------------------------------------------------------------------------------
void albaBehaviorTransform::SetVME(albaVME *vme)
//------------------------------------------------------------------------------
{
  Superclass::SetVME(vme);
  m_RotationConstraint->GetRefSys()->SetVME(vme);
  m_TranslationConstraint->GetRefSys()->SetVME(vme);
  m_ScaleConstraint->GetRefSys()->SetVME(vme);
  m_PivotRefSys->SetVME(vme);
}

//------------------------------------------------------------------------------
void albaBehaviorTransform::EnableTranslation(bool enable)
//------------------------------------------------------------------------------
{
  m_TranslationFlag = enable;
  m_RotationFlag = !enable;
}

//------------------------------------------------------------------------------
void albaBehaviorTransform::EnableRotation(bool enable)
//------------------------------------------------------------------------------
{
  m_RotationFlag = enable;
  m_TranslationFlag = !enable;
}

//----------------------------------------------------------------------------
void albaBehaviorTransform::Translate(double *p1, double *p2)
//----------------------------------------------------------------------------
{
  albaMatrix matr;     // create matrix to send
  double motionVec[3];// motion vector
  albaTransform::BuildVector(p1, p2, motionVec);
  int refSysType = GetTranslationConstraint()->GetRefSys()->GetType();// get the ref sys type  

  // set the constraint ref-sys to translation constraint, the matrix is referenced
  m_ConstrainRefSys = GetTranslationConstraint()->GetRefSys()->GetMatrix();
  if (GetTranslationConstraint()->GetNumberOfDOF() == 1)
  { 
    double rp[3];         // result point position
    double pos_rp;        // result point projected on constrain axis
    double motVecProj[3]; // projection of motion vector on constrain axis 
    double projVal;       // projection value along constrain axis
    double constrainVersor[3];//current axis versor
    assert(m_VME);
    albaTransform::GetPosition(*m_VME->GetOutput()->GetMatrix(), rp);    //get the result point pos
    int axis = GetTranslationConstraint()->GetConstraintAxis();         //get the constrain axis
    albaTransform::GetVersor(axis, *m_ConstrainRefSys, constrainVersor);   //project the motion vector on constrain axis
    projVal = albaTransform::ProjectVectorOnAxis(motionVec, constrainVersor, motVecProj);

    // project rp on constrain axis; if target ref-sys is local the projection 
    // is the value along the constrain axis
    if (refSysType == albaRefSys::GLOBAL)
      pos_rp = albaTransform::ProjectVectorOnAxis(rp, constrainVersor);
    else
      pos_rp = rp[axis];

    switch(GetTranslationConstraint()->GetConstraintModality(axis)) 
    {
      case (albaTransformConstraint::FREE):
      { 
        albaTransform::SetPosition(matr, motVecProj);//set the matrix to be send
        SendTransformMatrix(matr);                  //send the transform matrix
      }
      break;
      case (albaTransformConstraint::BOUNDS):
      {
  /*
      ------------proj-------->  
      
      0-------minb-------------rp------------------maxb------->      
                              pos_rp
      ^  
      |
      C-> constrainVersor  

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
      C-> constrainVersor              rp

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
    
      albaTransform::GetPosition(*m_VME->GetOutput()->GetMatrix(), rp);
    
      /*                         projVal  
                             -->
          -----|------------|-----------|------|---------------|----------->
               O          pos_rp      minb   pos_rp           maxb

          ------------------------>m_ProjectionAccumulator
      */ 

      //update projection accumulator
      m_ProjectionAccumulator += projVal;

      if (pos_rp < minb)
      {
        if (pos_rp + m_ProjectionAccumulator > minb)
        {
          albaTransform::BuildVector(minb - pos_rp, constrainVersor, tmpVec, refSysType, axis);
          albaTransform::SetPosition(matr, tmpVec);                             
          m_ProjectionAccumulator = 0;
        }
      }
      else if (pos_rp > maxb)
      { 
        if (pos_rp + m_ProjectionAccumulator < maxb)
        {
          albaTransform::BuildVector(maxb - pos_rp, constrainVersor, tmpVec, refSysType, axis);
          albaTransform::SetPosition(matr, tmpVec);            
          m_ProjectionAccumulator = 0;
        }
      }
      else if (minb < pos_rp && pos_rp < maxb)
      {
        if (pos_rp + projVal < minb)
        {              
          albaTransform::BuildVector(minb - pos_rp, constrainVersor, tmpVec, refSysType, axis);
          albaTransform::SetPosition(matr, tmpVec);                           
          m_ProjectionAccumulator = pos_rp + projVal - minb;
        }
        else if (pos_rp + projVal > maxb)
        {            
          albaTransform::BuildVector(maxb - pos_rp, constrainVersor, tmpVec, refSysType, axis);
          albaTransform::SetPosition(matr, tmpVec);
          m_ProjectionAccumulator = pos_rp + projVal - maxb;
        }
        else
        {
          if (refSysType == albaRefSys::GLOBAL)
          {
            albaTransform::SetPosition(matr, motVecProj);             
          }
          else
          {
            albaTransform::BuildVector(projVal, NULL, tmpVec, refSysType, axis);
            albaTransform::SetPosition(matr, tmpVec);
          }
        }
      }
      else if (pos_rp == minb) 
      {
                                                                
    /*                                              
                                                      projVal
                                                  <-------- 
                                                      m_ProjectionAccumulator
                                                  <----|
                                  2   |    1           |  3
        -----|------------------------|----------------|----------->
              O                       minb             maxb
                                    pos_rp 
                              
    */ 


          if (pos_rp + m_ProjectionAccumulator >= minb)
          {
            if (pos_rp + m_ProjectionAccumulator <= maxb)
            {
              albaTransform::BuildVector(m_ProjectionAccumulator, constrainVersor, tmpVec, refSysType, axis);
              albaTransform::SetPosition(matr, tmpVec);                       
  
              m_ProjectionAccumulator = 0;
            }
            else if (pos_rp + m_ProjectionAccumulator > maxb)
            {
              albaTransform::BuildVector(maxb - pos_rp, constrainVersor, tmpVec, refSysType, axis);
              albaTransform::SetPosition(matr, tmpVec);                         
              m_ProjectionAccumulator -= maxb - pos_rp;
            }
          }
        }
        else if (pos_rp == maxb)
        {
          if (pos_rp + m_ProjectionAccumulator <= maxb)
          {
            if (pos_rp + m_ProjectionAccumulator >= minb)
            {
              albaTransform::BuildVector(m_ProjectionAccumulator, constrainVersor, tmpVec, refSysType, axis);
              albaTransform::SetPosition(matr, tmpVec);     
              m_ProjectionAccumulator = 0;
            }
            else if (pos_rp + m_ProjectionAccumulator < minb)
            {
              albaTransform::BuildVector(minb - pos_rp, constrainVersor, tmpVec, refSysType, axis);
              albaTransform::SetPosition(matr, tmpVec);     
              m_ProjectionAccumulator -= minb - pos_rp;
            }

          }          
        }
        
        //send the transform matrix
        SendTransformMatrix(matr);
           
      }
      break;

      case (albaTransformConstraint::SNAP_STEP):
      {
        double step = GetTranslationConstraint()->GetStep(axis);
        int motion_direction = motVecProj[axis] > 0 ? 1 : -1;
        motVecProj[axis] = motion_direction * step;
        albaTransform::SetPosition(matr, motVecProj);//set the matrix to be send
        SendTransformMatrix(matr);                  //send the transform matrix
      }
      break;
  
      case (albaTransformConstraint::SNAP_ARRAY):
      {
        //get a pointer to the snap array
        vtkDoubleArray *darray = GetTranslationConstraint()->GetSnapArray(axis);
  
        double normalizedVectorToSend[3] = {0, 0, 0};

        if (refSysType == albaRefSys::GLOBAL)
        {
          normalizedVectorToSend[0] = constrainVersor[0];
          normalizedVectorToSend[1] = constrainVersor[1];
          normalizedVectorToSend[2] = constrainVersor[2];
        }
        else if (refSysType == albaRefSys::LOCAL)
        {
          normalizedVectorToSend[axis] = 1;
        } 
        else
        {
          albaWarningMacro(<< "Ref sys type:" << refSysType << "is unsupported!");
          return;
        }


        m_ProjectionAccumulator += projVal;

        if (m_HelperPointStatus == ON_GRID_POINT)
        {

//              pos_rp             m_ProjectionAccumulator
//    ^------------------------->|-------------->        
//    |                             
//   -O----------0-------1-------i----------3------N-1---------->
//                           m_HelpPIndex    ind    
//                               |--------->|--->
//    m_HelpPIndex  = i;               trval                
//    m_HelperPointStatus = ON_GRID_POINT;
        
          int status;
          int ind = BinarySearch(pos_rp + m_ProjectionAccumulator, darray, status);    

          if (m_ProjectionAccumulator > 0)
          {
            if (ind != m_HelpPIndex)
            {
              double trval = darray->GetValue(ind) - darray->GetValue(m_HelpPIndex);

              //send translation vector from m_HelpPIndex to ind
              SendTransformMatrix(normalizedVectorToSend, trval);  

              //UpdateProjAcc  
              m_ProjectionAccumulator -= trval;

              //update m_HelpPIndex
              m_HelpPIndex = ind;
            }
          }
          else if (m_ProjectionAccumulator < 0)
          {
            if (ind != m_HelpPIndex - 1)
            {
              double trval = darray->GetValue(ind + 1) - darray->GetValue(m_HelpPIndex);
   
              //send translation vector from m_HelpPIndex to ind
              SendTransformMatrix(normalizedVectorToSend, trval);  
  
              //UpdateProjAcc  
              m_ProjectionAccumulator -= trval;
   
              //update m_HelpPIndex
              m_HelpPIndex = ind + 1;
            }
          }
        }
        else if (m_HelperPointStatus == NOT_ON_GRID_POINT) 
        {
//              pos_rp             m_ProjectionAccumulator
//    ^------------------------->|---------->        
//    |                          |   
//   -O----------0-------1-------|------2------N-1---------->
//                   m_HelpPIndex  |----->| 
//                                 trval
//    m_HelpPIndex  = i;                               
//    m_HelperPointStatus = NOT_ON_GRID_POINT;
      
          int status;
          int ind = BinarySearch(pos_rp + m_ProjectionAccumulator, darray, status); 
      
          if (ind != m_HelpPIndex)
          {
            double trval = darray->GetValue(m_ProjectionAccumulator > 0 ? ind : ind + 1) - pos_rp;

            //send translation vector from m_HelpPIndex to ind
            SendTransformMatrix(normalizedVectorToSend, trval); 

            //UpdateProjAcc  
            m_ProjectionAccumulator -= trval;

            //update m_HelpPIndex
            m_HelpPIndex = ind;
          }
        }   
      }
      break;
    }
  }
  else if (GetTranslationConstraint()->GetNumberOfDOF() == 2)
  {    
    // the constrain plane
    int constrainPlane;

    // get the translation plane
    constrainPlane = GetTranslationConstraint()->GetConstraintPlane();

    // get constraint axes
    int axis1, axis2;
    GetTranslationConstraint()->GetConstraintPlaneAxes(axis1, axis2);
      
    if (GetTranslationConstraint()->GetConstraintModality(axis1) == albaTransformConstraint::FREE &&
        GetTranslationConstraint()->GetConstraintModality(axis2) == albaTransformConstraint::FREE)
    {
      if (GetTranslationConstraint()->GetRefSys()->GetType() == albaRefSys::CUSTOM
       || GetTranslationConstraint()->GetRefSys()->GetType() == albaRefSys::GLOBAL
       || GetTranslationConstraint()->GetRefSys()->GetType() == albaRefSys::PARENT
       || GetTranslationConstraint()->GetRefSys()->GetType() == albaRefSys::LOCAL        
         )
      {    
        // translate on a plane

        // the projection of motion vector on the constrain plane
        double motVecProj[3];
        double planeNormal[3];

        switch  (constrainPlane) 
        {
          case (albaTransformConstraint::XY):
          {
            albaTransform::GetVersor(albaTransformConstraint::Z, *m_ConstrainRefSys, planeNormal);
          }
  	      break;
          case (albaTransformConstraint::XZ):
          {
            albaTransform::GetVersor(albaTransformConstraint::Y, *m_ConstrainRefSys, planeNormal);
          }
          break;
          case (albaTransformConstraint::YZ):
          {
            albaTransform::GetVersor(albaTransformConstraint::X, *m_ConstrainRefSys, planeNormal);
          }
          break;
        }
    
        // project the motion vector on constrain plane    
        albaTransform::ProjectVectorOnPlane(motionVec, planeNormal, motVecProj);

       //set the matrix to be send
        albaTransform::SetPosition(matr, motVecProj);
          
        //send the transform matrix
        SendTransformMatrix(matr);
      }
      else if (GetTranslationConstraint()->GetRefSys()->GetType() == albaRefSys::VIEW)
      {
        if (constrainPlane == albaTransformConstraint::XY)
        {
          if (m_SurfaceSnap == false && m_SurfaceNormal == false)
          {
            TrackballTranslate();
          }
          else
          {
            if(m_SurfaceSnap)
              SnapOnSurface();
            else if(m_SurfaceNormal)
              NormalOnSurface();

          }

        }
        else
        {
          albaWarningMacro(<< "Transaltion on VIEW RefSys is not supported for " << constrainPlane << " plane");
        }
      }
      else
      {
        albaWarningMacro(<< "Refsys type" << GetTranslationConstraint()->GetRefSys()->GetType() << "is not supported!");
      }
    }
    else
    {
      albaWarningMacro(<< "Only supporting free movement on both plane axes!");
    }
  }

}
//----------------------------------------------------------------------------
void albaBehaviorTransform::Rotate(double *p1, double *p2, double *viewup)
//----------------------------------------------------------------------------
{
  // set the constraint refsys to rotation constraint
  m_ConstrainRefSys = m_CurrentRotationConstraint->GetRefSys()->GetMatrix();

  //motion vector
  double motionVec[3];
  albaTransform::BuildVector(p1, p2, motionVec);

  double pa[3], pb[3];
  for (int i = 0;i<3; i++)
  {
    pa[i] = p1[i];
    pb[i] = p2[i];
  }

  //view up vector
  double *viewUp = viewup;

  //transform
  albaTransform transform;

  int refSysType = m_CurrentRotationConstraint->GetRefSys()->GetType();

  if (m_CurrentRotationConstraint->GetNumberOfDOF() == 1)
  {    

    //get the constrain axis
    int axis = m_CurrentRotationConstraint->GetConstraintAxis();
  
    switch(m_CurrentRotationConstraint->GetConstraintModality(axis)) 
    {
      case (albaTransformConstraint::FREE):
      { 
        if (refSysType == albaRefSys::CUSTOM)
        {
          /**
          Build the rotation angle to be sent to listener by rotating around the circle
          */
          
          //current axis versor
          double constrainVers[3];

          //get the constrain axis
          int axis = m_CurrentRotationConstraint->GetConstraintAxis();

          albaTransform::GetVersor(axis, *m_ConstrainRefSys, constrainVers);
          
          // centre of rotation in abs coordinates
          double o[3] = {0,0,0};

          // get the centre of rotation from PivotRefSys
          albaTransform::GetPosition(*GetPivotRefSys()->GetMatrix(), o);

          double op1Vec[3] = {0, 0, 0};
          double op2Vec[3] = {0, 0, 0};
          
          albaTransform::BuildVector(o, p1, op1Vec);
          albaTransform::BuildVector(o, p2, op2Vec);

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
          
          double angle = vtkMath::DegreesFromRadians( acos((p1p2Squared - p2oSquared - p1oSquared) / (-2 * sqrt(p2oSquared) * sqrt(p1oSquared))) );

          if (angleIsPositive) 
          {
            angle = fabs(angle);
          }
          else
          {
            angle = -fabs(angle);
          }

          
          transform.Translate(-o[0], -o[1], -o[2],POST_MULTIPLY);
          transform.RotateWXYZ(angle, constrainVers,POST_MULTIPLY);
          transform.Translate(o,POST_MULTIPLY);
     
          //send the transform matrix along with rotation angle
          SendTransformMatrix(transform.GetMatrix(), MOUSE_ACTION_MOVE, angle);
          
        }
        else if (refSysType == albaRefSys::VIEW)
        {
          // only rotation around view look is supported
          if (axis == albaTransformConstraint::Z)
          {
            TrackballRoll();
          }
          else
          {
            albaWarningMacro(<< "Rotation in VIEW ref sys around" << axis << "axis is not supported yet!");
          }
        }
      }
      break;
  
      case (albaTransformConstraint::SNAP_STEP):
      {
        if (refSysType == albaRefSys::CUSTOM)
        {
          //current axis versor
          double constrainVers[3];

          //get the constrain axis
          int axis = m_CurrentRotationConstraint->GetConstraintAxis();

          albaTransform::GetVersor(axis, *m_ConstrainRefSys, constrainVers);
          // centre of rotation in abs coordinates
          double o[3] = {0,0,0};
          // get the centre of rotation from PivotRefSys
          albaTransform::GetPosition(*GetPivotRefSys()->GetMatrix(), o);

          double op1Vec[3] = {0, 0, 0};
          double op2Vec[3] = {0, 0, 0};

          albaTransform::BuildVector(o, p1, op1Vec);
          albaTransform::BuildVector(o, p2, op2Vec);

          // op1 X op2
          double op1Vec_X_op2Vec[3] = {0, 0, 0};
          vtkMath::Cross(op1Vec, op2Vec, op1Vec_X_op2Vec);

          // rotation angle should be positive or negative?
          bool angleIsPositive = (vtkMath::Dot(op1Vec_X_op2Vec, constrainVers) >= 0 ? true : false);

          double p1p2Squared = vtkMath::Distance2BetweenPoints(p1, p2);
          double p2oSquared = vtkMath::Distance2BetweenPoints(p2, o);
          double p1oSquared = vtkMath::Distance2BetweenPoints(p1, o);

          double angle = m_CurrentRotationConstraint->GetStep(axis);

          if (angleIsPositive) 
          {
            angle = fabs(angle);
          }
          else
          {
            angle = -fabs(angle);
          }

          transform.Translate(-o[0], -o[1], -o[2],POST_MULTIPLY);
          transform.RotateWXYZ(angle, constrainVers,POST_MULTIPLY);
          transform.Translate(o,POST_MULTIPLY);

          //send the transform matrix along with rotation angle
          SendTransformMatrix(transform.GetMatrix(), MOUSE_ACTION_MOVE, angle);
        }
      }
      break;
  
      case (albaTransformConstraint::SNAP_ARRAY):
      {
  

      }
      break;
    }//switch  
  }//if
  else if (m_CurrentRotationConstraint->GetNumberOfDOF() == 2)
  {
     // the constrain plane
    int constrainPlane;

    // get the translation plane
    constrainPlane = m_CurrentRotationConstraint->GetConstraintPlane();

    // get the constraint axes
    int axis1, axis2;
    m_CurrentRotationConstraint->GetConstraintPlaneAxes(axis1, axis2);

    if (m_CurrentRotationConstraint->GetConstraintModality(axis1) == albaTransformConstraint::FREE &&
        m_CurrentRotationConstraint->GetConstraintModality(axis2) == albaTransformConstraint::FREE)
    {
      if (m_CurrentRotationConstraint->GetRefSys()->GetType() == albaRefSys::VIEW)
      {
        if (constrainPlane == albaTransformConstraint::XY)
        {				
					TrackballRotate();
        }      
        else
        {
          albaWarningMacro(<< "Rotation in VIEW ref sys on" << constrainPlane << "is not supported!");
        }
      }
      else
      {
        albaWarningMacro(<< "Only supporting VIEW refsys!");
      }
    }
    else
    {
      albaWarningMacro(<< "Only supporting FREE movement on both axes!");
    }
  }
}


//----------------------------------------------------------------------------
void albaBehaviorTransform::Scale(double *p1, double *p2, double *viewup)
//----------------------------------------------------------------------------
{
  // NOT YET IMPLEMENTED
  //Produce a scale matrix based on current constrain

  /*
  //motion vector
  double motionVec[3];
  albaTransform::BuildVector(p1, p2, motionVec);

  //current axis versor
  double constrainVers[3];

  //view up vector
  double *viewUp = viewup;
  */
}

//----------------------------------------------------------------------------
int albaBehaviorTransform::BinarySearch(double pos, vtkDoubleArray *array, int& result_point_status)
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
void albaBehaviorTransform::ConcatenateToResultMatrix(const albaMatrix &matrix)
//----------------------------------------------------------------------------
{
  if (GetResultMatrix()) 
  {
    albaTransform tmptr;
    tmptr.SetMatrix(*GetResultMatrix());

    if (GetResultMatrixConcatenationSemantic() == POSTMULTIPLY)
    {
      tmptr.Concatenate(matrix,POST_MULTIPLY);
    }
    else if (GetResultMatrixConcatenationSemantic() == PREMULTIPLY)
    {
      tmptr.Concatenate(matrix,PRE_MULTIPLY);
    }
    
    GetResultMatrix()->DeepCopy(tmptr.GetMatrixPointer());
  }
}

//----------------------------------------------------------------------------
void albaBehaviorTransform::SendTransformMatrix(double *vector, int mouseAction)
//----------------------------------------------------------------------------
{
  albaMatrix mat;
  albaTransform::SetPosition(mat, vector);
  SendTransformMatrix(mat, mouseAction);
}

//----------------------------------------------------------------------------
void albaBehaviorTransform::SendTransformMatrix(double *versor, double translation, int mouseAction)
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
void albaBehaviorTransform::SendTransformMatrix(const albaMatrix &matrix, int mouseAction, double rotationAngle)
//----------------------------------------------------------------------------
{ 
  // matrix registering the pick position in world coordinates at MOUSE_ACTION_DOWN
  albaSmartPointer<albaMatrix> pickPosMatrix;

  if (GetResultMatrixConcatenation())
  {
    ConcatenateToResultMatrix(matrix);
  }

  albaSmartPointer<albaMatrix> matrix_copy;
  *matrix_copy=matrix;
  
  albaEvent e;
  e.SetMatrix(matrix_copy);
  e.SetId(ID_TRANSFORM);
  e.SetSender(this);
  e.SetArg(mouseAction);
  e.SetDouble(rotationAngle);

  // if mouseAction == MOUSE_ACTION_DOWN notify the listener about the
  // picked position in world coordinates
  if (mouseAction == MOUSE_ACTION_DOWN)
  {
    albaTransform::SetPosition(*pickPosMatrix, m_LastPickPosition);
    
    //e.SetVtkObj(pickPosMatrix);  //modified by Marco. 24-5-2005
    e.SetMafObject(pickPosMatrix);
    
  }  

  albaEventMacro(e);
}

//----------------------------------------------------------------------------
void albaBehaviorTransform::OnMouseMoveAction(int X, int Y)
//----------------------------------------------------------------------------
{
  // picked X and Y in screen coordinates
  m_MousePointer2DPosition[0] = X;
  m_MousePointer2DPosition[1] = Y;
    
   double focalPoint[4], pickPoint[4], prevPickPoint[4], viewUp[3];
   double z;

   assert(m_CurrentCamera);

   //get the view up vector
   m_CurrentCamera->OrthogonalizeViewUp();
   m_CurrentCamera->GetViewUp(viewUp);
   vtkMath::Normalize(viewUp);

   //get the focal point
   ComputeWorldToDisplay(m_LastPickPosition[0], m_LastPickPosition[1],
                               m_LastPickPosition[2], focalPoint);
   z = focalPoint[2];

   ComputeDisplayToWorld((double)m_LastMousePointer2DPosition[0], (double)m_LastMousePointer2DPosition[1],
                               z, prevPickPoint);
 
   ComputeDisplayToWorld(double(X), double(Y), z, pickPoint);

   // Now we have prevPickPoint, pickPoint and viewUp;

   // prevPickPoint and pickPoint are the world coordinates of picked points 
   // do different things depending on the state 

   if (m_RotationFlag == true)
   {
      // rotate
      Rotate(prevPickPoint, pickPoint, viewUp);
   }

   if (m_TranslationFlag == true)
   {
     // translate
     Translate(prevPickPoint, pickPoint);
   }

  // Update m_LastMousePointer2DPosition
  m_LastMousePointer2DPosition[0] = m_MousePointer2DPosition[0];
  m_LastMousePointer2DPosition[1] = m_MousePointer2DPosition[1];

}

//----------------------------------------------------------------------------
void albaBehaviorTransform::OnButtonDownAction(int X, int Y)
//----------------------------------------------------------------------------
{
  // debug
  // set the constraint refsys to translation constraint, the matrix is referenced
  m_ConstrainRefSys = GetTranslationConstraint()->GetRefSys()->GetMatrix();


  // register m_MousePointer2DPosition
  m_MousePointer2DPosition[0] = m_LastMousePointer2DPosition[0] = X;
  m_MousePointer2DPosition[1] = m_LastMousePointer2DPosition[1] = Y;

  // the VME has been picked by the PER at the button down
  m_LastPickPosition[0] = m_PickedPosition[0];
  m_LastPickPosition[1] = m_PickedPosition[1];
  m_LastPickPosition[2] = m_PickedPosition[2];
  //reset projection accumulator   
    m_ProjectionAccumulator = 0;

    /*
            ^
            |                   
     -------A->----0-------1-----i-----rp-----i+1------N-1-------rp-
                                      pos_rp
                                 |
            ^                m_HelpPIndex
            |
    --------C->  

    C:  constrain ref sys origin
    rp: result point refsys origin
    A: double array 

                     projVal
                      -->
    -----|------------|-----------|------|---------------|----------->
        O          pos_rp      minb   pos_rp           maxb

    ------------------------>m_ProjectionAccumulator
    */ 


  if (m_TranslationFlag == true)
  {
    // if translation is constrained on one axis:
    if (GetTranslationConstraint()->GetNumberOfDOF() == 1)
    {
      // constrain axis
      int axis; 

      // get the constrain axis
      axis = GetTranslationConstraint()->GetConstraintAxis();

      // set constrain refsys to translation constraint 
      m_ConstrainRefSys = GetTranslationConstraint()->GetRefSys()->GetMatrix();

      if (GetTranslationConstraint()->GetConstraintModality(axis) == albaTransformConstraint::SNAP_ARRAY)
      {         
        // current axis versor
        double constrainVers[3];

        // rp vector
        double rp[3] = {0, 0, 0}, proj[3] = {0, 0, 0};
        double pos_rp = 0;

        albaTransform::GetVersor(axis, *m_ConstrainRefSys, constrainVers);
        albaTransform::GetPosition(*m_VME->GetOutput()->GetMatrix(), rp);

        //project rp on constrain axis
        pos_rp = albaTransform::ProjectVectorOnAxis(rp, constrainVers, proj);

        //set m_HelpPIndex and m_HelperPointStatus
        m_HelpPIndex = BinarySearch(pos_rp, 
                                  GetTranslationConstraint()->GetSnapArray(axis),
                                  m_HelperPointStatus);      
      }
    }
    // else if translation is constrained on a plane
    else if (GetTranslationConstraint()->GetNumberOfDOF() == 2)
    {
      // currently supporting only free movement on a plane
      // TODO 
    }
  }

  m_LastX = m_MousePointer2DPosition[0];
  m_LastY = m_MousePointer2DPosition[1];

  // notify the listener about the mouse action that has been performed 
  albaMatrix identity;
  SendTransformMatrix(identity, MOUSE_ACTION_DOWN);
}

//----------------------------------------------------------------------------
void albaBehaviorTransform::OnButtonUpAction()
//----------------------------------------------------------------------------
{
  // reset projection accumulator
  m_ProjectionAccumulator = 0;

  // register m_LastX and Last
  m_LastX = m_MousePointer2DPosition[0];
  m_LastY = m_MousePointer2DPosition[1];

  // notify the listener about the mouse action that has been performed 
  albaMatrix identity;
  SendTransformMatrix(identity, MOUSE_ACTION_UP);
}

//----------------------------------------------------------------------------
void albaBehaviorTransform::TrackballRotate()
//----------------------------------------------------------------------------
{   
   albaTransform transform;
   double v2[3];

   // set constraint refsys to rotation constraint
   m_ConstrainRefSys = m_CurrentRotationConstraint->GetRefSys()->GetMatrix();

   // register last mouse position
   int x = m_MousePointer2DPosition[0];
   int y = m_MousePointer2DPosition[1];

   // the pivot center
   double pivotPoint[3];

   // get the centre of rotation from Pivot RefSys
   albaTransform::GetPosition(*GetPivotRefSys()->GetMatrix(), pivotPoint);

   // translate to center of the constrain ref sys
   transform.Identity();
   transform.Translate(pivotPoint[0], pivotPoint[1], pivotPoint[2],PRE_MULTIPLY);
   
   float dx = m_LastX - x;
   float dy = m_LastY - y;
   
   // azimuth
   m_CurrentCamera->OrthogonalizeViewUp();
   double *viewUp = m_CurrentCamera->GetViewUp();
   int *size = m_Renderer->GetSize();
   transform.RotateWXYZ(-360.0 * dx / size[0], viewUp[0], viewUp[1], viewUp[2],PRE_MULTIPLY);
   
   // elevation
   vtkMath::Cross(m_CurrentCamera->GetDirectionOfProjection(), viewUp, v2);
   transform.RotateWXYZ(360.0 * dy / size[1], v2[0], v2[1], v2[2],PRE_MULTIPLY);
   
   // translate back
   transform.Translate(-pivotPoint[0], -pivotPoint[1], -pivotPoint[2],PRE_MULTIPLY);
   
   //send the transform matrix
   SendTransformMatrix(transform.GetMatrix());
  
   // update m_LastX and m_LastY
   m_LastX = x;
   m_LastY = y;
}

//----------------------------------------------------------------------------
void albaBehaviorTransform::TrackballTranslate()
//----------------------------------------------------------------------------
{  
  // the position picked by the button down gives the depth of the translation in the view plane
  double disp_obj_center[3], new_pick_point[4], old_pick_point[4], motion_vector[3];
  
  ComputeWorldToDisplay(m_PickedPosition[0], m_PickedPosition[1], m_PickedPosition[2], disp_obj_center);

  ComputeDisplayToWorld((double)m_MousePointer2DPosition[0], (double)m_MousePointer2DPosition[1], disp_obj_center[2], new_pick_point);
  
  ComputeDisplayToWorld((double)m_LastMousePointer2DPosition[0], (double)m_LastMousePointer2DPosition[1], disp_obj_center[2], old_pick_point);
  
  motion_vector[0] = new_pick_point[0] - old_pick_point[0];
  motion_vector[1] = new_pick_point[1] - old_pick_point[1];
  motion_vector[2] = new_pick_point[2] - old_pick_point[2];
    
  albaTransform t;
  t.Translate(motion_vector[0], motion_vector[1], motion_vector[2],POST_MULTIPLY);
  
  //send the transform matrix
  SendTransformMatrix(t.GetMatrix());
}

//----------------------------------------------------------------------------
void albaBehaviorTransform::TrackballRoll()
//----------------------------------------------------------------------------
{
   // get the rotation refsys
   m_ConstrainRefSys = m_CurrentRotationConstraint->GetRefSys()->GetMatrix();  

   // the refsys center
   double pivotPoint[3];

   // get the centre of rotation from PivotRefSys
   albaTransform::GetPosition(*GetPivotRefSys()->GetMatrix(), pivotPoint); 
   
   // Get the axis to rotate around = vector from eye to origin
 
   double motion_vector[3];
   double view_point[3];
 
   if (m_CurrentCamera->GetParallelProjection())
   {
     // If parallel projection, want to get the view plane normal...
     m_CurrentCamera->ComputeViewPlaneNormal();
     m_CurrentCamera->GetViewPlaneNormal(motion_vector);
   }
   else
   {   
     // Perspective projection, get vector from eye to center of actor
     m_CurrentCamera->GetPosition(view_point);
     motion_vector[0] = view_point[0] - pivotPoint[0];
     motion_vector[1] = view_point[1] - pivotPoint[1];
     motion_vector[2] = view_point[2] - pivotPoint[2];
     vtkMath::Normalize(motion_vector);
   }
   
   double disp_refSysCenter[3];
   
   ComputeWorldToDisplay(pivotPoint[0], pivotPoint[1], pivotPoint[2], 
                               disp_refSysCenter);
   
   double newAngle = 
     atan2((double)m_MousePointer2DPosition[1] - (double)disp_refSysCenter[1],
           (double)m_MousePointer2DPosition[0] - (double)disp_refSysCenter[0]);
 
   double oldAngle = 
     atan2((double)m_LastMousePointer2DPosition[1] - (double)disp_refSysCenter[1],
           (double)m_LastMousePointer2DPosition[0] - (double)disp_refSysCenter[0]);
   
   newAngle = vtkMath::DegreesFromRadians(newAngle);
   oldAngle = vtkMath::DegreesFromRadians(oldAngle);
   
   albaTransform t;
   t.Translate(-pivotPoint[0], -pivotPoint[1], -pivotPoint[2],POST_MULTIPLY);
   t.RotateWXYZ(newAngle - oldAngle, motion_vector,POST_MULTIPLY);
   t.Translate(pivotPoint[0], pivotPoint[1], pivotPoint[2],POST_MULTIPLY);
     
   //send the transform matrix
   SendTransformMatrix(t.GetMatrix());
}
//----------------------------------------------------------------------------
void albaBehaviorTransform::NormalOnSurface()
//----------------------------------------------------------------------------
{
  if (m_Renderer == NULL || m_View == NULL || m_VME == NULL)
    return;

  SnapOnSurface();

  double newAbsPickPos[3];
  albaVME *pickedVme = NULL;
  if (!PickUnderVME(newAbsPickPos, pickedVme))
  {
    TrackballTranslate();
    return;
  }

	vtkPolyData *poly =  vtkPolyData::SafeDownCast(pickedVme->GetOutput()->GetVTKData());
	if(poly == NULL) return;


	//copy of poly
	vtkALBASmartPointer<vtkPolyData> polyCopy;
	polyCopy->DeepCopy(poly);

	// fill it with cellnormals
	vtkALBASmartPointer<vtkPolyDataNormals> normalsOnPoly;
	normalsOnPoly->SetInputData(polyCopy);
	normalsOnPoly->ComputeCellNormalsOff();
	normalsOnPoly->ComputePointNormalsOn();
	normalsOnPoly->Update();
	// search the cell of picking
	// pick up the normal

	int index = -1;
	index = poly->FindPoint(newAbsPickPos);

	if(index == -1) return;
	double *normal;

  if(normalsOnPoly->GetOutput()==NULL || normalsOnPoly->GetOutput()->GetPointData()==NULL || normalsOnPoly->GetOutput()->GetPointData()->GetNormals()==NULL || normalsOnPoly->GetOutput()->GetPointData()->GetNormals()->GetNumberOfComponents() <= 0)
    return;
  normal = normalsOnPoly->GetOutput()->GetPointData()->GetNormals()->GetTuple3(index);

	double v1[3],v2[3],v3[3];

	v1[0] = normal[0];
	v1[1] = normal[1];
	v1[2] = normal[2];

	v2[0] = newAbsPickPos[0];
	v2[1] = newAbsPickPos[1];
	v2[2] = newAbsPickPos[2];

	vtkMath::Normalize(v1);
	vtkMath::Normalize(v2);

	vtkMath::Cross(v1,v2,v3);
	vtkMath::Normalize(v3);
	vtkMath::Cross(v3,v1,v2);

	vtkMatrix4x4 *matrix_translation=vtkMatrix4x4::New();
	matrix_translation->Identity();
	for(int i=0;i<3;i++)
		matrix_translation->SetElement(i,3,newAbsPickPos[i]);

	vtkMatrix4x4 *matrix_rotation=vtkMatrix4x4::New();
	matrix_rotation->Identity();
	for(int i=0;i<3;i++)
		matrix_rotation->SetElement(i,0,v1[i]);
	for(int i=0;i<3;i++)
		matrix_rotation->SetElement(i,1,v2[i]);
	for(int i=0;i<3;i++)
		matrix_rotation->SetElement(i,2,v3[i]);

	albaMatrix a;
	a.SetVTKMatrix(matrix_rotation);
	albaMatrix b;
	b.SetVTKMatrix(matrix_translation);
	albaMatrix c;
	albaMatrix::Multiply4x4(b,a,c);

	vtkDEL(matrix_rotation);
	vtkDEL(matrix_translation);

	m_VME->GetOutput()->GetAbsMatrix()->DeepCopy(&c);


}
//----------------------------------------------------------------------------
void albaBehaviorTransform::SnapOnSurface()
//----------------------------------------------------------------------------
{
  if (m_Renderer == NULL || m_View == NULL || m_VME == NULL)
    return;

  double newAbsPickPos[3];
  albaVME *pickedVme = NULL;
  if (!PickUnderVME(newAbsPickPos, pickedVme))
  {
    TrackballTranslate();
    return;
  }
  double absPivotPos[3];
  assert(m_VME);
  albaTransform::GetPosition(m_VME->GetAbsMatrixPipe()->GetMatrix(), absPivotPos);

  double absMotionVec[3];
  absMotionVec[0] = newAbsPickPos[0] - absPivotPos[0];
  absMotionVec[1] = newAbsPickPos[1] - absPivotPos[1];
  absMotionVec[2] = newAbsPickPos[2] - absPivotPos[2];

  SendTransformMatrix(absMotionVec);
}
//----------------------------------------------------------------------------
bool albaBehaviorTransform::PickUnderVME(double pickedPos[3], albaVME *&pickedVme)
//----------------------------------------------------------------------------
{
  // the actors of the moved VME are not pickable while picking the surface under it
  albaSceneGraph *sg = m_View->GetSceneGraph();
  albaSceneNode *node = sg ? sg->Vme2Node(m_VME) : NULL;
  vtkALBASmartPointer<vtkPropCollection> actors;
  if (node && node->GetAssemblyFront())
    node->GetAssemblyFront()->GetActors(actors);

  actors->InitTraversal();
  for (vtkProp *p = actors->GetNextProp(); p; p = actors->GetNextProp())
    p->PickableOff();

  bool picked = m_View->Pick(m_MousePointer2DPosition[0], m_MousePointer2DPosition[1]);
  if (picked)
  {
    m_View->GetPickedPosition(pickedPos);
    pickedVme = m_View->GetPickedVme();
  }

  actors->InitTraversal();
  for (vtkProp *p = actors->GetNextProp(); p; p = actors->GetNextProp())
    p->PickableOn();

  return picked;
}
