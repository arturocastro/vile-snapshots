/*
 * vile:notabinsert sw=4:
 *
 * wvwrap.cpp:  A WinVile WRAPper .
 *
 * Originally written by Ed Henderson.
 *
 * This wrapper may be used to open one or more files via a right mouse
 * click in the Windows Explorer.  For more details, please read
 * doc/oleauto.doc .
 *
 * Note:  A great deal of the code included in this file is copied
 * (almost verbatim) from other vile modules.
 *
 * $Header: /users/source/archives/vile.vcs/RCS/wvwrap.cpp,v 1.14 2007/05/28 15:24:04 tom Exp $
 */

#include "w32vile.h"

#include <objbase.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include <initguid.h>

#include "w32reg.h"
#include "w32ole.h"
#include "vl_alloc.h"
#include "makeargv.h"

#define DQUOTE '"'
#define SQUOTE '\''

static size_t   olebuf_len;    /* scaled in wchar_t */
static OLECHAR  *olebuf;

#ifdef OPT_TRACE
static void
Trace(const char *fmt, ...)
{
    FILE *fp = fopen("c:\\temp\\wvwrap.log", "a");
    if (fp != 0)
    {
        va_list ap;
        va_start(ap, fmt);
        vfprintf(fp, fmt, ap);
        va_end(ap);
        fclose(fp);
    }
}
#define TRACE(params) Trace params
#else
#define OPT_TRACE 0
#define TRACE(params) /* nothing */
#endif

//--------------------------------------------------------------

/* from w32misc.c */
static char *
fmt_win32_error(ULONG errcode, char **buf, ULONG buflen)
{
    int flags = FORMAT_MESSAGE_FROM_SYSTEM;

    if (! *buf)
        flags |= FORMAT_MESSAGE_ALLOCATE_BUFFER;
    FormatMessage(flags,
                  NULL,
                  errcode,
                  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), /* dflt language */
                  (*buf) ? *buf : (LPTSTR) buf,
                  buflen,
                  NULL);
    return (*buf);
}

static int
nomem(void)
{
    char buf[512], *tmp;

    tmp = buf;
    fmt_win32_error((ULONG) E_OUTOFMEMORY, &tmp, 0);
    MessageBox(NULL, buf, "wvwrap", MB_OK|MB_ICONSTOP);
    return (1);
}

#if (! defined(_UNICODE) || defined(UNICODE))
/*
 * Quick & Dirty Unicode conversion routine.  Routine uses a
 * dynamic buffer to hold the converted string so it may be any arbitrary
 * size.  However, the same dynamic buffer is reused when the routine is
 * called a second time.  So make sure that the converted string is
 * used/copied before the conversion routine is called again.
 *
 *
 * from w32ole.cpp
 */
static OLECHAR *
ConvertToUnicode(char *szA)
{
    size_t len;

    len = strlen(szA) + 1;
    if (len > olebuf_len)
    {
        if (olebuf)
            free(olebuf);
        olebuf_len = olebuf_len * 2 + len;
        if ((olebuf = typeallocn(OLECHAR, olebuf_len)) == NULL)
            return (olebuf);  /* We're gonna' die */
    }
    mbstowcs(olebuf, szA, len);
    return (olebuf);
}
#endif

/*
 * vile's prompts for directory/filename do not expect to strip quotes; they
 * use the strings exactly as given.  At the same time, the initial "cd" and
 * "e" commands use the token parsing which stops on a blank.  That makes it
 * not simple to use OLE to send characters to the server to specify filenames
 * containing blanks.
 *
 * We work around the problem using the "eval" command, which forces a reparse
 * of the whole line.  That uses an extra level of interpretation, so we have
 * to double each single quote _twice_ to enter single quotes in the filename.
 *
 * Dollar-signs introduce another twist: vile attempts to expand the associated
 * variables unless the $'s are escaped.  In the globbing, all of the backslashes
 * count as escapes.
 *
 * Obscure, but it works.
 */
static char *
escape_quotes(const char *src)
{
    const char *must_quote = "${}";
    size_t len = 4 * strlen(src) + 1;
    char *result = typeallocn(char, len);

    if (result == 0)
        exit (nomem());

    char *dst = result;
    bool escape = (strcspn(src, must_quote) != strlen(src));
    while (*src != '\0')
    {
        if (*src == SQUOTE)
        {
            *dst++ = SQUOTE;
            *dst++ = SQUOTE;
            *dst++ = SQUOTE;
        }
        else if (strchr(must_quote, *src) != 0)
        {
            *dst++ = '\\';
        }
        else if (*src == '\\' && escape)
        {
            *dst++ = '\\';
        }
        *dst++ = *src++;
    }
    *dst = '\0';

    return result;
}


static void
append(char *&buffer, size_t &length, char *value)
{
    size_t newsize = strlen(buffer) + strlen(value);
    if ((newsize + 1) > length)
    {
        length = (newsize + 1) * 2;
        buffer = (char *) realloc(buffer, length);
    }
    strcat(buffer, value);
}


int WINAPI
WinMain( HINSTANCE hInstance,      // handle to current instance
         HINSTANCE hPrevInstance,  // handle to previous instance
         LPSTR     lpCmdLine,      // pointer to command line
         int       nCmdShow        // show state of window
  )
{
    BSTR         bstr;
    size_t       dynbuf_len;
    HRESULT      hr;
    HWND         hwnd;
    VARIANT_BOOL insert_mode, minimized;
    char         *lclbuf = NULL, tmp[512], *dynbuf;
    OLECHAR      *olestr;
    LPUNKNOWN    punk;
    IVileAuto    *pVileAuto;
    char         **argv;
    int          argc;

    if (make_argv(0, lpCmdLine, &argv, &argc, NULL) < 0)
        return (nomem());

#if OPT_TRACE
    Trace("cmdline:%s\n", lpCmdLine);
    for (int n = 0; n < argc; ++n)
        Trace("argv%d:%s\n", n, argv[n]);
#endif

    olebuf_len = 4096;
    dynbuf_len = 4096;
    olebuf     = typeallocn(OLECHAR, olebuf_len);
    dynbuf     = typeallocn(char, dynbuf_len);
    if (! (olebuf && dynbuf))
        return (nomem());

    hr = CoInitialize(NULL); // Fire up COM.
    if (FAILED(hr))
    {
        fmt_win32_error(hr, &lclbuf, 0);
        sprintf(tmp,
                "ERROR: CoInitialize() failed, errcode: %#lx, desc: %s",
                hr,
                lclbuf);
        MessageBox(NULL, tmp, "wvwrap", MB_OK|MB_ICONSTOP);
        return (1);
    }
    hr = CoCreateInstance(CLSID_VileAuto,
                          NULL,
                          CLSCTX_LOCAL_SERVER,
                          IID_IUnknown,
                          (void **) &punk);
    if (FAILED(hr))
    {
        fmt_win32_error(hr, &lclbuf, 0);
        sprintf(tmp,
                "ERROR: CoInitialize() failed, errcode: %#lx, desc: %s\n"
                "Try registering winvile via its -Or switch.",
                hr,
                lclbuf);
        MessageBox(NULL, tmp, "wvwrap", MB_OK|MB_ICONSTOP);
        return (1);
    }
    hr = punk->QueryInterface(IID_IVileAuto, (void **) &pVileAuto);
    punk->Release();
    if (FAILED(hr))
    {
        fmt_win32_error(hr, &lclbuf, 0);
        sprintf(tmp,
                "ERROR: QueryInterface() failed, errcode: %#lx, desc: %s\n\n",
                hr,
                lclbuf);
        MessageBox(NULL, tmp, "wvwrap", MB_OK|MB_ICONSTOP);
        return (1);
    }
    pVileAuto->put_Visible(VARIANT_TRUE);  // May not be necessary
    pVileAuto->get_InsertMode(&insert_mode);
    if (argc > 0)
    {
        char *cp;

        *dynbuf = '\0';

        /*
         * When wvwrap starts up (and subsequently launches winvile), the
         * editor's CWD is set to a path deep within the bowels of Windows.
         * Not very useful.  Change CWD to the directory of the first
         * file on the commandline.
         */
        cp = strrchr(*argv, '\\');
        if (cp)
        {
            int add_delim;

            if (insert_mode)
                append(dynbuf, dynbuf_len, "\033");

#if OPT_TRACE
            /*
             * Turn on tracing in the server, for debugging.
             */
            append(dynbuf, dynbuf_len, ":setv $debug=true\n");
#endif

            *cp = '\0';
            if (cp == *argv)
            {
                /* filename is of the form:  \<leaf> .  handle this. */
                append(dynbuf, dynbuf_len, ":cd \\\n");
            }
            else
            {
                 /*
                  * To add insult to injury, Windows Explorer has a habit
                  * of passing 8.3 filenames to wvwrap (noted on a win2K
                  * system and a FAT32 partition).  If the folder portion
                  * of the file's path happens to be in 8.3 format (i.e.,
                  * a tilde included in the folder name), then 8.3 folder
                  * names appear in winvile's Recent Folders list.
                  * Needless to say, it's no fun trying to decipher 8.3
                  * folder names.
                  */

                char folder[FILENAME_MAX], *fp;

                if (GetLongPathName(*argv, folder, sizeof(folder)) > 0)
                    fp = folder;
                else
                    fp = *argv;

                add_delim = (isalpha(fp[0]) && fp[1] == ':' && fp[2] == '\0');

                /*
                 * With regard to the following code, note that the
                 * original file might be in the form "<drive>:\leaf", in
                 * which case *argv now points at "<drive>:" .  Recall that
                 * cd'ing to <drive>:  on a DOS/WIN32 host has special
                 * semantics (which we don't want).
                 */
                char temp[4096];
                sprintf(temp, ":eval cd '%s%s'\n",
                        escape_quotes(fp),
                        (add_delim) ? "\\" : "");

                append(dynbuf, dynbuf_len, temp);
            }
            * cp = '\\';
        }

        while (argc--)
        {
            char temp[4096];
            sprintf(temp, ":eval e '%s'\n", escape_quotes(*argv++));
            append(dynbuf, dynbuf_len, temp);
        }

        TRACE(("dynbuf:\n%s\n", dynbuf));
        olestr = TO_OLE_STRING(dynbuf);
        bstr   = SysAllocString(olestr);
        if (! (bstr && olestr))
            return (nomem());
        pVileAuto->VileKeys(bstr);
        SysFreeString(bstr);
    }

    // Set foreground window using a method that's compatible with win2k
    pVileAuto->get_MainHwnd((LONG *) &hwnd);
    (void) SetForegroundWindow(hwnd);

    // If editor minimized, restore window
    hr = pVileAuto->get_IsMinimized(&minimized);
    if (SUCCEEDED(hr) && minimized)
        hr = pVileAuto->Restore();
    pVileAuto->Release();
    CoUninitialize(); // shut down COM
    return (0);
}
