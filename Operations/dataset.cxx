#include "dataset.h"
#include <string.h>
#include <iostream>
#include <math.h>


int SearchIds(float *coords, unsigned int dim, float x, unsigned int *ids) {
unsigned int id;


  if (x < coords[0] || x > coords[dim - 1])
    return 1;

  ids[0] = 0;
  ids[1] = dim - 1;

  do {
    id = (ids[0] + ids[1]) / 2;

    if ( x < coords[id]) 
      if (ids[1] == id) break;
      else ids[1] = id;
    else
      if ( x > coords[id]) 
	if (ids[0] == id) break;
	else ids[0] = id;
      else {
	// x == coords[id]
	ids[0] = ids[1] = id;
	break;
      }
  } while (1);
      
  return 0;
}
    
void DataSet::SetDimensions(unsigned int xdim, unsigned int ydim, unsigned int zdim) {
  this->dims[0] = xdim;
  this->dims[1] = ydim;
  this->dims[2] = zdim;  
}


int DataSet::ReadPointData(FILE *fp, int type) {
  static char line[256];
  unsigned int id, numScalars;
  float scalar;

  fscanf(fp, "%s %u\n", line, &numScalars);
  if ( strncmp(line,"POINT_DATA",10) ) {
    std::cerr << "Error reading point data!";
    return 1;
  }

  fscanf(fp, "%s", line);
  if ( strncmp(line,"SCALARS",7) ) {
    std::cerr << "Error reading keyword scalars";
    return 1;
  }

  fscanf(fp, "%s", line);
 
  fscanf(fp, "%s", line);  
  if ( !strncmp(line,"short",5) ) {
    this->scalars = TScalars<short>::New();
    this->scalars->SetNumberOfScalars(numScalars);
  } else if ( !strncmp(line,"unsigned_short", 14) ) {
    this->scalars = TScalars<unsigned short>::New();
    this->scalars->SetNumberOfScalars(numScalars);
  } else if ( !strncmp(line,"unsigned_char", 13) ) {
    this->scalars = TScalars<unsigned char>::New();
    this->scalars->SetNumberOfScalars(numScalars);
  } else {
    std::cerr << "Error reading scalars type\n";
    return 1;
  }



  fgets(line, 255, fp);
  fgets(line, 255, fp);
  if (type)
    for (id=0;  id < numScalars; id++) 
      this->scalars->ReadBinaryScalar(id, fp);
  else
    for (id=0;  id < numScalars; id++) {
      fscanf(fp, "%f", &scalar);   
      this->scalars->SetScalar(id, scalar);
    }

  return 0;
}

#define r rst[0]
#define s rst[1]
#define t rst[2]

#define cr crst[0]
#define cs crst[1]
#define ct crst[2]


float DataSet::InterpolateScalars(float *rst, float *crst, unsigned int *ids) {
  float w[8];
  float scalar;
  int i,j,k;
  unsigned int id;

  w[0] = cr * cs * ct;
  w[1] = r * cs * ct;  
  w[2] = cr * s * ct;
  w[3] = r * s * ct;
  w[4] = cr * cs * t;
  w[5] = r * cs * t;
  w[6] = cr * s * t;
  w[7] = r * s * t;
 

  scalar = 0;
  for (k=0; k < 2; k++)
    for (j=0; j < 2; j++)
      for (i=0; i < 2; i++) {
	id = ids[i] + ids[2 + j] * this->dims[0] + ids[4 + k] *  this->dims[0] * this->dims[1];	
	scalar += w[i + 2*j + 4*k] * this->scalars->GetScalar(id);
      }

  return scalar; 
};


// SP class

void SP::SetDimensions(unsigned int xdim, unsigned int ydim, unsigned int zdim) {
  DataSet::SetDimensions(xdim, ydim, zdim);
}



int SP::ReadStructure(FILE *fp, int type) {
  char line[256];  
  
  fscanf(fp, "%s %f %f %f\n", line, &this->spc[0], &this->spc[1], &this->spc[2]);
  if (  strncmp(line, "SPACING", 7)) {
    std::cerr << "Error reading spacing!:" << line << "\n";
    return 1;
  }
  fscanf(fp, "%s %f %f %f\n", line, &this->origin[0], &this->origin[1] , &this->origin[2]);
  if (  strncmp(line, "ORIGIN", 6) ) {
    std::cerr << "Error reading origin!";
    return 1;
  }

  return 0;
}




float  SP::ComputeScalar(float *x) { 
  float idf[3];
  unsigned int ids[3][2];
  int i; 
  float rst[3], crst[3]; 


  for (i=0; i<3; i++) {
    idf[i] = (x[i] - this->origin[i]) / this->spc[i];
    if (idf[i] < 0 || idf[i] > this->dims[i] - 1)
      return 0;
    ids[i][0] = (unsigned int) floor(idf[i]);
    ids[i][1] = (unsigned int) ceil (idf[i]);
    
    rst[i] = (idf[i] - ids[i][0]) * this->spc[i]; 
    crst[i] = 1 -  rst[i];
  }
   
  return this->InterpolateScalars(rst, crst, &ids[0][0]); 
  
}

  
// CT class
int CT::ReadStructure(FILE *fp, int type) { 
  char line[256];  
  unsigned int i;
  
  fscanf(fp, "%s %f %f\n", line, &this->spc[0], &this->spc[1]);
  if (  strncmp(line, "SPACING", 7)) {
    std::cerr << "Error reading spacing!:" << line << "\n";
    return 1;
  }
  fscanf(fp, "%s %f %f\n", line, &this->origin[0], &this->origin[1]);
  if (  strncmp(line, "ORIGIN", 6) ) {
    std::cerr << "Error reading origin!";
    return 1;
  }

  fgets(line, 255, fp);
  if (  strncmp(line, "LOCATION", 8) ) {
    std::cerr << "Error reading locations!";
    return 1;
  }
  

  if (type)     
    for (i=0; i < this->dims[2]; i++) {
      if (fread(&this->locations[i], sizeof(float), 1, fp) != 1) {
std::cerr << "Error reading location " << i << std::endl;
	return 1;
      } 
    }
  else
    for (i=0; i < this->dims[2]; i++)
      if (fscanf(fp, "%f", &this->locations[i]) != 1) {
	std::cerr << "Error reading location " << i << std::endl;
	return 1;
      } 

  return 0;
}

void CT::SetDimensions(unsigned int xdim, unsigned int ydim, unsigned int zdim) {
  DataSet::SetDimensions(xdim, ydim, zdim);
  this->locations = new float[zdim];
}

float  CT::ComputeScalar(float *x) { 
  float idf[2];
  unsigned int ids[3][2];
  int i; 
  float rst[3], crst[3];
  
  for (i=0; i<2; i++) {
    idf[i] = (x[i] - this->origin[i]) / this->spc[i];  
    if (idf[i] < 0 || idf[i] > this->dims[i] - 1)
      return 0;
    
    ids[i][0] = (unsigned int) floor(idf[i]);
    ids[i][1] = (unsigned int) ceil (idf[i]);
    
    rst[i] = (idf[i] - ids[i][0]) * this->spc[i]; 
    crst[i] = 1 -  rst[i];
  }
      
  if (SearchIds(this->locations, this->dims[2], x[2], ids[2]))
      return 0;


  if (ids[2][0] == ids[2][1])
    rst[i] = 0;
  else      
    rst[2] = (x[2] - this->locations[ids[2][0]]) / (this->locations[ids[2][1]] -  this->locations[ids[2][0]]);

  crst[2] = 1 -  rst[2];

  return this->InterpolateScalars(rst, crst, &ids[0][0]);
}

// RG class

int RG::ReadStructure(FILE *fp, int type) { 
  char line[256];  
  unsigned int i,j;
  char *tag[3] = { "X_COORDINATES\0", "Y_COORDINATES\0", "Z_COORDINATES\0" };
  
  for (j=0; j<3; j++) {
    fgets(line, 255, fp);
    if (  strncmp(line, tag[j], 12) ) {
      std::cerr << "Error reading " << tag[j] << std::endl;
      return 1;
    }
  
    if (type)
      for (i=0; i < this->dims[j]; i++) {
	if (fread(&this->coords[j][i], sizeof(float), 1, fp) != 1) {
	  std::cerr << "Error reading coordinate " << i << " of "<< tag[j] << std::endl;
	  return 1;
	}
      }
    else
      for (i=0; i < this->dims[j]; i++)
	if (fscanf(fp, "%f", &this->coords[j][i]) != 1) {
	  std::cerr << "Error reading coordinate " << i << " of "<< tag[j] << std::endl;
	  return 1;
	} 
    
    fscanf(fp, "\n"); // eat EOL
  }

  return 0;
}

void RG::SetDimensions(unsigned int xdim, unsigned int ydim, unsigned int zdim) {
  DataSet::SetDimensions(xdim, ydim, zdim);
  this->coords[0] = new float[xdim];
  this->coords[1] = new float[ydim];
  this->coords[2] = new float[zdim];
}

float RG::ComputeScalar(float *x) { 
  unsigned int ids[3][2];
  int i; 
  float rst[3], crst[3];

  for (i=0; i < 3; i++) {
    if (SearchIds(this->coords[i], this->dims[i], x[i], ids[i]))
      return 0;
      
    if (ids[i][0] == ids[i][1])
      rst[i] = 0;
    else
      rst[i] = (x[i] - this->coords[i][ids[i][0]]) / (this->coords[i][ids[i][1]] -  this->coords[i][ids[i][0]]);

    //rst[i] = x[i] / (this->coords[i][ids[i][1]] -  this->coords[i][ids[i][0]]) - 
    //this->coords[i][ids[i][0]] / (this->coords[i][ids[i][1]] -  this->  coords[i][ids[i][0]]);

    crst[i] = 1 -  rst[i];
  }

  return this->InterpolateScalars(rst, crst, &ids[0][0]); 
}
