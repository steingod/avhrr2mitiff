#
# NAME:
# avhrr2mitiff.mk
#
# PURPOSE:
# To create avhrr2mitiff that converts MEOS/HDF (v4.x) files to MITIFF
# format. See avhrr2mitiff.c for details...
#
# REQUIREMENTS:
#
# INPUT:
#
# OUTPUT:
#
# NOTES:
#
# BUGS:
# No proper Makefil, still need to implement make, make install and make
# clean...start
#
# AUTHOR:
# �ystein God�y, DNMI/FOU, 21/08/1999
#
# MODIFIED:
# �ystein God�y, DNMI/FOU, 16/03/2001
# for use on 16 bits imagery and use on Linux.
# �ystein God�y, DNMI/FOU, 05.03.2004
# Adapted for use with new version of libsatimg supporting both MEOS HDF
# and HDF5 as well as METSAT input.
# Changed name of read_image.c to readandbytepack.c
# �ystein God�y, METNO/FOU, 21.02.2008: Switched to fmlibs
#

LIBSDIR = $(HOME)/software
SRCDIR = $(HOME)/software/avhrr2mitiff

#LDFLAGS =  \
#  -L $(LIBSDIR)/libsatimg/lib -lsatimg \
#  -L $(LIBSDIR)/libmeos/lib -lhdf_format -ldf -ljpeg -lz \
#  -L $(LIBSDIR)/libmeoshdf5/lib -lMEOSHDF5 \
#   /disk1/hdf5lib-1p6p/lib/libhdf5.a -lz \
#  -L $(LIBSDIR)/libfmcoord/lib -lfmcoord \
#  -L $(LIBSDIR)/libproj/lib -lproj \
#  -ltiff -lm -g

LDFLAGS = \
  -L $(LIBSDIR)/lib -lfmutil \
  -L $(LIBSDIR)/lib -lfmio \
  -lproj \
  -lhdf5 \
  -ltiff -lm -g

#CFLAGS = \
#  -I./ \
#  -I$(LIBSDIR)/libmeos/include \
#  -I$(LIBSDIR)/libmeoshdf5/include \
#  -I$(LIBSDIR)/libsatimg/include  \
#  -I$(LIBSDIR)/libfmcoord/include  \
#  -I$(LIBSDIR)/libproj/include  \
#  -g

CFLAGS = \
  -I./ \
  -I$(LIBSDIR)/include \
  -g

RUNFILE = \
  avhrr2mitiff

OBJS = \
  avhrr2mitiff.o \
  readandbytepack.o

$(RUNFILE): $(OBJS)
	$(CC) $(CFLAGS) -o $(RUNFILE) $(OBJS) $(LDFLAGS) 

clean:
	find . -name "*.o" -exec rm {} \;

rambo:
	find . -name "*.o" -exec rm {} \;
	find . -name $(RUNFILE) -exec rm {} \;
