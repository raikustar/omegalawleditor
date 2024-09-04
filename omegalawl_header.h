#ifndef OMEGALAWL_HEADER_M
#define OMEGALAWL_HEADER_M

#include <wx/wx.h>
#include <wx/textfile.h>
#include <wx/stc/stc.h>
#include <wx/wfstream.h>
#include <wx/file.h>
#include <wx/richtext/richtextbuffer.h>
#include <wx/scrolbar.h>
#include <wx/filefn.h> 


struct AppData {
    
    wxString dataFilePath{ "" };
    wxColour backgroundColour{ 50,50,50 };
    wxColour lineIdentifierBackgroundColour{ 50,50,50 };
    wxColour lineIdentifierTextColour{ 250,100,0 };
    wxColour backgroundColourLight{ 100,100,100 };
    wxColour textColour{ 225,100,0 };
};


class MyApp : public wxApp
{
public:
    bool OnInit() override;

};

class MyFrame : public wxFrame
{
public:
    MyFrame();

private:
    wxTextCtrl* numberField;
    wxScrollBar* numberScroll;
    wxTextCtrl* textField;
    wxTextAttr newStyle;

    void redrawTextCtrlWindow();
    void updateLineNumbers();
    void updateScrollPosition();
    int getLineNumber(long);
    void statusUpdateText(wxString, bool path = true);

    void OnExit(wxCommandEvent& event);
    void OnFileOpen(wxCommandEvent& event);
    void OnSave(wxCommandEvent& event);
    void OnSaveAs(wxCommandEvent& event);
    void OnTextChanged(wxCommandEvent& event);
    void OnRemoveNumberFieldFocus(wxFocusEvent& event);
    void OnScrollUpdate(wxCommandEvent& event);
};
#endif 

