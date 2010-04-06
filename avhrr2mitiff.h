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
 * Øystein Godøy, DNMI/FoU, 21/08/1999
 *
 * MODIFIED:
 * Øystein Godøy, met.no/FOU, 25.02.2005
 * See avhrr2mitiff.c
 *
 * VERSION:
 * $Id: avhrr2mitiff.h,v 1.4 2010-04-06 11:36:59 steingod Exp $
 */

#include <fmutil.h>
#include <fmio.h>
#include <math.h>

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

short readandbytepack(char *infile, unsigned char **img, 
    fmio_mihead *ginfo, char area[], short mode); 
char *dateconv(struct midate date);
char *ofilenam(struct midate date, char area[]);
void memerror(void);
void usage(void);

