/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafTransformFrame.cpp,v $
  Language:  C++
  Date:      $Date: 2005-03-11 15:50:13 $
  Version:   $Revision: 1.4 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#include "mafTransformFrame.h"
#include "mafTransform.h"
#include <assert.h>

//----------------------------------------------------------------------------
mafTransformFrame::mafTransformFrame()
//----------------------------------------------------------------------------
{
  m_Input = NULL;
  m_InputFrame = NULL;
  m_TargetFrame = NULL;
}

//----------------------------------------------------------------------------
mafTransformFrame::~mafTransformFrame()
//----------------------------------------------------------------------------
{
}
/*
//----------------------------------------------------------------------------
void mafTransformFrame::PrintSelf(ostream& os, vtkIndent indent)
//----------------------------------------------------------------------------
{
  this->Update();

  this->Superclass::PrintSelf(os, indent);
  os << indent << "Input: ";
  if (Input)
  {
    os << Input << " Matrix:\n";
    Input->GetMatrix()->PrintSelf(os,indent.GetNextIndent());
  }
  else
    os << "NULL\n";

  os << indent << "InputFrame: ";
  if (InputFrame)
  {
    os << InputFrame << "\n";
    InputFrame->GetMatrix()->PrintSelf(os,indent.GetNextIndent());
  }
  else
    os << "NULL\n";

  os << indent << "TargetFrame: ";
  if (TargetFrame)
  {
    os << TargetFrame << "\n";
    TargetFrame->GetMatrix()->PrintSelf(os,indent.GetNextIndent()); 
  }
  else
    os << "NULL\n";

  os << indent << "InverseFlag: " << this->InverseFlag << "\n";
}
*/

//----------------------------------------------------------------------------
void mafTransformFrame::SetInput(mafTransformBase *matrix)
//----------------------------------------------------------------------------
{
  if (m_Input) m_Input->UnRegister(this);
  m_Input = matrix;
  if (matrix)
    matrix->Register(this);
}

//----------------------------------------------------------------------------
void mafTransformFrame::SetInput(mafMatrix *matrix)
//----------------------------------------------------------------------------
{
  mafTransform *trans= new mafTransform;
  trans->SetMatrix(*matrix);
  SetInput(trans);
}


//----------------------------------------------------------------------------
void mafTransformFrame::SetInputFrame(mafTransformBase *frame)
//----------------------------------------------------------------------------
{
  if (m_InputFrame) m_InputFrame->UnRegister(this);
  m_InputFrame = frame;
  if (frame)
    frame->Register(this);
}

//----------------------------------------------------------------------------
void mafTransformFrame::SetInputFrame(mafMatrix *matrix)
//----------------------------------------------------------------------------
{
  mafTransform *trans= new mafTransform;
  trans->SetMatrix(*matrix);
  SetInputFrame(trans);
}

//----------------------------------------------------------------------------
void mafTransformFrame::SetTargetFrame(mafTransformBase *frame)
//----------------------------------------------------------------------------
{
  if (m_TargetFrame) m_TargetFrame->UnRegister(this);
  m_TargetFrame = frame;
  if (frame)
    frame->Register(this);
}

//----------------------------------------------------------------------------
void mafTransformFrame::SetTargetFrame(mafMatrix *matrix)
//----------------------------------------------------------------------------
{
  mafTransform *trans= new mafTransform;
  trans->SetMatrix(*matrix);
  SetTargetFrame(trans);
}


//----------------------------------------------------------------------------
void mafTransformFrame::InternalUpdate()
//----------------------------------------------------------------------------
{

  /*

  SceneGraph matrix concatenation order:
  
  root
  |
   -Mtf-     => M   = M  * M
       |         abs   tf   in
       -Min-

  transformation are applied in reverse order:
  M  first then M
   in            tf


  Mif: input frame matrix
  Mtf: target frame matrix

                                  -1          
  M   * M  = M   * M   =>  M   = M   * M   * M  
    if   in   tf    out      out  tf    if    in

  */

	mafMatrix  inv_target_frame_matrix;

  if (m_Input)
  {
		//change input matrix base and copy the result in this->Matrix(); if the 
		//input frame or reference frame is null identity matrix is assumed as input
		mafMatrix  in_fr_mat_x_in_mat;

		if (m_TargetFrame == NULL)
		{
		  inv_target_frame_matrix.Identity();
		}
		else
		{
		  mafMatrix::Invert(m_TargetFrame->GetMatrix(),  inv_target_frame_matrix);
		}
	
		if (m_InputFrame == NULL)
		{
		  in_fr_mat_x_in_mat = m_Input->GetMatrix();     
		}
		else
		{
		  mafMatrix::Multiply4x4(m_InputFrame->GetMatrix(), m_Input->GetMatrix(),
								in_fr_mat_x_in_mat);
		}

	  mafMatrix::Multiply4x4(inv_target_frame_matrix, in_fr_mat_x_in_mat, *m_Matrix);

  }
  else
  {
		//compute the transformation matrix between the two reference systems
		if (m_TargetFrame == NULL)
		{
      if (m_InputFrame)
      {
        *m_Matrix = m_InputFrame->GetMatrix();
      }
			  
      // if input, inputframe, targetframe are all NULL this->Matrix is left unchanged,
      // this is useful in compbination with SetMatrix to avoid overwriting.
		}
		else
		{
			mafMatrix::Invert(m_TargetFrame->GetMatrix(),	inv_target_frame_matrix);

			if (m_InputFrame == NULL)
			{
			  *m_Matrix = inv_target_frame_matrix;
			}
			else
			{
			mafMatrix::Multiply4x4(inv_target_frame_matrix, 
						m_InputFrame->GetMatrix(), *m_Matrix);				
			}

		}
	
  }
  m_Matrix->SetTimeStamp(m_TimeStamp);
}

//----------------------------------------------------------------------------
unsigned long mafTransformFrame::GetMTime()
//----------------------------------------------------------------------------
{
  unsigned long mtime = Superclass::GetMTime();

  if (m_Input)
  {
    unsigned long matrixMTime = m_Input->GetMTime();
    if (matrixMTime > mtime)
    {
      mtime = matrixMTime;
    }
  }
  
  if (m_InputFrame)
  {
    unsigned long matrixMTime = m_InputFrame->GetMTime();
    if (matrixMTime > mtime)
    {
      mtime = matrixMTime;
    }
  }

  if (m_TargetFrame)
  {
    unsigned long matrixMTime = m_TargetFrame->GetMTime();
    if (matrixMTime > mtime)
    {
      mtime = matrixMTime;
    }
  }

  return mtime;
}

