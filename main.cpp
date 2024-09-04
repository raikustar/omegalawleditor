//wxWidget header
#include "omegalawl_header.h"
// other includes




// App colours and other visual misc..
AppData app_data{};

wxIMPLEMENT_APP(MyApp);


bool MyApp::OnInit()
{
    MyFrame* frame = new MyFrame();
    frame->Show(true);
    return true;
}

MyFrame::MyFrame()
    : wxFrame(nullptr, wxID_ANY, "Omegalawl Editor", wxDefaultPosition, wxSize(800,500), wxDEFAULT_FRAME_STYLE)
{
    wxFont* font = new wxFont(14, wxFONTFAMILY_SCRIPT, wxFONTSTYLE_SLANT, wxFONTWEIGHT_HEAVY, false);

    this->SetBackgroundColour(app_data.backgroundColour);

    // Menu
    wxMenu* menuFile = new wxMenu;
    menuFile->Append(wxID_OPEN, "Open", "Open a file");
    menuFile->Append(wxID_SAVE, "Save\tCtrl-S", "Quick save");
    menuFile->Append(wxID_SAVEAS, "Save as\tCtrl-Shift-S", "Save file as ..");
    menuFile->AppendSeparator();
    menuFile->Append(wxID_EXIT, "Exit\tAlt-X", "Exit the program");

    wxMenuBar* menuBar = new wxMenuBar;
    menuBar->Append(menuFile, "&File");
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
    newStyle.SetTextColour(app_data.textColour);
    newStyle.SetBackgroundColour(app_data.backgroundColourLight);

    redrawTextCtrlWindow();
    if (textField->IsEmpty()) 
    {
        redrawTextCtrlWindow();
    }
    
    updateLineNumbers();

    mainBoxSizer->Add(numberField, 0, wxALL | wxEXPAND, 0);
    mainBoxSizer->Add(textField, 1, wxALL | wxEXPAND, 0);

    this->SetSizer(mainBoxSizer);
    this->SetInitialSize(wxSize(800, 500));

    CreateStatusBar();
    SetStatusText(app_data.dataFilePath); 


    textField->Bind(wxEVT_TEXT, &MyFrame::OnScrollUpdate, this);
    textField->Bind(wxEVT_TEXT, &MyFrame::OnTextChanged, this);

    numberField->Bind(wxEVT_KILL_FOCUS, &MyFrame::OnRemoveNumberFieldFocus, this);
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
    statusUpdateText("Opened file from");
    
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
        statusUpdateText("Saved file to");
    }

}

void MyFrame::OnSave(wxCommandEvent& event)
{
    wxString fileContent{};

    if (app_data.dataFilePath == "") {
        MyFrame::OnSaveAs(event);
    }
    else {
        wxFileOutputStream output_stream(app_data.dataFilePath);
        fileContent = textField->GetValue();
        output_stream.Write(fileContent.mb_str(), fileContent.length());
        output_stream.Sync();
        statusUpdateText("Quick save to file");
    }

}

void MyFrame::OnTextChanged(wxCommandEvent& event)
{
    updateLineNumbers();
    event.Skip();
}

void MyFrame::OnScrollUpdate(wxCommandEvent& event)
{
    updateScrollPosition();
    event.Skip();
}

void MyFrame::OnRemoveNumberFieldFocus(wxFocusEvent& event) {
    event.Skip();
    // remove focus from number lines
    numberField->DisableFocusFromKeyboard();
}
// Misc member functions
void MyFrame::redrawTextCtrlWindow()
{
    textField->SetFont(newStyle.GetFont());
    textField->SetBackgroundColour(newStyle.GetBackgroundColour());
    textField->SetDefaultStyle(newStyle);
    textField->SetStyle(0, textField->GetLastPosition(), newStyle);
    textField->SetFocus();
    // Force re-draw
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
    int textFieldLines{ textField->GetNumberOfLines() };
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
    int numField{ numberField->GetNumberOfLines() }, txtField{ textField->GetNumberOfLines() + 1};

    if (numField == txtField) {
        numberField->SetScrollPos(wxVERTICAL, scrollPos);

        // Get textField current line
        int pos = textField->GetInsertionPoint();
        int lineNum = getLineNumber(pos);

        // Set follow for numberField based on textField line number
        numberField->SetInsertionPoint(lineNum);
        numberField->ShowPosition(lineNum);
        numberField->Refresh();
        numberField->Update();
    }
}

int MyFrame::getLineNumber(long pos) {
    long col{}, line{};
    textField->PositionToXY(pos, &col, &line);
    return line + 1;

}

void MyFrame::statusUpdateText(wxString a, bool path) {
    wxString status = wxString::Format(a + " - " + app_data.dataFilePath);
    SetStatusText(status);
}


