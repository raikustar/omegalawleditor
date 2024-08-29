//wxWidget header
#include "omegalawl_header.h"
// other includes
#include <vector>


// App colours and other visual misc..
struct AppVisuals {
    wxColour backgroundColour{ 75,75,75 };
    wxColour lineIdentifierBackgroundColour{ 50,50,50 };
    wxColour lineIdentifierTextColour{ 250,100,0 };
    wxColour backgroundColourLight{ 100,100,100 };
    wxColour textColour{ 225,100,0 };
};

AppVisuals vis{};

class MyApp : public wxApp
{
public:
    bool OnInit() override;
    
};

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
    wxString lineCounter{"1"};

    wxFont* font = new wxFont(14, wxFONTFAMILY_SCRIPT, wxFONTSTYLE_SLANT, wxFONTWEIGHT_HEAVY, false);

    this->SetBackgroundColour(vis.backgroundColour);

    // Menu
    wxMenu* menuFile = new wxMenu;
    menuFile->Append(wxID_OPEN, "Open", "Open a file");
    menuFile->AppendSeparator();
    menuFile->Append(wxID_EXIT, "Exit\tAlt-X", "Exit the program");

    wxMenuBar* menuBar = new wxMenuBar;
    menuBar->Append(menuFile, "&File");
    SetMenuBar(menuBar);

    // After menu window for all other boxes to be organized 
    wxBoxSizer* mainBoxSizer = new wxBoxSizer(wxHORIZONTAL);

    // Numbers for lines of text
    wxStaticText* numberField = new wxStaticText(this, wxID_ANY, wxString(lineCounter), wxDefaultPosition, wxSize(50, 300), wxTE_MULTILINE | wxTE_RICH | wxTE_CENTER | wxTE_NOHIDESEL | wxTE_READONLY);
    numberField->SetFont(*font);
    numberField->SetForegroundColour(vis.lineIdentifierTextColour);
    numberField->SetBackgroundColour(vis.lineIdentifierBackgroundColour);
    numberField->Refresh();

    // Notepad 
    textField = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_RICH | wxUSE_SCROLLBAR);
    wxTextAttr newStyle{};
    newStyle.SetTextColour(vis.textColour);
    newStyle.SetBackgroundColour(vis.backgroundColourLight);

    // Visual colours

    textField->SetFont(*font);
    textField->SetBackgroundColour(vis.backgroundColourLight);
    textField->SetDefaultStyle(wxTextAttr(vis.textColour, vis.backgroundColourLight));
    textField->SetStyle(0, textField->GetLastPosition(), newStyle);
    


    mainBoxSizer->Add(numberField, 0, wxALL | wxEXPAND, 0);
    mainBoxSizer->Add(textField, 1, wxALL | wxEXPAND, 0);

    this->SetSizer(mainBoxSizer);

    CreateStatusBar();
    SetStatusText("Welcome to Omegalawl Editor!");

    Bind(wxEVT_MENU, &MyFrame::OnExit, this, wxID_EXIT);
    Bind(wxEVT_MENU, &MyFrame::OnFileOpen, this, wxID_OPEN);
    

}

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

    wxFile openFile(filePath);
    
    if (!openFile.IsOpened()) {
        wxMessageBox("Couldn't open the file", "Error", wxOK);
        return;
    }

    wxString fileContent{};
    openFile.ReadAll(&fileContent);
    
    textField->SetValue(fileContent);
    

}

// Refresh colours after loading data from file.

// Make header file 

// add numbers at the start of new line
// generate number per line.
