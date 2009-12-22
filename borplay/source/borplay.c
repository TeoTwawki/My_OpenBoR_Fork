/*
	Copyright 2006 Luigi Auriemma

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA

	http://www.gnu.org/licenses/gpl.txt
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "adpcm.h"

#ifdef WIN32
	#include <conio.h>
	#include "ao.h"
#else
	#include <ao/ao.h>
	#include "kbhit.h"

	#define getch       readch
	#define stricmp     strcasecmp
	#define strnicmp    strncasecmp
	#define stristr     strcasestr
#endif



#define VER     "0.2"

#define MYFOPEN printf("- open file %s\n", fname);  \
				fd = fopen(fname, "rb");            \
				if(!fd)

/** Borplay verison 0.2 - stereo support added by Plombo */

int borplay(FILE *fd, u_char *fname, u_int off, u_int size);
int check_kbhit(void);
void aoinit(u_char *driver);
void aoinit_fmt(int channels, int frequency);
void ao_error(char *device);
void std_err(void);



#define     BOR_MUSIC_VERSION   0x00010000
#define     NEW_MUSIC_VERSION   0x00010001
#define     BOR_IDENTIFIER      "BOR music"
#define     MUSIC_BUF_SIZE      8192

typedef struct {
	u_int   pns_len;
	u_int   filestart;
	u_int   filesize;
	char    namebuf[80];
} pnamestruct;

typedef struct {
	char    identifier[16];
	char    artist[64];
	char    title[64];
	u_int   version;
	int     frequency;
	int     channels;
	int     datastart;
} bor_header;



ao_device           *device = NULL;
ao_sample_format    format;
int                 ao_driver;  // not used, this tool is very simple

// This method copied from original BOR source file PACKFILE.C
// Convert slashes (UNIX) to backslashes (DOS).
// Return a pointer to buffer with filename converted to DOS format.
static char * slashback(char *sz){
    int i=0;
    static char new[256];

    while(sz[i] && i<255){
	new[i] = sz[i];
	if(new[i]=='/') new[i]='\\';
	++i;
    }
    new[i] = 0;

    return new;
}


int main(int argc, char *argv[]) {
	pnamestruct pn;
	FILE    *fd;
	u_int   off;
	int     len;
	int		count = 0;
	u_char  pack[4],
			*fname = NULL,
			*pbor  = NULL,
			*p;

	setbuf(stdin,  NULL);
	setbuf(stdout, NULL);
	setbuf(stderr, NULL);

	fputs("\n"
		"BOR music player "VER"\n"
		"v0.1 by Luigi Auriemma\n"
		"e-mail: aluigi@autistici.org\n"
		"web:    aluigi.org\n"
		"v0.2 by Plombo\n"
		"e-mail: plombex342@gmail.com\n"
		"\n", stdout);

	if(argc < 2) {
		printf("\n"
			"Usage: %s <file.BOR/PAK> [filename]\n"
			"\n"
			"filename is a part of the bor file you want play\n"
			"use this function for playing a specific file in a PAK archive\n"
			"\n", argv[0]);
		exit(1);
	}

	if(argc > 2) pbor = slashback(argv[2]);
	fname = argv[1];

	MYFOPEN {
		fname = malloc(strlen(argv[1]) + 5);
		if(!fname) std_err();
		p = fname + sprintf(fname, "%s", argv[1]);

		strcpy(p, ".bor");
		MYFOPEN {

			strcpy(p, ".pak");
			MYFOPEN std_err();
		}
	}

	aoinit(NULL);
	printf ("- press Ctrl+Z, Ctrl+D, Ctrl+C, Esc, 'x', or 'q' to exit\n"
			"- press Tab or Enter to go to the next song\n"
			"\n");
	if(!fread(pack, 4, 1, fd)) goto quit;
	if(memcmp(pack, "PACK", 4)) {
		borplay(fd, fname, 0, -1);
		goto quit;
	}

	if(fseek(fd, -4, SEEK_END) < 0) std_err();

	if(!fread(&off, 4, 1, fd)) goto quit;
	if(fseek(fd, off, SEEK_SET) < 0) std_err();
	
	
	if (!pbor)
		printf("- Playing all music files in %s\n", argv[1]);
	while((len = fread(&pn, 1, sizeof(pn), fd)) > 12) {
		p = strrchr(pn.namebuf, '.');
		
		if((p && !stricmp(p, ".bor")) || (stristr(pn.namebuf, "music"))) {
			if(pbor && !stristr(pn.namebuf, pbor)) goto next;
			if(borplay(fd, pn.namebuf, pn.filestart, pn.filesize) < 0) break;
			count++;
		}

next:
		off += pn.pns_len;
		if(fseek(fd, off, SEEK_SET) < 0) std_err();
	}
	
	if (!count) {
		printf("- no matching files found\n");
	}

quit:
	fclose(fd);
	if(device) {
		ao_shutdown();
	}
	printf("- finished\n");
	return(0);
}

int borplay(FILE *fd, u_char *fname, u_int off, u_int size) {
	bor_header  bh;
	int     len,
			kb = 0;
	short   *out;
	char    *in, *p;
	char	*artist;
	char	*title;
	

	if(fseek(fd, off, SEEK_SET) < 0) std_err();
	if(!fread(&bh, sizeof(bh), 1, fd)) return(-1);
	size -= sizeof(bh);

	if(strncmp(bh.identifier, BOR_IDENTIFIER, sizeof(bh.identifier))) return(0);

	if((bh.version != BOR_MUSIC_VERSION) && (bh.version != NEW_MUSIC_VERSION)) {
		printf("- alert: this file version could be not supported (%08x)\n", bh.version);
	}
	
	// fix title
	for(p = bh.title; *p; p++) {
		if (*p == '_') *p = ' ';
	}
	
	// fix artist
	for(p = bh.artist; *p; p++) {
		if (*p == '_') *p = ' ';
	}
	
	// force mono if it's a v1 file
	if ((bh.version == BOR_MUSIC_VERSION) && (bh.channels != 1))
	{
		printf("- alert: forcing mono playback because of file version; "
				"use the newest Wav2Bor from LavaLit.com to create BOR files "
				"with more than 1 channel of audio\n");
		bh.channels = 1;
	}

	printf("- %08x (%dhz %dch) %s\n",
		off,
		bh.frequency,   bh.channels,
		fname);
	
	// only print title and artist if at least one of them is specified
	if (strlen(bh.artist) || strlen(bh.title))
		printf("  \"%s\" - \"%s\"\n", bh.artist, bh.title);

	if(bh.datastart > sizeof(bh)) {
		bh.datastart -= sizeof(bh);
		if(fseek(fd, bh.datastart, SEEK_CUR) < 0) std_err();
		size -= bh.datastart;
	}

	in = malloc(MUSIC_BUF_SIZE);
	if(!in) std_err();
	out = malloc(MUSIC_BUF_SIZE << 2);
	if(!out) std_err();

	aoinit_fmt(bh.channels, bh.frequency);

#ifndef WIN32
	init_keyboard();
#endif

	while(size && (len = fread(in, 1, MUSIC_BUF_SIZE, fd))) {
		if(size < len) len = size;
		size -= len;
		
		adpcm_decode(in, out, len, bh.channels);
		
		if(!ao_play(device, (void *)out, len << 2)) break;
		
		kb = check_kbhit();
		if(kb) break;
	}

#ifndef WIN32
	close_keyboard();
#endif

	ao_close(device);
	free(in);
	free(out);
	return(kb);
}



int check_kbhit(void) {
	if(kbhit()) {
		switch(getch()) {
			case 0x03:  // CTRL-C
			case 0x04:  // CTRL-D
			case 0x1a:  // CTRL-Z
			case 0x1b:  // ESC
			case 'q':
			case 'x':   return(-1); break;  // exit
			case '\t':
			case '\r':
			case '\n':  return(1);  break;  // next song
			case ' ': {
				fgetc(stdin);
				} break;
			}
	}
	return(0);
}



void aoinit(u_char *driver) {
	printf("- initialize audio device\n");
	ao_initialize();
	if(driver) {
		ao_driver = (ao_driver < 0) ? atoi(driver) : ao_driver_id(driver);
	} else {
		ao_driver = ao_default_driver_id();
	}
	if(ao_driver < 0) ao_error(driver);
}



void aoinit_fmt(int channels, int frequency) {
	format.bits        = 16;
	format.channels    = channels;
	format.rate        = frequency;
	format.byte_format = AO_FMT_LITTLE;
	device = ao_open_live(ao_driver, &format, NULL);
	if(!device) ao_error("");
}



void ao_error(char *device) {
	fprintf(stderr, "\nError: impossible to open the audio device %s\n", device);
	exit(1);
}



void std_err(void) {
	perror("\nError");
	exit(1);
}


