#ifndef PARTICLEFILE_H
#define PARTICLEFILE_H

/** @addtogroup MT_Core
 * @{
 */

/** @file
 * ParticleFile.h
 *
 * @brief Defines a class for reading trajectory data from one of a
 * handful of file formats.
 *
 * It would be relatively easy to add a format plugin to this module.
 * See the source code.  It would be good in the future if this could
 * be modified so that it didn't require editing of the actual MT
 * source code - e.g. some sort of RegisterFileType method.
 *
 */

#include <stdio.h>
#include <vector>

#include "MT/MT_Core/primitives/BufferAgent.h"
#include "MT/MT_Core/primitives/BoundingBox.h"

/*****************************************************************/

/** Specifies the type of data contained in an MT_ParticleFile
 * @see MT_ParticleFile for a description of the types. */
enum MT_ParticleFileTypes
{
    invalid,     /**< Indicates an error in reading data. */
    unknown,     /**< Indicates type is unknown and that the program
                  * should determine it automatically. */
    classic,     /**< "Classic" format. */
    fishdata,    /**< "Fish Data" format. */
    threedee     /**< "3D" format. */
};

/** Used in MT_ParticleFile to count individual trajectories. It
 * should be safe to use unsigned long or even int when the number of
 * trajectories is not huge. */
typedef unsigned long particle_counter;

/*****************************************************************/

/* TODO this should be moved to stringsupport or some such. */
int isFloatDigit(char ch);

/*****************************************************************/

// Forward declaration necessary
class MT_FileReaderPrototype;
class MT_FileReaderClassic;
class MT_FileReaderFishData;
class MT_FileReaderThreeDee;

/*****************************************************************/


/** @class MT_ParticleFile
 *
 * @brief Automatically reads a variety of trajectory data file
 * formats.
 *
 * Loads the data into MT_BufferAgent instances.
 *
 * This is a list of formats available.
 *  - "Classic" - Each row starts with N and then contains either the
 * x or y positions of each successive agent.  All of the x positions
 * are listed first and then all of the y positions.  One agent per
 * line.  This version is very easy to generate in Matlab.  The
 * showInPlaySwarm.m file in the matlab directory of the source code
 * uses this method.
 *  - "Fish Data" - Each line corresponds to all of the data for one
 * individual at one time instant.  The columns are in the order
 * individual index, time, x position, y position, then potentially
 * other data.  All of the data for one individual is listed before
 * the next individual is started.
 *  - "3D" - Experimental and not well supported.  Just like the fish
 * data but there are only five columns, with the fifth column being
 * the z position.
 *
 * The decision of which file type is determined as follows. Let NCols
 * be the number of columns in the file and NRows be the number of rows.
 *  - The type can be forced in the constructor
 *  - Classic if NCols > NRows
 *  - 3D if NCols = 5
 *  - Fishdata if NCols = 4 or >= 6
 *
 * If the file could not be read or the file type could not be
 * determined, then IsValid will return false.
 *
 * When the file is read, a bounding box is automatically constructed
 * by looking at each position at each time step.
 *
 * Example of usage:
 * @code
 * // read data from data.dat
 * MT_ParticleFile pf("data.dat");
 *
 * // make sure the file read OK
 * if(!pf.IsValid())
 * {
 *    return;
 * }
 *
 * // get the bounding box e.g. for display purposes
 * MT_BoundingBox bb = pf.GetBoundingBox();
 *
 * // retrieve the agent representations for use in our program 
 * std::vector<MT_BufferAgent*> agents = pf.GetAgents();
 *
 * // we could now conceivably dispose of pf - i.e. it is OK for it to
 * // go out of scope
 * @endcode
 * 
 * @see MT_BufferAgent
 * @see MT_BoundingBox
 * 
 */
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
    /* TODO many of these don't really need to be public... */

    /** Ctor takes the filename and an optional forced type.  Will
     * read the file, determine the type, and generate the boudning
     * box and MT_BufferAgents
     * @param inFileName File name for the data.
     * @param Type Use to force a specific type.  Default (unknown)
     * will cause the program to determine the type automatically. */
    MT_ParticleFile(const char* inFileName, MT_ParticleFileTypes Type = unknown);

    /* TODO There should be a check to see if GetAgents has been
     * called.  If not, then the agents should be deleted here. */
    /** The Dtor doesn't need to do anything.  Note that this means
     * you should destroy the agents yourself after calling
     * GetAgents. */
    virtual ~MT_ParticleFile(){};

    /** Returns the number particles / agents found in the file as
     * determined by the rules for that file type. */
    particle_counter getNumParticles(){return Nparticles;};

    /** Returns the type of the file, either as forced in the ctor or
     * as determined automatically when the file was read. */
    MT_ParticleFileTypes getFileType(){return myType;};
  
    float getNextFloat();
    float readRowCol( unsigned long row, unsigned long col );

    /** Returns the name of the file associated with this object. */
    const char* getFileName() const { return filename; };

    /** Returns the number of rows found in the data file. */
    particle_counter getNRows() const { return NRows; };
    /** Returns the number of columns found in the data file. */
    particle_counter getNCols() const { return NCols; };

    /* QUARANTINED - is this used anywhere? it seems dangerous */
    /* void setNparticles( particle_counter setN ){ Nparticles = setN;
     * }; */

    /** Can be used to query whether data was successfully read from
     * the file. True indicates success, False indicates failure. */
    bool IsValid() const {return (myType != invalid);};

    /* TODO these should DEFINITELY be private (with friend access to
     * the readers) */
    void PushX(particle_counter particle, float value);
    void PushY(particle_counter particle, float value);
    void PushZ(particle_counter particle, float value);

    /** Get the read data from the file stuffed into MT_BufferAgent
     * objects.  These are generated automatically by the ctor, so
     * they are available whenever IsValid returns true.
     *
     * Note that the objects are created here but not
     * deleted - thus you are responsible for deleting them later. */
    std::vector<MT_BufferAgent*> GetAgents() const {return m_vpAgents;};
    
    /** Get the bounding box for all data found in the file.
     * Generated automatically by the ctor, so available whenever
     * IsValid returns true. */
    MT_BoundingBox GetBoundingBox() const {return myBoundingBox;};
  
};

/*****************************************************************/

/** @class MT_FileReaderPrototype
 *
 * @brief Base class for MT_ParticleFile rule sets.
 *
 * In the future there may be a way to develop these as plugins
 * without modifying the MT source code.
 * 
 */
class MT_FileReaderPrototype
{
  
protected:
  
    MT_ParticleFile* myFile;
    
public:
  
    MT_FileReaderPrototype(MT_ParticleFile* setFile);
    virtual ~MT_FileReaderPrototype(){};

    /** Calculate the number of agents/particles in the file.  The
     * prototype returns zero for safety. */
    virtual particle_counter getNumParticles(){return 0;};
  
    /** Read data from file into myFile's particle buffers. */
    virtual void ReadFile(){};
  
};

/* NOTE the specific readers are not doxy commented here because they
 * should not be accessed by the user. */

/*****************************************************************/
class MT_FileReaderClassic : public MT_FileReaderPrototype
{
  
protected:
    
public:
  
MT_FileReaderClassic(MT_ParticleFile* setFile) : MT_FileReaderPrototype( setFile ){};
  
    virtual particle_counter getNumParticles();
  
    void ReadFile();
    
};

/*****************************************************************/
class MT_FileReaderFishData : public MT_FileReaderPrototype
{
  
protected:
    
public:
  
MT_FileReaderFishData(MT_ParticleFile* setFile) : MT_FileReaderPrototype( setFile ){};
  
    virtual particle_counter getNumParticles();
  
    void ReadFile();
  
};

/*****************************************************************/
class MT_FileReaderThreeDee : public MT_FileReaderPrototype
{
    
protected:
    
public:
    
MT_FileReaderThreeDee(MT_ParticleFile* setFile) : MT_FileReaderPrototype( setFile ){};
    
    virtual particle_counter getNumParticles();
    
    void ReadFile();
    
};

/** @} */

#endif
