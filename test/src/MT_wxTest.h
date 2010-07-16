#include "wx/defs.h"
#include "wx/app.h"
#include "wx/log.h"

#define WX_CONSOLE_APP_INIT                                                            \
    wxApp::CheckBuildOptions(WX_BUILD_OPTIONS_SIGNATURE, "program");                   \
    wxInitializer initializer;                                                         \
    if ( !initializer )                                                                \
    {                                                                                  \
        wxLogError( wxT( "Failed to initialize the wxWidgets library, aborting." ) );  \
        return -1;                                                                     \
    }
