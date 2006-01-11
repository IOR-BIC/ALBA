#include <stdio.h>
#include "types.h"

class Scalars {
public:
  virtual void SetNumberOfScalars(ID_TYPE numScalars) = 0;   
  virtual void SetScalar(ID_TYPE id, float scalar) = 0;
  virtual float GetScalar(ID_TYPE id) = 0 ;  
  virtual int ReadBinaryScalar(ID_TYPE id, FILE *fp) = 0 ;
};

template <class T> class TScalars: public Scalars {
public:
  static TScalars *New() { return new TScalars;};
  void SetNumberOfScalars(ID_TYPE numScalars) {
    scalars = new T[numScalars];
  }; 
  void SetScalar(ID_TYPE id, float scalar) {
    this->scalars[id] = (T) scalar;
  };

  float  GetScalar(ID_TYPE id) {
    return this->scalars[id];
  };

  int ReadBinaryScalar(ID_TYPE id, FILE *fp) {
    return fread(&this->scalars[id], sizeof(T), 1, fp);
  };

  	 
private:
  T *scalars;
};  
  

class DataSet {
public:
  virtual void SetDimensions(unsigned int xdim, unsigned int ydim, unsigned int zdim);  
  virtual int ReadStructure(FILE *fp, int type) = 0;  
  int ReadPointData(FILE *fp, int type);
  int ReadData(FILE *fp, int type) { 
    if (this->ReadStructure(fp, type))
      return 1;
    else 
      return this->ReadPointData(fp, type);
  }; 
  unsigned int *GetDimensions() { return this->dims; };    
  virtual float ComputeScalar(float *x) = 0;
  float ComputeScalar(float x, float y, float z) { 
    float xyz[3];
    
    xyz[0] = x;
    xyz[1] = y;
    xyz[2] = z;
    return this->ComputeScalar(xyz);
  };
  float InterpolateScalars(float *rst, float *crst, unsigned int *ids);
  Scalars *GetScalars() {return scalars;};

protected:
  unsigned int dims[3];
  Scalars *scalars;
};

class SP: public DataSet {
public:
  static SP *New() { return new SP; };
  void SetDimensions(unsigned int xdim, unsigned int ydim, unsigned int zdim);    
  int ReadStructure(FILE *fp, int type);  
  float ComputeScalar(float *x);
  void SetSpacing (float xspc, float yspc, float zspc) {    
    spc[0] = xspc;
    spc[1] = yspc;
    spc[2] = zspc;
  };
  void SetOrigin (float x, float y, float z) {    
    origin[0] = x;
    origin[1] = y;
    origin[2] = z;
  };
    
private:
  float spc[3];
  float origin[3];
};

class CT: public DataSet {
public:
  static CT *New() { return new CT; };    
  void SetDimensions(unsigned int xdim, unsigned int ydim, unsigned int zdim);
  int ReadStructure(FILE *fp, int type);
  float ComputeScalar(float *x);
  void SetSpacing (float xspc, float yspc);
  void SetLocations(float *locs);
  void SetLoc(unsigned int id, float *loc);

private:
  float spc[2];
  float origin[2];
  float *locations;
};

class RG: public DataSet {
public:
  static RG *New() { return new RG; };
  void SetDimensions(unsigned int xdim, unsigned int ydim, unsigned int zdim);
  int ReadStructure(FILE *fp, int type);
  float ComputeScalar(float *x);
  void SetCoords(float *xcoords, float *ycoords, float *zcoords) {
    this->coords[0] = xcoords;
    this->coords[1] = ycoords;
    this->coords[2] = zcoords;
  }
 
private:
  float *coords[3];  
};


