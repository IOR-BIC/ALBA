/*========================================================================= 
  Program: Multimod Application Framework RELOADED 
  Module: $RCSfile: vtkALBAErythrocyteSource.cpp,v $ 
  Language: C++ 
  Date: $Date: 2009-11-19 10:43:53 $ 
  Version: $Revision: 1.1.2.2 $ 
  Authors: Josef Kohout (Josef.Kohout *AT* beds.ac.uk)
  ========================================================================== 
  Copyright (c) 2009 University of Bedfordshire (www.beds.ac.uk)
  See the COPYINGS file for license details 
  =========================================================================
*/

#include "vtkALBAErythrocyteSource.h"
#include "vtkObjectFactory.h"
#include "vtkPolyData.h"
#include "vtkPoints.h"
#include "vtkCellArray.h"
#include "vtkMath.h"

#include "albaDbg.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"


vtkStandardNewMacro(vtkALBAErythrocyteSource);

vtkALBAErythrocyteSource::vtkALBAErythrocyteSource(void)
{
  this->Radius = 0.1;
  this->ThetaResolution = this->PhiResolution = 8;
	this->SetNumberOfInputPorts(0);
}


//------------------------------------------------------------------------
//This method is the one that should be used by subclasses, right now the 
//default implementation is to call the backwards compatibility method
/*virtual*/int vtkALBAErythrocyteSource::RequestData(vtkInformation *request, vtkInformationVector **inputVector, vtkInformationVector *outputVector)
//------------------------------------------------------------------------
{
	vtkInformation* outInfo = outputVector->GetInformationObject(0);
	vtkPolyData *output = vtkPolyData::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT()));

  if (output == NULL)
  {
    vtkErrorMacro(<< "Output polydata cannot be NULL.");
    return 0;
  }

  //the model is based on: 
  //S. Mu?oz San Martín, J.L. Sebastián,M. Sancho and G. Álvarez:
  //Modeling Human Erythrocyte Shape and Size Abnormalities
  //http://arxiv.org/ftp/q-bio/papers/0507/0507024.pdf

  //Normal erythrocyte parameters are: 7.8 0.5 0.9447 85.1 2.19 8
  double l_2 = 7.8 / 2;     //radius in micrometers
  double h = 0.5;           //height (narrowest place)  
  double m = 0.9447;        //parameter for Jaccobi elliptic function  

  //x = l_2*cn(u,m)*cos(t)
  //y = l_2*cn(u,m)*sin(t)
  //z = +/-h*sn(u,m)*(dn(u,m)/dn(U,m))
  //where [u,t] are parameters from range [0..U, 0..2*Pi],
  //cn, sn and dn are Jaccobi elliptic functions as defined at:
  //http://mathworld.wolfram.com/JacobiEllipticFunctions.html
  //and finally U is the solution of cn(U,m)=0, sn(U,m)=1 and dn(U,m) = sqrt(1-m),  

  //first, compute dn(U,m)
  double dnUm = sqrt(1.0 - m);
  double U = 2.5439309843214373;  //computed for the given parameters
  double emmc = 1.0 - m*m;
  
  vtkPoints* points = vtkPoints::New();
  points->Allocate(2*this->PhiResolution*this->ThetaResolution);

  //scaling
  double scale = 2*this->Radius / l_2;  //e6 because l_2 is given in micrometers
  double dblMaxT = 2*vtkMath::Pi();
  double t_step = dblMaxT / this->ThetaResolution;
  //double u_step = U / this->PhiResolution;

  //ecliptic functions are not suitable for uniform sampling
  //we will use adaptive sampling for it

  typedef struct SAMPLE_PT
  {
    double u;
    double cn;
    double sn_dn;    

    SAMPLE_PT* pNext;
  } SAMPLE_PT;
  
  double sn, dn;
  SAMPLE_PT* us = new SAMPLE_PT[this->PhiResolution];    
  Sncndn(us[0].u = U, emmc, sn, us[0].cn, dn);
  us[0].sn_dn = sn*dn; us[0].pNext = &us[1];     

  Sncndn(us[1].u = 0, emmc, sn, us[1].cn, dn);
  us[1].sn_dn = sn*dn; us[1].pNext = NULL;

  int nSamples = 2;
  for (int i = 0; i < this->PhiResolution - 2; i++)
  {
    //find the interval with the largest difference
    double dblMaxDiff = 0;
    SAMPLE_PT* pA = &us[0];
    SAMPLE_PT* pTmp = &us[0];
    while (pTmp->pNext != NULL)
    {
      //L1 distance
      double dblDiff = pTmp->u - pTmp->pNext->u +
        fabs(pTmp->sn_dn - pTmp->pNext->sn_dn);
      if (dblDiff > dblMaxDiff){
        dblMaxDiff = dblDiff; pA = pTmp;
      }

      pTmp = pTmp->pNext;
    }

    //split the interval in the mid point
    SAMPLE_PT* pNew = &us[nSamples++];    
    pNew->u = (pA->u + pA->pNext->u) / 2;
    Sncndn(pNew->u, emmc, sn, pNew->cn, dn);
    pNew->sn_dn = sn*dn;

    pNew->pNext = pA->pNext;
    pA->pNext = pNew;    
  }
  
  //top and bottom point  
  points->InsertNextPoint(0.0, 0.0, scale*h*us[0].sn_dn / dnUm);
  points->InsertNextPoint(0.0, 0.0, -scale*h*us[0].sn_dn / dnUm);

  int nCircles = 0;
  int nPointsPerCircle = 0;
  for (SAMPLE_PT* pA = us[0].pNext; pA != NULL; pA = pA->pNext)
  {    
    double r = scale*l_2*pA->cn;
    double z = scale*h*pA->sn_dn / dnUm;    

    for (double t = 0.0; t < dblMaxT; t += t_step)
    {
      double x = r*cos(t);
      double y = r*sin(t);

      points->InsertNextPoint(x, y, z);
      points->InsertNextPoint(x, y, -z);      

      nPointsPerCircle++;
    }

    nCircles++;
  } //end for u   

  nPointsPerCircle /= nCircles;
  points->Squeeze();

  delete[] us;

  //there is ThetaResolution points per one ring
  //there are 2*(PhiResolution-1) rings + two isolated top & bottom points (first two)
  vtkCellArray* cells = vtkCellArray::New();
  cells->Allocate(3*2*nPointsPerCircle*nCircles);

  //for both hemispheres
  vtkIdType tri[3];
  for (int iHemi = 0; iHemi < 2; iHemi++)
  {
    //construct a triangle fan on pole    
    tri[0] = iHemi; tri[1] = tri[0] + 2;

    for (int i = 0; i < nPointsPerCircle - 1; i++)
    {
      tri[2] = tri[1] + 2;
      cells->InsertNextCell(3, tri);
      tri[1] = tri[2];
    }

    tri[2] = tri[0] + 2;
    cells->InsertNextCell(3, tri);
  }

  //construct rings for northern hemisphere
  tri[2] = 2;
  for (int j = 1; j < nCircles - 1; j++)
  {
    //upper saw
    tri[0] = tri[2];
    tri[1] = tri[0] + 2*nPointsPerCircle;
    tri[2] = tri[0] + 2;

    for (int i = 0; i < nPointsPerCircle - 1; i++)
    {        
      cells->InsertNextCell(3, tri);
      for (int k = 0; k < 3; k++){
        tri[k] += 2;
      }
    }

    tri[2] -= 2*nPointsPerCircle;
    cells->InsertNextCell(3, tri);

    //lower saw
    tri[0] = tri[1];
    tri[1] = tri[2] + 2*nPointsPerCircle;
    cells->InsertNextCell(3, tri);

    tri[0] = tri[1]; tri[1] += 2; tri[2] += 2;
    for (int i = 0; i < nPointsPerCircle - 1; i++)
    {        
      cells->InsertNextCell(3, tri);
      for (int k = 0; k < 3; k++){
        tri[k] += 2;
      }
    }
  } //end for j

  //remember the last vertex
  vtkIdType nNHVert = tri[2];

  //construct rings for southern hemisphere
  tri[0] = 1;
  for (int j = 1; j < nCircles - 1; j++)
  {
    //upper saw
    tri[0] += 2;
    tri[1] = tri[0] + 2;
    tri[2] = tri[1] + 2*nPointsPerCircle;    

    for (int i = 0; i < nPointsPerCircle - 1; i++)
    {        
      cells->InsertNextCell(3, tri);
      for (int k = 0; k < 3; k++){
        tri[k] += 2;
      }
    }

    tri[2] = tri[1]; tri[1] -= 2*nPointsPerCircle;
    cells->InsertNextCell(3, tri);

    //lower saw   
    tri[1] = tri[2]; 
    tri[2] = tri[0] + 2*nPointsPerCircle;
    cells->InsertNextCell(3, tri);

    tri[2] = tri[1]; tri[1] = tri[2] + 2;    
    tri[0] = tri[2] - 2*nPointsPerCircle;
    for (int i = 0; i < nPointsPerCircle - 1; i++)
    {        
      cells->InsertNextCell(3, tri);
      for (int k = 0; k < 3; k++){
        tri[k] += 2;
      }
    }    
  } //end for j  

  //remember the last vertex
  vtkIdType nSHVert = tri[0] + 2;

  //connect both hemispheres
  //upper saw
  tri[0] = nNHVert; tri[1] = nSHVert; tri[2] = nNHVert + 2;
  for (int i = 0; i < nPointsPerCircle - 1; i++)
  {        
    cells->InsertNextCell(3, tri);
    for (int k = 0; k < 3; k++){
      tri[k] += 2;
    }
  }

  tri[2] = nNHVert;
  cells->InsertNextCell(3, tri);

  //lower saw
  tri[0] = tri[1]; tri[1] = nSHVert;
  cells->InsertNextCell(3, tri);

  tri[0] = tri[1]; tri[1] += 2; tri[2] += 2;
  for (int i = 0; i < nPointsPerCircle - 1; i++)
  {        
    cells->InsertNextCell(3, tri);
    for (int k = 0; k < 3; k++){
      tri[k] += 2;
    }
  }
        
  cells->Squeeze();
  
  output->SetPoints(points);
  output->SetPolys(cells);

  points->Delete();
  cells->Delete();

	return 1;
}


//------------------------------------------------------------------------
//Computes Jacobi elliptic functions sn(u,k), cn(u,k) and dn(u,k).
//In order to speedup the computation, parameters are uu = u, emmc = 1-k^2.
//N.B. routine was adopted from Numerical Recipes in C (3rd edition)
#define SNCNDN_DIGITS 13
void vtkALBAErythrocyteSource::Sncndn(const double uu, const double emmc, 
                                      double &sn, double &cn, double &dn)
//------------------------------------------------------------------------
{
  static const double CA = 1.0e-8;
  bool bo;
  int i,ii,l;
  double a,b,c,d,emc,u;
  double em[SNCNDN_DIGITS], en[SNCNDN_DIGITS];
  emc=emmc;
  u=uu;
  if (emc != 0.0) {
    bo=(emc < 0.0);
    if (bo) {
      d=1.0-emc;
      emc /= -1.0/d;
      u *= (d=sqrt(d));
    }
    a=1.0;
    dn=1.0;
    for (i=0;i<SNCNDN_DIGITS;i++) {
      l=i;
      em[i]=a;
      en[i]=(emc=sqrt(emc));
      c=0.5*(a+emc);
      if (abs(a-emc) <= CA*a) break;
      emc *= a;
      a=c;
    }
    u *= c;
    sn=sin(u);
    cn=cos(u);
    if (sn != 0.0) {
      a=cn/sn;
      c *= a;
      for (ii=l;ii>=0;ii--) {
        b=em[ii];
        a *= c;
        c *= dn;
        dn=(en[ii]+a)/(b+a);
        a=c/b;
      }
      a=1.0/sqrt(c*c+1.0);
      sn=(sn >= 0.0 ? a : -a);
      cn=c*sn;
    }
    if (bo) {
      a=dn;
      dn=cn;
      cn=a;
      sn /= d;
    }
  } else {
    cn=1.0/cosh(u);
    dn=cn;
    sn=tanh(u);
  }
}
