/* note this isn't really a "test" app, we're just hijacking
 * the MT_TestFrame class b/c it servers our purposes: It pops
 * up a frame that is off-screen.  We need this for proper
 * support of threads that are possibly present in MT_Capture. */
#include "MT_Test.h"
#include "MT_TestApp.h"

#include "MT_Core.h"
#include "MT_Tracking.h"

#include <wx/cmdline.h>

class BGC_frame : public MT_TestFrame
{
public:
    void doTest(int argc, wxChar** argv);
};

IMPLEMENT_MT_TESTAPP(BGC_frame);

void BGC_frame::doTest(int argc, wxChar** argv)
{
    wxCmdLineParser clp;
    clp.SetSwitchChars(wxT("-"));

    clp.SetLogo(
        wxT("BackgroundCreator - create a background frame from a video "
            "by averaging randomly selected frames.")
        );
    clp.AddSwitch(wxT("h"),
                  wxT("help"),
                  wxT("Display help message."),
                  wxCMD_LINE_OPTION_HELP);


    clp.AddParam(wxT("Input video file."),
                 wxCMD_LINE_VAL_STRING,
                 wxCMD_LINE_OPTION_MANDATORY);

    clp.AddOption(wxT("f"),
                  wxT("frames"),
                  wxT("Frame range or start frame.  Examples: "
                      "-f 100, -f 100:300, -f 100:, -f :300"
                      "  Default is all frames."),
                  wxCMD_LINE_VAL_STRING);
    clp.AddOption(wxT("n"),
                  wxT("navg"),
                  wxT("Number of frames to average.  Default 100."),
                  wxCMD_LINE_VAL_NUMBER);
    clp.AddOption(wxT("o"),
                  wxT("output"),
                  wxT("Output file name.  Default is ./background.bmp"),
                  wxCMD_LINE_VAL_STRING);
    clp.AddSwitch(wxT("F"),
                  wxT("Force"),
                  wxT("Force over-write of existing output files."));
    
    clp.SetCmdLine(wxTheApp->argc, wxTheApp->argv);
    if(clp.Parse() != 0)
    {
        Close();
        return;  /* avoids the rest of the function from executing
                    before close can be successful. */
    }

    wxString infile = clp.GetParam(0);


    unsigned int n_avg = 100;
    wxString output_file_name("./background.bmp");

    long temp;
    if(clp.Found(wxT("n"), &temp))
    {
        n_avg = temp;
    }
    
    wxString tstr;
    if(clp.Found(wxT("o"), &tstr))
    {
        output_file_name = tstr;
    }

    wxString r;
    unsigned int first_frame = MT_MAKEBG_FIRST_FRAME;
    unsigned int last_frame = MT_MAKEBG_LAST_FRAME;
    if(clp.Found(wxT("f"), &r))
    {
        wxString b = r.BeforeFirst(wxT(':'));
        wxString a = r.AfterFirst(wxT(':'));

        long s1 = 0;
        long s2 = 0;

        bool found_s1 = b.ToLong(&s1);
        bool found_s2 = a.ToLong(&s2);

        if(s1 >= s2 && a != wxEmptyString)
        {
            fprintf(stderr,
                    "BackgroundCreator Error:  Range error.\n");
            clp.Usage();
            Close();
            return;
        }
        first_frame = s1;
        last_frame = s2;

    }

    bool force_overwrite = clp.Found(wxT("F"));
    
    MT_Capture cap;
    
    if(!cap.initCaptureFromFile(infile.mb_str()))
    {
        fprintf(stderr,
                "BackgroundCreator Error:  Failed to initialize capture "
                "from file %s\n.",
                infile.mb_str());
        Close();
        return;
    }

    /* note calling getFrame also makes sure that e.g. getNChannels
     * will return the correct value. */
    if(!cap.getFrame())
    {
        fprintf(stderr,
                "BackgroundCreator Error:  Could not acquire frame "
                "from file %s\n.",
                infile.mb_str());
        Close();
        return;
    }

    if(!force_overwrite && MT_FileIsAvailable(output_file_name.mb_str(), "r"))
    {
        fprintf(stderr,
                "BackgroundCreator Error:  File %s exists.  "
                "Use -F to force overwrite.\n",
                output_file_name.mb_str());
        Close();
        return;
    }
    if(!MT_FileIsAvailable(output_file_name.mb_str(), "a"))
    {
        fprintf(stderr,
                "BackgroundCreator Error:  Cannot write to output "
                "file %s\n",
                output_file_name.mb_str());
        Close();
        return;
    }

    IplImage* bg = cvCreateImage(cap.getFrameSize(),
                                 IPL_DEPTH_8U,
                                 cap.getNChannels());

    MT_BackgroundFrameCreator BGFC(bg,
                                   &cap,
                                   MT_MAKEBG_MODE_STEP,
                                   n_avg,
                                   MT_MAKEBG_NO_INPAINT,
                                   0,
                                   MT_MAKEBG_WHOLE_IMAGE,
                                   first_frame,
                                   last_frame);

    if(BGFC.GetStatus() != MT_MAKEBG_OK)
    {
        fprintf(stderr,
                "BackgroundCreator Error:  Failure creating "
                "MT_BackgroundFrameCreator.\n");
        Close();
        return;
    }

    printf("Creating frame\n[");
    for(unsigned int i = 0; i < n_avg; i++)
    {
        BGFC.DoStep();
        if(!(i % (n_avg / 100)))
        {
            printf("|");
            fflush(stdout);
        }
    }
    printf("]\n");

    if(BGFC.GetStatus() != MT_MAKEBG_OK)
    {
        fprintf(stderr,
                "BackgroundCreator Error:  Failure creating "
                "MT_BackgroundFrameCreator.\n");
        Close();
        return;
    }

    BGFC.Finish();

    cvSaveImage(output_file_name.mb_str(), bg);

    cvReleaseImage(&bg);

    Close();
}
