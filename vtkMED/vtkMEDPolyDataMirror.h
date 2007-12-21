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
  
  void SetMirrorXCoordinate(int value){m_MirrorXCoordinate = value;};
  int GetMirrorXCoordinate(){return m_MirrorXCoordinate;};
  void MirrorXCoordinateOn(){m_MirrorXCoordinate = 1;}
  void MirrorXCoordinateOff(){m_MirrorXCoordinate = 0;}

  void SetMirrorYCoordinate(int value){m_MirrorYCoordinate = value;};
  int GetMirrorYCoordinate(){return m_MirrorYCoordinate;};
  void MirrorYCoordinateOn(){m_MirrorYCoordinate = 1;}
  void MirrorYCoordinateOff(){m_MirrorYCoordinate = 0;}

  void SetMirrorZCoordinate(int value){m_MirrorZCoordinate = value;};
  int GetMirrorZCoordinate(){return m_MirrorZCoordinate;};
  void MirrorZCoordinateOn(){m_MirrorZCoordinate = 1;}
  void MirrorZCoordinateOff(){m_MirrorZCoordinate = 0;}

  void SetFlipNormals(int value){m_FlipNormals = value;};
  int GetFlipNormals(){return m_FlipNormals;};
  void FlipNormalsOn(){m_FlipNormals = 1;}
  void FlipNormalsOff(){m_FlipNormals = 0;}

protected:
  vtkMEDPolyDataMirror();
  ~vtkMEDPolyDataMirror() {};

  // Usual data generation method
  void Execute();

  int m_FlipNormals;
  int m_MirrorXCoordinate;
  int m_MirrorYCoordinate;
  int m_MirrorZCoordinate;

private:
  vtkMEDPolyDataMirror(const vtkMEDPolyDataMirror&);  // Not implemented.
  void operator=(const vtkMEDPolyDataMirror&);  // Not implemented.
};

#endif
