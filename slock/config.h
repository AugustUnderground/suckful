/* user and group to drop privileges to */
static const char *user  = "nobody";
static const char *group = "nogroup";

static const char *colorname[NUMCOLS] = {
    [INIT] =   "#0d0d0e",   /* after initialization */
    [INPUT] =  "#7a002b",   /* during input */
    [FAILED] = "#5174e1",   /* wrong password */
};

/* treat a cleared input like a wrong password (color) */
static const int failonclear = 1;

/* default message */
static const char * message = "Hack me gently.";

/* text color */
static const char * text_color = "#dedede";

/* text size (must be a valid size) */
static const char * font_name = "fixed";

/* enable blur*/
#define BLUR

/*Set blur radius*/
static const int blurRadius = 10;

/* enable pixelation*/
//#define PIXELATION

/*Set pixelation radius*/
static const int pixelSize = 0;

