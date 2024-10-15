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
#include <iostream>
#include <vector>



struct AppData {
    wxString dataFilePath{ "" };
    int currentLineNum{1};
    int totalLines{ 0 };
    std::vector<int> rainbowStyle{0,1,2};
    wxColour backgroundColour{ 50,50,50 };
    wxColour lineIdentifierBackgroundColour{ 50,50,50 };
    wxColour lineIdentifierTextColour{ 250,100,0 };
    wxColour backgroundColourLight{ 100,100,100 };
    wxColour textHighlightColour{ 255,255,250 };
    wxColour textColour{ 225,100,0 };
    wxColour rainbowBraces{ 200,0,200 };
    wxColour rainbowCurlyBraces{ 200,250,0 };
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
    // wxWidget main window variables
    wxTextCtrl* numberField;
    wxScrollBar* numberScroll;
    wxStyledTextCtrl* textField;
    wxTextAttr newStyle;
    wxDialog* visualsPanel;
    wxFont* font;
    wxFont* styledFont;

    // wxWidget option visuals
    bool rainbowTextToggle{ false };
    wxCheckBox* visualsCheckBoxRainbow{};
    wxButton* visualsOkButton{};
    wxButton* visualsCancelButton{};

    // Misc variables
    bool highlightNumberMoveUp{ false };
    bool customizationWnd{ false };
    bool customizationWndRainbow{ false };




    // Misc functions
    void redrawTextCtrlWindow();
    void updateLineNumbers();
    void currentRainbowColourChange();
    void changeFullTextToRainbow();
    int getLineNumber(long currentPos);
    void statusUpdateText(wxString statusText, bool path = true);
    void customRainbowLogic();
    void syncTextAndNumberField();
    void defaultStyleTextField(bool reset = false);
    void changeColourAtChar(int pos, wxColour colour, int vector_index);
    void highlightLineNumber();

    // Event functions
    void OnExit(wxCommandEvent& event);
    void OnFileOpen(wxCommandEvent& event);
    void OnSave(wxCommandEvent& event);
    void OnSaveAs(wxCommandEvent& event);
    void OnTextChanged(wxStyledTextEvent& event);
    void OnCustomOpen(wxCommandEvent& event);
    void OnRemoveNumberFieldFocus(wxFocusEvent& event);
    void OnVisualCancelButtonPressed(wxCommandEvent& event);
    void OnVisualOkButtonPressed(wxCommandEvent& event);

    // old
    void old_RainbowColourSwapFunction(int startPos, wxString currentLineString);
    void old_changeTextFieldColourToRainbow();
};
#endif 

