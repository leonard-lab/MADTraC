/*
 *  MT_MovieExporterDialog.cpp
 *
 *  Created by Daniel Swain on 12/20/09.
 *
 */

#include <wx/filename.h>

#include "MT_MovieExporterDialog.h"

enum
{
    ID_PATH = wxID_HIGHEST + 1,
    ID_RADIO,
    ID_NUMTOSKIP,
    ID_FPS 
};

static const int N_CHOICES = 2;
static const wxString CHOICES[] = {wxT("CvVideoWriter (Uncompressed AVI)"), wxT("Image Sequence")};

static const wxSize BOX_SIZE = wxSize(400,20);

MT_MovieExporterDialog::MT_MovieExporterDialog(const wxString& directory,
                                               MT_ME_Type method,
                                               int frame_width,
                                               int frame_height,
                                               int num_to_skip,
                                               double FPS,
                                               wxWindow* parent,
                                               wxWindowID id)
  : wxDialog(parent, 
             id, 
             wxT("Movie Parameters..."), 
             wxDefaultPosition, 
             wxDefaultSize, 
             wxDEFAULT_DIALOG_STYLE),
    m_sDirectory(directory),
    m_sPath(directory),
    m_iFrameWidth(frame_width),
    m_iFrameHeight(frame_height),
    m_METype(method)
{
    wxBoxSizer* vbox = new wxBoxSizer(wxVERTICAL);

    wxString tmp;

    m_pPathDescriptor = new wxStaticText(this, wxID_ANY, wxT("Filename / Format"));
    vbox->Add(m_pPathDescriptor, 0, wxALL | wxALIGN_LEFT, 10);
    m_pPathCtrl = new wxTextCtrl(this, wxID_ANY, directory, wxDefaultPosition, BOX_SIZE);
    vbox->Add(m_pPathCtrl, 0, wxLEFT | wxRIGHT, 10);
    vbox->Add(new wxButton(this, ID_PATH, wxT("Select...")), 0, wxALL | wxALIGN_RIGHT, 10);
    Connect(ID_PATH,
            wxEVT_COMMAND_BUTTON_CLICKED,
            wxCommandEventHandler(MT_MovieExporterDialog::onPathSelect));

    m_pRadioBox = new wxRadioBox(this, 
                                 ID_RADIO, 
                                 wxT("Method"), 
                                 wxDefaultPosition,
                                 wxDefaultSize,
                                 N_CHOICES,
                                 CHOICES,
                                 2,
                                 wxRA_SPECIFY_ROWS);
    vbox->Add(m_pRadioBox, 0, wxALL, 10);
    Connect(ID_RADIO,
            wxEVT_COMMAND_RADIOBOX_SELECTED,
            wxCommandEventHandler(MT_MovieExporterDialog::onRadioBoxClicked));
    switch(m_METype)
    {
    case MT_ME_CV_VIDEO_WRITER:
        m_pRadioBox->SetSelection(0);
        m_pPathCtrl->SetValue(directory + 
                              wxFileName::GetPathSeparator() + wxT("movie.avi"));
        break;
    case MT_ME_IMAGE_SEQUENCE:
        m_pRadioBox->SetSelection(1);
        break;
    default:
        EndModal(wxID_CANCEL);
        break;
    }

    tmp.Printf(wxT("%d"), num_to_skip);
    vbox->Add(new wxStaticText(this, wxID_ANY, wxT("Number of Frames to Skip")), 0, wxLEFT | wxTOP | wxRIGHT | wxALIGN_LEFT, 10);
    m_pNumToSkipCtrl = new wxTextCtrl(this, ID_NUMTOSKIP, tmp, wxDefaultPosition, BOX_SIZE);
    vbox->Add(m_pNumToSkipCtrl, 0, wxALL, 10);
    Connect(ID_NUMTOSKIP,
            wxEVT_COMMAND_TEXT_UPDATED,
            wxCommandEventHandler(MT_MovieExporterDialog::onNumToSkipChange));

    tmp.Printf(wxT("%f"), FPS);
    vbox->Add(new wxStaticText(this, wxID_ANY, wxT("Frames Per Second")), 0, wxLEFT | wxTOP | wxRIGHT | wxALIGN_LEFT, 10);
    m_pFPSCtrl = new wxTextCtrl(this, ID_FPS, tmp, wxDefaultPosition, BOX_SIZE);
    vbox->Add(m_pFPSCtrl, 0, wxALL, 10);
    Connect(ID_FPS,
            wxEVT_COMMAND_TEXT_UPDATED,
            wxCommandEventHandler(MT_MovieExporterDialog::onFPSChange));
  
    vbox->Add(CreateButtonSizer(wxOK | wxCANCEL), 0, wxALL | wxALIGN_RIGHT, 10);

    SetSizerAndFit(vbox);

    wxCommandEvent dummy_event;
    onRadioBoxClicked(dummy_event);

}

void MT_MovieExporterDialog::onPathSelect(wxCommandEvent& event)
{
    wxString path = m_pPathCtrl->GetValue();
    wxString dir;
    int result;

    switch(m_METype)
    {
    case MT_ME_CV_VIDEO_WRITER:
        result = MT_SaveFileDialog(this,
                                path,
                                wxT("Select Movie Output File"),
                                MT_FILTER_MOVIE_FILES,
                                &path,
                                &dir);
        break;
    case MT_ME_IMAGE_SEQUENCE:
        result = MT_SelectDirectoryDialog(this,
                                       path,
                                       wxT("Select Directory for Image Sequence"),
                                       &path);
        m_pPathCtrl->SetValue(path);
        break;
    default:
        result = wxID_CANCEL;
        break;
    }

    if(result == wxID_OK)
    {
        m_pPathCtrl->SetValue(path);
    }
}

void MT_MovieExporterDialog::onNumToSkipChange(wxCommandEvent& event)
{
    if(!(m_pNumToSkipCtrl->IsModified()))
    {
        return;
    }
  
    MT_ValidateTextCtrlInteger(m_pNumToSkipCtrl);
}

void MT_MovieExporterDialog::onFPSChange(wxCommandEvent& event)
{
    if(!(m_pFPSCtrl->IsModified()))
    {
        return;
    }
  
    MT_ValidateTextCtrlNumeric(m_pFPSCtrl);
}

void MT_MovieExporterDialog::onRadioBoxClicked(wxCommandEvent& event)
{
    int sel = m_pRadioBox->GetSelection();
    if(sel == 0)
    {
        m_METype = MT_ME_CV_VIDEO_WRITER;
        m_pPathDescriptor->SetLabel(wxT("Movie filename (avi or mov):"));
    }
    else if(sel == 1)
    {
        m_METype = MT_ME_IMAGE_SEQUENCE;
        m_pPathDescriptor->SetLabel(wxT("Directory and name format for frames (jpg, png, or bmp):"));
    }
    else
    {
        m_METype = MT_ME_NONE;
    }
}

void MT_MovieExporterDialog::getInfo(wxString* filename, wxString* directory, MT_ME_Type* method, int* num_to_skip, double* FPS)
{
    *filename = m_pPathCtrl->GetValue();
    *directory = wxPathOnly(*filename);
    *method = m_METype;
    long tmp;
    m_pNumToSkipCtrl->GetValue().ToLong(&tmp);
    *num_to_skip = tmp;
    m_pFPSCtrl->GetValue().ToDouble(FPS);
}

void MT_MovieExporterDialog::getInfo(MT_MovieExporter* exporter, wxString* directory)
{
    wxString filename;
    MT_ME_Type method;
    int num_to_skip;
    double fps;
    getInfo(&filename, directory, &method, &num_to_skip, &fps);
    wxString ext;
    wxString dir;
    wxString name;
    wxFileName::SplitPath(filename, NULL, &dir, &name, &ext);
    switch(method)
    {
    case MT_ME_CV_VIDEO_WRITER:
        exporter->initForCvVideoWriter(filename.mb_str(),
                                       fps,
                                       m_iFrameWidth,
                                       m_iFrameHeight,
                                       num_to_skip);
        break;
    case MT_ME_IMAGE_SEQUENCE:
        filename = name + wxT(".") + ext;
        exporter->initForImageSequence(dir.mb_str(),
                                       filename.mb_str(),
                                       num_to_skip);
        break;
    default:
        break;
    }
}
