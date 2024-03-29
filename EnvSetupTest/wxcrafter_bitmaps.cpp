//
// This file was automatically generated by wxrc, do not edit by hand.
//

#include <wx/wxprec.h>

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include <wx/filesys.h>
#include <wx/fs_mem.h>
#include <wx/xrc/xmlres.h>
#include <wx/xrc/xh_all.h>

#if wxCHECK_VERSION(2,8,5) && wxABI_VERSION >= 20805
    #define XRC_ADD_FILE(name, data, size, mime) \
        wxMemoryFSHandler::AddFileWithMimeType(name, data, size, mime)
#else
    #define XRC_ADD_FILE(name, data, size, mime) \
        wxMemoryFSHandler::AddFile(name, data, size)
#endif

static size_t xml_res_size_0 = 339;
static unsigned char xml_res_file_0[] = {
137,80,78,71,13,10,26,10,0,0,0,13,73,72,68,82,0,0,0,16,0,0,0,16,8,6,0,0,
0,31,243,255,97,0,0,0,9,112,72,89,115,0,0,14,195,0,0,14,195,1,199,111,168,
100,0,0,0,25,116,69,88,116,83,111,102,116,119,97,114,101,0,119,119,119,
46,105,110,107,115,99,97,112,101,46,111,114,103,155,238,60,26,0,0,0,224,
73,68,65,84,56,141,157,210,49,74,3,65,20,135,241,159,22,74,186,136,101,
64,140,94,32,98,26,11,11,65,180,204,13,36,149,39,16,15,161,23,88,130,10,
57,129,136,141,85,60,68,180,240,18,162,144,52,42,172,197,206,64,76,178,
236,142,127,120,48,51,188,239,227,61,24,170,51,198,126,141,190,63,57,68,
134,38,186,88,79,21,100,200,113,132,27,124,160,151,34,104,6,120,37,192,
57,6,169,83,196,221,123,1,222,78,21,252,107,247,152,77,197,216,199,56,13,
231,141,101,141,171,37,130,189,0,189,224,45,188,117,82,38,24,98,52,115,
31,227,174,46,188,131,111,156,204,188,245,241,133,118,29,193,131,98,252,
88,79,88,83,172,114,95,5,247,231,224,88,231,56,192,15,206,202,224,22,222,
75,4,19,236,226,74,241,177,182,150,9,30,75,224,88,207,104,224,53,244,46,
228,179,66,144,227,26,183,161,119,33,151,152,214,144,76,113,17,161,95,217,
214,76,46,111,79,16,59,0,0,0,0,73,69,78,68,174,66,96,130};

static size_t xml_res_size_1 = 249;
static unsigned char xml_res_file_1[] = {
60,63,120,109,108,32,118,101,114,115,105,111,110,61,34,49,46,48,34,32,101,
110,99,111,100,105,110,103,61,34,85,84,70,45,56,34,63,62,10,60,114,101,
115,111,117,114,99,101,32,120,109,108,110,115,61,34,104,116,116,112,58,
47,47,119,119,119,46,119,120,119,105,100,103,101,116,115,46,111,114,103,
47,119,120,120,114,99,34,62,10,32,32,60,33,45,45,32,72,97,110,100,108,101,
114,32,71,101,110,101,114,97,116,105,111,110,32,105,115,32,79,78,32,45,
45,62,10,32,32,60,111,98,106,101,99,116,32,99,108,97,115,115,61,34,119,
120,66,105,116,109,97,112,34,32,110,97,109,101,61,34,67,110,99,84,101,115,
116,34,62,119,120,99,114,97,102,116,101,114,95,98,105,116,109,97,112,115,
46,99,112,112,36,46,46,95,67,110,99,67,111,110,116,114,111,108,101,114,
71,117,105,95,98,105,116,109,97,112,115,95,67,110,99,84,101,115,116,46,
112,110,103,60,47,111,98,106,101,99,116,62,10,60,47,114,101,115,111,117,
114,99,101,62,10};

void wxC9ED9InitBitmapResources()
{

    // Check for memory FS. If not present, load the handler:
    {
        wxMemoryFSHandler::AddFile(wxT("XRC_resource/dummy_file"), wxT("dummy one"));
        wxFileSystem fsys;
        wxFSFile *f = fsys.OpenFile(wxT("memory:XRC_resource/dummy_file"));
        wxMemoryFSHandler::RemoveFile(wxT("XRC_resource/dummy_file"));
        if (f) delete f;
        else wxFileSystem::AddHandler(new wxMemoryFSHandlerBase);
    }

    XRC_ADD_FILE(wxT("XRC_resource/wxcrafter_bitmaps.cpp$.._CncControlerGui_bitmaps_CncTest.png"), xml_res_file_0, xml_res_size_0, wxT("image/png"));
    XRC_ADD_FILE(wxT("XRC_resource/wxcrafter_bitmaps.cpp$C__@Development_@Projekte_c++_CNCGuiController_EnvSetupTest_wxcrafter_bitmaps.xrc"), xml_res_file_1, xml_res_size_1, wxT("text/xml"));
    wxXmlResource::Get()->Load(wxT("memory:XRC_resource/wxcrafter_bitmaps.cpp$C__@Development_@Projekte_c++_CNCGuiController_EnvSetupTest_wxcrafter_bitmaps.xrc"));
}
