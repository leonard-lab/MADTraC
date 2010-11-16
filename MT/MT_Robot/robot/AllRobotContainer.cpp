/*
 *  AllRobotContainer.cpp
 *
 *  Created by Daniel Swain on 12/6/08.
 *
 */

#include "AllRobotContainer.h"

MT_AllRobotContainer::MT_AllRobotContainer(const std::string robot_names[])
{    

    for(unsigned int i = 0; i < MT_MAX_NROBOTS; i++)
    {
        myRobots.push_back(NULL);
        PortName[i] = MT_DefaultPortName[i];
        RobotName[i] = robot_names[i];
        GamepadIndex[i] = MT_NO_GAMEPAD;
        StatusChange[i] = MT_ROBOT_NO_CHANGE;
        TrackingIndex[i] = MT_NOT_TRACKED;
    }

}

MT_AllRobotContainer::~MT_AllRobotContainer()
{

    /* Robot memory should be released here */
    for(unsigned int i = 0; i < MT_MAX_NROBOTS; i++)
    {
        if(IsPhysical(i))
        {
            ClearBot(i);
        }
    }

}

void MT_AllRobotContainer::SetRobotNames(std::vector<std::string> newnames)
{
    for(unsigned int i = 0; i < MT_MIN(newnames.size(), MT_MAX_NROBOTS); i++)
    {
        RobotName[i] = newnames[i];
    }
}

void MT_AllRobotContainer::DumpRobotNames()
{
    for(unsigned int i = 0; i < MT_MAX_NROBOTS; i++)
    {
        printf("Robot Name %d: %s\n", i, RobotName[i].c_str());
    }
}

int MT_AllRobotContainer::GetNumAvailable() const
{

    int n = 0;

    for(unsigned int i = 0; i < MT_MAX_NROBOTS; i++)
    {
        if(IsPhysical(i) && IsConnected(i))
        {
            n += 1;
        }
    }

    return n;

}

int MT_AllRobotContainer::GetNumTracked() const
{

    int n = 0;

    for(unsigned int i = 0; i < MT_MAX_NROBOTS; i++)
    {
        if(IsPhysical(i) && IsConnected(i) && (TrackingIndex[i] != MT_NOT_TRACKED))
        {
            n += 1;
        }
    }

    return n;

}

int MT_AllRobotContainer::GetNextUntracked() const
{

    for(unsigned int i = 0; i < MT_MAX_NROBOTS; i++)
    {
        if(IsPhysical(i) && IsConnected(i) && (TrackingIndex[i] == MT_NOT_TRACKED))
        {
            return i;
        }
    }

    return MT_NONE_AVAILABLE;

}


void MT_AllRobotContainer::ClearStatusChanges()
{

    for(unsigned int i = 0; i < MT_MAX_NROBOTS; i++)
    {
        StatusChange[i] = MT_ROBOT_NO_CHANGE;
    }

}

void MT_AllRobotContainer::ClearTrackingIDs()
{

    for(unsigned int i = 0; i < MT_MAX_NROBOTS; i++)
    {
        TrackingIndex[i] = MT_NOT_TRACKED;
    }

}

void MT_AllRobotContainer::SetBot(unsigned int i, MT_RobotBase* newBot)
{

    // index too high, refuse to do anything
    if(i >= MT_MAX_NROBOTS)
    {
        return;
    }


    // assign to the robot list
    if(myRobots[i] != NULL)
    {  // if there was a robot here, it was not connected, so delete it
        delete myRobots[i];
    }
    myRobots[i] = newBot;

    // mark status change
    StatusChange[i] = MT_ROBOT_JUST_CONNECTED;

}

void MT_AllRobotContainer::ClearBot(unsigned int i)
{

    // we should really test the connection here, but this
    //  doesn't seem to work quite right yet...
    // if(myRobots[i]->TestConnection())....

    // delete the object (disconnects robot)
    delete myRobots[i];
    // set to NULL to avoid problems
    myRobots[i] = NULL;

    // flag status change
    StatusChange[i] = MT_ROBOT_JUST_DISCONNECTED;

}

MT_RobotBase*& MT_AllRobotContainer::operator[](unsigned int i)
{

    return myRobots[MT_SAFE_ROBOT_INDEX(i)];

}

MT_RobotBase*& MT_AllRobotContainer::GetRobot(unsigned int i)
{

    return myRobots[MT_SAFE_ROBOT_INDEX(i)];

}

bool MT_AllRobotContainer::IsPhysical(unsigned int i) const
{

    return (myRobots[MT_SAFE_ROBOT_INDEX(i)] != NULL);

}

bool MT_AllRobotContainer::IsConnected(unsigned int i) const
{

    return myRobots[MT_SAFE_ROBOT_INDEX(i)]->IsConnected();

}

void MT_AllRobotContainer::UpdateState(unsigned int i, float xpos, float ypos, float theta)
{

    // calls MT_RobotBase::Update(x, y, z = 0, theta)
    myRobots[MT_SAFE_ROBOT_INDEX(i)]->Update(xpos, ypos, theta);

}

float MT_AllRobotContainer::GetX(unsigned int i) const
{
    return myRobots[MT_SAFE_ROBOT_INDEX(i)]->GetX();
}

float MT_AllRobotContainer::GetY(unsigned int i) const
{
    return myRobots[MT_SAFE_ROBOT_INDEX(i)]->GetY();
}

float MT_AllRobotContainer::GetHeading(unsigned int i) const
{
    return myRobots[MT_SAFE_ROBOT_INDEX(i)]->GetTheta();
}

#ifdef MT_USE_XML

bool MT_WriteRobotXML(MT_AllRobotContainer* robots,
                      MT_XMLFile* xmlfile,
                      bool strict_root)
{

    if(strict_root)
    {
        if(xmlfile->HasRoot())
        {
            if(!xmlfile->HasRootname("RobotConfig"))
            {
                return false;
            }
        }
        else
        {
            xmlfile->InitNew("RobotConfig");
        }
    }

    MT_RobotBase* cbot;
    for(unsigned int i = 0; i < MT_MAX_NROBOTS; i++)
    {
        cbot = robots->GetRobot(i);
        if(cbot && cbot->GetParameters())
        {
            WriteDataGroupToXML(xmlfile, cbot->GetParameters());
        }
    }

    TiXmlElement* rootelem = xmlfile->RootAsElement();
    TiXmlNode* tnode;

    // write joystick control parameters
/*    TiXmlNode* tnode = rootelem->FirstChild("GamepadParameters");
    if(tnode)
    {
        rootelem->RemoveChild(tnode);
        tnode = NULL;
    }
    TiXmlElement* gamepadparams = new TiXmlElement("GamepadParameters");
    gamepadparams->SetDoubleAttribute("MaxSpeed",maxspeed);
    gamepadparams->SetDoubleAttribute("MaxTurningRate",maxturningrate);
    rootelem->LinkEndChild(gamepadparams); */

    // write last command issued from "Send Command" dialog
    tnode = rootelem->FirstChild("LastCommand");
    if(tnode)
    {
        rootelem->RemoveChild(tnode);
        tnode = NULL;
    }
    TiXmlElement* lastcommand = new TiXmlElement("LastCommand");
    lastcommand->SetAttribute("CommandString",robots->LastCommand.c_str());
    rootelem->LinkEndChild(lastcommand);

    // write robot names, ports, connection status
    tnode = rootelem->FirstChild("Robots");
    if(tnode)
    {
        rootelem->RemoveChild(tnode);
        tnode = NULL;
    }
    TiXmlElement* botlistelem = new TiXmlElement("Robots");
    rootelem->LinkEndChild(botlistelem);

    TiXmlElement* current_bot;
    for(unsigned int i = 0; i < MT_MAX_NROBOTS; i++)
    {
        current_bot = new TiXmlElement("Robot");
        current_bot->SetAttribute("Name",robots->RobotName[i].c_str());
        current_bot->SetAttribute("PortName",robots->PortName[i].c_str());
        current_bot->SetAttribute("Status",robots->IsPhysical(i));
        botlistelem->LinkEndChild(current_bot);
    }

    return true;

}

bool MT_ReadRobotXML(MT_AllRobotContainer* robots,
                     MT_XMLFile* xmlfile,
                     bool strict_root)
{

    if(!xmlfile->ReadFile())
    {   
        fprintf(stderr, "Could not read Robot XML file %s\n", xmlfile->GetFilename());
        return false;
    }

    if(strict_root)
    {
        if(!xmlfile->HasRootname("RobotConfig"))
        {
            return false;
        }
    }

    MT_RobotBase* cbot;
    for(unsigned int i = 0; i < MT_MAX_NROBOTS; i++)
    {
        cbot = robots->GetRobot(i);
        if(cbot && cbot->GetParameters())
        {
            ReadDataGroupFromXML(*xmlfile, cbot->GetParameters());
        }
    }

/*    TiXmlElement* gamepadparams = xmlfile->FirstChild("GamepadParameters").ToElement();
    if(gamepadparams)
    {
        float val = 0;
        if(gamepadparams->QueryFloatAttribute("MaxSpeed",&val) == TIXML_SUCCESS)
        {
            *maxspeed = val;
        }
        if(gamepadparams->QueryFloatAttribute("MaxTurningRate",&val) == TIXML_SUCCESS)
        {
            *maxturningrate = val;
        }
        }*/

    TiXmlElement* lastcommand = xmlfile->FirstChild("LastCommand").ToElement();
    if(lastcommand)
    {
        const char* commandstring = lastcommand->Attribute("CommandString");
        if(commandstring)
        {
            robots->LastCommand = std::string(commandstring);
        }
    }

    TiXmlElement* botlist = xmlfile->FirstChild("Robots").ToElement();
    TiXmlElement* current_bot;
    if(botlist)
    {      
        const char* cval;
        int bval;
        TiXmlHandle hbotlist(botlist);
        for(unsigned int i = 0; i < MT_MAX_NROBOTS; i++)
        {
            current_bot = hbotlist.Child("Robot",i).ToElement();
            if(!current_bot)
            {
                break;
            }
            cval = current_bot->Attribute("Name");
            if(cval)
            {
                robots->RobotName[i] = std::string(cval);
            }
            cval = current_bot->Attribute("PortName");
            if(cval)
            {
                robots->PortName[i] = std::string(cval);
            }
            if(current_bot->QueryIntAttribute("Status",&bval))
            {
                /// todo
            }
        }
    }

    return true;

}

#endif // MT_USE_XML
