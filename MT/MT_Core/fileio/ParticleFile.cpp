#include <stdlib.h>
#include <stdio.h>

#include "ParticleFile.h"
#include "MT/MT_Core/primitives/BufferAgent.h"

#pragma mark MT_ParticleFile

MT_ParticleFile::MT_ParticleFile(const char* inFileName, MT_ParticleFileTypes Type)
  : myBoundingBox()
{
  
    filename = inFileName;
    datadim = 2;
    Nparticles = 0;
    Nfields = 0;
    myType = Type;

    file = fopen(filename,"r");
    if(file){
        NCols = CountNumberOfCols(file);
        NRows = CountNumberOfRows(file);
    
        // file type determination logic for unknown type
        if( myType == unknown ){
  
            myType = DetermineFileType();
      
        }
    
        // if the type is invalid, we should bail
        if( myType == invalid )
        {
      
            fprintf(stderr,"I could not determine the type of data. \n");
      
        } else {
    
            switch( myType ){
            case classic:
                myReader = new MT_FileReaderClassic( this );
                break;
            case fishdata:
                myReader = new MT_FileReaderFishData( this );
                break;
            case threedee:
                myReader = new MT_FileReaderThreeDee( this );
                break;
            default:
                myReader = new MT_FileReaderClassic(this);
                break;
            }
      
            Nparticles = myReader->getNumParticles();
            
            CreateParticles();
      
            myReader->ReadFile();
      
            printf("Found data bounding box (%f,%f) to (%f,%f).\n", 
                   myBoundingBox.xmin,
                   myBoundingBox.ymin, 
                   myBoundingBox.xmax, 
                   myBoundingBox.ymax);
      
            if( myType == threedee )
            {
        
                printf("\t 3D Data with z limits %f - %f.\n", myBoundingBox.zmin, myBoundingBox.zmax);
        
            }
      
            /*swarm->Resize(myBoundingBox.xmin,
              myBoundingBox.ymin,
              myBoundingBox.xmax,
              myBoundingBox.ymax);*/
      
        }
          
        fclose(file);
    
    } else {
    
        fprintf(stderr,"Cannot Open File %s. Skipping this step.\n",filename);
        myType = invalid;
    
    }
}

/* Creates and assigns particles in the swarm. */
void MT_ParticleFile::CreateParticles()
{
 
    if(Nparticles){
        for(unsigned int cp = 0; cp < Nparticles; cp++)
        {
            m_vpAgents.push_back(new MT_BufferAgent());
        }
    }
  
}


/* Determines which type of file.  Current logic is very simple:
    NCols > NRows => classic, otherwise we need at least 4 columns
    to have valid fish data (columns are id, time, x, y, then possibly
    other data.  Exactly 5 columns indicates 3d data.  */
MT_ParticleFileTypes MT_ParticleFile::DetermineFileType()
{
  
    // if the number of columns is large, this indicates classic
    if( NCols > NRows )
        return classic;
    
    // if there are exactly 5 columns, this is 3d data
    if( NCols == 5 )
        return threedee;
  
    // otherwise, we need at least 4 columns for valid fishdata
    if( NCols == 4 || NCols >= 6)
        return fishdata;
  
    // at this point, we don't know - say it is invalid
    return invalid;
  
}

/* Return the next float in the file. */
float MT_ParticleFile::getNextFloat()
{
 
    float value = 0;
  
    fscanf(file, "%f", &value);
  
    return value;
  
}

/* Read at the specified row, column location. 
    row and col are indexed from zero. */
float MT_ParticleFile::readRowCol( unsigned long row, unsigned long col )
{
 
    // save the current position of the file
    fpos_t starting_position;
    fgetpos( file, &starting_position );
  
    // rewind the file
    rewind( file );
  
    // calculate the number of floats we have to go through
    unsigned long goto_float = row*NCols + col;
  
    // cycle through until we get the correct position
    float value = 0;
    for( unsigned long current_float = 0; current_float <= goto_float; current_float++ )
    {
    
        value = getNextFloat();
    
    }  
  
    // return to the previous position in the file
    fsetpos(file, &starting_position);
  
    // the correct value should be the one in "value" at the
    //  end of the above loop
    return value;
  
}


/* Counts the number of rows per column in file.  Rewinds
    to the beginning of the file upon completion. */
particle_counter MT_ParticleFile::CountNumberOfRows(FILE* file)
{
    float junk;
    particle_counter count = 0;
    particle_counter nr = 0;

    printf("Counting the Number of Rows in File %s:  ",filename);
    while(!feof(file))
    {
        fscanf(file,"%f", &junk);
        count++;
    }
    rewind(file);

    nr = count/NCols;
    printf("%ld\n", (unsigned long) nr);

    return(nr);

}


/* Counts the number of columns per line in file.  Rewinds
    to the beginning of the file upon completion. */
particle_counter MT_ParticleFile::CountNumberOfCols(FILE* file)
{
    char c;
    int fieldcount = 0;

    printf("Counting the Number of Columns per Line in File %s:  ",filename);
    c = fgetc(file);
    // ignore leading whitespaces
    while(c == ' ' || c == '\t'){c = fgetc(file); }
    do{
        c = fgetc(file);
        // look for floating point numbers
        while(isFloatDigit(c)) { c = fgetc(file); }
        // then the whitespace between fields
        while(c == ' ' || c == '\t') { c = fgetc(file); }
        fieldcount++;
    } while(c != '\n' && c != '\r' && c != '\f');

    rewind(file);

    printf("%d\n",fieldcount);

    return fieldcount;
}

/* Push a value on the a particle's x buffer. */
void MT_ParticleFile::PushX( particle_counter particle, float value)
{
    if(particle < m_vpAgents.size())
    {
        m_vpAgents[particle]->AppendX(value);
    }
    myBoundingBox.ShowX(value);
}

/* Push a value on the a particle's y buffer. */
void MT_ParticleFile::PushY( particle_counter particle, float value)
{
    if(particle < m_vpAgents.size())
    {
        m_vpAgents[particle]->AppendY(value);
    }  
    myBoundingBox.ShowY(value);
}

/* Push a value on the a particle's z buffer. */
void MT_ParticleFile::PushZ( particle_counter particle, float value)
{
    if(particle < m_vpAgents.size())
    {
        m_vpAgents[particle]->AppendZ(value);
    }  
    myBoundingBox.ShowZ(value);
}


/****************************************************************/
#pragma mark MT_FileReaderPrototype

/* Prototype constructor for file reader.  */
MT_FileReaderPrototype::MT_FileReaderPrototype( MT_ParticleFile* setFile )
{
  
    myFile = setFile;
  
}

/****************************************************************/
#pragma mark MT_FileReaderClassic

/* Get the number of particles. */
particle_counter MT_FileReaderClassic::getNumParticles()
{
 
    return ( myFile->getNRows() >> 1 );
  
}

/* Read the data from the file.  Structure of the "classic" data
    file is 
 
    N x1[1] x1[2] x1[3] ....
    N x2[1] x2[2] x2[3] ....
    ....
    N xN[1] xN[2] xN[3] ....
    N y1[1] y1[2] y1[3] ....
    ....
    N yN[1] yN[2] yN[3] ....
 
*/
void MT_FileReaderClassic::ReadFile()
{
  
    particle_counter cp = 0;
    particle_counter np = myFile->getNumParticles();
    particle_counter nc = myFile->getNCols();

    printf("Reading X Data from %s\n", myFile->getFileName() );
    for(cp = 0; cp < np; cp++)
    {
        float t;
    
        for(unsigned long i = 0; i < nc; i++)
        {
      
            t = myFile->getNextFloat();
      
            // The first field is N, so throw it away    
            if(i > 0)
            {
        
                myFile->PushX( cp, t );
        
            }
        }
    
    }
  
    printf("Reading Y Data from %s\n", myFile->getFileName() );
    for(cp = 0; cp < np; cp++){
        float t;
    
        for(unsigned int i = 0; i < nc; i++)
        {
      
            t = myFile->getNextFloat(); 
      
            // The first field is N, so throw it away
            if(i > 0)
            {
        
                myFile->PushY(cp, t); 
        
                myFile->PushZ(cp, 0);
        
            }
      
        }
    }
  
}


/****************************************************************/
#pragma mark MT_FileReaderFishData

/* Get the number of particles. */
particle_counter MT_FileReaderFishData::getNumParticles()
{
  
    return ((particle_counter) ( myFile->readRowCol( myFile->getNRows() - 1, 0 )) + 1 );
  
}

/* Read the data from the file.  Structure of the "fishdata" data
    file is 
 
    index1 t1[1] x1[1] y1[1] ... (other data) ...
    index1 t1[2] x1[2] y1[2] ... (other data) ...
    ....  (as many times as there are)
    index2 t2[1] x2[1] y2[1] ... (other data) ...
    index2 t2[2] x2[2] y2[2] ... (other data) ...
    ....  (as many times as there are)
    ....  (as many fish as there are)
    indexN tN[1] xN[1] yN[1] ... (other data) ...
    indexN tN[2] xN[2] yN[2] ... (other data) ...
    ....  (as many times as there are)
 
*/
void MT_FileReaderFishData::ReadFile()
{
  
    particle_counter cp = 0;
    particle_counter np = myFile->getNumParticles();
    particle_counter nc = myFile->getNCols();
    particle_counter nr = myFile->getNRows();
  
    float t;
  
    unsigned long nt = nr / np;
  
    for( cp = 0; cp < np; cp++ )
    {
        
        for( unsigned long ct = 0; ct < nt; ct++ )
        {
      
            // first number is the index
            t = myFile->getNextFloat();
            // second is the time
            t = myFile->getNextFloat();
      
            // next is x
            t = myFile->getNextFloat();
            myFile->PushX( cp, t );
      
            // next is y
            t = myFile->getNextFloat();
            myFile->PushY( cp, t );
      
            myFile->PushZ( cp, 0 );
      
            // the remaining columns we won't use
            for( unsigned long cc = 4; cc < nc; cc++)
            {
      
                t = myFile->getNextFloat();
        
            }
      
        }
    
    }
  
}


/****************************************************************/
#pragma mark MT_FileReaderThreeDee

/* Get the number of particles. */
particle_counter MT_FileReaderThreeDee::getNumParticles()
{
  
    return ((particle_counter) ( myFile->readRowCol( myFile->getNRows() - 1, 0 ))  );
  
}

/* Read the data from the file.  Structure of the "threedee" data
    file is the same as fishdata, with the column after y being z. 
*/
void MT_FileReaderThreeDee::ReadFile()
{
  
    particle_counter cp = 0;
    particle_counter np = myFile->getNumParticles();
    particle_counter nc = myFile->getNCols();
    particle_counter nr = myFile->getNRows();
  
    float t;
  
    unsigned long nt = nr / np;
  
    for( cp = 0; cp < np; cp++ )
    {
    
        for( unsigned long ct = 0; ct < nt; ct++ )
        {
      
            // first number is the index
            t = myFile->getNextFloat();
            // second is the time
            t = myFile->getNextFloat();
      
            // next is x
            t = myFile->getNextFloat();
            myFile->PushX( cp, t );
      
            // next is y
            t = myFile->getNextFloat();
            myFile->PushY( cp, t );
      
            // next is z
            t = myFile->getNextFloat();
            myFile->PushZ( cp, t );
      
            // the remaining columns we won't use
            for( unsigned long cc = 5; cc < nc; cc++)
            {
        
                t = myFile->getNextFloat();
        
            }
      
        }
    
    }
  
}

/****************************************************************/
#pragma mark Static Functions

int isFloatDigit(char ch)
{
    char valids[] = "0123456789eE-.+";
    int nvalids = 15;
  
    for(int i = 0; i < nvalids; i++){
        if (ch == valids[i]){return 1;}
    }

    return 0;
}
