/*
 * w32pipe:  win32 clone of npopen.c, utilizes native pipes (not temp files).
 *
 * Background
 * ==========
 * The techniques used in w32_npclose() and w32_inout_popen() are derived
 * from much trial and error and support "pipe" I/O in both a console and
 * GUI environment.  You may _think_ you have a better way of effecting the
 * functionality provided in this module and that may well be the case.
 * But be sure you test your new code with at least these versions of Win32:
 *
 *      win95 (original version), OSR2, NT 4.0
 *
 * For each HOST, be sure to test read pipes, write pipes, and filters (and
 * test repeatedly within the same vile session).
 *
 *
 * Acknowledgments
 * ===============
 * Until I read Steve Kirkendall's code for the Win32 version of elvis, I
 * did not realize that attempting to redirect stdin to a device is a
 * _not_ a good strategy.
 *
 *
 * Caveats
 * =======
 *
 * -- This code has not been tested with NT 3.51 .
 *
 * -- The MSDN Knowledge Base has example code that uses anonymous pipes
 *    to redirect a spawned process's stdin, stdout, and stderr.  Don't go
 *    there.
 *
 * -- The original Win95 console shell (command.com) accesses the floppy
 *    drive each and every time a process communicates with it via a pipe
 *    and the OS R2 shell abruptly hangs under similar conditions.  By
 *    default, then, on a WinNT host, vile's pipes are implemented using
 *    native pipes (i.e., with the code in this module), while Win95 hosts
 *    fall back to temp file communication.  If the user's replacement
 *	  Win95 shell does not exhibit communication problems similar to
 *    those described above (e.g., Thompson Toolkit Shell), vile may be
 *    forced to use native Win32 pipes by setting the global mode
 *    "w32pipes" (e.g., "se w32pipes").
 *
 * -- This module's native pipes implementation exhibits various problems
 *    when a 16-bit console app is exec'd.  On a win95 host, the editor
 *    and shell generally hang.  WinNT does better, but winvile creates
 *    "background" shell windows that require manual closure.
 *
 * -- This module configures read pipes so that the exec'd app reads
 *    it's input from an empty file.  That's a necessity, not a bug.
 *    Consequently, if an attempt is made to read data from an app
 *    that itself reads input (why would you do that?), the app will
 *    appear to hang if it reopens stdin on the console (because vile's
 *    stdin is not available to the app--another necessity).  In this
 *    situation, kill the app by typing ^C (and then please apply for a
 *    QA position with a certain Redmond company).
 *
 * $Header: /users/source/archives/vile.vcs/RCS/w32pipe.c,v 1.7 1998/04/28 10:15:32 tom Exp $
 */

#include <windows.h>
#include <io.h>
#include <share.h>
#include <process.h>
#include <assert.h>

#define HAVE_FCNTL_H 1

#include "estruct.h"
#include "edef.h"

#define BAD_FD          (-1)
#define BAD_PROC_HANDLE (INVALID_HANDLE_VALUE)
#define PIPESIZ         (4096)
#define SHELL_ERR_MSG   \
          "error: shell process \"%s\" failed, check COMSPEC env var\n"

static HANDLE proc_handle;
static char   *shell = NULL,
              *shell_c = "/c",
              *tmpin_name;

/* ------------------------------------------------------------------ */

#ifndef DISP_NTWIN

static DWORD console_mode;
static char  orig_title[256];

/*
 * Need to install an event handler before spawning a child so that
 * typing ^C in the child process does not cause the waiting vile
 * process to exit.  Don't understand why this is necessary, but it
 * is required for win95 (at least).
 */
static BOOL WINAPI
event_handler(DWORD ctrl_type)
{
    switch (ctrl_type)
    {
        case CTRL_CLOSE_EVENT:
        case CTRL_LOGOFF_EVENT:
        case CTRL_SHUTDOWN_EVENT:
            imdying(1);
        break;
    }
    return (TRUE);
}



/* Temporarily setup child's console input for typical line-oriented I/O. */
void
push_console_mode(char *shell_cmd)
{
    HANDLE hConIn = GetStdHandle(STD_INPUT_HANDLE);

    GetConsoleTitle(orig_title, sizeof(orig_title));
    SetConsoleTitle(shell_cmd);
    (void) GetConsoleMode(hConIn, &console_mode);
    (void) SetConsoleMode(hConIn,
                  ENABLE_LINE_INPUT|ENABLE_ECHO_INPUT|ENABLE_PROCESSED_INPUT);
    SetConsoleCtrlHandler(event_handler, TRUE);
}



/* Put everything back the way it was. */
void
pop_console_mode(void)
{
    SetConsoleTitle(orig_title);
    SetConsoleCtrlHandler(event_handler, FALSE);
    (void) SetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), console_mode);
}

#else /* DISP_NTWIN */
#define push_console_mode(shell_cmd) /* nothing */
#define pop_console_mode() /* nothing */
#endif



static void
global_cleanup(void)
{
    if (tmpin_name)
    {
        (void) remove(tmpin_name);
        (void) free(tmpin_name);
        tmpin_name = NULL;
    }
    pop_console_mode();
}



static HANDLE
exec_shell(char *cmd, HANDLE *handles, int child_behind)
{
    char                 cmdbuf[2048];
    HWND                 fgnd;
    PROCESS_INFORMATION  pi;
    STARTUPINFO          si;

    if (shell == 0)
        shell = get_shell();

    if (!strcmp(shell, "/bin/sh"))
        shell_c = "-c";

    _snprintf(cmdbuf, sizeof(cmdbuf), "%s %s %s", shell, shell_c, cmd);

    TRACE(("exec_shell %s\n", shell));
    TRACE(("shell cmd: %s\n", cmd));

    memset(&si, 0, sizeof(si));
    proc_handle    = BAD_PROC_HANDLE;  /* in case of failure */
    si.cb          = sizeof(si);
    si.dwFlags     = STARTF_USESTDHANDLES;
    si.hStdInput   = handles[0];
    si.hStdOutput  = handles[1];
    si.hStdError   = handles[2];
#ifdef DISP_NTWIN
    if (child_behind)
        fgnd = GetForegroundWindow();
    AllocConsole();
#endif
    if (CreateProcess(NULL,
                      cmdbuf,
                      NULL,
                      NULL,
                      TRUE,       /* Inherit handles */
                      0,
                      NULL,
                      NULL,
                      &si,
                      &pi))
    {
        /* Success */

#ifdef DISP_NTWIN
    if (child_behind)
        SetForegroundWindow(fgnd);
#endif
        CloseHandle(pi.hThread);
        proc_handle = pi.hProcess;
    }
    return (proc_handle);
}



int
w32_inout_popen(FILE **fr, FILE **fw, char *cmd)
{
    HANDLE handles[3];
    int    i, rc, rp[2], tmpin_fd, wp[2];

    proc_handle  = BAD_PROC_HANDLE;
    rp[0]        = rp[1]      = wp[0]      = wp[1] = BAD_FD;
    handles[0]   = handles[1] = handles[2] = INVALID_HANDLE_VALUE;
    tmpin_fd     = BAD_FD;
    tmpin_name   = NULL;
    push_console_mode(cmd);
    do
    {
        if (fr)
        {
            *fr = NULL;

            /*
             * Open (parent's) input pipe in TEXT mode, which will force
             * translation of the child's CR/LF record delimiters to NL
             * and keep the dreaded ^M chars from temporarily appearing
             * in a vile buffer (ugly).
             */
            if (_pipe(rp, PIPESIZ, O_TEXT|O_NOINHERIT) == -1)
                break;
            if (! DuplicateHandle(GetCurrentProcess(),
                                  (HANDLE) _get_osfhandle(rp[1]),
                                  GetCurrentProcess(),
                                  handles + 1,
                                  0,
                                  TRUE,
                                  DUPLICATE_SAME_ACCESS|DUPLICATE_CLOSE_SOURCE))
            {
                break;
            }
            handles[2] = handles[1];
            rp[1]      = BAD_FD;   /* closed by DuplicateHandle() */
            if (! fw)
            {
                /*
                 * This is a read pipe (only).  Connect child's stdin to
                 * an empty file.  Under no circumstances should the
                 * child's stdin be connected to a device (else lots of
                 * screwy things will occur).  In particular, connecting
                 * the child's stdin to the parent's stdin will cause
                 * aborts and hangs on the various Win32 hosts.  You've
                 * been warned.
                 */

                if ((tmpin_name = _tempnam(getenv("TEMP"), "vile")) == NULL)
                    break;
                if ((tmpin_fd = open(tmpin_name,
                                     O_RDONLY|O_CREAT|O_TRUNC,
                                     _S_IWRITE|_S_IREAD)) == BAD_FD)
                {
                    break;
                }
                handles[0] = (HANDLE) _get_osfhandle(tmpin_fd);
            }
            if (! (*fr = fdopen(rp[0], "r")))
                break;
        }
        if (fw)
        {
            *fw = NULL;

            /*
             * Open (child's) output pipe in binary mode, which will
             * prevent translation of the parent's CR/LF record delimiters
             * to NL.  Apparently, many apps want those delimiters :-) .
             */
            if (_pipe(wp, PIPESIZ, O_BINARY|O_NOINHERIT) == -1)
                break;
            if (! DuplicateHandle(GetCurrentProcess(),
                                  (HANDLE) _get_osfhandle(wp[0]),
                                  GetCurrentProcess(),
                                  handles + 0,
                                  0,
                                  TRUE,
                                  DUPLICATE_SAME_ACCESS|DUPLICATE_CLOSE_SOURCE))
            {
                break;
            }
            wp[0] = BAD_FD;     /* closed by DuplicateHandle() */
            if (! fr)
                handles[1] = handles[2] = GetStdHandle(STD_OUTPUT_HANDLE);
            if (! (*fw = fdopen(wp[1], "w")))
                break;
        }
        rc = (exec_shell(cmd,
                         handles,
                         fr != NULL  /* Child wdw behind unless write pipe. */
                         ) == BAD_PROC_HANDLE) ? FALSE : TRUE;
        if (fw)
        {
            if (! rc)
            {
                 /* Shell process failed, put complaint in user's face. */

                fputc('\n', stdout);
                printf(SHELL_ERR_MSG, shell);
                fflush(stdout);
            }
            CloseHandle(handles[0]);
        }
        if (fr)
        {
            if (! rc)
            {
                char  buf[200];
                DWORD dummy, len;

                /* Shell process failed, put complaint in user's buffer. */

                len = _snprintf(buf, sizeof(buf), SHELL_ERR_MSG, shell);
                (void) WriteFile(handles[1], buf, len, &dummy, NULL);
                FlushFileBuffers(handles[1]);
            }
            CloseHandle(handles[1]);
            if (tmpin_fd != BAD_FD)
                close(tmpin_fd);
        }
        return (rc);
    }
    while (FALSE);

    /* If we get here -- some operation has failed.  Clean up. */

    if (wp[0] != BAD_FD)
        close(wp[0]);
    if (wp[1] != BAD_FD)
        close(wp[1]);
    if (rp[0] != BAD_FD)
        close(rp[0]);
    if (rp[1] != BAD_FD)
        close(rp[1]);
    if (tmpin_fd != BAD_FD)
        close(tmpin_fd);
    for (i = 0; i < 3; i++)
    {
        if (handles[i] != INVALID_HANDLE_VALUE)
            CloseHandle(handles[i]);
    }
    global_cleanup();
    return (FALSE);
}



void
w32_npclose(FILE *fp)
{
    int term_status;

    (void) fflush(fp);
    (void) fclose(fp);
    if (proc_handle != BAD_PROC_HANDLE)
    {
        (void) _cwait(&term_status, (int) proc_handle, 0);
        (void) CloseHandle(proc_handle);
        proc_handle = BAD_PROC_HANDLE;
    }
    global_cleanup();
#ifdef DISP_NTWIN
    FreeConsole();
#endif
}



#define     HOST_95    0
#define     HOST_NT    1
#define     HOST_UNDEF (-1)
static int  host_type = HOST_UNDEF; /* nt or 95? */

static void
set_host(void)
{
    OSVERSIONINFO info;

    info.dwOSVersionInfoSize = sizeof(info);
    GetVersionEx(&info);
    host_type = (info.dwPlatformId == VER_PLATFORM_WIN32_NT) ?
                HOST_NT : HOST_95;
}


int
is_winnt(void)
{
    if (host_type == HOST_UNDEF)
        set_host();
    return (host_type == HOST_NT);
}

int
is_win95(void)
{
    if (host_type == HOST_UNDEF)
        set_host();
    return (host_type == HOST_95);
}