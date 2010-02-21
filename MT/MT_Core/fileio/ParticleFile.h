  #ifndef PARTICLEFILE_H
#define PARTICLEFILE_H

#include <stdio.h>
#include <vector>
//#include "wxSwarm/glswarm/swarm/glSwarm.h"
#include "MT/MT_Core/primitives/BufferAgent.h"
#include "MT/MT_Core/primitives/BoundingBox.h"

/*****************************************************************/
#pragma mark Types
#pragma mark -

enum MT_ParticleFileTypes
{
    invalid,
    unknown,
    classic,
    fishdata,
    threedee
};

typedef unsigned long particle_counter;

/*****************************************************************/
# pragma mark Static Functions

int isFloatDigit(char ch);

/*****************************************************************/
# pragma mark -
# pragma mark Classes

// Forward declaration necessary
class MT_FileReaderPrototype;
class MT_FileReaderClassic;
class MT_FileReaderFishData;
class MT_FileReaderThreeDee;

/*****************************************************************/
class MT_ParticleFile 
{
protected: 
  
    int datadim;
    particle_counter Nparticles;
    particle_counter Nfields;
    particle_counter NCols;
    particle_counter NRows;
    MT_ParticleFileTypes myType;
    MT_FileReaderPrototype* myReader;
  
    FILE* file;
  
    MT_BoundingBox myBoundingBox;
  
    std::vector<MT_BufferAgent*> m_vpAgents;

    const char *filename;
  
    virtual particle_counter CountNumberOfCols(FILE* file);
    virtual particle_counter CountNumberOfRows(FILE* file);
    virtual MT_ParticleFileTypes DetermineFileType();
    void CreateParticles();
  
public:
    MT_ParticleFile(const char* inFileName, MT_ParticleFileTypes Type = unknown);
    virtual ~MT_ParticleFile(){};
  
    particle_counter getNumParticles(){return Nparticles;};
  
    MT_ParticleFileTypes getFileType(){return myType;};
  
    float getNextFloat();
    float readRowCol( unsigned long row, unsigned long col );
    const char* getFileName() const { return filename; };
  
    particle_counter getNRows() const { return NRows; };
    particle_counter getNCols() const { return NCols; };
    void setNparticles( particle_counter setN ){ Nparticles = setN; };

    bool IsValid() const {return (myType != invalid);};

    void PushX(particle_counter particle, float value);
    void PushY(particle_counter particle, float value);
    void PushZ(particle_counter particle, float value);
  
    std::vector<MT_BufferAgent*> GetAgents() const {return m_vpAgents;};
    MT_BoundingBox GetBoundingBox() const {return myBoundingBox;};
  
};

/*****************************************************************/
#pragma mark -
class MT_FileReaderPrototype
{
  
protected:
  
    MT_ParticleFile* myFile;
    
public:
  
    MT_FileReaderPrototype(MT_ParticleFile* setFile);
    virtual ~MT_FileReaderPrototype(){};
  
    /** Get the number of particles in this file.  Prototype
        returns zero for safety. */
    virtual particle_counter getNumParticles(){return 0;};
  
    /** Read data from file into myFile's particle buffers. */
    virtual void ReadFile(){};
  
};

/*****************************************************************/
#pragma mark -
class MT_FileReaderClassic : public MT_FileReaderPrototype
{
  
protected:
    
public:
  
MT_FileReaderClassic(MT_ParticleFile* setFile) : MT_FileReaderPrototype( setFile ){};
  
    virtual particle_counter getNumParticles();
  
    void ReadFile();
    
};

/*****************************************************************/
#pragma mark -
class MT_FileReaderFishData : public MT_FileReaderPrototype
{
  
protected:
    
public:
  
MT_FileReaderFishData(MT_ParticleFile* setFile) : MT_FileReaderPrototype( setFile ){};
  
    virtual particle_counter getNumParticles();
  
    void ReadFile();
  
};

/*****************************************************************/
#pragma mark -
class MT_FileReaderThreeDee : public MT_FileReaderPrototype
{
    
protected:
    
public:
    
MT_FileReaderThreeDee(MT_ParticleFile* setFile) : MT_FileReaderPrototype( setFile ){};
    
    virtual particle_counter getNumParticles();
    
    void ReadFile();
    
};

#endif
