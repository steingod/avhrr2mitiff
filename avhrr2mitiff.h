/*
 * NAME:
 * avhrr2mitiff.h
 *
 * PURPOSE:
 * Header file for program avhrr2mitiff.c, see avhrr2mitiff.c for details.
 *
 * REQUIREMENTS:
 *
 * INPUT:
 *
 * OUTPUT:
 *
 * NOTES:
 *
 * BUGS:
 *
 * AUTHOR:
 * �ystein God�y, DNMI/FoU, 21/08/1999
 *
 * MODIFIED:
 * NA
 */

#include <satimg.h>
#include <products.h>

#define SEQUENCE 1
#define MILEN 512

struct midate {
    unsigned short int year;
    unsigned short int month;
    unsigned short int day;
    unsigned short int hour;
    unsigned short int minute;   
    unsigned short int second;   
};

short conv16to8(char *infile, unsigned char **img, 
    struct mihead *ginfo, char area[], short mode); 
char *dateconv(struct midate date);
char *ofilenam(struct midate date, char area[]);
void memerror(void);
void usage(void);

