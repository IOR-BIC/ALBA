/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafOBB.cpp,v $
  Language:  C++
  Date:      $Date: 2005-01-11 17:34:59 $
  Version:   $Revision: 1.1 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#include "mafOBB.h"
#include "mafMatrix.h"

//-------------------------------------------------------------------------
mafOBB::mafOBB()
//-------------------------------------------------------------------------
{
  this->Reset();
}

//-------------------------------------------------------------------------
mafOBB::mafOBB(double source[6])
//-------------------------------------------------------------------------
{
  for (int i=0;i<6;i++)
  {
    this->Bounds[i]=source[i];
  }
}

//-------------------------------------------------------------------------
mafOBB::~mafOBB()
//-------------------------------------------------------------------------
{
}

//-------------------------------------------------------------------------
void mafOBB::Reset()
//-------------------------------------------------------------------------
{
  this->Bounds[0]=0;
  this->Bounds[1]=-1;
  this->Bounds[2]=0;
  this->Bounds[3]=-1;
  this->Bounds[4]=0;
  this->Bounds[5]=-1;
  this->Modified();
}

//-------------------------------------------------------------------------
int mafOBB::Equals(mafOBB &bounds)
//-------------------------------------------------------------------------
{
  if (this->Bounds[0]==bounds.Bounds[0] && \
    this->Bounds[1]==bounds.Bounds[1] && \
    this->Bounds[2]==bounds.Bounds[2] && \
    this->Bounds[3]==bounds.Bounds[3] && \
    this->Bounds[4]==bounds.Bounds[4] && \
    this->Bounds[5]==bounds.Bounds[5])
  {
    return 1;
  }

  return 0;
}

//-------------------------------------------------------------------------
int mafOBB::Equals(double bounds[6])
//-------------------------------------------------------------------------
{

  if (Bounds[0]==bounds[0] && \
    Bounds[1]==bounds[1] && \
    Bounds[2]==bounds[2] && \
    Bounds[3]==bounds[3] && \
    Bounds[4]==bounds[4] && \
    Bounds[5]==bounds[5])
  {
    return 1;
  }

  return 0;
}

//-------------------------------------------------------------------------
int mafOBB::Equals(float bounds[6])
//-------------------------------------------------------------------------
{
  float myBounds[6];
  this->CopyTo(myBounds);

  if (myBounds[0]==bounds[0] && \
    myBounds[1]==bounds[1] && \
    myBounds[2]==bounds[2] && \
    myBounds[3]==bounds[3] && \
    myBounds[4]==bounds[4] && \
    myBounds[5]==bounds[5])
  {
    return 1;
  }

  return 0;
}

//-------------------------------------------------------------------------
void mafOBB::DeepCopy(float bounds[6])
//-------------------------------------------------------------------------
{
  for (int i=0;i<6;i++)
  {
    this->Bounds[i]=bounds[i];
  }

  this->Modified();
}


//-------------------------------------------------------------------------
void mafOBB::DeepCopy(double bounds[6])
//-------------------------------------------------------------------------
{
  for (int i=0;i<6;i++)
  {
    this->Bounds[i]=bounds[i];
  }

  this->Modified();
}

//-------------------------------------------------------------------------
void mafOBB::DeepCopy(mafOBB *source)
//-------------------------------------------------------------------------
{
  for (int i=0;i<6;i++)
  {
    this->Bounds[i]=source->Bounds[i];
  }

  this->Modified();
}

//-------------------------------------------------------------------------
void mafOBB::CopyTo(float target[6])
//-------------------------------------------------------------------------
{
  for (int i=0;i<6;i++)
  {
    target[i]=this->Bounds[i];
  }
}

//-------------------------------------------------------------------------
void mafOBB::CopyTo(double target[6])
//-------------------------------------------------------------------------
{
  for (int i=0;i<6;i++)
  {
    target[i]=this->Bounds[i];
  }
}
//-------------------------------------------------------------------------
void mafOBB::ApplyTransform()
//-------------------------------------------------------------------------
{
  ApplyTransform(*this); // self apply the transform
  
}

//-------------------------------------------------------------------------
void mafOBB::ApplyTransform(mafOBB &newbounds)
//-------------------------------------------------------------------------
{
  double newpoints[4*8];

  int i=0;

  if (this->IsValid())
  {

    for (int x=0;x<2;x++)
    {
      double X=this->Bounds[x];
      for (int y=0;y<2;y++)
      {
        double Y=this->Bounds[y+2];
        for (int z=0;z<2;z++)
        {
          double Z=this->Bounds[z+4];
        
          newpoints[i*4]=X;
          newpoints[i*4+1]=Y;
          newpoints[i*4+2]=Z;
          newpoints[i*4+3]=1;

          Matrix.MultiplyPoint(&(newpoints[4*i]),&(newpoints[4*i]));
        
          i++;
        }
      }
    }
  
    // Initialize the new bounds coincident with a single point
    newbounds.Bounds[0]=newpoints[0];
    newbounds.Bounds[1]=newpoints[0];
    newbounds.Bounds[2]=newpoints[1];
    newbounds.Bounds[3]=newpoints[1];
    newbounds.Bounds[4]=newpoints[2];
    newbounds.Bounds[5]=newpoints[2];

    // find the right bounds
    for (i=0;i<8;i++)
    {
      // is this point outside current X bounds
      if (newpoints[i*4]<newbounds.Bounds[0])
        newbounds.Bounds[0]=newpoints[i*4];
      if (newpoints[i*4]>newbounds.Bounds[1])
        newbounds.Bounds[1]=newpoints[i*4];

      // is this point outside current Y bounds
      if (newpoints[i*4+1]<newbounds.Bounds[2])
        newbounds.Bounds[2]=newpoints[i*4+1];
      if (newpoints[i*4+1]>newbounds.Bounds[3])
        newbounds.Bounds[3]=newpoints[i*4+1];

      // is this point outside current Z bounds
      if (newpoints[i*4+2]<newbounds.Bounds[4])
        newbounds.Bounds[4]=newpoints[i*4+2];
      if (newpoints[i*4+2]>newbounds.Bounds[5])
        newbounds.Bounds[5]=newpoints[i*4+2];
    }
    
    newbounds.Matrix.Identity(); // reset matrix to identiy
    newbounds.Modified();
  }
  else
  {
    newbounds.Reset();
  }

}

//-------------------------------------------------------------------------
void mafOBB::MergeBounds(mafOBB &bounds)
//-------------------------------------------------------------------------
{
  if (this->IsValid())
  {
    if (bounds.IsValid())
    {
      this->MergeBounds(this->Bounds,bounds.Bounds);
      this->Modified();
    }
  }
  else
  {
    this->DeepCopy(bounds);
  }
}

//-------------------------------------------------------------------------
void mafOBB::MergeBounds(double b1[6], double b2[6])
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
void mafOBB::GetDimensions(float dims[3])
//-------------------------------------------------------------------------
{
  dims[0]=Bounds[1]-Bounds[0];
  dims[1]=Bounds[3]-Bounds[2];
  dims[2]=Bounds[5]-Bounds[4];
}

//-------------------------------------------------------------------------
void mafOBB::GetDimensions(double dims[3])
//-------------------------------------------------------------------------
{
  dims[0]=Bounds[1]-Bounds[0];
  dims[1]=Bounds[3]-Bounds[2];
  dims[2]=Bounds[5]-Bounds[4];
}

//-------------------------------------------------------------------------
void mafOBB::GetCenter(float center[3])
//-------------------------------------------------------------------------
{
  center[0]=(Bounds[0]+Bounds[1])/2;
  center[1]=(Bounds[2]+Bounds[3])/2;
  center[2]=(Bounds[4]+Bounds[5])/2;
}

//-------------------------------------------------------------------------
void mafOBB::GetCenter(double center[3])
//-------------------------------------------------------------------------
{
  center[0]=(Bounds[0]+Bounds[1])/2;
  center[1]=(Bounds[2]+Bounds[3])/2;
  center[2]=(Bounds[4]+Bounds[5])/2;
}

//-------------------------------------------------------------------------
void mafOBB::SetCenter(double center[3])
//-------------------------------------------------------------------------
{
  double dims[3];
  GetDimensions(dims);
  Bounds[0]=center[0]-dims[0]/2;
  Bounds[1]=center[0]+dims[0]/2;
  Bounds[2]=center[1]-dims[1]/2;
  Bounds[3]=center[1]+dims[1]/2;
  Bounds[4]=center[2]-dims[2]/2;
  Bounds[5]=center[2]+dims[2]/2;
  Modified();
}

//-------------------------------------------------------------------------
void mafOBB::SetCenter(float center[3])
//-------------------------------------------------------------------------
{
  double dims[3];
  GetDimensions(dims);
  Bounds[0]=center[0]-dims[0]/2;
  Bounds[1]=center[0]+dims[0]/2;
  Bounds[2]=center[1]-dims[1]/2;
  Bounds[3]=center[1]+dims[1]/2;
  Bounds[4]=center[2]-dims[2]/2;
  Bounds[5]=center[2]+dims[2]/2;
  Modified();
}

//-------------------------------------------------------------------------
void mafOBB::SetDimensions(double dims[3])
//-------------------------------------------------------------------------
{
  double center[3];
  GetCenter(center);
  Bounds[0]=center[0]-dims[0]/2;
  Bounds[1]=center[0]+dims[0]/2;
  Bounds[2]=center[1]-dims[1]/2;
  Bounds[3]=center[1]+dims[1]/2;
  Bounds[4]=center[2]-dims[2]/2;
  Bounds[5]=center[2]+dims[2]/2;
  Modified();
}

//-------------------------------------------------------------------------
void mafOBB::SetDimensions(float dims[3])
//-------------------------------------------------------------------------
{
  float center[3];
  GetCenter(center);
  Bounds[0]=center[0]-dims[0]/2;
  Bounds[1]=center[0]+dims[0]/2;
  Bounds[2]=center[1]-dims[1]/2;
  Bounds[3]=center[1]+dims[1]/2;
  Bounds[4]=center[2]-dims[2]/2;
  Bounds[5]=center[2]+dims[2]/2;
  Modified();
}

//-------------------------------------------------------------------------
double mafOBB::GetWidth()
//-------------------------------------------------------------------------
{
  double dims[3];
  GetDimensions(dims);
  return dims[0];
}

//-------------------------------------------------------------------------
double mafOBB::GetHeight()
//-------------------------------------------------------------------------
{
  double dims[3];
  GetDimensions(dims);
  return dims[1];
}
//-------------------------------------------------------------------------
double mafOBB::GetDepth()
//-------------------------------------------------------------------------
{
  double dims[3];
  GetDimensions(dims);
  return dims[2];
}

