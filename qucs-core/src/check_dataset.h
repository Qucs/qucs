/*
 * check_dataset.h - checker definitions for the Qucs dataset
 *
 * Copyright (C) 2003 Stefan Jahn <stefan@lkcc.org>
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this package; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.  
 *
 * $Id: check_dataset.h,v 1.1 2003/12/20 19:03:26 ela Exp $
 *
 */

#ifndef __CHECK_DATASET_H__
#define __CHECK_DATASET_H__

extern dataset * dataset_result;
extern vector  * dataset_vector;
extern strlist * dataset_idents;

__BEGIN_DECLS

/* Externalize variables used by the scanner and parser. */
extern int dataset_lineno;
extern FILE * dataset_in;

/* Available functions of the checker. */
int dataset_parse (void);
int dataset_error (char *);
int dataset_lex (void);
int dataset_check (dataset *);

__END_DECLS

#endif /* __CHECK_DATASET_H__ */
