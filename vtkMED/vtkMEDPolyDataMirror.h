/////////////////////////////////////////////////////////////////////////////
// Name:        vtkMEDPolyDataMirror.h
//
// project:     MULTIMOD 
// Author:      Silvano Imboden
// Date:        28/10/2003
/////////////////////////////////////////////////////////////////////////////

// .NAME vtkPolyDataMirror - mirror a PolyData along the specified axis, 
// .SECTION Description
// vtkPolyDataMirror is a filter that make a mirrored copy of the Polydata in input.
// Normals are recomputed as required

#ifndef __vtkMEDPolyDataMirror_h
#define __vtkMEDPolyDataMirror_h

#include "vtkPolyDataToPolyDataFilter.h"
#include "vtkMAFConfigure.h"

class vtkDoubleArray;
class vtkIdList;
class vtkPolyData;

//----------------------------------------------------------------------------
class VTK_vtkMAF_EXPORT vtkMEDPolyDataMirror : public vtkPolyDataToPolyDataFilter
//----------------------------------------------------------------------------
{
public:
  vtkTypeRevisionMacro(vtkMEDPolyDataMirror,vtkPolyDataToPolyDataFilter);
  void PrintSelf(ostream& os, vtkIndent indent);

  static vtkMEDPolyDataMirror *New();
  
  void SetMirrorXCoordinate(int value){MirrorXCoordinate = value;};
  int GetMirrorXCoordinate(){return MirrorXCoordinate;};
  void MirrorXCoordinateOn(){MirrorXCoordinate = 1;}
  void MirrorXCoordinateOff(){MirrorXCoordinate = 0;}

  void SetMirrorYCoordinate(int value){MirrorYCoordinate = value;};
  int GetMirrorYCoordinate(){return MirrorYCoordinate;};
  void MirrorYCoordinateOn(){MirrorYCoordinate = 1;}
  void MirrorYCoordinateOff(){MirrorYCoordinate = 0;}

  void SetMirrorZCoordinate(int value){MirrorZCoordinate = value;};
  int GetMirrorZCoordinate(){return MirrorZCoordinate;};
  void MirrorZCoordinateOn(){MirrorZCoordinate = 1;}
  void MirrorZCoordinateOff(){MirrorZCoordinate = 0;}

  void SetFlipNormals(int value){FlipNormals = value;};
  int GetFlipNormals(){return FlipNormals;};
  void FlipNormalsOn(){FlipNormals = 1;}
  void FlipNormalsOff(){FlipNormals = 0;}

protected:
  vtkMEDPolyDataMirror();
  ~vtkMEDPolyDataMirror() {};

  // Usual data generation method
  void Execute();

  int FlipNormals;
  int MirrorXCoordinate;
  int MirrorYCoordinate;
  int MirrorZCoordinate;

private:
  vtkMEDPolyDataMirror(const vtkMEDPolyDataMirror&);  // Not implemented.
  void operator=(const vtkMEDPolyDataMirror&);  // Not implemented.
};

#endif
