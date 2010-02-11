/*
 * SYNTAX.H - Configure SCHEME Syntax Modes for CodeWarrior x86
 *
 * Source Version: 5.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#define DEF_SYNTAX_MODES() \
    SS_init_m_syntax_mode(); \
    SS_init_f_syntax_mode(); \
    SS_init_c_syntax_mode()

extern void
 SS_init_m_syntax_mode(),
 SS_init_f_syntax_mode(),
 SS_init_c_syntax_mode();
 
