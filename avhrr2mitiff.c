/*
 * NAME:
 * avhrr2mitiff.c
 *
 * PURPOSE:
 * This program reads level 2 AVHRR data produced either by one of the
 * KSPT MEOS processing chains in use or the AAPP based AAPP4AVHRR
 * processing chain used by FOU and converts the files to MITIFF which can
 * be fed into DIANA or satsplit. Further specification of the supported
 * file formats is found in read_image.c.
 *
 * REQUIREMENTS:
 * o libsatimg
 * o libmeos (library names differ)
 * o libmeos5 (library names differ)
 *
 * INPUT:
 * o AVHRR Level 2 files in KSPT MEOS HDF or HDF5 format or METSAT format.
 * KSPT MEOS HDF files can be in either 16 or 8 bit format, use command
 * line arguments to chose algorithm.
 *
 * OUTPUT:
 * o MITIFF
 *
 * NOTES:
 * Not all dynamically allocated memory is freed in a proper way yet. However,
 * this is not critical as the program runs once and then stops. It is not
 * constructed for continous access to data files.
 *
 * The software should be adapted for fmlibs when this is available and
 * memory leaks removed...
 *
 * BUGS:
 * Handling of 8 and 16 bit data is not fully tested after METSAT
 * integration... This will probably not work...
 *
 * AUTHOR:
 * Øystein Godøy, DNMI/FoU, 21/08/1999
 *
 * MODIFIED:
 * Øystein Godøy, DNMI/FoU, 06/03/2001
 * Rewrote input data handling and added options interface. Memory handling
 * should be slightly improved compared with previous versions.
 * Øystein Godøy, met.no/FOU, 21.12.2004
 * Adapted for use with new MEOS formats and METSAT.
 * Øystein Godøy, met.no/FOU, 25.02.2005
 * Changed some names and added some tests.
 *
 * VERSION:
 * $Id: avhrr2mitiff.c,v 1.4 2010-04-06 11:36:59 steingod Exp $
 */

#include <avhrr2mitiff.h>
#include <unistd.h>

int main(int argc, char **argv) {

    extern char *optarg;
    char filename[FMSTRING1024], outpath[FMSTRING1024], *outfile;
    char imginfo[MILEN], *date, channels[20], truelat[10];
    char calib[100], tcalib[50], acalib[50], dummy[3], area[20];
    unsigned char **img;
    char *where="avhrr2mitiff";
    int i, getoptret, status;
    short errflg = 0, iflg = 0,  pflg = 0, oflg = 0, eflg = 0;
    short mode = 0;
    struct midate idate;
    fmio_mihead info;
    
    /*
     * Decode commandline arguments.
     */
     while ((getoptret=getopt(argc, argv, "i:p:oe")) != EOF) {
	switch (getoptret) {
	    case 'i':
		if (!strcpy(filename, optarg)) exit(0);
		iflg++;
                break;
	    case 'p':
                if (!strcpy(outpath, optarg)) exit(0);
		pflg++;
		break;
	    case 'o':
	        oflg++;
		break;
	    case 'e':
	        eflg++;
		break;
	    default:
		usage();
	}
    }
    if (oflg && eflg) {
	usage();
    } else if (oflg) {
	mode = 1;
    } else if (eflg) {
	mode = 2;
    } else {
	mode = 0;
    }
    if (!iflg || !pflg) errflg++;
    if (errflg) usage();

    /*
     * Create ASCII string with navigation information etc.
     */
    img = (unsigned char **) malloc(FMIO_NCHAN*sizeof(unsigned char *));
    if (!img) {
	fmerrmsg(where,"Could not allocate img");
	exit(3);
    }
    for (i=0; i<FMIO_NCHAN; i++) {
	img[i] = NULL;
    }
    if (readandbytepack(filename,img,&info,area,mode) != 0) {
	fmerrmsg(where,"Could not read file");
	exit(10);
    }
    idate.year = info.year;
    idate.month = info.month;
    idate.day = info.day;
    idate.hour = info.hour;
    idate.minute = info.minute;
    idate.second = 00;
    date = dateconv(idate);
    if (info.ch[0] == 0) sprintf(channels, "1");
    for (i=0; i<info.zsize; i++) {
	if (info.ch[i] != 0) {	    
	    sprintf(dummy, "%d ", info.ch[i]);
	    strcat(channels, dummy);
	}
    }    
    sprintf(truelat, "%s", "60.0 N");
    sprintf(tcalib, "%s", " Calibration IR: T=(323.15)+(-0.5)*C");
    sprintf(acalib, "%s", " Calibration VIS: A=(0.00)+(0.392157)*C");
    sprintf(calib, "%s\n%s", acalib, tcalib);
    fm_MITIFF_create_head(imginfo, info.satellite, date, 0, 
	info.zsize, channels, info.xsize, info.ysize, "Polar Stereographic", 
	truelat, 0.0, 1000., 1000., 0., 0., 
	info.Ax, info.Ay, info.Bx, info.By, 
	calib);
    
    outfile = ofilenam(idate, area);
    strcat(outpath, outfile);

    status = fm_MITIFF_write_multi(outpath, img, imginfo, info);
    exit(0);
}

/*
 * The following function create output filenames of type:
 *	n140835.sn
 *
 * Oystein Godoy, DNMI, 15.04.1997.
 */
char *ofilenam(struct midate date, char area[]) {
    static char fnam[30];

    sprintf(fnam, "n%4d%02d%02d%02d%02dm%s.%s", 
	date.year, date.month, date.day, date.hour, date.minute, 
	area,"mitiff");
        
    return(fnam);
}

/*
 * The following function creates a string of type:
 *	07:46 03/10-1996
 *
 * Oystein Godoy, DNMI, 03.01.1997.
 */
char *dateconv(struct midate date) {
    static char ldate[17];
    
    sprintf(ldate, "%02d:%02d %02d/%02d-%4d", 
	date.hour, date.minute, date.day, date.month, date.year);
        
    return(ldate);
}

void memerror() {
    fprintf(stderr," %s\n","Memory allocation trouble.");
}

void usage(void) {

    fprintf(stdout,"\n");
    fprintf(stdout," SYNTAX: avhrr2mitiff -i <infile> -p <outpath> [-o|-e]\n\n");
    fprintf(stdout," <infile>: Full path and filename\n");
    fprintf(stdout," <outpath>: Full path to output file (only path, not filename).\n");
    fprintf(stdout," -o: Operational data (8bit)\n");
    fprintf(stdout," -e: Experimental data (16bit)\n");
    fprintf(stdout,"\n");
    fprintf(stdout," Name of output file is created by the program (n200103150846msn.mitiff)\n");
    fprintf(stdout,"\n");
    exit(2);
}
