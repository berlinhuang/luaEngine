//
// Created by root on 18-2-2.
//

#include "LuaStart.h"

char AppProgArgs[] = "";//全局变量
void IGNSignal()
{
    signal( SIGHUP, SIG_IGN );
    signal( SIGALRM, SIG_IGN );
    signal( SIGPIPE, SIG_IGN );
    signal( SIGXCPU, SIG_IGN );
    signal( SIGXFSZ, SIG_IGN );
    signal( SIGPROF, SIG_IGN );
    signal( SIGVTALRM, SIG_IGN );
    signal( SIGQUIT, SIG_IGN );
    signal( SIGCHLD, SIG_IGN);
}

void HelpInfo()
{
    cout << "----------------------usage:" << endl;
    cout << "--help or /? show usage" << endl;
    cout << "-f run as a foreground program, accept input command" << endl;
    cout << "-v show build time and svn version" << endl;
    cout << "-s [WorldServer|SceneServer|DBServer] run the specific server" << endl;
    cout << "egg: ./server -f -s SceneServer" << endl;
}


int main(int argc, char* argv[])
{
#ifndef _WIN32
    IGNSignal(); //! linux下需要屏蔽的一些信号
#endif

    if (strcmp(argv[1], "--help") == 0|| strcmp(argv[1], "/?") == 0)
    {
        HelpInfo();
        return 0;
    }
    for(int i = 1; i<argc; i++ )
    {
        strcat(AppProgArgs, argv[i]);
        strcat(AppProgArgs," ");
    }
    Logger::setLogLevel(Logger::INFO);//要有Logger::setLogLevel 不然 会报未定义

    LuaStart();
    return 0;
}