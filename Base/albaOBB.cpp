/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOBB
 Authors: Marco Petrone
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "albaOBB.h"
#include "albaMatrix.h"
#include "albaIndent.h"
#include "albaTransform.h"
#include "math.h"
//-------------------------------------------------------------------------
albaOBB::albaOBB()
//-------------------------------------------------------------------------
{
  m_Bounds[0]=0;
  m_Bounds[1]=-1;
  m_Bounds[2]=0;
  m_Bounds[3]=-1;
  m_Bounds[4]=0;
  m_Bounds[5]=-1;
}

//-------------------------------------------------------------------------
albaOBB::albaOBB(double source[6])
//-------------------------------------------------------------------------
{

  for (int i=0;i<6;i++)
  {
    m_Bounds[i]=source[i];
  }
}

//-------------------------------------------------------------------------
albaOBB::~albaOBB()
//-------------------------------------------------------------------------
{
}

//-------------------------------------------------------------------------
void albaOBB::Reset()
//-------------------------------------------------------------------------
{
  m_Bounds[0]=0;
  m_Bounds[1]=-1;
  m_Bounds[2]=0;
  m_Bounds[3]=-1;
  m_Bounds[4]=0;
  m_Bounds[5]=-1;
  Modified();
}

//-------------------------------------------------------------------------
bool albaOBB::Equals(albaOBB &bounds) const
//-------------------------------------------------------------------------
{
  for (int i=0;i<6;i++)
  {
    if (!albaEquals(m_Bounds[i],bounds.m_Bounds[i])) // consider only 15 digits to avoid dirty bits
    {
      return false;
    }
  }
  
  return  m_Matrix==bounds.m_Matrix;
}

//-------------------------------------------------------------------------
bool albaOBB::Equals(double bounds[6]) const
//-------------------------------------------------------------------------
{
  for (int i=0;i<6;i++)
  {
    if (!albaEquals(m_Bounds[i],bounds[i])) // consider only 15 digits to avoid dirty bits
    {
      return false;
    }
  }
  return true;
}

//-------------------------------------------------------------------------
bool albaOBB::Equals(float bounds[6]) const
//-------------------------------------------------------------------------
{
  float myBounds[6];
  CopyTo(myBounds);

  for (int i=0;i<6;i++)
  {
    if (!albaFloatEquals(myBounds[i],bounds[i]))
    {
      return false;
    }
  }
  return true;
}

//-------------------------------------------------------------------------
void albaOBB::DeepCopy(float bounds[6])
//-------------------------------------------------------------------------
{
  for (int i=0;i<6;i++)
  {
    m_Bounds[i]=bounds[i];
  }

  Modified();
}


//-------------------------------------------------------------------------
void albaOBB::DeepCopy(double bounds[6])
//-------------------------------------------------------------------------
{
  for (int i=0;i<6;i++)
  {
    m_Bounds[i]=bounds[i];
  }

  Modified();
}

//-------------------------------------------------------------------------
void albaOBB::DeepCopy(albaOBB *source)
//-------------------------------------------------------------------------
{
  for (int i=0;i<6;i++)
  {
    m_Bounds[i]=source->m_Bounds[i];
  }

  m_Matrix=source->m_Matrix; 
  Modified();
}

//-------------------------------------------------------------------------
void albaOBB::CopyTo(float target[6]) const
//-------------------------------------------------------------------------
{
  for (int i=0;i<6;i++)
  {
    target[i]=(float)m_Bounds[i];
  }
}

//-------------------------------------------------------------------------
void albaOBB::CopyTo(double target[6]) const
//-------------------------------------------------------------------------
{
  for (int i=0;i<6;i++)
  {
    target[i]=m_Bounds[i];
  }
}

//-------------------------------------------------------------------------
void albaOBB::ApplyTransform(const albaMatrix &mat, albaOBB &newbounds)
//-------------------------------------------------------------------------
{
  double newpoints[4*8];

  int i=0;

  if (newbounds.IsValid())
  {

    for (int x=0;x<2;x++)
    {
      double X=newbounds.m_Bounds[x];
      for (int y=0;y<2;y++)
      {
        double Y=newbounds.m_Bounds[y+2];
        for (int z=0;z<2;z++)
        {
          double Z=newbounds.m_Bounds[z+4];
        
          newpoints[i*4]=X;
          newpoints[i*4+1]=Y;
          newpoints[i*4+2]=Z;
          newpoints[i*4+3]=1;

          mat.MultiplyPoint(&(newpoints[4*i]),&(newpoints[4*i]));
        
          i++;
        }
      }
    }
  
    // Initialize the new bounds coincident with a single point
    newbounds.m_Bounds[0]=newpoints[0];
    newbounds.m_Bounds[1]=newpoints[0];
    newbounds.m_Bounds[2]=newpoints[1];
    newbounds.m_Bounds[3]=newpoints[1];
    newbounds.m_Bounds[4]=newpoints[2];
    newbounds.m_Bounds[5]=newpoints[2];

    // find the right bounds
    for (i=0;i<8;i++)
    {
      // is this point outside current X bounds
      if (newpoints[i*4]<newbounds.m_Bounds[0])
        newbounds.m_Bounds[0]=newpoints[i*4];
      if (newpoints[i*4]>newbounds.m_Bounds[1])
        newbounds.m_Bounds[1]=newpoints[i*4];

      // is this point outside current Y bounds
      if (newpoints[i*4+1]<newbounds.m_Bounds[2])
        newbounds.m_Bounds[2]=newpoints[i*4+1];
      if (newpoints[i*4+1]>newbounds.m_Bounds[3])
        newbounds.m_Bounds[3]=newpoints[i*4+1];

      // is this point outside current Z bounds
      if (newpoints[i*4+2]<newbounds.m_Bounds[4])
        newbounds.m_Bounds[4]=newpoints[i*4+2];
      if (newpoints[i*4+2]>newbounds.m_Bounds[5])
        newbounds.m_Bounds[5]=newpoints[i*4+2];
    }
    
    newbounds.m_Matrix.Identity(); // reset matrix to identiy
    newbounds.Modified();
  }
  else
  {
    newbounds.Reset();
  }

}

//-------------------------------------------------------------------------
void albaOBB::MergeBounds(albaOBB &bounds)
//-------------------------------------------------------------------------
{
  if (IsValid())
  {
    if (bounds.IsValid())
    {
      MergeBounds(m_Bounds,bounds.m_Bounds);
      Modified();
    }
  }
  else
  {
    DeepCopy(bounds);
  }
}

//-------------------------------------------------------------------------
void albaOBB::MergeBounds(double b1[6], double b2[6])
//-------------------------------------------------------------------------
{
  b1[0] = (b1[0]<b2[0]) ?	b1[0] : b2[0];    
	b1[2] = (b1[2]<b2[2]) ?	b1[2] : b2[2];    
	b1[4] = (b1[4]<b2[4]) ?	b1[4] : b2[4];    
	b1[1] = (b1[1]>b2[1]) ?	b1[1] : b2[1];    
	b1[3] = (b1[3]>b2[3]) ?	b1[3] : b2[3];    
	b1[5] = (b1[5]>b2[5]) ?	b1[5] : b2[5];
}

//-------------------------------------------------------------------------
void albaOBB::GetDimensions(float dims[3]) const
//-------------------------------------------------------------------------
{
  dims[0]=(float)(m_Bounds[1]-m_Bounds[0]);
  dims[1]=(float)(m_Bounds[3]-m_Bounds[2]);
  dims[2]=(float)(m_Bounds[5]-m_Bounds[4]);
}

//-------------------------------------------------------------------------
void albaOBB::GetDimensions(double dims[3]) const
//-------------------------------------------------------------------------
{
  dims[0]=m_Bounds[1]-m_Bounds[0];
  dims[1]=m_Bounds[3]-m_Bounds[2];
  dims[2]=m_Bounds[5]-m_Bounds[4];
}

//-------------------------------------------------------------------------
void albaOBB::GetCenter(float center[3]) const
//-------------------------------------------------------------------------
{
  center[0]=(float)(m_Bounds[0]+m_Bounds[1])/2;
  center[1]=(float)(m_Bounds[2]+m_Bounds[3])/2;
  center[2]=(float)(m_Bounds[4]+m_Bounds[5])/2;
}

//-------------------------------------------------------------------------
void albaOBB::GetCenter(double center[3]) const
//-------------------------------------------------------------------------
{
  center[0]=(m_Bounds[0]+m_Bounds[1])/2;
  center[1]=(m_Bounds[2]+m_Bounds[3])/2;
  center[2]=(m_Bounds[4]+m_Bounds[5])/2;
}

//-------------------------------------------------------------------------
void albaOBB::SetCenter(double center[3])
//-------------------------------------------------------------------------
{
  double dims[3];
  GetDimensions(dims);
  m_Bounds[0]=center[0]-dims[0]/2;
  m_Bounds[1]=center[0]+dims[0]/2;
  m_Bounds[2]=center[1]-dims[1]/2;
  m_Bounds[3]=center[1]+dims[1]/2;
  m_Bounds[4]=center[2]-dims[2]/2;
  m_Bounds[5]=center[2]+dims[2]/2;
  Modified();
}

//-------------------------------------------------------------------------
void albaOBB::SetCenter(float center[3])
//-------------------------------------------------------------------------
{
  double dims[3];
  GetDimensions(dims);
  m_Bounds[0]=center[0]-dims[0]/2;
  m_Bounds[1]=center[0]+dims[0]/2;
  m_Bounds[2]=center[1]-dims[1]/2;
  m_Bounds[3]=center[1]+dims[1]/2;
  m_Bounds[4]=center[2]-dims[2]/2;
  m_Bounds[5]=center[2]+dims[2]/2;
  Modified();
}

//-------------------------------------------------------------------------
void albaOBB::SetDimensions(double dims[3])
//-------------------------------------------------------------------------
{
  double center[3];
  GetCenter(center);
  m_Bounds[0]=center[0]-dims[0]/2;
  m_Bounds[1]=center[0]+dims[0]/2;
  m_Bounds[2]=center[1]-dims[1]/2;
  m_Bounds[3]=center[1]+dims[1]/2;
  m_Bounds[4]=center[2]-dims[2]/2;
  m_Bounds[5]=center[2]+dims[2]/2;
  Modified();
}

//-------------------------------------------------------------------------
void albaOBB::SetDimensions(float dims[3])
//-------------------------------------------------------------------------
{
  float center[3];
  GetCenter(center);
  m_Bounds[0]=center[0]-dims[0]/2;
  m_Bounds[1]=center[0]+dims[0]/2;
  m_Bounds[2]=center[1]-dims[1]/2;
  m_Bounds[3]=center[1]+dims[1]/2;
  m_Bounds[4]=center[2]-dims[2]/2;
  m_Bounds[5]=center[2]+dims[2]/2;
  Modified();
}

//-------------------------------------------------------------------------
double albaOBB::GetWidth() const
//-------------------------------------------------------------------------
{
  double dims[3];
  GetDimensions(dims);
  return dims[0];
}

//-------------------------------------------------------------------------
double albaOBB::GetHeight() const
//-------------------------------------------------------------------------
{
  double dims[3];
  GetDimensions(dims);
  return dims[1];
}
//-------------------------------------------------------------------------
double albaOBB::GetDepth() const
//-------------------------------------------------------------------------
{
  double dims[3];
  GetDimensions(dims);
  return dims[2];
}

//-------------------------------------------------------------------------
void albaOBB::SetOrientation(double rx, double ry, double rz)
//-------------------------------------------------------------------------
{
  albaTransform::SetOrientation(m_Matrix,rx,ry,rz);
  Modified();
}

//-------------------------------------------------------------------------
void albaOBB::GetOrientation(double *rxyz)
//-------------------------------------------------------------------------
{
  albaTransform::GetOrientation(m_Matrix,rxyz);
  Modified();
}
//-------------------------------------------------------------------------
void albaOBB::Print(std::ostream& os, const int tabs) const
//-------------------------------------------------------------------------
{
  albaIndent indent(tabs);
  os << indent << "Bounds: ["<<m_Bounds[0]<<","<<m_Bounds[1]<<","<<m_Bounds[2]<<"," \
    <<m_Bounds[3]<<","<<m_Bounds[4]<<","<<m_Bounds[5]<<"]\n";
  os << indent << "Matrix: \n";
  m_Matrix.Print(os,indent.GetNextIndent()); 
}
