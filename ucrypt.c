/*
 * Unix crypt(1)-style interface.
 * Written by T.E.Dickey for vile (March 1999).
 *
 * $Header: /users/source/archives/vile.vcs/RCS/ucrypt.c,v 1.4 1999/03/09 11:55:20 tom Exp $
 *
 */

# include	"estruct.h"
# include	"edef.h"

#if	OPT_ENCRYPT

#define MASKED(n) ((n)&(N_chars-1))

#define LEN_CLEAR 8
#define LEN_CRYPT 13

static unsigned	index_1;
static unsigned	index_2;

static char	table1[N_chars];
static char	table2[N_chars];
static char	table3[N_chars];

/*
 * Get the string to use as an encryption string.
 */
static int
get_encryption_key(		/* make encryption key */
char	*key,			/* where to write key */
UINT	len)
{
    int status;			/* return status */
    int odisinp = disinp;	/* original value of disinp */
    char temp[NPAT];

    /* turn command input echo off */
    disinp = FALSE;

    temp[0] = EOS;
    status = mlreply("-Encryption String: ", temp, len-1);
    disinp = odisinp;

    if (status == TRUE)
	vl_make_encrypt_key(key, temp);

    mlerase();
    return(status);
}

void
vl_encrypt_blok(char *buf, UINT len)
{
    unsigned c1, c2;

    for (c2 = 0; c2 < len; c2++) {
	c1 = buf[c2];
	buf[c2] = table2[
	    	MASKED(table3[
		    MASKED(table1[
			MASKED(c1+index_1)] + index_2)
			    ] - index_2)
			] - index_1;
	if (++index_1 >= N_chars) {
	    index_1 = 0;
	    if(++index_2 >= N_chars)
		index_2 = 0;
	}
    }
}

int
vl_encrypt_char(int ch)
{
    char buffer[2];
    buffer[0] = ch;
    vl_encrypt_blok(buffer, 1);
    return buffer[0];
}

/*
 * Calls to 'crypt()' are slow; do this only after we have gotten a new
 * key from the user.
 */
void
vl_make_encrypt_key (char *dst, char *src)
{
    char key[LEN_CLEAR];
    char salt[2];

    memcpy(key,  src,  sizeof(key));
    memcpy(salt, src,  sizeof(salt));

    while (*src)
    	*src++ = 0;

    memcpy(dst,  crypt(key, salt), LEN_CRYPT);
    dst[LEN_CRYPT] = 0;

    TRACE(("made encryption key(%s)\n", dst))
}

int
vl_resetkey(		/* reset the encryption key if needed */
BUFFER	*bp,
const char *fname)
{
    register int s;	/* return status */

    /* turn off the encryption flag */
    cryptflag = FALSE;

    /* if we are in crypt mode */
    if (b_val(bp, MDCRYPT)) {
	char temp[NFILEN];

	/* don't automatically inherit key from other buffers */
	if (bp->b_key[0] != EOS
	 && !b_is_argument(bp)       
	 && strcmp(lengthen_path(strcpy(temp, fname)), bp->b_fname)) {
	    char	prompt[80];
	    (void)lsprintf(prompt, "Use crypt-key from %s", bp->b_bname);
	    s = mlyesno(prompt);
	    if (s != TRUE)
		return (s == FALSE);
	}

	/* make a key if we don't have one */
	if (bp->b_key[0] == EOS) {
	    s = get_encryption_key(bp->b_key, sizeof(bp->b_key));
	    if (s != TRUE)
		return (s == FALSE);
	}

	cryptflag = TRUE;
	vl_setup_encrypt(bp->b_key);
    }

    return TRUE;
}

/* ARGSUSED */
int
vl_setkey(		/* set/reset encryption key of current buffer */
int f GCC_UNUSED,	/* default flag */
int n GCC_UNUSED)	/* numeric argument */
{
    char result[NPAT];
    int rc = get_encryption_key(result, sizeof(result));

    if (rc == TRUE) {
	TRACE(("set key for %s\n", curbp->b_bname))
	(void)strcpy(curbp->b_key, result);
	make_local_b_val(curbp, MDCRYPT);
	set_b_val(curbp, MDCRYPT, TRUE);
	curwp->w_flag |= WFMODE;
    } else if (rc == FALSE) {
	if (curbp->b_key[0] != EOS) {
	    rc = mlyesno("Discard encryption key");
	    if (rc == TRUE) {
		TRACE(("reset key for %s\n", curbp->b_bname))
		curbp->b_key[0] = EOS;
		if (global_b_val(MDCRYPT)) {
		    make_local_b_val(curbp, MDCRYPT);
		    set_b_val(curbp, MDCRYPT, FALSE);
		    curwp->w_flag |= WFMODE;
		} else if (b_val(curbp,MDCRYPT)) {
		    make_global_val(curbp->b_values.bv, global_b_values.bv, MDCRYPT);
		    curwp->w_flag |= WFMODE;
		}
	    }
	}
    }
    return (rc);
}

/*
 * Call this function at the beginning of encrypting/decrypting a file, i.e.,
 * while writing or reading it.
 */
void
vl_setup_encrypt(char *encrypted_password)
{
    int j, c1, c2, temp;
    unsigned mixs;
    long myseed = seed;

    TRACE(("setup_encrypt(%s)\n", encrypted_password))

    for (j = 0; j < LEN_CRYPT; j++)
	myseed = myseed * encrypted_password[j] + j;

    for (j = 0; j < N_chars; j++)
	table1[j] = j;
    memset(table3, 0, sizeof(table3));

    for (j = 0; j < N_chars; j++) {
	myseed = 5 * myseed + encrypted_password[j % LEN_CRYPT];
	mixs = myseed % 65521;
	c2   = N_chars-1 - j;
	c1   = MASKED(mixs) % (c2+1);
	mixs >>= 8;
	temp = table1[c2];
	table1[c2] = table1[c1];
	table1[c1] = temp;
	if (table3[c2] == 0) {
	    c1 = MASKED(mixs) % c2;
	    while (table3[c1] != 0)
		c1 = (c1+1) % c2;
	    table3[c2] = c1;
	    table3[c1] = c2;
	}
    }

    for(j = 0; j < N_chars; j++)
	table2[MASKED(table1[j])] = j;

    index_1 = 0;
    index_2 = 0;
}

#endif
