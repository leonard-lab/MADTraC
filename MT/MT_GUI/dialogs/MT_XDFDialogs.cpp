#include "MT_XDFDialogs.h"

enum
{
    ID_X_CHOICE = wxID_HIGHEST + 1,
    ID_Y_CHOICE,
    ID_TIMESTEP
};

MT_LoadXDFDialog::MT_LoadXDFDialog(wxWindow* parent,
                                   MT_ExperimentDataFile* pXDF)
    : wxDialog(parent,
               wxID_ANY,
               wxT("XDF Settings"),
               wxDefaultPosition,
               wxDefaultSize,
               wxDEFAULT_DIALOG_STYLE),
      m_pXDF(pXDF)
{
    /* gather relevant information from XDF */
    std::vector<std::string> datanames;
    std::vector<std::string> datafiles;
    m_pXDF->getFilesFromXML(&datanames, &datafiles);

    unsigned int x_guess = 0;
    unsigned int y_guess = 0;
    unsigned int frame_period_guess = 32;
    for(unsigned int i = 0; i < datanames.size(); i++)
    {
        wxString cname(MT_StringToWxString(datanames[i].c_str()));
        m_asXChoices.Add(cname);
        m_asYChoices.Add(cname);
        if(cname.Find(wxT("X")) != wxNOT_FOUND ||
           cname.Find(wxT("x")) != wxNOT_FOUND)
        {
            x_guess = i;
        }
        if(cname.Find(wxT("Y")) != wxNOT_FOUND ||
           cname.Find(wxT("y")) != wxNOT_FOUND)
        {
            y_guess = i;
        }
    }

    MT_XDFSettingsGroup* pDG = m_pXDF->getSettingsGroup();
    m_pDG = pDG;

    if(pDG && (pDG->GetGroupName() == MT_XDFSettingsGroup::SettingsName))
    {
        unsigned int i = pDG->GetIndexByName(MT_XDFSettingsGroup::XPlaybackName);
        if(i >= 0 && i < pDG->GetGroupSize())
        {
            wxString X_Found(MT_StringToWxString(pDG->GetStringValue(i).c_str()));
            int j = m_asXChoices.Index(X_Found);
            if(j != wxNOT_FOUND)
            {
                x_guess = j;
            }
        }
        i = pDG->GetIndexByName(MT_XDFSettingsGroup::YPlaybackName);
        if(i >= 0 && i < pDG->GetGroupSize())
        {
            wxString Y_Found(MT_StringToWxString(pDG->GetStringValue(i).c_str()));
            int j = m_asXChoices.Index(Y_Found);
            if(j != wxNOT_FOUND)
            {
                y_guess = j;
            }
        }
        i = pDG->GetIndexByName(MT_XDFSettingsGroup::PlaybackFramePeriodName);
        if(i >= 0 && i < pDG->GetGroupSize())
        {
            int v = (int) pDG->GetNumericValue(i);
            if(v > 0)
            {
                frame_period_guess = v;
            }
        }
    }
    if(x_guess == y_guess)
    {
        y_guess = x_guess + 1;
    }

    /* GUI setup */
    wxBoxSizer* vbox0 = new wxBoxSizer(wxVERTICAL);

    wxBoxSizer* hbox0 = new wxBoxSizer(wxHORIZONTAL);

    wxBoxSizer* vbox1 = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* vbox2 = new wxBoxSizer(wxVERTICAL);

    vbox1->Add(new wxStaticText(this, wxID_ANY, wxT("X Data")),
               0, wxLEFT | wxRIGHT, 10);

    m_pXChoice = new wxChoice(this,
                              ID_X_CHOICE,
                              wxDefaultPosition,
                              wxSize(150, 20),
                              m_asXChoices);
    vbox1->Add(m_pXChoice, 0, wxLEFT | wxRIGHT, 10);

    vbox2->Add(new wxStaticText(this, wxID_ANY, wxT("Y Data")),
               0, wxLEFT | wxRIGHT, 10);

    m_pYChoice = new wxChoice(this,
                              ID_Y_CHOICE,
                              wxDefaultPosition,
                              wxSize(150, 20),
                              m_asYChoices);
    vbox2->Add(m_pYChoice, 0, wxLEFT | wxRIGHT, 10);    

    hbox0->Add(vbox1);
    hbox0->Add(vbox2);
    vbox0->Add(hbox0, 0, wxTOP, 10);
    
    wxBoxSizer* hbox1 = new wxBoxSizer(wxHORIZONTAL);

    hbox1->Add(new wxStaticText(this, wxID_ANY, wxT("Time Step [msec]")),
               0, wxLEFT | wxRIGHT, 10);
    wxString sDt;
    sDt.Printf(wxT("%d"), frame_period_guess);
    m_pTimeStepCtrl = new wxTextCtrl(this,
                                     ID_TIMESTEP,
                                     sDt,
                                     wxDefaultPosition,
                                     wxSize(100, 20));
    hbox1->Add(m_pTimeStepCtrl, 0, wxLEFT | wxRIGHT | wxALIGN_RIGHT, 10);

    vbox0->Add(hbox1, 0, wxLEFT | wxRIGHT | wxTOP | wxALIGN_RIGHT, 10);

    vbox0->Add(CreateButtonSizer(wxOK | wxCANCEL), 0, wxALL | wxALIGN_RIGHT, 10);

    SetSizerAndFit(vbox0);

    Connect(wxID_OK,
            wxEVT_COMMAND_BUTTON_CLICKED,
            wxCommandEventHandler(MT_LoadXDFDialog::onOKClicked));
    Connect(ID_TIMESTEP,
            wxEVT_COMMAND_TEXT_UPDATED,
            wxCommandEventHandler(MT_LoadXDFDialog::onTSChanged));

    m_pXChoice->SetSelection(x_guess);
    m_pYChoice->SetSelection(y_guess);
}

void MT_LoadXDFDialog::onTSChanged(wxCommandEvent& event)
{
    if(!(m_pTimeStepCtrl->IsModified()))
    {
        return;
    }
    MT_ValidateTextCtrlInteger(m_pTimeStepCtrl);
}

void MT_LoadXDFDialog::onOKClicked(wxCommandEvent& event)
{

    std::string X_sel(m_pXChoice->GetStringSelection().mb_str());
    std::string Y_sel(m_pYChoice->GetStringSelection().mb_str());
    std::string FR(m_pTimeStepCtrl->GetValue().mb_str());

    m_pDG->SetStringValueByName(MT_XDFSettingsGroup::XPlaybackName,
                                X_sel);
    m_pDG->SetStringValueByName(MT_XDFSettingsGroup::YPlaybackName,
                                Y_sel);
    m_pDG->SetStringValueByName(MT_XDFSettingsGroup::PlaybackFramePeriodName,
                                FR);

    if(IsModal())
    {
        EndModal(wxID_OK);
    }
    else
    {
        Close();
    }
}

void MT_LoadXDFDialog::getInfo(std::string* x_name,
                               std::string* y_name,
                               unsigned int* frame_period_msec)
{
    *x_name = std::string(m_pXChoice->GetStringSelection().mb_str());
    *y_name = std::string(m_pYChoice->GetStringSelection().mb_str());
    *frame_period_msec = MT_ClampTextCtrlInt(m_pTimeStepCtrl,
                                             0,
                                             MT_max_int);
    
}
