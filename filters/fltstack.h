/*
 * $Header: /users/source/archives/vile.vcs/filters/RCS/fltstack.h,v 1.12 2007/11/25 20:19:02 tom Exp $
 * A simple stack for lex states
 */
typedef struct {
    int state;
#ifdef FLTSTACK_EXTRA
    FLTSTACK_EXTRA
#endif
} STACK;
static STACK *stk_state = 0;

static int stk_limit = 0;
static int stk_level = -1;

#define FLTSTACK_OK   (stk_level >= 0 && stk_level < stk_limit)
#define FLTSTACK_THIS stk_state[stk_level]
#define FLT_STATE     FLTSTACK_THIS.state

/*
 * Record the given state at the current stack level, and tell lex about it.
 */
static void
new_state(int code)
{
    if (FLTSTACK_OK && stk_state != 0)
	FLT_STATE = code;
    BEGIN(code);
}

static void
pop_state(void)
{
#ifdef INITIAL
    int state = INITIAL;
#else
    int state = 0;	/* cater to broken "new" flex */
#endif
    --stk_level;
    if (FLTSTACK_OK)
	state = FLT_STATE;
    new_state(state);
}

static void
push_state(int state)
{
    ++stk_level;
    if ((stk_level >= stk_limit) || (stk_state == 0)) {
	unsigned have = sizeof(STACK) * stk_limit;
	unsigned want = sizeof(STACK) * (stk_limit += (20 + stk_level));
	stk_state = type_alloc(STACK, (void *) stk_state, want, &have);
    }
#ifdef FLTSTACK_EXTRA_PUSH
    if (FLTSTACK_OK) {
	FLTSTACK_EXTRA_PUSH
    }
#endif
    new_state(state);
}

/*
 * Call this from do_filter(), to set the initial state and store that value
 * on the stack.
 */
static void
begin_state(int code)
{
    stk_level = -1;
    push_state(code);
}

/*
 * Cleanup
 */
static void
end_state(void)
{
#if NO_LEAKS
    if (stk_state != 0) {
	free(stk_state);
	stk_state = 0;
    }
#endif
}
