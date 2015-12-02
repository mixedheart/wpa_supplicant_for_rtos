/*
 * file.h
 *
 *  Created on: 2015-7-13
 *      Author: niuyue
 */

#ifndef FILE_H_
#define FILE_H_

#ifdef	FILE
#undef	FILE
#endif
//#define	FILE	FIL

#define	FILE_IDLE	0
#define FILE_USED	1

//#include	"ff.h"
#include	"common.h"

#define SEEK_SET 0 /* start of stream (see fseek) */
#define SEEK_CUR 1 /* current position in stream (see fseek) */
#define SEEK_END 2 /* end of stream (see fseek) */

//FIL *_fopen(const char * filename, const char * mode);
//int _fclose(FIL *stream);
//size_t _fread(void *ptr, size_t size, size_t nmemb, FIL * fp);
//size_t _fwrite(void *ptr, size_t size, size_t nmemb, FIL * fp);
//int _fputs(const char * str, int n, FIL* fp);
//long int _ftell(FIL *fp);

#define	_feof		f_eof
#define _ferror		f_error

#define	fopen		_fopen
#define	fclose		_fclose
#define	fread		_fread
#define	fwrite		_fwrite
#define fputs		_fputs
#define ftell		_ftell
#define	fseek		_fseek_new
#define perror		_perror
#define	exit		_exit
//#define	feof		_feof

#define fprintf		f_printf
#define fputc		f_putc
#define	fgets		f_gets

#endif /* SYS_H_ */
