/*
  ==============================================================================

    CabbageApplication.h
    Created: 12 Oct 2016 11:22:13am
    Author:  rory

  ==============================================================================
*/

#ifndef CABBAGEAPPLICATION_H_INCLUDED
#define CABBAGEAPPLICATION_H_INCLUDED

#include "CabbageIds.h"
#include "CabbageSettings.h"
#include "CabbageIDELookAndFeel.h"
#include "CabbageSettingsWindow.h"
#include "CabbageNewProjectWindow.h"

//==============================================================================
class CabbageApplication  : public JUCEApplication, public ChangeListener
{
public:
    //==============================================================================
    CabbageApplication();

    static CabbageApplication& getApp();
    static ApplicationCommandManager& getCommandManager();
    void changeListenerCallback(ChangeBroadcaster* source);
    const String getApplicationName() override
    {
        return ProjectInfo::projectName;
    }
    const String getApplicationVersion() override
    {
        return ProjectInfo::versionString;
    }
    bool moreThanOneInstanceAllowed() override
    {
        return true;
    }

    void initialiseBasics();
    bool initialiseLogger (const char* filePrefix);
    void initCommandManager();
    void deleteLogger();
    void shutdown();
    //==============================================================================
    MenuBarModel* getMenuModel();
    StringArray getMenuNames();
    void createMenu (PopupMenu&, const String& menuName);
    void createFileMenu (PopupMenu&);
    void createEditMenu (PopupMenu&);
    void createViewMenu (PopupMenu&);
    void createBuildMenu (PopupMenu&);
    void createColourSchemeItems (PopupMenu&);
    void createWindowMenu (PopupMenu&);
    void createToolsMenu (PopupMenu&);
    void handleMainMenuCommand (int menuItemID);
    void getAllCommands (Array<CommandID>&) override;
    void getCommandInfo (CommandID commandID, ApplicationCommandInfo&) override;
    bool perform (const InvocationInfo&) override;

    void createNewProject();
    void askUserToOpenFile();
    bool openFile (const File&);
    bool closeAllDocuments (bool askUserToSave);
    bool closeAllMainWindows();
    PropertySet* getDefaultSettings();
    ScopedPointer<ApplicationCommandManager> commandManager;

    ScopedPointer<CabbageSettings> cabbageSettings;
    ScopedPointer<FileLogger> logger;
    PropertiesFile::Options getPropertyFileOptionsFor (const String& filename);
    ScopedPointer<Component> appearanceEditorWindow, globalPreferencesWindow;
    CabbageIDELookAndFeel lookAndFeel;
    bool isRunningCommandLine;

    //==============================================================================


    void initialise (const String& commandLine) override;

    //==============================================================================
    void systemRequestedQuit() override
    {
        // This is called when the app is being asked to quit: you can ignore this
        // request and let the app carry on running, or call quit() to allow the app to close.
        quit();
    }

    void anotherInstanceStarted (const String& commandLine) override
    {
        // When another instance of the app is launched while this one is running,
        // this method is invoked, and the commandLine parameter tells you what
        // the other instance's command-line arguments were.
    }

    struct MainMenuModel  : public MenuBarModel
    {
        MainMenuModel()
        {
            setApplicationCommandManagerToWatch (&getCommandManager());
        }

        StringArray getMenuBarNames() override
        {
            return getApp().getMenuNames();
        }

        PopupMenu getMenuForIndex (int /*topLevelMenuIndex*/, const String& menuName) override
        {
            PopupMenu menu;
            getApp().createMenu (menu, menuName);
            return menu;
        }

        void menuItemSelected (int menuItemID, int /*topLevelMenuIndex*/) override
        {
            getApp().handleMainMenuCommand (menuItemID);
        }
    };
    ScopedPointer<MainMenuModel> menuModel;

private:
    ScopedPointer<CabbageMainDocumentWindow> cabbageMainDocumentWindow;
};




#endif  // CABBAGEAPPLICATION_H_INCLUDED