/* See LICENSE file for copyright and license details. */
#include "fibonacci.c"

/* appearance */
static const unsigned int borderpx  = 2;        /* border pixel of windows */
static const unsigned int gappx     = 10;       /* gaps between windows */
static const unsigned int snap      = 4;        /* snap pixel */
static const int showbar            = 1;        /* 0 means no bar */
static const int topbar             = 0;        /* 0 means bottom bar */
static const int vertpad            = 10;       /* vertical padding of bar */
static const int sidepad            = 10;       /* horizontal padding of bar */
static const int horizpadbar        = 2;        /* horizontal padding for statusbar */
static const int vertpadbar         = 6;        /* vertical padding for statusbar */
static const char *fonts[]          = { "Monofur Nerd Font:pixelsize=15:autohint=true:style=Book"
                                      , "Wuncon Siji:style=Regular"
                                      , "monospace:size=10" 
                                      };
static const char dmenufont[]       = "Monofur Nerd Font:pixelsize=15:autohint=true:style=Book";

static const unsigned int baralpha  = OPAQUE;
static const unsigned int borderalpha = OPAQUE;

static const char col_bg[]          = "#282a2e";
static const char col_bg_alt[]      = "#18161a";
static const char col_fg[]          = "#c5c8c6";
static const char col_fg_alt[]      = "#85678f";
static const char col_hi[]          = "#8abeb7";
static const char status_bg[]       = "#5f819d";
static const char status_fg[]       = "#18161a";
static const char tags_bg[]         = "#5e8d87";
static const char tags_fg[]         = "#18161a";
static const char tags_bg_sel[]     = "#8abeb7";
static const char tags_fg_sel[]     = "#18161a";
static const char info_bg[]         = "#b294bb";
static const char info_fg[]         = "#18161a";
static const char info_bg_sel[]     = "#85678f";
static const char info_fg_sel[]     = "#18161a";

static const char *colors[][3]      = 
    { [SchemeNorm]      = { col_fg,     col_bg,     col_fg_alt }
    , [SchemeSel]       = { col_fg_alt, col_bg_alt, col_hi      }
	, [SchemeStatus]    = { status_fg,  status_bg,  "#000000"   } 
    , [SchemeTagsNorm]  = { tags_fg,    tags_bg,    "#000000"   } 
	, [SchemeTagsSel]   = { tags_fg_sel,tags_bg_sel,"#000000"   } 
    , [SchemeInfoNorm]  = { info_fg,    info_bg,    "#000000"   } 
    , [SchemeInfoSel]   = { info_fg_sel,info_bg_sel,"#000000"   } 
    /*                          fg         bg         border    */
    };

static const unsigned int alphas[][3]      = {
	/*               fg      bg        border     */
	[SchemeNorm] = { OPAQUE, baralpha, borderalpha },
	[SchemeSel]  = { OPAQUE, baralpha, borderalpha },
};

/* tagging: 一 二 三 四 五 六 七 八 九 十 / ᚠ ᚢ ᚦ ᚨ ᚱ ᚲ ᚷ ᚹ ᚺ ᚾ ᛁ ᛃ ᛈ ᛇ ᛉ ᛊ ᛏ ᛒ ᛖ ᛗ ᛚ ᛜ ᛞ ᛟ */
static const char *tags[] = { "ᚠ", "ᚢ", "ᚦ", "ᚨ", "ᚱ", "ᚲ", "ᛉ", "ᛊ", "ᛏ" };

static const Rule rules[] = {
    /* xprop(1):
     *  WM_CLASS(STRING) = instance, class
     *  WM_NAME(STRING) = title
     */
	/* class      instance    title       tags mask     isfloating   isterminal noswallow   monitor */
    { "zathura",  NULL,       NULL,       0,            0,           0,         0,          -1 },
	{ "st",       NULL,       NULL,       0,            0,           1,         0,          -1 },
	{ "xterm",    NULL,       NULL,       0,            0,           1,         1,          -1 },
    { NULL,       NULL,       "Event Tester", 0,        0,           0,         1,          -1 },
    { "MATLAB R2019a - academic use", NULL, 0, 0,       0,           0,         1,          -1 }
};

/* layout(s) */
static const float mfact     = 0.55; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 0;    /* 1 means respect size hints in tiled resizals */

static const Layout layouts[] = {
    /* symbol     arrange function */
    { "[ᛈ]",    tile },    /* first entry is default */
    { "[ᛟ]",    NULL },    /* no layout function means floating behavior */ 
    { "[ᛜ]",    monocle }, /* monocle means full screen stacked */
    { "[ᛃ]",    spiral },
    { "[ᛇ]",    dwindle },
};

/* key definitions */
#define MODKEY Mod4Mask
#define ALTKEY Mod1Mask
#define TAGKEYS(KEY,TAG) \
    { MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
    { MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
    { MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
    { MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = { "dmenu_run", "-m", dmenumon
                                , "-fn", dmenufont
                                , "-nb", col_bg
                                , "-nf", col_fg
                                , "-sb", col_fg_alt
                                , "-sf", col_bg_alt
                                , "-nhb", col_bg
                                , "-nhf", col_fg_alt
                                , "-shb", col_fg_alt
                                , "-shf", col_hi
                                , "-bw", "2"
                                , "-l", "7"
                                , "-h", "25"
                                , "-x", "390"
                                , "-y", "150"
                                , "-w", "500"
                                , NULL };

static const char *termcmd[]  = { "st", NULL };

static const char *scrotcmd[]  = { "grabscreen.sh", NULL };
static const char *scrotcmd_s[]  = { "grabsection.sh", NULL };
static const char *scrotcmd_w[]  = { "grabwindow.sh", NULL };

static const char *mutecmd[] = { "pactl", "set-sink-mute", "0", "toggle", NULL };
static const char *volupcmd[] = { "pactl", "set-sink-volume", "0", "+2%", NULL };
static const char *voldowncmd[] = { "pactl", "set-sink-volume", "0", "-2%", NULL };

static const char *brupcmd[] = { "xbacklight", "+", "10", NULL };
static const char *brdowncmd[] = { "xbacklight", "-", "10", NULL };

static const char *remotecmd[]  = { "consrv.sh", NULL };

static const char *lockcmd[]  = { "i3lock-fancy", "-t", NULL };

static const char *outcmd[]  = { "outdwm.sh", NULL };

static Key keys[] = {
    /* modifier                     key         function        argument */
    { MODKEY,                       XK_Return,  spawn,          {.v = termcmd } },
    { MODKEY,                       XK_Print,   spawn,          {.v = scrotcmd } },
    { MODKEY|ShiftMask,             XK_Print,   spawn,          {.v = scrotcmd_s } },
    { ALTKEY,                       XK_Print,   spawn,          {.v = scrotcmd_w } },
    { MODKEY,                       XK_F4,      spawn,          {.v = mutecmd } },
    { MODKEY,                       XK_F5,      spawn,          {.v = voldowncmd } },
    { MODKEY,                       XK_F6,      spawn,          {.v = volupcmd } },
    { MODKEY,                       XK_F2,      spawn,          {.v = brupcmd } },
    { MODKEY,                       XK_F1,      spawn,          {.v = brdowncmd } },
    { MODKEY,                       XK_p,       spawn,          {.v = dmenucmd } },
    { MODKEY,                       XK_b,       togglebar,      {0} },
    { MODKEY,                       XK_j,       focusstack,     {.i = +1 } },
    { MODKEY,                       XK_k,       focusstack,     {.i = -1 } },
    { MODKEY,                       XK_i,       incnmaster,     {.i = +1 } },
    { MODKEY,                       XK_d,       incnmaster,     {.i = -1 } },
    { MODKEY,                       XK_h,       setmfact,       {.f = -0.05} },
    { MODKEY,                       XK_l,       setmfact,       {.f = +0.05} },
    { MODKEY|ShiftMask,             XK_Return,  zoom,           {0} },
    { MODKEY,                       XK_Tab,     view,           {0} },
    { MODKEY|ShiftMask,             XK_c,       killclient,     {0} },
    { MODKEY,                       XK_t,       setlayout,      {.v = &layouts[0]} },
    { MODKEY,                       XK_f,       setlayout,      {.v = &layouts[1]} },
    { MODKEY,                       XK_m,       setlayout,      {.v = &layouts[2]} },
    { MODKEY,                       XK_space,   setlayout,      {0} },
    { MODKEY|ShiftMask,             XK_space,   togglefloating, {0} },
    { MODKEY,                       XK_s,       togglesticky,   {0} },
    { MODKEY,                       XK_0,       view,           {.ui = ~0 } },
    { MODKEY|ShiftMask,             XK_0,       tag,            {.ui = ~0 } },
    { MODKEY,                       XK_comma,   focusmon,       {.i = -1 } },
    { MODKEY,                       XK_period,  focusmon,       {.i = +1 } },
    { MODKEY|ShiftMask,             XK_comma,   tagmon,         {.i = -1 } },
    { MODKEY|ShiftMask,             XK_period,  tagmon,         {.i = +1 } },
    { MODKEY,                       XK_minus,   setgaps,        {.i = -1 } },
    { MODKEY,                       XK_equal,   setgaps,        {.i = 0 } },
    { MODKEY|ShiftMask,             XK_equal,   setgaps,        {.i = +1 } },
    { MODKEY,					    XK_Down,	moveresize,		{.v = (int []){ 0, 25, 0, 0 }}},
    { MODKEY,					    XK_Up,		moveresize,		{.v = (int []){ 0, -25, 0, 0 }}},
    { MODKEY,					    XK_Right,	moveresize,		{.v = (int []){ 25, 0, 0, 0 }}},
    { MODKEY,					    XK_Left,	moveresize,		{.v = (int []){ -25, 0, 0, 0 }}},
    { MODKEY|ShiftMask,			    XK_Down,	moveresize,		{.v = (int []){ 0, 0, 0, 25 }}},
    { MODKEY|ShiftMask,			    XK_Up,		moveresize,		{.v = (int []){ 0, 0, 0, -25 }}},
    { MODKEY|ShiftMask,			    XK_Right,	moveresize,		{.v = (int []){ 0, 0, 25, 0 }}},
    { MODKEY|ShiftMask,			    XK_Left,	moveresize,		{.v = (int []){ 0, 0, -25, 0 }}},
    TAGKEYS(                        XK_1,                       0)
    TAGKEYS(                        XK_2,                       1)
    TAGKEYS(                        XK_3,                       2)
    TAGKEYS(                        XK_4,                       3)
    TAGKEYS(                        XK_5,                       4)
    TAGKEYS(                        XK_6,                       5)
    TAGKEYS(                        XK_7,                       6)
    TAGKEYS(                        XK_8,                       7)
    TAGKEYS(                        XK_9,                       8)
    { MODKEY,                       XK_r,       spawn,          {.v = remotecmd } },
    { ALTKEY,                       XK_l,       spawn,          {.v = lockcmd } },
    { MODKEY,                       XK_o,       spawn,          {.v = outcmd } },
    { MODKEY|ShiftMask,             XK_e,       spawn,          SHCMD("[ \"$(printf \"No\\nYes\" | dmenu -i -nb darkred -sb red -sf white -nf gray -p \"Close Xorg?\")\" = Yes ] && killall Xorg") },
    { MODKEY,                       XK_Escape,  quit,           {0} },
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static Button buttons[] = {
    /* click                event mask      button          function        argument */
    { ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
    { ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
    { ClkWinTitle,          0,              Button2,        zoom,           {0} },
    { ClkStatusText,        0,              Button2,        spawn,          {.v = termcmd } },
    { ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
    { ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
    { ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
    { ClkTagBar,            0,              Button1,        view,           {0} },
    { ClkTagBar,            0,              Button3,        toggleview,     {0} },
    { ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
    { ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};

