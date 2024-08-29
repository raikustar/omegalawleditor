#ifndef OMEGALAWL_HEADER_M
#define OMEGALAWL_HEADER_M

#include <wx/wx.h>
#include <wx/textfile.h>
#include <wx/stc/stc.h>
#include <wx/wfstream.h>
#include <wx/file.h>
#include <wx/richtext/richtextbuffer.h>

class MyFrame : public wxFrame
{
public:
    MyFrame();

private:
    wxTextCtrl* textField{};
    void OnExit(wxCommandEvent&);
    void OnFileOpen(wxCommandEvent&);
};
#endif 

