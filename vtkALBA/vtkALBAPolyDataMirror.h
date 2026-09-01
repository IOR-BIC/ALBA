/////////////////////////////////////////////////////////////////////////////
// Name:        vtkALBAPolyDataMirror.h
//
// project:     MULTIMOD 
// Author:      Silvano Imboden
// Date:        28/10/2003
/////////////////////////////////////////////////////////////////////////////

// .NAME vtkPolyDataMirror - mirror a PolyData along the specified axis, 
// .SECTION Description
// vtkPolyDataMirror is a filter that make a mirrored copy of the Polydata in input.
// Normals are recomputed as required

#ifndef __vtkALBAPolyDataMirror_h
#define __vtkALBAPolyDataMirror_h

#include "vtkPolyDataAlgorithm.h"
#include "albaConfigure.h"

class vtkDoubleArray;
class vtkIdList;
class vtkPolyData;

/**
  class name: vtkALBAPolyDataMirror
  Mirror the polydata over one or more axises.
*/
class ALBA_EXPORT vtkALBAPolyDataMirror : public vtkPolyDataAlgorithm
{
public:
  vtkTypeMacro(vtkALBAPolyDataMirror,vtkPolyDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);

  static vtkALBAPolyDataMirror *New();
  
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
  vtkALBAPolyDataMirror();
  ~vtkALBAPolyDataMirror() {};

  // Usual data generation method
  int RequestData( vtkInformation *vtkNotUsed(request), vtkInformationVector **inputVector, vtkInformationVector *outputVector);

  int FlipNormals;
  int MirrorXCoordinate;
  int MirrorYCoordinate;
  int MirrorZCoordinate;

private:
  vtkALBAPolyDataMirror(const vtkALBAPolyDataMirror&);  // Not implemented.
  void operator=(const vtkALBAPolyDataMirror&);  // Not implemented.
};

#endif
