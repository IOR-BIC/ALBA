/////////////////////////////////////////////////////////////////////////////
// Name:        vtkMAFPolyDataMirror.h
//
// project:     MULTIMOD 
// Author:      Silvano Imboden
// Date:        28/10/2003
/////////////////////////////////////////////////////////////////////////////

// .NAME vtkPolyDataMirror - mirror a PolyData along the specified axis, 
// .SECTION Description
// vtkPolyDataMirror is a filter that make a mirrored copy of the Polydata in input.
// Normals are recomputed as required

#ifndef __vtkMAFPolyDataMirror_h
#define __vtkMAFPolyDataMirror_h

#include "vtkPolyDataAlgorithm.h"
#include "mafConfigure.h"

class vtkDoubleArray;
class vtkIdList;
class vtkPolyData;

/**
  class name: vtkMAFPolyDataMirror
  Mirror the polydata over one or more axises.
*/
class MAF_EXPORT vtkMAFPolyDataMirror : public vtkPolyDataAlgorithm
{
public:
  vtkTypeMacro(vtkMAFPolyDataMirror,vtkPolyDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);

  static vtkMAFPolyDataMirror *New();
  
  void SetMirrorXCoordinate(int value){MirrorXCoordinate = value; Modified();};
  int GetMirrorXCoordinate(){return MirrorXCoordinate;};
  void MirrorXCoordinateOn(){MirrorXCoordinate = 1; Modified();}
  void MirrorXCoordinateOff(){MirrorXCoordinate = 0; Modified();}

  void SetMirrorYCoordinate(int value){MirrorYCoordinate = value; Modified();};
  int GetMirrorYCoordinate(){return MirrorYCoordinate;};
  void MirrorYCoordinateOn(){MirrorYCoordinate = 1; Modified();}
  void MirrorYCoordinateOff(){MirrorYCoordinate = 0; Modified();}

  void SetMirrorZCoordinate(int value){MirrorZCoordinate = value; Modified();};
  int GetMirrorZCoordinate(){return MirrorZCoordinate;};
  void MirrorZCoordinateOn(){MirrorZCoordinate = 1; Modified();}
  void MirrorZCoordinateOff(){MirrorZCoordinate = 0; Modified();}

  void SetFlipNormals(int value){FlipNormals = value; Modified();};
  int GetFlipNormals(){return FlipNormals;};
  void FlipNormalsOn(){FlipNormals = 1; Modified();}
  void FlipNormalsOff(){FlipNormals = 0; Modified();}

protected:
  vtkMAFPolyDataMirror();
  ~vtkMAFPolyDataMirror() {};

  // Usual data generation method
  int RequestData( vtkInformation *vtkNotUsed(request), vtkInformationVector **inputVector, vtkInformationVector *outputVector);

  int FlipNormals;
  int MirrorXCoordinate;
  int MirrorYCoordinate;
  int MirrorZCoordinate;

private:
  vtkMAFPolyDataMirror(const vtkMAFPolyDataMirror&);  // Not implemented.
  void operator=(const vtkMAFPolyDataMirror&);  // Not implemented.
};

#endif
