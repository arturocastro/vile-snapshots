@echo off
rem $Header: /users/source/archives/vile.vcs/filters/RCS/mk-2nd.bat,v 1.4 2000/08/08 22:01:02 tom Exp $
rem like mk-2nd.awk, used to generate rules from genmake.mak

goto %2

:compile_c
echo %4$o :
echo 	$(cc) -Dfilter_def=define_%3 $(CFLAGS) -c %4.c -Fo$@
echo.
goto done

:compile_l
rem The odd "LEX.%3_.c" happens to be what flex generates.
echo %4$o :
echo 	$(LEX) -P%3_ %4.l
echo 	$(CC) -Dfilter_def=define_%3 $(CFLAGS) -c LEX.%3_.c -Fo$@
echo 	- erase LEX.%3_.c
echo.
goto done

:link_c
echo vile-%3-filt$x : %4$o $(CF_DEPS)
echo 	$(link) -out:$@ $(CON_LDFLAGS) %4$o $(CF_ARGS)
echo.
goto done

:link_l
echo vile-%3-filt$x : %4$o $(LF_DEPS)
echo 	$(link) -out:$@ $(CON_LDFLAGS) %4$o $(LF_ARGS)
echo.
goto done

:extern
echo # generated by %0.bat
genmake.exe -x%0 ". link_%%k %%i %%j %%k" <%1
goto done

:intern
echo # generated by %0.bat
rem library rule is in makefile.wnt, since we cannot echo redirection
rem chars needed for "inline" (aka here-document).
genmake.exe -x%0 ". compile_%%k %%i %%j %%k" <%1
goto done

:done
