/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaTransformFrame
 Authors: Marco Petrone, Stefano Perticoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "albaTransformFrame.h"
#include "albaTransform.h"
#include <assert.h>

//----------------------------------------------------------------------------
albaCxxTypeMacro(albaTransformFrame)
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaTransformFrame::albaTransformFrame()
//----------------------------------------------------------------------------
{
  m_Input = NULL;
  m_InputFrame = NULL;
  m_TargetFrame = NULL;
}

//----------------------------------------------------------------------------
albaTransformFrame::~albaTransformFrame()
//----------------------------------------------------------------------------
{
	if(m_Input)
		m_Input->UnRegister(this);

	if(m_InputFrame)
		m_InputFrame->UnRegister(this);

	if(m_TargetFrame)
		m_TargetFrame->UnRegister(this);
}
/*
//----------------------------------------------------------------------------
virtual void Print(std::ostream& os, const int tabs) const;
//----------------------------------------------------------------------------
{
  Update();

  albaIndent indent(tabs);

  Superclass::Print(os, indent);
  os << indent << "Input: ";
  if (m_Input)
  {
    os << Input << " Matrix:\n";
    Input->GetMatrix()->PrintSelf(os,indent.GetNextIndent());
  }
  else
    os << "NULL\n";

  os << indent << "InputFrame: ";
  if (m_InputFrame)
  {
    m_InputFrame->GetMatrix()->PrintSelf(os,indent.GetNextIndent());
  }
  else
    os << "NULL\n";

  os << indent << "TargetFrame: ";
  if (m_TargetFrame)
  {
    m_TargetFrame->GetMatrix()->PrintSelf(os,indent.GetNextIndent()); 
  }
  else
    os << "NULL\n";
}
*/
//----------------------------------------------------------------------------
void albaTransformFrame::SetInput(albaTransformBase *frame)
//----------------------------------------------------------------------------
{
  if (m_Input) m_Input->UnRegister(this);
  m_Input = frame;
  if (frame)
    frame->Register(this);
}

//----------------------------------------------------------------------------
void albaTransformFrame::SetInput(albaMatrix *frame)
//----------------------------------------------------------------------------
{
  albaTransform *trans= albaTransform::New();
  trans->SetMatrix(*frame);
  SetInput(trans);
}


//----------------------------------------------------------------------------
void albaTransformFrame::SetInputFrame(albaTransformBase *frame)
//----------------------------------------------------------------------------
{
  if (m_InputFrame) m_InputFrame->UnRegister(this);
  m_InputFrame = frame;
  if (frame)
    frame->Register(this);
}

//----------------------------------------------------------------------------
void albaTransformFrame::SetInputFrame(albaMatrix *matrix)
//----------------------------------------------------------------------------
{
  //albaTransform *trans= new albaTransform;
  albaTransform *trans= albaTransform::New();
  trans->SetMatrix(*matrix);
  SetInputFrame(trans);
}

//----------------------------------------------------------------------------
void albaTransformFrame::SetTargetFrame(albaTransformBase *frame)
//----------------------------------------------------------------------------
{
  if (m_TargetFrame) m_TargetFrame->UnRegister(this);
  m_TargetFrame = frame;
  if (frame)
    frame->Register(this);
}

//----------------------------------------------------------------------------
void albaTransformFrame::SetTargetFrame(albaMatrix *matrix)
//----------------------------------------------------------------------------
{
  //albaTransform *trans= new albaTransform;
  albaTransform *trans = albaTransform::New();
  trans->SetMatrix(*matrix);
  SetTargetFrame(trans);
}


//----------------------------------------------------------------------------
void albaTransformFrame::InternalUpdate()
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

	albaMatrix  inv_target_frame_matrix;

  if (m_Input)
  {
		//change input matrix base and copy the result in this->Matrix(); if the 
		//input frame or reference frame is null identity matrix is assumed as input
		albaMatrix  in_fr_mat_x_in_mat;

		if (m_TargetFrame == NULL)
		{
		  inv_target_frame_matrix.Identity();
		}
		else
		{
		  albaMatrix::Invert(m_TargetFrame->GetMatrix(),  inv_target_frame_matrix);
		}
	
		if (m_InputFrame == NULL)
		{
		  in_fr_mat_x_in_mat = m_Input->GetMatrix();     
		}
		else
		{
		  albaMatrix::Multiply4x4(m_InputFrame->GetMatrix(), m_Input->GetMatrix(),
								in_fr_mat_x_in_mat);
		}

	  albaMatrix::Multiply4x4(inv_target_frame_matrix, in_fr_mat_x_in_mat, *m_Matrix);

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
			albaMatrix::Invert(m_TargetFrame->GetMatrix(),	inv_target_frame_matrix);

			if (m_InputFrame == NULL)
			{
			  *m_Matrix = inv_target_frame_matrix;
			}
			else
			{
			albaMatrix::Multiply4x4(inv_target_frame_matrix, 
						m_InputFrame->GetMatrix(), *m_Matrix);				
			}

		}
	
  }
  if (m_Input)
  {
    m_Matrix->SetTimeStamp(m_Input->GetTimeStamp());
  }
  else
    m_Matrix->SetTimeStamp(m_TimeStamp);
}

//----------------------------------------------------------------------------
vtkMTimeType albaTransformFrame::GetMTime()
//----------------------------------------------------------------------------
{
	vtkMTimeType mtime = Superclass::GetMTime();

  if (m_Input)
  {
		vtkMTimeType matrixMTime = m_Input->GetMTime();
    if (matrixMTime > mtime)
    {
      mtime = matrixMTime;
    }
  }
  
  if (m_InputFrame)
  {
		vtkMTimeType matrixMTime = m_InputFrame->GetMTime();
    if (matrixMTime > mtime)
    {
      mtime = matrixMTime;
    }
  }

  if (m_TargetFrame)
  {
		vtkMTimeType matrixMTime = m_TargetFrame->GetMTime();
    if (matrixMTime > mtime)
    {
      mtime = matrixMTime;
    }
  }

  return mtime;
}

