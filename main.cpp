//wxWidget header
#include "omegalawl_header.h"
// other includes

/*
    - Properly follow current line, and sync up textField with numberField
    - Highlight current active line
    - Make into executable

*/

// App colours and other visual misc..
AppData app_data{};

wxIMPLEMENT_APP(MyApp);

bool MyApp::OnInit()
{
    MyFrame* frame = new MyFrame();
    frame->Show(true);
    return true;
}

enum {
    CUSTOM_OPTION = 10
};
MyFrame::MyFrame()
    : wxFrame(nullptr, wxID_ANY, "Omegalawl Editor", wxDefaultPosition, wxSize(1000,800), wxDEFAULT_FRAME_STYLE)
{
    wxFont* font = new wxFont(14, wxFONTFAMILY_SCRIPT, wxFONTSTYLE_SLANT, wxFONTWEIGHT_HEAVY, false);

    this->SetBackgroundColour(app_data.backgroundColour);

    // Menu
    wxMenu* menuFile = new wxMenu{};
    menuFile->Append(wxID_OPEN, "Open", "Open a file");
    menuFile->Append(wxID_SAVE, "Save\tCtrl-S", "Quick save");
    menuFile->Append(wxID_SAVEAS, "Save as\tCtrl-Shift-S", "Save file as ..");
    menuFile->AppendSeparator();
    menuFile->Append(wxID_EXIT, "Exit\tAlt-X", "Exit the program");

    wxMenu* menuOptions = new wxMenu{};
    menuOptions->Append(CUSTOM_OPTION, "Visuals", "Options - Visuals");

    wxMenuBar* menuBar = new wxMenuBar{};
    menuBar->Append(menuFile, "File");
    menuBar->Append(menuOptions, "Options");
    SetMenuBar(menuBar);

    // After menu window for all other boxes to be organized 
    wxBoxSizer* mainBoxSizer = new wxBoxSizer(wxHORIZONTAL);

    // Numbers for lines of text
    numberField = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxSize(60, 350), wxTE_MULTILINE | wxTE_RICH | wxTE_CENTER | wxTE_READONLY | wxTE_NOHIDESEL | wxTE_DONTWRAP | wxTE_NO_VSCROLL);

    numberField->SetFont(*font);
    numberField->SetForegroundColour(app_data.lineIdentifierTextColour);
    numberField->SetBackgroundColour(app_data.lineIdentifierBackgroundColour);
    numberField->Refresh();
    numberField->Update();

    // Notepad 
    textField = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_RICH | wxUSE_SCROLLBAR | wxTE_DONTWRAP);

    // Visual colours
    newStyle.SetFont(*font);
    newStyle.SetBackgroundColour(app_data.backgroundColourLight);
    newStyle.SetTextColour(app_data.textColour);
    
    redrawTextCtrlWindow();
    updateLineNumbers();

    mainBoxSizer->Add(numberField, 0, wxALL | wxEXPAND, 0);
    mainBoxSizer->Add(textField, 1, wxALL | wxEXPAND, 0);

    this->SetSizer(mainBoxSizer);
    this->SetInitialSize(wxSize(800, 500));

    CreateStatusBar();
    SetStatusText(app_data.dataFilePath); 

    textField->Bind(wxEVT_TEXT, &MyFrame::OnTextChanged, this);
    textField->Bind(wxEVT_KEY_DOWN, &MyFrame::OnKeyUpdate, this);
    numberField->Bind(wxEVT_KILL_FOCUS, &MyFrame::OnRemoveNumberFieldFocus, this);

    Bind(wxEVT_MENU, &MyFrame::OnCustomOpen, this, CUSTOM_OPTION);
    Bind(wxEVT_MENU, &MyFrame::OnExit, this, wxID_EXIT);
    Bind(wxEVT_MENU, &MyFrame::OnFileOpen, this, wxID_OPEN);
    Bind(wxEVT_MENU, &MyFrame::OnSave, this, wxID_SAVE);
    Bind(wxEVT_MENU, &MyFrame::OnSaveAs, this, wxID_SAVEAS);
    
}

// Menu member functions

void MyFrame::OnExit(wxCommandEvent& event)
{
    Close(true);
}

void MyFrame::OnFileOpen(wxCommandEvent& event)
{
    wxFileDialog fileDialog(this, _("Open"), wxEmptyString, wxEmptyString, "All files (*.*)|*.*|Text files (*.txt)|*.txt|Python (*.py)|*.py", wxFD_OPEN || wxFD_FILE_MUST_EXIST);

    if (fileDialog.ShowModal() == wxID_CANCEL) 
        return;
    
    wxString filePath = fileDialog.GetPath();
    app_data.dataFilePath = filePath;
    wxFile openFile(filePath);
    
    if (!openFile.IsOpened()) {
        wxMessageBox("Couldn't open the file", "Error", wxOK);
        return;
    }

    wxString fileContent{};
    openFile.ReadAll(&fileContent);
    
    textField->SetValue(fileContent);

    redrawTextCtrlWindow();
    statusUpdateText("Opened file from -");
    
}

void MyFrame::OnSaveAs(wxCommandEvent& event) 
{
    wxFileDialog saveFileDialog(this, "Save file as", wxEmptyString, wxEmptyString, "All files (*.*)|*.*|Text files (*.txt)|*.txt|Python (*.py)|*.py", wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
    if (saveFileDialog.ShowModal() == wxID_CANCEL) {
        return;
    }
    wxFileOutputStream output_stream(saveFileDialog.GetPath());
    wxString content{};
    if (!output_stream.IsOk())
    {
        wxLogError("Cannot save contents in file '%s'.", saveFileDialog.GetPath());
        return;
    }
    else {
        app_data.dataFilePath = saveFileDialog.GetPath();
        content = textField->GetValue();
        output_stream.Write(content.mb_str(), content.length());
        output_stream.Sync();
        statusUpdateText("Saved file to -");
    }
}

void MyFrame::OnSave(wxCommandEvent& event)
{
    wxString fileContent{};
    if (app_data.dataFilePath == "") {
        MyFrame::OnSaveAs(event);
    } else {
        wxFileOutputStream output_stream(app_data.dataFilePath);
        fileContent = textField->GetValue();
        output_stream.Write(fileContent.mb_str(), fileContent.length());
        output_stream.Sync();
        statusUpdateText("Quick save to file -");
    }
}

void MyFrame::OnTextChanged(wxCommandEvent& event)
{
    updateLineNumbers();
    redrawTextCtrlWindow();
    statusUpdateText("Unsaved changes..", false);
    event.Skip();
}

void MyFrame::OnScrollUpdate(wxCommandEvent& event)
{
    updateScrollPosition();
    event.Skip();
}

void MyFrame::OnKeyUpdate(wxKeyEvent& event)
{
    updateScrollPosition();
    event.Skip();
}

void MyFrame::OnRemoveNumberFieldFocus(wxFocusEvent& event) {
    event.Skip();
    // remove focus from number lines
    numberField->DisableFocusFromKeyboard();
}

void MyFrame::OnCustomOpen(wxCommandEvent& event) {
    /* 
        Open new wxDialog window for options
    */

    visualsPanel = new wxDialog(this, wxID_ANY, "Options - Visuals", wxDefaultPosition, wxSize(300, 300), wxDEFAULT_DIALOG_STYLE | wxSTAY_ON_TOP);
    visualsCheckBoxRainbow = new wxCheckBox(visualsPanel, wxID_ANY, "Toggle rainbow text (Coding)", wxPoint(50,50), wxDefaultSize, wxALIGN_RIGHT);
    visualsCheckBoxRainbow->SetValue(rainbowTextToggle);
    visualsOkButton = new wxButton(visualsPanel, wxID_ANY, "Ok", wxPoint(65,220), wxSize(70, 30));
    visualsCancelButton = new wxButton(visualsPanel, wxID_ANY, "Cancel", wxPoint(160, 220), wxSize(70, 30));

    visualsCancelButton->Bind(wxEVT_BUTTON, &MyFrame::OnVisualCancelButtonPressed, this);
    visualsOkButton->Bind(wxEVT_BUTTON, &MyFrame::OnVisualOkButtonPressed, this);

    visualsPanel->Show();    
}

void MyFrame::OnVisualCancelButtonPressed(wxCommandEvent& event) {
    if (visualsPanel) {
        visualsPanel->Close();
        visualsPanel = nullptr;
    } 
}

void MyFrame::OnVisualOkButtonPressed(wxCommandEvent& event) {
    if (visualsPanel) {
        customRainbowLogic();

        visualsPanel->Close();
        visualsPanel = nullptr;
    }
}

// Misc member functions
void MyFrame::redrawTextCtrlWindow()
{
    textField->SetFocus();
    textField->SetBackgroundColour(newStyle.GetBackgroundColour());
    textField->SetFont(newStyle.GetFont());
    if (rainbowTextToggle) {changeTextFieldColourToRainbow();}
    else { textField->SetDefaultStyle(newStyle); }
    textField->Refresh();
    textField->Update();
    
};

void MyFrame::updateLineNumbers() 
{
    /*
        Get how many lines are in the wxTextCtrl object
        Add to string how many lines there are with for loop
        Set numbers to numberField object
    */
    int textFieldLines = textField->GetNumberOfLines();
    wxString lineNumberString{};

    for (int i = 1; i <= textFieldLines; i++) 
    {
        lineNumberString += wxString::Format("%d\n", i); 
    }

    numberField->SetLabel(lineNumberString);
    
}

void MyFrame::updateScrollPosition() 
{
    int scrollPos = textField->GetScrollPos(wxVERTICAL);
    //int numField = numberField->GetNumberOfLines(), txtField = textField->GetNumberOfLines() + 1;


/*
Currently it changes line on the left instantly
Create an offset or point where it follows to a new line and when it doesn't

i.e. move up from the last line of the window, 
and only start updating to new lines if I reach at the top of the current window(highest line per window)
        
   
*/

    // Get textField current line
    int pos = textField->GetInsertionPoint();
    int lineNum = getLineNumber(pos);
    app_data.currentLineNum = lineNum;

    numberField->SetScrollPos(wxVERTICAL, lineNum, true);
    numberField->ShowPosition(lineNum);
    numberField->Refresh();
    numberField->Update();
    
}

int MyFrame::getLineNumber(long pos) {
    long col{}, line{};
    textField->PositionToXY(pos, &col, &line);
    return line + 1;
}

void MyFrame::statusUpdateText(wxString statusText, bool path) {
    wxString status{};
    if (path) {
        status = wxString::Format(statusText + " " + app_data.dataFilePath);
    }
    else {
        status = statusText;
    }
    SetStatusText(status);
}

void MyFrame::customRainbowLogic() {
    if (visualsCheckBoxRainbow->IsChecked()) {
        statusUpdateText("Enabled Rainbow Text", false);
        rainbowTextToggle = true;
        wxString fullText = textField->GetValue();
        RainbowColourSwapFunction(0, fullText);
    }
    else {
        statusUpdateText("Disabled Rainbow Text", false);
        rainbowTextToggle = false;
        textField->SetStyle(0, textField->GetLastPosition(), newStyle);
    }
}

void MyFrame::changeTextFieldColourToRainbow() {
    /*
        rainbow colors for specific characters / symbols to improve readability of code.

         - Future improvement, maybe use getInsertion to update singular character per line of string, not entire string:
            if string[i-1] == '(' -> setstyle(i-1, i, colour)
    */ 
    int currentLine = app_data.currentLineNum - 1;
   
    int currentLineStart = textField->XYToPosition(0, currentLine);
    int currentLineEnd = textField->XYToPosition(textField->GetLineLength(currentLine), currentLine);
    wxString currentLineString = textField->GetRange(currentLineStart, currentLineEnd);

    if (currentLineString.IsEmpty()) {
        return;
    }
    RainbowColourSwapFunction(currentLineStart,currentLineString);
}

void MyFrame::RainbowColourSwapFunction(int startPos, wxString currentLineString) {
    for (int i = 0; i < currentLineString.length(); i++) {
        char currentChar = currentLineString[i].GetValue();
        int offset = startPos + i;
        if (currentChar == '(' || currentChar == ')') {textField->SetStyle(offset, offset + 1, app_data.rainbowBraces);}
        else if (currentChar == '{' || currentChar == '}') {textField->SetStyle(offset, offset + 1, app_data.rainbowCurlyBraces);} 
        else {textField->SetStyle(offset, offset + 1, newStyle);}
    }

}
