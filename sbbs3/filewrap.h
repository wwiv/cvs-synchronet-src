/* filewrap.h */

/* File system-call wrappers */

/* $Id$ */

/****************************************************************************
 * @format.tab-size 4		(Plain Text/Source Code File Header)			*
 * @format.use-tabs true	(see http://www.synchro.net/ptsc_hdr.html)		*
 *																			*
 * Copyright 2000 Rob Swindell - http://www.synchro.net/copyright.html		*
 *																			*
 * This program is free software; you can redistribute it and/or			*
 * modify it under the terms of the GNU General Public License				*
 * as published by the Free Software Foundation; either version 2			*
 * of the License, or (at your option) any later version.					*
 * See the GNU General Public License for more details: gpl.txt or			*
 * http://www.fsf.org/copyleft/gpl.html										*
 *																			*
 * Anonymous FTP access to the most recent released source is available at	*
 * ftp://vert.synchro.net, ftp://cvs.synchro.net and ftp://ftp.synchro.net	*
 *																			*
 * Anonymous CVS access to the development source and modification history	*
 * is available at cvs.synchro.net:/cvsroot/sbbs, example:					*
 * cvs -d :pserver:anonymous@cvs.synchro.net:/cvsroot/sbbs login			*
 *     (just hit return, no password is necessary)							*
 * cvs -d :pserver:anonymous@cvs.synchro.net:/cvsroot/sbbs checkout src		*
 *																			*
 * For Synchronet coding style and modification guidelines, see				*
 * http://www.synchro.net/source.html										*
 *																			*
 * You are encouraged to submit any modifications (preferably in Unix diff	*
 * format) via e-mail to mods@synchro.net									*
 *																			*
 * Note: If this box doesn't appear square, then you need to fix your tabs.	*
 ****************************************************************************/

#ifndef _FILEWRAP_H
#define _FILEWRAP_H

/**********/
/* Macros */
/**********/

#if defined(_WIN32)

	#include <io.h>				/* _sopen */
	#include <sys/stat.h>		/* S_IREAD */
	#include <fcntl.h>			/* O_BINARY */
	#include <windows.h>		/* OF_SHARE_ */

	#define sopen(f,o,s)		_sopen(f,o,s,S_IREAD|S_IWRITE)
	#define close(f)			_close(f)
							
	#ifndef SH_DENYNO
	#define SH_DENYNO			OF_SHARE_DENY_NONE
	#define SH_DENYWR			OF_SHARE_DENY_WRITE
	#define SH_DENYRW			OF_SHARE_EXCLUSIVE
	#endif
	#ifndef O_DENYNONE
	#define O_DENYNONE			SH_DENYNO
	#endif

#elif defined(__unix__)

	#include <fcntl.h>

	#define O_TEXT		0		/* all files in binary mode on Unix */
	#define O_BINARY	0		/* all files in binary mode on Unix */
	#define O_DENYNONE  (1<<31)	/* req'd for Baja/nopen compatibility */

	#define SH_DENYNO	2          // no locks
	#define SH_DENYRW	F_WRLCK	   // exclusive lock
	#define SH_DENYWR   F_RDLCK    // shareable lock
	
	#define chsize(fd,size)		ftruncate(fd,size)
	#define tell(fd)			lseek(fd,0,SEEK_CUR)

#endif

/**************/
/* Prototypes */
/**************/

#ifdef DLLEXPORT
#undef DLLEXPORT
#endif
#ifdef DLLCALL
#undef DLLCALL
#endif

#ifdef _WIN32
	#ifdef WRAPPER_DLL
		#define DLLEXPORT	__declspec(dllexport)
	#else
		#define DLLEXPORT	__declspec(dllimport)
	#endif
	#ifdef __BORLANDC__
		#define DLLCALL __stdcall
	#else
		#define DLLCALL
	#endif
#else	/* !_WIN32 */
	#define DLLEXPORT
	#define DLLCALL
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(__BORLANDC__)
	DLLEXPORT int	DLLCALL	lock(int fd, long pos, int len);
	DLLEXPORT int	DLLCALL unlock(int fd, long pos, int len);
#endif

#if defined(__unix__)
	DLLEXPORT int	DLLCALL sopen(char *fn, int access, int share);
	DLLEXPORT long	DLLCALL filelength(int fd);
#endif

DLLEXPORT time_t	DLLCALL filetime(int fd);

#ifdef __cplusplus
}
#endif

#endif	/* Don't add anything after this line */
