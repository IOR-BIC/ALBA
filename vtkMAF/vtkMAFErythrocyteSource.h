/*========================================================================= 
  Program: Multimod Application Framework RELOADED 
  Module: $RCSfile: vtkMAFErythrocyteSource.h,v $ 
  Language: C++ 
  Date: $Date: 2011-05-26 08:33:31 $ 
  Version: $Revision: 1.1.2.3 $ 
  Authors: Josef Kohout (Josef.Kohout *AT* beds.ac.uk)
  ========================================================================== 
  Copyright (c) 2009 University of Bedfordshire (www.beds.ac.uk)
  See the COPYINGS file for license details 
  =========================================================================
*/
#ifndef vtkMAFErythrocyteSource_h__
#define vtkMAFErythrocyteSource_h__

#pragma once

#include "mafConfigure.h"
#include "vtkPolyDataAlgorithm.h"

/** This class generates polydata with model of erythrocyte (blood cell).
The technique behind it is general enough to be generalized for other
types of cells, if needed. Model is based on the paper:
S. Mu?oz San Mart�n, J.L. Sebasti�n,M. Sancho and G. �lvarez:
Modeling Human Erythrocyte Shape and Size Abnormalities
http://arxiv.org/ftp/q-bio/papers/0507/0507024.pdf*/
class MAF_EXPORT vtkMAFErythrocyteSource : public vtkPolyDataAlgorithm
{
public:  
  vtkTypeMacro(vtkMAFErythrocyteSource,vtkPolyDataAlgorithm);
  static vtkMAFErythrocyteSource *New();
  
protected:
  double Radius;         ///<radius of the model
  int ThetaResolution;   ///<resolution in longitude
  int PhiResolution;     ///<resolution in latitude

public:
  
  /** Sets a new radius of the model. Default is 0.5. */  
  vtkSetClampMacro(Radius,double,0.0,VTK_DOUBLE_MAX);
  
  /** Gets the current radius of the model. */
  vtkGetMacro(Radius,double);
      
  /** Sets a new resolution in the longitude direction. */
  vtkSetClampMacro(ThetaResolution, int, 3, 1024);

  /** Gets the current resolution in the longitude direction. */
  vtkGetMacro(ThetaResolution,int);

  /** Sets a new resolution in the latitude direction. */
  vtkSetClampMacro(PhiResolution, int, 3, 1024);

  /** Gets the current resolution in the latitude direction. */
  vtkGetMacro(PhiResolution,int);  

protected:
  vtkMAFErythrocyteSource();
  ~vtkMAFErythrocyteSource() {
  }

  /**
  This method is the one that should be used by subclasses, right now the 
  default implementation is to call the backwards compatibility method */
  /*virtual*/int RequestData(vtkInformation *request, vtkInformationVector **inputVector, vtkInformationVector *outputVector);

  /** Computes Jacobi elliptic functions sn(u,k), cn(u,k) and dn(u,k).
  In order to speedup the computation, parameters are uu = u, emmc = 1-k^2.
  N.B. routine was adopted from Numerical Recipes in C (3rd edition) */
  void Sncndn(const double uu, const double emmc, double &sn, double &cn, double &dn);

private:
  vtkMAFErythrocyteSource(const vtkMAFErythrocyteSource&);  // Not implemented.
  void operator=(const vtkMAFErythrocyteSource&);  // Not implemented.
};
#endif // vtkMAFErythrocyteSource_h__
