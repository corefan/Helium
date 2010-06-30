#include "Precompile.h"
#include "Application.h"
#include "AppPreferences.h"
#include "ArtProvider.h"

#include "Application/Application.h"
#include "Asset/AssetInit.h"
#include "Browser/Browser.h"
#include "Foundation/InitializerStack.h"
#include "Foundation/Log.h"
#include "Core/CoreInit.h"
#include "Platform/Exception.h"
#include "Editor/ApplicationPreferences.h"
#include "Editor/Editor.h"
#include "Editor/EditorInit.h"
#include "Editor/Preferences.h"
#include "Foundation/Math/Utils.h"
#include "Scene/SceneEditor.h"
#include "Scene/SceneInit.h"
#include "Task/TaskInit.h"
#include "Application/UI/ImageManager.h"
#include "Platform/Windows/Windows.h"
#include "Platform/Process.h"
#include "Application/Worker/Process.h"

#include "Application/UI/DebugUI/DebugUI.h"
#include "Application/UI/PerforceUI/PerforceUI.h"


#include "Foundation/CommandLine/Option.h"
#include "Foundation/CommandLine/Command.h"
#include "Foundation/CommandLine/Commands/Help.h"
#include "Foundation/CommandLine/Processor.h"

//#include "Commands/BuildCommand.h"
#include "Commands/ProfileDumpCommand.h"
#include "Commands/RebuildCommand.h"

#include <wx/cmdline.h>
#include <wx/splash.h>

using namespace Luna;
using namespace Nocturnal;
using namespace Nocturnal::CommandLine;

namespace Luna
{
    IMPLEMENT_APP( LunaApp );
}

LunaApp::LunaApp()
: wxApp()
, m_DocumentManager( new DocumentManager() )
, m_SceneEditor( NULL )
{
}

LunaApp::~LunaApp()
{
    if ( m_DocumentManager )
    {
        delete m_DocumentManager;
    }
}

///////////////////////////////////////////////////////////////////////////////
// Called after OnInitCmdLine.  The base class handles the /help command line
// switch and exits.  If we get this far, we need to parse the command line
// and determine what mode to launch the app in.
// 
bool LunaApp::OnInit()
{
    wxArtProvider::Push( new ::Luna::ArtProvider() );

    SetVendorName( TXT( "Nocturnal" ) );

    //SetLogo( wxT( "Luna (c) 2010 - Nocturnal\n" ) );

    // don't spend a lot of time updating idle events for windows that don't need it
    wxUpdateUIEvent::SetMode( wxUPDATE_UI_PROCESS_SPECIFIED );
    wxIdleEvent::SetMode( wxIDLE_PROCESS_SPECIFIED );

    tchar module[MAX_PATH];
    GetModuleFileName( 0, module, MAX_PATH );

    Nocturnal::Path exePath( module );
    Nocturnal::Path iconFolder( exePath.Directory() + TXT( "Icons/" ) );

    Nocturnal::ImageManagerInit( iconFolder.Get(), TXT( "" ) );
    Nocturnal::GlobalImageManager().LoadGuiArt();

    {
        Log::Bullet initialize( TXT( "Initializing\n" ) );

        m_InitializerStack.Push( PerforceUI::Initialize, PerforceUI::Cleanup );

        {
            Log::Bullet modules( TXT( "Modules:\n" ) );

            {
                Log::Bullet bullet( TXT( "Core...\n" ) );
                m_InitializerStack.Push( CoreInitialize, CoreCleanup );
            }

            {
                Log::Bullet bullet( TXT( "Editor...\n" ) );
                m_InitializerStack.Push( PreferencesBase::InitializeType, PreferencesBase::CleanupType );
                m_InitializerStack.Push( Preferences::InitializeType, Preferences::CleanupType );
                m_InitializerStack.Push( AppPreferences::InitializeType, AppPreferences::CleanupType );
                m_InitializerStack.Push( EditorInitialize, EditorCleanup );
            }

            {
                Log::Bullet bullet( TXT( "Task...\n" ) );
                m_InitializerStack.Push( TaskInitialize, TaskCleanup );
            }

            {
                Log::Bullet bullet( TXT( "Asset Vault...\n" ) );
                m_InitializerStack.Push( Browser::Initialize, Browser::Cleanup );
            }

            {
                Log::Bullet bullet( TXT( "Asset Editor...\n" ) );
                m_InitializerStack.Push( LunaAsset::InitializeModule, LunaAsset::CleanupModule );
            }

            {
                Log::Bullet bullet( TXT( "Scene Editor...\n" ) );
                m_InitializerStack.Push( SceneInitialize, SceneCleanup );
            }
        }

        {
            Log::Bullet systems( TXT( "Systems:\n" ) );

            {
                Log::Bullet vault( TXT( "Asset Tracker...\n" ) );
                GetAppPreferences()->UseTracker( false ); //!parser.Found( "disable_tracker" ) );
            }
        }
    }

    Log::Print( TXT( "\n" ) ); 

    if ( Log::GetErrorCount() )
    {
        wxMessageBox( TXT( "There were errors during startup, use Luna with caution." ), TXT( "Error" ), wxCENTER | wxICON_ERROR | wxOK );
    }

    GetSceneEditor()->Show();

    //return __super::OnCmdLineParsed( parser );
    return true;
}

///////////////////////////////////////////////////////////////////////////////
// Called when the application is ready to start running.
// 
int LunaApp::OnRun()
{
    return __super::OnRun();
}

///////////////////////////////////////////////////////////////////////////////
// Called when the application is being exited.  Cleans up resources.
// 
int LunaApp::OnExit()
{
    // Save preferences
    ::Luna::GetApplicationPreferences()->SavePreferences();

    Nocturnal::ImageManagerCleanup();

    m_InitializerStack.Cleanup();

    return __super::OnExit();
}

#pragma TODO("Apparently wx 2.8 doesn't support unicode command lines, please to fix in 2.9.0")
static int wxEntryWrapper(HINSTANCE hInstance, HINSTANCE hPrevInstance, tchar* pCmdLine, int nCmdShow)
{
    std::string cmdLine;
    Platform::ConvertString( pCmdLine, cmdLine );
    return wxEntry( hInstance, hPrevInstance, const_cast<char*>(cmdLine.c_str()), nCmdShow );
}

/////////////////////////////////////////////////////////////////////////////////
int Main ( int argc, const tchar** argv )
{
	// print physical memory
	MEMORYSTATUSEX status;
	memset(&status, 0, sizeof(status));
	status.dwLength = sizeof(status);
	::GlobalMemoryStatusEx(&status);
	Log::Print( TXT( "Physical Memory: %I64u M bytes total, %I64u M bytes available\n" ), status.ullTotalPhys >> 20, status.ullAvailPhys >> 20);

	// fill out the options vector
	std::vector< tstring > options;
	for ( int i = 1; i < argc; ++i )
	{
		options.push_back( argv[ i ] );
	}
    std::vector< tstring >::const_iterator& argsBegin = options.begin(), argsEnd = options.end();

    bool success = true;
	tstring error; 


    Processor processor( TXT( "luna" ), TXT( "[COMMAND <ARGS>]" ), TXT( "Luna (c) 2010 - Nocturnal" ) );

    //BuildCommand buildCommand;
    //success &= buildCommand.Initialize( error );
    //success &= processor.RegisterCommand( &buildCommand, error );

    ProfileDumpCommand profileDumpCommand;
    success &= profileDumpCommand.Initialize( error );
    success &= processor.RegisterCommand( &profileDumpCommand, error );

    RebuildCommand rebuildCommand;
    success &= rebuildCommand.Initialize( error );
    success &= processor.RegisterCommand( &rebuildCommand, error );

    Nocturnal::CommandLine::Help helpCommand;
    helpCommand.SetOwner( &processor );
    success &= helpCommand.Initialize( error );
    success &= processor.RegisterCommand( &helpCommand, error );

    //success &= processor.AddOption( new FlagOption(  , "pipe", "use pipe for console connection" ), error ); 
    //success &= processor.AddOption( new FlagOption(  , "disable_tracker", "disable Asset Tracker" ), error );
    //
    //success &= processor.AddOption( new FlagOption(  , WindowSettings::s_Reset, "reset all window positions" ), error );
    //success &= processor.AddOption( new FlagOption(  , Preferences::s_ResetPreferences, "resets all preferences for all of Luna" ), error );
    //
    //success &= processor.AddOption( new FlagOption(  , Worker::Args::Debug, "debug use of background processes" ), error );
    //success &= processor.AddOption( new FlagOption(  , Worker::Args::Wait, "wait forever for background processes" ), error );

    bool scriptFlag = false;
    success &= processor.AddOption( new FlagOption( &scriptFlag, Application::Args::Script, TXT( "omit prefix and suffix in console output" ) ), error );
    
    bool attachFlag = false;
    success &= processor.AddOption( new FlagOption( &attachFlag, Application::Args::Attach, TXT( "wait for a debugger to attach to the process on startup" ) ), error );
    
    bool profileFlag = false;
    success &= processor.AddOption( new FlagOption( &profileFlag, Application::Args::Profile, TXT( "enable profile output to the console windows" ) ), error );
    
    bool memoryFlag = false;
    success &= processor.AddOption( new FlagOption( &memoryFlag, Application::Args::Memory, TXT( "profile and report memory usage to the console" ) ), error );
    
    bool vreboseFlag = false;
    success &= processor.AddOption( new FlagOption( &vreboseFlag, Application::Args::Verbose, TXT( "output a verbose level of console output" ) ), error );
    
    bool extremeFlag = false;
    success &= processor.AddOption( new FlagOption( &extremeFlag, Application::Args::Extreme, TXT( "output an extremely verbose level of console output" ) ), error );
    
    bool debugFlag = false;
    success &= processor.AddOption( new FlagOption( &debugFlag, Application::Args::Debug, TXT( "output debug console output" ) ), error );
    
    int nice = 0;
    success &= processor.AddOption( new SimpleOption<int>( &nice , TXT( "nice" ), TXT( "<NUM>" ), TXT( "number of processors to nice (for other processes)" ) ), error );
    
    bool helpFlag;
    success &= processor.AddOption( new FlagOption( &helpFlag, TXT( "h|help" ), TXT( "print program usage" ) ), error );

    success &= processor.ParseOptions( argsBegin, argsEnd, error );

	if ( success )
	{
        if ( helpFlag )
        {
            Log::Print( TXT( "\nPrinting help for Luna...\n" ) );
            Log::Print( processor.Help().c_str() );
            Log::Print( TXT( "\n" ) );
            success = true;
        }
        else if ( argsBegin != argsEnd )
        {
            while ( success && ( argsBegin != argsEnd ) )
            {
                const tstring& arg = (*argsBegin);
                ++argsBegin;

                if ( arg.length() < 1 )
                {
                    continue;
                }

                if ( arg[ 0 ] == '-' )
                {
                    error = TXT( "Unknown option, or option passed out of order: " ) + arg;
                    success = false;
                }
                else
                {
                    Command* command = processor.GetCommand( arg );
                    if ( command )
                    {
                        success = command->Process( argsBegin, argsEnd, error );
                    }
                    else
                    {
                        error = TXT( "Unknown commandline parameter: " ) + arg + TXT( "\n\n" );
                        success = false;
                    }
                }
            }
        }
        else
        {
            //buildCommand.Cleanup();
            rebuildCommand.Cleanup();

            ::FreeConsole();
            return Application::StandardWinMain( &wxEntryWrapper );
        }
    }

    //buildCommand.Cleanup();
    rebuildCommand.Cleanup();

    if ( !success && !error.empty() )
    {
        Log::Error( TXT( "%s\n" ), error.c_str() );
    }

    return success ? 0 : 1;
}


///////////////////////////////////////////////////////////////////////////////
// Main entry point for the application.
//
int main( int argc, const tchar** argv )
{
    Nocturnal::InitializerStack initializerStack( true );
    initializerStack.Push( &DebugUI::Initialize, &DebugUI::Cleanup );

    return Application::StandardMain( &Main, argc, argv );
}