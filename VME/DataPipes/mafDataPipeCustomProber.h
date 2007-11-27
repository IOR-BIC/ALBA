/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafDataPipeCustomProber.h,v $
  Language:  C++
  Date:      $Date: 2007-11-27 15:17:03 $
  Version:   $Revision: 1.1 $
  Authors:   Paolo Quadrani
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __mafDataPipeCustomProber_h
#define __mafDataPipeCustomProber_h

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafDataPipeCustom.h"

//----------------------------------------------------------------------------
// forward declarations
//----------------------------------------------------------------------------
class vtkDistanceFilter;
class vtkPolyDataNormals;

/** a data pipe which calculate new data depending on parameters given by the VME.
  This data pipe creates an internal vtkSource objects and links to it to 
  receive update events and calculate new data for the VME.
  @sa mafDataPipeCustom
*/
class MAF_EXPORT mafDataPipeCustomProber : public mafDataPipeCustom
{
public:
  mafTypeMacro(mafDataPipeCustomProber,mafDataPipe);

  enum PROBING_MODALITY
  {
    DENSITY_MODE = 0,
    DISTANCE_MODE,
    DISTANCE_MODE_SCALAR = 0,
    DISTANCE_MODE_VECTOR
  };

  /** Make a copy of itself.*/
  int DeepCopy(mafDataPipe *pipe);

  /** Set the surface to probe volume data*/
  void SetSurface(mafNode *surface);

  /** Return the probing surface node*/
  mafNode *GetSurface() {return m_Surface;};

  /** Set the volume to probe*/
  void SetVolume(mafNode *volume);

  /** Return the probed volume*/
  mafNode *GetVolume() {return m_Volume;};

  /** Set the mode to Density or Distance Map.*/
  void SetMode(int mode);

  /** Set the Density mode Map.*/
  void SetModeToDensity();

  /** Set the Distance mode Map.*/
  void SetModeToDistance();

  /** Get the mode.*/
  int GetMode();

  /** Set the threshold for Distance mode Map.*/
  void SetDistanceThreshold(float thr);

  /** Get the threshold of Distance mode Map.*/
  float GetDistanceThreshold();

  /** Set/Get the max distance for Distance mode Map.*/
  float GetMaxDistance();

  /** Set/Get the max distance for Distance mode Map.*/
  void SetMaxDistance(float max_dist);

  /** Set the Distance mode to produce scalar data or vector data.*/
  void SetDistanceMode(int mode);

  /** Set the Distance mode to produce scalar data.*/
  void SetDistanceModeToScalar() {this->SetDistanceMode(mafDataPipeCustomProber::DISTANCE_MODE_SCALAR);};

  /** Set the Distance mode to produce vector data.*/
  void SetDistanceModeToVector() {this->SetDistanceMode(mafDataPipeCustomProber::DISTANCE_MODE_VECTOR);};

  /** Get the distance mode type.*/
  int GetDistanceMode();

  /** Set/Get the High Density Threshold for Density mode Map.*/
  float GetHighDensity();

  /** Set/Get the High Density Threshold for Density mode Map.*/
  void SetHighDensity(float high_dens);

  /** Set/Get the Low Density Threshold for Density mode Map.*/
  float GetLowDensity();

  /** Set/Get the Low Density Threshold for Density mode Map.*/
  void SetLowDensity(float low_dens);

protected:
  mafDataPipeCustomProber();
  virtual ~mafDataPipeCustomProber();

  /** function called before of data pipe execution */
  virtual void PreExecute();

  /** function called to updated the data pipe output */
  virtual void Execute();

  mafNode *m_Surface;
  mafNode *m_Volume;

  vtkPolyDataNormals *m_Normals;
  vtkDistanceFilter  *m_Prober;

  float  m_DistThreshold;
  float  m_MaxDistance;
  int		 m_DistanceModeType;
  int		 m_ProberMode;
  float  m_HighDensity;
  float  m_LowDensity;

private:
  mafDataPipeCustomProber(const mafDataPipeCustomProber&); // Not implemented
  void operator=(const mafDataPipeCustomProber&); // Not implemented  
};
#endif /* __mafDataPipeCustomProber_h */
