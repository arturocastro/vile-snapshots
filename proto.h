/*
 *   This file was automatically generated by cextract version 1.2.
 *   Manual editing now recommended, since I've done a whole lot of it.
 *
 *   Created: Thu May 14 15:44:40 1992
 *
 * $Header: /users/source/archives/vile.vcs/RCS/proto.h,v 1.237 1997/03/30 22:19:47 tom Exp $
 *
 */

extern int main (int argc, char **argv);
extern char *strmalloc (const char *s);
extern char *strend (const char *s);
extern void tidy_exit (int code);
extern int no_memory (const char *s);
extern SIGT catchintr (int ACTUAL_SIG_ARGS);
#ifndef interrupted
extern int interrupted (void);
#endif
extern void not_interrupted (void);
extern void do_repeats (int *cp, int *fp, int *np);
extern int writeall (int f, int n, int promptuser, int leaving, int autowriting);
extern int rdonly (void);
extern void charinit (void);
#if OPT_RAMSIZE
extern char *reallocate (char *mp, unsigned nbytes);
extern char *allocate (unsigned nbytes);
extern void release (char *mp);
#endif
extern char *strncpy0 (char *t, const char *f, SIZE_T l);
extern void setup_handler (int sig, void (*disp) (int ACTUAL_SIG_ARGS));

/* screen-drivers */
#if OPT_XTERM >= 3
extern int xterm_mouse_t (int f, int n);
extern int xterm_mouse_T (int f, int n);
#endif

/* basic.c */
extern int firstchar (LINE *lp);
extern int nextchar (LINE *lp, int off);
extern int lastchar (LINE *lp);
extern int getgoal (LINE *dlp);
extern int next_column (int c, int col);
extern int gonmmark (int c);
extern int setmark (void);
extern void swapmark (void);
#if OPT_MOUSE
extern	int setwmark (int row, int col);
extern	int setcursor (int row, int col);
#endif
#if SMALLER	/* cancel 'neproto.h' */
extern int gotobob (int f, int n);
extern int gotoeob (int f, int n);
#endif

/* bind.c */
extern int no_such_function (const char *fname);
extern int startup (const char *sfname);
extern const char *flook (const char *fname, int hflag);
extern char *kcod2pstr (int c, char *seq);
extern int kcod2escape_seq (int c, char *ptr);
extern int fnc2kcod (const CMDFUNC *);
#if DISP_X11
extern char *fnc2pstr (const CMDFUNC *f);
#endif
extern const CMDFUNC *engl2fnc (const char *fname);
extern const CMDFUNC *kcod2fnc (int c);
#if OPT_EVAL
extern const char *prc2engl (const char *skey);
#endif
extern char *kbd_engl (const char *prompt, char *buffer);
extern void kbd_alarm (void);
extern void kbd_putc (int c);
extern void kbd_puts (const char *s);
extern void kbd_erase (void);
extern void kbd_init (void);
extern void kbd_unquery (void);
extern int kbd_complete (int case_insensitive, int c, char *buf, int *pos, const char *table, SIZE_T size_entry);
extern int kbd_engl_stat (const char *prompt, char *buffer);
extern void popdown_completions (void);

/* buffer.c */
extern WINDOW *bp2any_wp (BUFFER *bp);
extern void imply_alt (const char *fname, int copy, int lockfl);
extern BUFFER *find_b_file (const char *fname);
extern BUFFER *find_alt (void);
extern BUFFER *find_bp (BUFFER *bp1);
extern void make_current (BUFFER *nbp);
extern int swbuffer (BUFFER *bp);
extern int swbuffer_lfl (BUFFER *bp, int lockfl);
extern void undispbuff (BUFFER *bp, WINDOW *wp);
extern int tabstop_val (BUFFER *bp);
extern int shiftwid_val (BUFFER *bp);
extern int has_C_suffix (BUFFER *bp);
extern int delink_bp (BUFFER *bp);
extern int zotbuf (BUFFER *bp);
extern int zotwp (BUFFER *bp);
extern BUFFER *find_any_buffer (const char *name);
extern int popupbuff (BUFFER *bp);
extern void sortlistbuffers (void);
#if OPT_UPBUFF
void updatelistbuffers (void);
void update_scratch (const char *name, int (*func)(BUFFER *));
#else
#define updatelistbuffers()
#define update_scratch(name, func)
#endif
extern int addline (BUFFER *bp, const char *text, int len);
extern int add_line_at (BUFFER *bp, LINEPTR prevp, const char *text, int len);
extern int any_changed_buf (BUFFER **bpp);
extern int any_unread_buf (BUFFER **bpp);
extern void set_bname (BUFFER *bp, const char *name);
#if BEFORE
extern char *get_bname (BUFFER *bp);
#endif
extern BUFFER *find_b_name (const char *name);
extern BUFFER *bfind (const char *bname, int bflag);
BUFFER *make_bp (const char *fname, int flags);
extern int bclear (BUFFER *bp);
extern int bsizes (BUFFER *bp);
extern void chg_buff (BUFFER *bp, int flag);
extern void unchg_buff (BUFFER *bp, int flag);
extern BUFFER *getfile2bp (const char *fname, int ok_to_ask, int cmdline);

/* crypt.c */
#if	OPT_ENCRYPT
extern	int	ue_makekey (char *key, ALLOC_T len);
extern	void	ue_crypt (char *bptr, ALLOC_T len);
#endif	/* OPT_ENCRYPT */

/* csrch.c */

/* display.c */
extern int nu_width (WINDOW *wp);
extern int col_limit (WINDOW *wp);
extern int vtinit (void);
extern int video_alloc (VIDEO **vpp);
extern int update (int force);
extern void upmode (void);
extern int offs2col (WINDOW *wp, LINEPTR lp, C_NUM offset);
#if OPT_MOUSE || defined(WMDLINEWRAP)
extern int col2offs (WINDOW *wp, LINEPTR lp, C_NUM col);
#endif
#ifdef WMDLINEWRAP
extern int line_height (WINDOW *wp, LINEPTR lp);
#else
#define line_height(wp,lp) 1
#endif
#if defined(WMDLINEWRAP) || OPT_MOUSE
extern WINDOW *row2window (int row);
#endif
extern void hilite (int row, int colfrom, int colto, int on);
extern void movecursor (int row, int col);
extern void bottomleft (void);
extern void mlerase (void);
extern void mlsavec (int c);
extern void mlwrite (const char *fmt, ...);
extern void mlforce (const char *fmt, ...);
extern void mlprompt (const char *fmt, ...);
extern void mlerror (const char *s);
extern void mlwarn (const char *fmt, ...);
extern void dbgwrite (const char *fmt, ...);
extern char *lsprintf (char *buf, const char *fmt, ...);
extern void bputc (int c);
extern void bprintf (const char *fmt, ...);
#if !DISP_X11
extern void getscreensize (int *widthp, int *heightp);
#if defined(SIGWINCH)
extern SIGT sizesignal (int ACTUAL_SIG_ARGS);
#endif
#endif
extern void newscreensize (int h, int w);
extern int im_waiting (int flag);
#if OPT_WORKING
extern SIGT imworking (int ACTUAL_SIG_ARGS);
#endif
#if defined(SIGWINCH) || OPT_WORKING
extern void beginDisplay (void);
extern void endofDisplay (void);
#endif
#if OPT_PSCREEN
extern	OUTC_DCL psc_putchar	(OUTC_ARGS);
extern	void	psc_flush	(void);
extern	void	psc_move	(int row, int col);
extern	void	psc_eeol	(void);
extern	void	psc_eeop	(void);
extern	void	psc_rev		(int huh);
#endif	/* OPT_PSCREEN */

/* eval.c */
extern char *l_itoa (int i);
extern int absol (int x);
extern int is_truem (const char *val);
extern int is_falsem (const char *val);
#if OPT_EVAL || DISP_X11
extern int stol (const char *val);
#endif
#if OPT_EVAL
extern const char *gtenv (const char *vname);
#endif
#if OPT_EVAL || !SMALLER
extern char *mkupper (char *str);
#endif
extern char *mklower (char *str);
extern char *mktrimmed (char *str);
#if OPT_EVAL
extern int set_variable (char *name);
#endif
#if OPT_EVAL || OPT_COLOR
extern int set_palette (const char *value);
#endif

/* exec.c */
extern int end_named_cmd (void);
extern int more_named_cmd (void);
extern int dobuf (BUFFER *bp);
extern int dofile (const char *fname);
extern int execute (const CMDFUNC *execfunc, int f, int n);
#if OPT_PROCEDURES
extern int run_procedure (const char *name);
#endif

/* file.c */
extern time_t file_modified (const char *path);
#ifdef MDCHK_MODTIME
extern int ask_shouldchange (BUFFER *bp);
extern int get_modtime (BUFFER *bp, time_t *the_time);
extern void set_modtime (BUFFER *bp, const char *fn);
extern int check_modtime (BUFFER *bp, const char *fn);
extern int check_visible_modtimes (void);
#endif
extern void set_last_file_edited(const char *);
extern int no_such_file (const char *fname);
extern int same_fname (const char *fname, BUFFER *bp, int lengthen);
extern int getfile (const char *fname, int lockfl);
extern int readin (const char *fname, int lockfl, BUFFER *bp, int mflg);
extern int bp2readin (BUFFER *bp, int lockfl);
extern int slowreadf (BUFFER *bp, int *nlinep);
extern void makename (char *bname, const char *fname);
extern void unqname (char *name);
extern int writeout (const char *fn, BUFFER *bp, int forced, int msgf);
extern int writeregion (void);
extern int kwrite (const char *fn, int msgf);
extern int ifile (const char *fname, int belowthisline, FILE *haveffp);
extern SIGT imdying (int ACTUAL_SIG_ARGS);
extern void markWFMODE (BUFFER *bp);
#if OPT_ENCRYPT
extern int resetkey (BUFFER *bp, const char *fname);
#endif
#if SMALLER	/* cancel neproto.h */
extern int filesave (int f, int n);
#endif

/* filec.c */
#if COMPLETE_FILES || COMPLETE_DIRS
extern int path_completion (DONE_ARGS);
extern void init_filec(const char *buffer_name);
#endif
extern int mlreply_file (const char *prompt, TBUFF **buf, int flag, char *result);
extern int mlreply_dir (const char *prompt, TBUFF **buf, char *result);
extern char *filec_expand (void);

/* fileio.c */
extern int ffropen (const char *fn);
extern int ffwopen (const char *fn, int forced);
extern int ffaccess (const char *fn, int mode);
extern int ffronly (const char *fn);
extern off_t ffsize (void);
extern int ffexists (const char *p);
#if !(SYS_MSDOS || SYS_WIN31)
extern int ffread (char *buf, long len);
extern void ffseek (long n);
extern void ffrewind (void);
#endif
extern int ffclose (void);
extern int ffputline (const char *buf, int nbuf, const char *ending);
extern int ffputc (int c);
extern int ffhasdata (void);

/* finderr.c */
#if OPT_FINDERR
extern void set_febuff (const char *name);
#endif

/* glob.c */
#if !SYS_UNIX
extern	int	glob_needed (char **list_of_items);
#endif
extern	char **	glob_string (char *item);
extern	int	glob_length (char **list_of_items);
extern	char **	glob_free   (char **list_of_items);

#if !SYS_UNIX
extern	void	expand_wild_args (int *argcp, char ***argvp);
#endif

extern	int	doglob (char *path);

/* globals.c */

/* history.c */
#if OPT_HISTORY
extern void hst_init (int c);
extern void hst_glue (int c);
extern void hst_append (char *cmd, int glue);
extern void hst_remove (const char *cmd);
extern void hst_flush (void);
extern int edithistory (char *buffer, int *position, int *given, int options, int (*func)(EOL_ARGS), int eolchar);
#else
#define hst_init(c)
#define hst_glue(c)
#define hst_append(p,c)
#define hst_remove(p)
#define hst_flush()
#endif

/* input.c */
extern int no_completion (DONE_ARGS);
#if COMPLETE_FILES
extern int shell_complete (DONE_ARGS);
#else
#define shell_complete no_completion
#endif
extern int mlyesno (const char *prompt);
extern int mlquickask (const char *prompt, const char *respchars, int *cp);
extern int mlreply (const char *prompt, char *buf, int bufn);
extern int mlreply_reg (const char *prompt, char *cbuf, int *retp, int at_dft);
extern int mlreply_reg_count (int state, int *retp, int *next);
extern int mlreply_no_bs (const char *prompt, char *buf, int bufn);
extern int mlreply_no_opts (const char *prompt, char *buf, int bufn);
extern void incr_dot_kregnum (void);
extern int mapped_keystroke (void);
extern int keystroke (void);
extern int keystroke8 (void);
extern int keystroke_raw8 (void);
extern int keystroke_avail (void);
extern void unkeystroke (int c);
extern int tgetc (int quoted);
extern int tgetc_avail (void);
extern int get_recorded_char (int eatit);
extern int kbd_seq (void);
extern int screen_string (char *buf, int bufn, CHARTYPE inclchartype);
extern int end_string (void);
extern void set_end_string (int c);
extern int kbd_delimiter (void);
extern int is_edit_char (int c);
extern void kbd_kill_response (char *buf, int *position, int c);
extern int kbd_show_response (char *dst, char *src, int bufn, int eolchar, int options);
extern int kbd_is_pushed_back (void);
extern void kbd_pushback (char *buffer, int skip);
extern int kbd_string (const char *prompt, char *extbuf, int bufn, int eolchar, int options, int (*func)(DONE_ARGS));
extern int kbd_reply (const char *prompt, char *extbuf, int bufn, int (*efunc)(EOL_ARGS), int eolchar, int options, int (*cfunc)(DONE_ARGS));
extern int dotcmdbegin (void);
extern int dotcmdfinish (void);
extern void dotcmdstop (void);
extern int kbd_replaying (int match);
extern int kbm_started (int macnum, int force);
extern int start_kbm (int n, int macnum, ITBUFF *ptr);

/* insert.c */
extern int ins_mode (WINDOW *wp);
extern int ins (void);
extern int inschar (int c, int *backsp_limit_p);
extern int previndent (int *bracefp);
extern int indentlen (LINE *lp);
#if OPT_EVAL
extern const char *current_modename (void);
#endif
#if SMALLER	/* cancel 'neproto.h' */
extern int newline (int f, int n);
extern int wrapword (int f, int n);
#endif

/* isearch.c */
#if SMALLER	/* cancel 'neproto.h' */
extern int forwhunt (int f, int n);
extern int backhunt (int f, int n);
#endif

/* lckfiles.c */
#if OPT_LCKFILES
extern int set_lock (const char *fname, char *who, int n);
extern void release_lock (const char *fname);
#endif

/* line.c */
extern int do_report (L_NUM value);
extern LINEPTR lalloc (int used, BUFFER *bp);
extern void lfree (LINEPTR lp, BUFFER *bp);
extern void ltextfree (LINE *lp, BUFFER *bp);
extern void lremove (BUFFER *bp, LINEPTR lp);
#if SMALLER	/* cancel neproto.h */
extern int insspace (int f, int n);
#endif
extern int lstrinsert (const char *s, int len);
extern int linsert (int n, int c);
extern int lnewline (void);
extern int ldelete (long n, int kflag);
#if OPT_EVAL
extern char * getctext (CHARTYPE type);
extern int putctext (const char *iline);
#endif
extern void ksetup (void);
extern void kdone (void);
extern int kinsertlater (int c);
extern int kinsert (int c);
extern int index2reg (int c);
extern int reg2index (int c);
extern int index2ukb (int inx);
extern void kregcirculate (int killing);

/* map.c */
extern void addtosysmap (const char *seq, int seqlen, int code);
extern int sysmapped_c (void);
extern int sysmapped_c_avail (void);
extern void mapungetc (int c);
extern int mapped_c (int remap, int raw);
extern int mapped_c_avail (void);
extern void abbr_check (int *backsp_limit_p);

/* msgs.c */
#if OPT_POPUP_MSGS
void msg_putc (int c);
void popup_msgs (void);
void purge_msgs (void);
#endif

/* modes.c */
extern int string_to_number (const char *from, int *np);
extern int adjvalueset (const char *cp, int setting, int global, VALARGS *args);
extern const char *string_mode_val (VALARGS *args);
extern REGEXVAL *new_regexval (const char *pattern, int magic);
extern void copy_mvals (int maximum, struct VAL *dst, struct VAL *src);
#if OPT_UPBUFF
extern void save_vals (int maximum, struct VAL *gbl, struct VAL *dst, struct VAL *src);
#endif
extern void free_local_vals (const struct VALNAMES *names, struct VAL *gbl, struct VAL *val);
extern int find_mode (const char *mode, int global, VALARGS *args);
extern int mode_eol (EOL_ARGS);

/* npopen.c */
#if SYS_UNIX || SYS_MSDOS || SYS_WIN31 || SYS_OS2 || SYS_WINNT
extern FILE * npopen (const char *cmd, const char *type);
extern void npclose (FILE *fp);
extern int inout_popen (FILE **fr, FILE **fw, const char *cmd);
extern int softfork (void);
#endif
#if SYS_UNIX || SYS_OS2 || SYS_WINNT
extern int system_SHELL (const char *cmd);
#endif
#if SYS_MSDOS || SYS_WIN31 || (SYS_OS2 && CC_CSETPP) || TEST_DOS_PIPES
extern void npflush (void);
#endif

/* oneliner.c */
extern int llineregion (void);
extern int plineregion (void);
extern int pplineregion (void);
extern int substregion (void);
extern int subst_again_region (void);

/* opers.c */
extern int operator (int f, int n, OpsFunc fn, const char *str);

/* path.c */
#if OPT_MSDOS_PATH
extern char * is_msdos_drive (const char *path);
#endif
#if OPT_VMS_PATH
extern int is_vms_pathname (const char *path, int option);
extern char * vms_pathleaf (const char *path);
extern char * unix_pathleaf (const char *path);
#endif
#if SYS_UNIX
extern char * home_path (char *path);
#endif
extern char * pathleaf (const char *path);
extern char * pathcat (char *dst, const char *path, const char *leaf);
extern char * last_slash (const char *fn);
extern char * shorten_path (char *path, int keep_cwd);
extern char * lengthen_path (char *path);
extern int is_pathname (const char *path);
extern int maybe_pathname (const char *fn);
extern char * is_appendname (const char *fn);
extern int is_internalname (const char *fn);
extern int is_scratchname (const char *fn);
extern int is_directory (const char *path);
#if (SYS_UNIX||SYS_VMS||OPT_MSDOS_PATH) && OPT_PATHLOOKUP
extern const char *parse_pathlist (const char *list, char *result);
#endif
#if OPT_MSDOS_PATH
extern char *sl_to_bsl (const char *p);
extern void bsl_to_sl_inplace (char *p);
#endif
#if OPT_CASELESS && SYS_OS2
extern int is_case_preserving (const char *name);
#endif

/* random.c */
extern int line_report (L_NUM before);
extern L_NUM line_count (BUFFER *the_buffer);
extern L_NUM line_no (BUFFER *the_buffer, LINEPTR the_line);
#if OPT_EVAL
extern L_NUM getcline (void);
#endif
extern void set_rdonly (BUFFER *bp, const char *name, int mode);
extern int liststuff (const char *name, int appendit, void (*)(LIST_ARGS), int iarg, void *vargp);
extern int getccol (int bflg);
extern int getcol (MARK mark, int actual);
extern int getoff (C_NUM goal, C_NUM *rcolp);
extern int gocol (int n);
extern int is_user_fence (int ch, int *sdirp);
extern int fmatchindent (int c);
extern void catnap (int milli, int watchinput);
extern const char * current_directory (int force);
#if OPT_EVAL
extern const char * previous_directory (void);
#endif
extern int set_directory (const char *dir);
extern void ch_fname (BUFFER *bp, const char *fname);

/* regexp.c */
extern regexp * regcomp (char *origexp, int magic);
extern int regexec (regexp *prog, char *string, char *stringend, int startoff, int endoff);
extern int lregexec (regexp *prog, LINE *lp, int startoff, int endoff);

/* region.c */
extern int killregion (void);
extern int killregionmaybesave (int save);
extern int openregion (void);
extern int stringrect (void);
extern int shiftrregion (void);
extern int shiftlregion (void);
extern int detabline (void *flagp, int l, int r);
extern int detab_region (void);
extern int entabline (void *flagp, int l, int r);
extern int entab_region (void);
extern int trimline (void *flagp, int l, int r);
extern int trim_region (void);
extern int blank_region (void);
extern int yankregion (void);
extern int flipregion (void);
extern int lowerregion (void);
extern int upperregion (void);
#if NEEDED
extern int charprocreg (int (*)(int));
#endif
extern int getregion (REGION *rp);
extern int get_fl_region (REGION *rp);

/* search.c */
#if OPT_HILITEMATCH
void clobber_save_curbp(BUFFER *bp);
#endif
extern int fsearch (int f, int n, int marking, int fromscreen);
extern int scanner (regexp *exp, int direct, int wrapok, int *wrappedp);
extern void attrib_matches (void);
extern void regerror (const char *s);
#if OPT_EVAL || UNUSED
extern int eq (int bc, int pc);
#endif
extern int readpattern (const char *prompt, char *apat, regexp **srchexpp, int c, int fromscreen);
extern void scanboundry (int wrapok, MARK dot, int dir);
extern int findpat (int f, int n, regexp *exp, int direc);

/* spawn.c */
extern SIGT rtfrmshell (int ACTUAL_SIG_ARGS);
extern void pressreturn (void);
extern int filterregion (void);

/* tags.c */
#if OPT_TAGS
extern int cmdlinetag (const char *t);
#endif /* OPT_TAGS */

/* termio.c */
extern void ttopen (void);
extern void ttclose (void);
extern void ttclean (int f);
extern void ttunclean (void);
extern OUTC_DCL ttputc (OUTC_ARGS);
extern void ttflush (void);
extern int ttgetc (void);
extern int tttypahead (void);
extern int open_terminal (TERM *termp);
extern void null_t_setfor (int f);
extern void null_t_setback (int b);
extern void null_t_setpal (char *p);
extern void null_t_scroll (int f, int t, int n);
extern void null_t_pflush (void);
extern void null_t_icursor (int c);
extern void null_t_title (char *t);

/* undo.c */
extern void toss_to_undo (LINEPTR lp);
extern void copy_for_undo (LINEPTR lp);
extern void tag_for_undo (LINEPTR lp);
extern void nounmodifiable (BUFFER *bp);
extern void freeundostacks (BUFFER *bp, int both);
extern void mayneedundo (void);
extern void dumpuline (LINEPTR lp);

/* version.c */
extern void print_usage (void);
extern const char * getversion (void);
extern const char * non_filename (void);

/* vmspipe.c */
#if SYS_VMS
extern FILE *vms_rpipe (const char *cmd, int fd, const char *input_file);
#endif

/* window.c */
extern int set_curwp (WINDOW *wp);
#if OPT_EVAL
extern int getwpos (void);
#endif
extern int delwp (WINDOW *thewp);
extern void copy_traits (W_TRAITS *dst, W_TRAITS *src);
extern WINDOW * wpopup (void);
extern void shrinkwrap (void);
extern void winit (int screen);
#if SMALLER	/* cancel neproto.h */
extern int reposition (int f, int n);
extern int resize (int f, int n);
#endif

/* word.c */
extern int joinregion (void);
extern int formatregion (void);

extern void setchartype (void);
extern int isnewwordf (void);
extern int isnewwordb (void);
extern int isnewviwordf (void);
extern int isnewviwordb (void);
extern int isendwordf (void);
extern int isendviwordf (void);
extern int toktyp (const char *tokn);
extern const char * tokval (const char *tokn);
extern const char * token (const char *src, char *tok, int eolchar);
extern int ffgetline (int *lenp);
extern int macroize (TBUFF **p, const char *src, const char *ref);
extern int macarg (char *tok);
extern int macliteralarg (char *tok);
extern void fmatch (int rch);

/* tbuff.c */
TBUFF *	tb_alloc (TBUFF **p, ALLOC_T n);
TBUFF *	tb_init (TBUFF **p, int c);
void	tb_free (TBUFF **p);
void	tb_stuff (TBUFF *p, int c);
int	tb_get (TBUFF *p, ALLOC_T n);
void	tb_unput (TBUFF *p);
TBUFF *	tb_append (TBUFF **p, int c);
TBUFF *	tb_copy (TBUFF **d, TBUFF *s);
TBUFF *	tb_bappend (TBUFF **p, const char *s, ALLOC_T len);
TBUFF *	tb_sappend (TBUFF **p, const char *s);
TBUFF *	tb_scopy (TBUFF **p, const char *s);
void	tb_first (TBUFF *p);
int	tb_more (TBUFF *p);
int	tb_next (TBUFF *p);
void	tb_unnext (TBUFF *p);
int	tb_peek (TBUFF *p);
char *	tb_values (TBUFF *p);
ALLOC_T	tb_length (TBUFF *p);

/* itbuff.c */
ITBUFF * itb_alloc (ITBUFF **p, ALLOC_T n);
ITBUFF * itb_init (ITBUFF **p, int c);
void	 itb_free (ITBUFF **p);
void	 itb_stuff (ITBUFF *p, int c);
int	 itb_get (ITBUFF *p, ALLOC_T n);
#if NEEDED
void	 itb_unput (ITBUFF *p);
#endif
ITBUFF * itb_append (ITBUFF **p, int c);
ITBUFF * itb_copy (ITBUFF **d, ITBUFF *s);
ITBUFF * itb_bappend (ITBUFF **, const char *s, ALLOC_T len);
ITBUFF * itb_sappend (ITBUFF **, const char *s);
#if NEEDED
void	 itb_delete (ITBUFF	*p, ALLOC_T cnt);
ITBUFF * itb_insert (ITBUFF	**p, int c);
#endif
void	 itb_first (ITBUFF *p);
int	 itb_more (ITBUFF *p);
int	 itb_next (ITBUFF *p);
int	 itb_last (ITBUFF *p);
#if NEEDED
void	 itb_unnext (ITBUFF *p);
#endif
	
int	 itb_peek (ITBUFF *p);
ALLOC_T	 itb_length (ITBUFF *p);

#if NO_LEAKS
extern	void bind_leaks (void);
extern	void onel_leaks (void);
extern	void path_leaks (void);
extern	void kbs_leaks (void);
extern	void map_leaks (void);
extern	void itb_leaks (void);
extern	void tb_leaks (void);
extern	void wp_leaks (void);
extern	void bp_leaks (void);
extern	void vt_leaks (void);
extern	void ev_leaks (void);
#endif

#if DISP_X11
#if XTOOLKIT
extern	void	own_selection		(void);
extern	void	update_scrollbar	(WINDOW *uwp);
#else	/* !XTOOLKIT */
extern	void	x_set_rv		(void);
extern	void	x_setname		(char *name);
extern	void	x_set_wm_title		(char *name);
extern	void	x_setforeground		(char *colorname);
extern	void	x_setbackground		(char *colorname);
extern  void	x_set_geometry		(char *g);
extern	void	x_set_dpy		(char *dn);
extern	int	x_key_events_ready	(void);
#endif	/* !XTOOKIT */
extern	int	x_setfont		(const char *fname);
extern	char *	x_current_fontname	(void);
extern	void	x_preparse_args		(int *pargc, char ***pargv);
extern	int	x_typahead		(int milli);
extern	void	x_move_events		(void);
#if OPT_WORKING
extern	void	x_working		(void);
#endif
#if NO_LEAKS
extern	void	x11_leaks		(void);
#endif
extern void x_set_icon_name (char *name);
extern char * x_get_icon_name (void);
extern void x_set_window_name (char *name);
extern char * x_get_window_name (void);
extern	void	x_resize		(int cols, int rows);
#endif	/* DISP_X11 */

#if SYS_MSDOS || SYS_OS2 || SYS_WINNT
/* ibmpc.c */
extern	void scwrite (int row, int col, int nchar, const char *outstr, VIDEO_ATTR *attrstr, int forg, int bacg);
extern VIDEO *scread (VIDEO *vp, int row);
/* random.c */
extern const char * curr_dir_on_drive (int drive);
extern int curdrive (void);
extern int setdrive (int d);
extern void update_dos_drv_dir (const char * cwd);
# if CC_WATCOM
     extern int dos_crit_handler (unsigned deverror, unsigned errcode, unsigned *devhdr);
# else
     extern void dos_crit_handler (void);
# endif
# if OPT_MS_MOUSE
     extern int ms_exists (void);
     extern void ms_processing (void);
# endif
#endif

#if SYS_UNIX
#if MISSING_EXTERN__FILBUF
extern	int	_filbuf	(FILE *fp);
#endif
#if MISSING_EXTERN__FLSBUF
extern	int	_flsbuf	(int n, FILE *fp);
#endif
#if MISSING_EXTERN_ACCESS
extern	int	access	(const char *path, int mode);
#endif
#if MISSING_EXTERN_ALARM
extern	UINT	alarm	(UINT secs);
#endif
#if MISSING_EXTERN_ATOI
extern int	atoi	(const char *s);
#endif
#if MISSING_EXTERN_BZERO
extern	void	bzero	(char *b, int n);
#endif
#if MISSING_EXTERN_CHDIR
extern	int	chdir	(const char *path);
#endif
#if MISSING_EXTERN_CLOSE
extern	int	close	(int fd);
#endif
#if MISSING_EXTERN_DUP
extern	int	dup	(int fd);
#endif
#if MISSING_EXTERN_EXECLP
extern	int	execlp	(const char *path, ...);
#endif
#if MISSING_EXTERN_FCLOSE
extern	int	fclose	(FILE *fp);
#endif
#if MISSING_EXTERN_FCLOSE
extern	int	fflush	(FILE *fp);
#endif
#if MISSING_EXTERN_FGETC
extern	int	fgetc	(FILE *fp);
#endif
#if !defined(fileno) && MISSING_EXTERN_FILENO
extern	int	fileno	(FILE *fp);
#endif
#if MISSING_EXTERN_FORK
extern	int	fork	(void);
#endif
#if MISSING_EXTERN_FPRINTF
extern	int	fprintf	(FILE *fp, const char *fmt, ...);
#endif
#if MISSING_EXTERN_FPUTC
extern	int	fputc	(int c, FILE *fp);
#endif
#if MISSING_EXTERN_FPUTS
extern	int	fputs	(const char *s, FILE *fp);
#endif
#if MISSING_EXTERN_FREAD
extern	int	fread	(char *ptr, SIZE_T size, SIZE_T nmemb, FILE *fp);
#endif
#if MISSING_EXTERN_FREE
extern void	free	(void *ptr);
#endif
#if MISSING_EXTERN_FSEEK
extern	int	fseek	(FILE *fp, long offset, int whence);
#endif
#if MISSING_EXTERN_FWRITE
extern	int	fwrite	(const char *ptr, SIZE_T size, SIZE_T nmemb, FILE *fp);
#endif
#if MISSING_EXTERN_GETENV
extern	char *	getenv	(const char *name);
#endif
#if HAVE_GETHOSTNAME && MISSING_EXTERN_GETHOSTNAME
extern	int	gethostname (char *name, int len);
#endif
#if MISSING_EXTERN_GETPGRP
extern	int	getpgrp	(int pid);
#endif
#if MISSING_EXTERN_GETPID
extern	int	getpid	(void);
#endif
#if MISSING_EXTERN_GETUID
extern	int	getuid	(void);
#endif
#if HAVE_GETCWD && MISSING_EXTERN_GETCWD
extern	char *	getcwd (char *buffer, int len);
#endif
#if HAVE_GETWD && MISSING_EXTERN_GETWD
extern	char *	getwd	(char *buffer);
#endif
#if MISSING_EXTERN_IOCTL
extern	int	ioctl	(int fd, ULONG mask, caddr_t ptr);
#endif
#if MISSING_EXTERN_ISATTY
extern	int	isatty	(int fd);
#endif
#if MISSING_EXTERN_KILL
extern	int	kill	(int pid, int sig);
#endif
#if HAVE_KILLPG && MISSING_EXTERN_KILLPG
extern	int	killpg	(int pgrp, int sig);
#endif
#if HAVE_LINK && MISSING_EXTERN_LINK
extern	int	link	(const char *, const char *);
#endif
#if MISSING_EXTERN_LONGJMP
extern	void	longjmp	(jmp_buf env, int val);
#endif
#if MISSING_EXTERN_LSTAT
extern	int	lstat (const char *path, struct stat *sb);
#endif
#if MISSING_EXTERN_MEMSET
extern	void	memset	(char *dst, int ch, int n);
#endif
#if HAVE_MKDIR && MISSING_EXTERN_MKDIR
extern	int	mkdir	(const char *path, int mode);
#endif
#if MISSING_EXTERN_MKTEMP
extern	char *	mktemp (const char *template);
#endif
#if MISSING_EXTERN_OPEN
extern	int	open	(char *path, int flags);
#endif
#if MISSING_EXTERN_PERROR
extern	void	perror	(const char *s);
#endif
#if MISSING_EXTERN_PIPE
extern	int	pipe	(int filedes[2]);
#endif
#if MISSING_EXTERN_PRINTF
extern	int	printf	(const char *fmt, ...);
#endif
#if MISSING_EXTERN_PUTS
extern	int	puts	(const char *s);
#endif
#if MISSING_EXTERN_QSORT
#if ANSI_QSORT
extern void qsort (void *base, size_t nmemb, size_t size, int (*compar)(const void *a, const void *b);
#else
extern void qsort (void *base, size_t nmemb, size_t size, int (*compar)(char **a, char **b);
#endif
#endif
#if MISSING_EXTERN_READ
extern	int	read	(int fd, char *buffer, SIZE_T size);
#endif
#if MISSING_EXTERN_READLINK
extern	int	readlink (const char *path, char *buffer, size_t size);
#endif
#if HAVE_SELECT && MISSING_EXTERN_SELECT
extern	int	select	(int numfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout);
#endif
#if MISSING_EXTERN_SETBUF
extern	void	setbuf	(FILE *fp, char *buffer);
#endif
#if MISSING_EXTERN_SETBUFFER
extern	void	setbuffer (FILE *fp, char *buffer, int size);
#endif
#if MISSING_EXTERN_SETITIMER
extern	int setitimer (int which, const struct itimerval *value, struct itimerval *ovalue);
#endif
#if MISSING_EXTERN_SETJMP && !defined(setjmp)
extern	int	setjmp	(jmp_buf env);
#endif
#if MISSING_EXTERN_SETPGRP
#if SETPGRP_VOID
extern	pid_t	setpgrp	(void);
#else
extern	int	setpgrp	(int pid, int pgid);
#endif
#endif
#if MISSING_EXTERN_SETSID
extern	pid_t	setsid	(void);
#endif
#if MISSING_EXTERN_SETVBUF
#if SETVBUF_REVERSED
extern	int	setvbuf (FILE *fp, int mode, char *buffer, size_t size);
#else
extern	int	setvbuf (FILE *fp, char *buffer, int mode, size_t size);
#endif
#endif
#if MISSING_EXTERN_SLEEP
extern	int	sleep	(UINT secs);
#endif
#if MISSING_EXTERN_SSCANF
extern	int	sscanf	(const char *src, const char *fmt, ...);
#endif
#if MISSING_EXTERN_STRTOL
extern	long	strtol	(const char *nptr, char **endptr, int base);
#endif
#if MISSING_EXTERN_SYSTEM
extern	int	system	(const char *cmd);
#endif
#if MISSING_EXTERN_TGETSTR
extern	char *	tgetstr (const char *name, char **area);
#endif
#if MISSING_EXTERN_TGOTO
extern	char *	tgoto (const char *cstring, int hpos, int vpos);
#endif
#if MISSING_EXTERN_TIGETSTR
extern	char *	tigetstr (char *name);
#endif
#if MISSING_EXTERN_TIME
extern	time_t	time	(time_t *t);
#endif
#if MISSING_EXTERN_TPARM
extern	char *	tparm (const char *fmt, ...);
#endif
#if MISSING_EXTERN_UNLINK
extern	int	unlink	(char *path);
#endif
#if HAVE_UTIME && MISSING_EXTERN_UTIME
extern	int	utime	(const char *path, const struct utimbuf *t);
#endif
#if HAVE_UTIMES && MISSING_EXTERN_UTIMES
extern	int	utimes	(const char *path, struct timeval *t);
#endif
#if MISSING_EXTERN_WAIT
extern	int	wait	(int *sb);
#endif
#if MISSING_EXTERN_WRITE
extern	int	write	(int fd, const char *buffer, int size);
#endif
#endif

#if CC_DJGPP
/* djhandl.c */
extern unsigned long was_ctrl_c_hit (void);
extern void want_ctrl_c (int yes);
extern void clear_hard_error (void);
extern void hard_error_catch_setup (void);
extern void hard_error_teardown (void);
extern int did_hard_error_occur (void);
#endif

#if OPT_SELECTIONS
/* select.c */
extern	void	free_attribs	(BUFFER *bp);
extern	void	free_attrib	(BUFFER *bp, AREGION *ap);
extern	int	assign_attr_id	(void);
extern	void	find_release_attr (BUFFER *bp, REGION *rp);
extern	int	sel_begin	(void);
extern	int	sel_extend	(int wiping, int include_dot);
extern	void	sel_release	(void);
extern	void	sel_reassert_ownership (BUFFER *bp);
#if (DISP_X11 && XTOOLKIT) || DISP_NTCONS
extern	int	sel_yank	(int reg);
extern	int	sel_attached	(void);
extern	BUFFER *sel_buffer	(void);
#endif
extern	int	sel_setshape	(REGIONSHAPE shape);
extern	int	attributeregion (void);
#endif /* OPT_SELECTIONS */

#if OPT_VMS_PATH
/* vms2unix.c */
extern	char *	is_vms_dirtype	(char *path);
extern	char *	is_vms_rootdir	(char *path);
extern	char *	unix2vms_path   (char *dst, const char *src);
extern	char *	vms2unix_path   (char *dst, const char *src);
extern	char *	vms_path2dir    (const char *src);
extern	void	vms_dir2path	(char *path);
extern	int	vms_fix_umask	(const char *filespec);
#endif
