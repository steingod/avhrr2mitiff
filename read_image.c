/*
 * NAME:
 * read_image.c
 *
 * PURPOSE:
 * Contains useful functions for avhrr2mitiff.c.
 *
 * REQUIREMENTS:
 *
 * INPUT:
 *
 * OUTPUT:
 *
 * NOTES:
 * Rewrite when fmlibs are available...
 *
 * BUGS:
 * Handling of 8 and 16 bit data is not fully tested after METSAT
 * integration... This will probably not work...
 *
 * AUTHOR:
 *
 * MODIFIED:
 * NA
 */

/*
 * FUNCTION: 
 * conv16to8
 * 
 * PURPOSE: 
 * Read and decode MEOS/HDF image files containing NOAA/AVHRR data. These 
 * images are originally in 10 bit format and are reduced to 8 bit images 
 * prior to return of data to the main program. As the MEOS system has 
 * developed, two different production chains exist at DNMI. One operational 
 * producing 8 bit data and one experimental producing 16 bit data. This 
 * function should now handle both types.
 *
 * RETURN VALUES:
 * 0: OK
 * 2: input/output problem
 * 3: memory problem
 * 10: internal problem, function could be unstable in memory
 * 
 * REQUIREMENTS:
 * libsatimg
 * libmeos (required by libsatimg, not used directly)
 * 
 * NOTES:
 * NA
 *
 * BUGS:
 * None known yet.
 *  
 * AUTHOR: 
 * Øystein Godøy, DNMI/FOU, 05/05/1995
 * MODIFICATIONS:
 * Øystein Godøy, DNMI/FOU, 06/02/1998
 * Do not remeber what I did.
 * Øystein Godøy, DNMI/FOU, 06/03/2001
 * Rewrote data reading and conversion to handle both 8 and 10 bits data.
 * Cleaned a lot of messy code, especially the memory handling.
 * Øystein Godøy, met.no/FOU, 17.11.2004
 * Adapted to handle METSAT also...
 */

#include <satimg.h>
#include <fmcoord.h>

void rtrim(char *s);

short conv16to8(char *infile, unsigned char **img, 
    struct mihead *ginfo, char area[], short mode) {
    
    char *where="conv16to8";
    int i, j;
    float fval;
    struct imgh h;
    struct clb cal;
    fmposxy old, new;

    /*
     * Deceide which data to read, operational or experimental.
     */

    initdata(&h);
    if (readdata(infile, &h)) {
	errmsg(where,"Could not read file...");
	return(2);
    }
    imghead2cal(h, &cal);
    /*
    if (mode == 1) {
	readdata8(infile, &h);
    } else if (mode == 2) {
	readdata(infile, &h);
    } else {
	fprintf(stderr," %s%s\n",where,"Could not choose data chain.");
	return(2);
    }
    */

    /*
     Fill the ginfo structure and the iw and ih variables...
     */
    if (mode == 2) {
	old.x = h.Bx;
	old.y = h.By;
	new = meosxy2mi(old);
    } else {
	new.x = h.Bx;
	new.y = h.By;
    }
    sprintf(ginfo->satellite,"%s",h.sa);
    ginfo->hour = h.ho;
    ginfo->minute = h.mi;
    ginfo->day = h.dd;
    ginfo->month = h.mm;
    ginfo->year = h.yy;
    ginfo->zsize = h.z;
    ginfo->xsize = h.iw;
    ginfo->ysize = h.ih;
    ginfo->Ax = h.Ax;
    ginfo->Ay = h.Ay;
    /*
    ginfo->Bx = h.Bx;
    ginfo->By = h.By;
    */
    ginfo->Bx = new.x;
    ginfo->By = new.y;
    sprintf(area,"%s",h.area);
    for (i=0; i<h.z; i++) {
	ginfo->ch[i] = h.ch[i];
    }

    /*
     * Allocates memory for the image data and fills it.
     */
    if (h.z > MAXCHANNELS) {
	errmsg(where,"Not enough pointers to hold data.");
	fprintf(stderr," %s\n",
	    "Trying to clear data structure before return to main.");
	if (cleardata(&h) != 0) {
	    fprintf(stderr," %s\n","Could not clear data properly.");
	    fprintf(stderr," %s\n",
		"Program execturion terminates for safety.");
	    exit(0);
	}
	return(3);
    }
    for (i=0; i<h.z; i++) {
	img[i] = (unsigned char *) malloc((h.size)*sizeof(char));
	if (!img[i]) {
	    errmsg(where, "Could not allocate memory...");
	    return(3);
	}
	for (j=0; j<h.size; j++) {
	    /*
	    if (strstr(h.source,"METSAT") || strstr(h.source,"MEOS4")) {
		if (h.image[3][j] == 0) img[i][j] = 0;
		continue;
	    } else if(strstr(h.source,"MEOS5")) {
		if (h.image[3][j] <= 4) img[i][j] = 0;
		continue;
	    }
	     the following test is used until libsatimg is properly adapted...
	    */
	    if (h.image[3][j] == 0) {
		img[i][j] = 0;
		continue;
	    }
	    
	    fval = calib(h.image[i][j], h.ch[i], cal);
	    if (h.ch[i] == 1 || h.ch[i] == 2 || h.ch[i] == 6) {
		img[i][j] = (unsigned char) (0.+fval*2.55);
	    } else if (h.ch[i] == 3 || h.ch[i] == 4 || h.ch[i] == 5) {
		img[i][j] = (unsigned char) (323.15*2.-fval*2.);
	    } else {
		errmsg(where,"Unsupported channel id.");
		return(2);
	    }
	}
	/*
	if (mode == 1) {
	    for (j=0; j<h.size; j++) {
		img[i][j] = (unsigned char) h.image[i][j];
	    }
	} else if (mode == 2 || mode == 0) {
	    for (j=0; j<h.size; j++) {
		img[i][j] = (unsigned char) ((float) h.image[i][j]/257.);
	    }
	} else {
	    errmsg(where, "Could not deceide data source.");
	    fprintf(stderr," %s\n",
		"Data should not be used, memory could be unstable.");
	    return(10);
	}
	*/
    }

    if (cleardata(&h) != 0) {
	errmsg(where, "Could not clear data properly.");
	fprintf(stderr," %s\n",
	    "Program execution terminates for safety.");
	exit(0);
    }

    return(0);
}

