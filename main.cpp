//wxWidget header
#include "omegalawl_header.h"
// other includes

/*


    - Refresh rainbowtext properly when changing state.
    - Properly follow current line:
        Hold key down and update pos, some event?
    
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

    font = new wxFont(14, wxFONTFAMILY_SCRIPT, wxFONTSTYLE_SLANT, wxFONTWEIGHT_HEAVY, false);
    styledFont = new wxFont(16, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);

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
    numberField = new wxTextCtrl(this, wxID_ANY, "1", wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_RICH | wxTE_CENTRE | wxTE_READONLY | wxTE_NOHIDESEL | wxTE_NO_VSCROLL); 
    
    numberField->SetFont(*font);
    numberField->SetMaxSize(wxSize(60, 2000));
    numberField->SetVirtualSize(wxSize(60, 2000));
    numberField->SetForegroundColour(app_data.lineIdentifierTextColour);
    numberField->SetBackgroundColour(app_data.lineIdentifierBackgroundColour);


    // Notepad 
    textField = new wxStyledTextCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_RICH | wxUSE_SCROLLBAR | wxTE_DONTWRAP);
    textField->SetMarginWidth(1, 0);
    textField->SetScrollWidth(1);

    // Visual colours
    newStyle.SetFont(*font);
    newStyle.SetBackgroundColour(app_data.backgroundColourLight);
    newStyle.SetTextColour(app_data.textColour);
    
    
    redrawTextCtrlWindow();
    defaultStyleTextField();
    mainBoxSizer->Add(numberField, 0, wxALL | wxEXPAND, 0);
    mainBoxSizer->Add(textField, 1, wxALL | wxEXPAND, 0);

    this->SetSizer(mainBoxSizer);
    this->SetInitialSize(wxSize(800, 500));

    CreateStatusBar();
    SetStatusText(app_data.dataFilePath); 
    

    textField->Bind(wxEVT_STC_CHANGE, &MyFrame::OnTextChanged, this); // wxEVT_STC_CHARADDED
    /*
    
        different event for updateLineNumberHighlightTextChange?
    */

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

void MyFrame::OnTextChanged(wxStyledTextEvent& event)
{
    redrawTextCtrlWindow();
    updateLineNumbers();
    statusUpdateText("Unsaved changes..", false);
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
    if (textField->GetValue().length() == 0) {
        numberField->SetLabel("1");
    }
    textField->SetFocus();
    textField->StyleSetFont(wxSTC_STYLE_DEFAULT, *styledFont);
    
    if (rainbowTextToggle){ 
        currentRainbowColourChange();
    }
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
    syncTextAndNumberField();
    
    if (app_data.totalLines != textFieldLines) {
        wxString lineNumberString{};
        for (int i = 1; i <= textFieldLines; i++) { lineNumberString += wxString::Format("%d\n", i); }
        numberField->SetLabel(lineNumberString);
        numberField->Refresh();
        numberField->Update();
        app_data.totalLines = textFieldLines;
    }
    //highlightLineNumber();
}

void MyFrame::highlightLineNumber() {
    /*
        logic for highlight current number at numberField
    */
}

void MyFrame::syncTextAndNumberField() {
    /*
        Currently it changes line on the left instantly
        Create an offset or point where it follows to a new line and when it doesn't

        i.e. move up from the last line of the window, 
        and only start updating to new lines if I reach at the top of the current window(highest line per window)
        
   
    */
    int pos = textField->GetInsertionPoint();
    int lineNum = getLineNumber(pos);

    int tFScrollPos = textField->GetScrollPos(wxVERTICAL);
    int nFScrollPos = numberField->GetScrollPos(wxVERTICAL);

    if (tFScrollPos != nFScrollPos) {
        numberField->SetScrollPos(wxVERTICAL, tFScrollPos, true);
        if (lineNum < app_data.currentLineNum)
            numberField->ShowPosition(tFScrollPos);
        else {
            numberField->ShowPosition(tFScrollPos+30);
        }
        numberField->Refresh();
        numberField->Update();
    }
    app_data.currentLineNum = lineNum;
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
        changeFullTextToRainbow();
    }
    else {
        statusUpdateText("Disabled Rainbow Text", false);
        rainbowTextToggle = false;
        defaultStyleTextField(true);
    }
}
void MyFrame::changeFullTextToRainbow() {
    wxString text = textField->GetText();
    if (text.length() == 0) {
        return;
    }
    for (int i = 0; i < text.length(); i++) {
        if (text[i] == '{' || text[i] == '}') {
            changeColourAtChar(i, app_data.rainbowCurlyBraces, 1);
        }
        else if (text[i] == '(' || text[i] == ')') {
            changeColourAtChar(i, app_data.rainbowBraces, 2);
        }
        else {
            changeColourAtChar(i, app_data.textColour, 0);
        }
    }
}

void MyFrame::defaultStyleTextField(bool reset) {
    textField->StyleSetBackground(32, app_data.backgroundColour);
    textField->StyleSetBackground(0, app_data.backgroundColour);
    textField->StyleSetForeground(0, app_data.textColour);
    if (reset) {
        int p = textField->GetLastPosition();
        textField->StartStyling(0);
        textField->SetStyling(p, 0);
    }
}

void MyFrame::currentRainbowColourChange() {
    int pos = textField->GetInsertionPoint();;
    int c_int = textField->GetCharAt(pos);
    char c = static_cast<char>(c_int);
    
    if (c == '{' || c == '}') {
        changeColourAtChar(pos, app_data.rainbowCurlyBraces,1);
    }
    else if (c == '(' || c == ')') {
        changeColourAtChar(pos, app_data.rainbowBraces, 2);
    }
}

void MyFrame::changeColourAtChar(int pos, wxColour colour, int vector_index) {
    textField->StyleSetBackground(32, app_data.backgroundColour);
    textField->StyleSetBackground(app_data.rainbowStyle[vector_index], app_data.backgroundColour);
    textField->StyleSetForeground(app_data.rainbowStyle[vector_index], colour);
    textField->StartStyling(pos);
    textField->SetStyling(pos, app_data.rainbowStyle[vector_index]);
}




// old
void MyFrame::old_changeTextFieldColourToRainbow() {
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
    old_RainbowColourSwapFunction(currentLineStart,currentLineString);
}

void MyFrame::old_RainbowColourSwapFunction(int startPos, wxString currentLineString) {
    for (int i = 0; i < currentLineString.length(); i++) {
        char currentChar = currentLineString[i].GetValue();
        int offset = startPos + i;
        if (currentChar == '(' || currentChar == ')') {textField->SetStyle(offset, offset + 1, app_data.rainbowBraces);}
        else if (currentChar == '{' || currentChar == '}') {textField->SetStyle(offset, offset + 1, app_data.rainbowCurlyBraces);} 
        else {textField->SetStyle(offset, offset + 1, newStyle);}
    }

}
