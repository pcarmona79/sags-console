//
// SAGS - Secure Administrator of Game Servers
// Copyright (C) 2005 Pablo Carmona Amigo
// 
// This file is part of SAGS Text Client.
//
// SAGS Text Client is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// SAGS Text Client is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Library General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//
// $Source: /home/pablo/Desarrollo/sags-cvs/clienttext/src/Utils.cpp,v $
// $Revision: 1.1 $
// $Date: 2005/03/21 15:33:27 $
//

#include <iostream>
#include <cstring>
#include <cmath>
#include <openssl/md5.h>

#include "Utils.hpp"

char **Utilities::StrSplit (const char *str, char delim, int elem, int *count)
{
	int n = 0, i, len;
	const char *p = str, *start;
	char **vect;

	while (*p != '\0')
	{
		if (*p == delim)
			++n;
		++p;
	}
	++n;

	if (elem > n || elem < 0)
		elem = n;

	vect = new char* [elem + 1];
	p = str;

	for (i = 0; i <= elem - 1; ++i)
	{
		start = p;

		for (len = 0; *p != delim; ++len, ++p)
		{
			if (*p == '\0')
				break;
		}

		while (*p == delim)
			++p;

		vect[i] = new char[len + 1];
		memset (vect[i], 0, len + 1);

		if (len > 0)
			strncpy (vect[i], start, len);
		else
			vect[i][0] = '\0';
	}

	vect[elem] = NULL;

	if (count != NULL)
		*count = elem;

	return vect;
}

char *Utilities::SubString (const char *src, int from, int to)
{
	char *dest = (char) NULL;
	int len_src, len_dest, i, step;

	if ( src == NULL ) return NULL;

	len_src = strlen (src);

	if (from < 0)
		from = len_src - 1;
	if (to < 0)
		to = len_src - 1;

	if (len_src > 0 && from >= 0 && to >= 0)
	{
		if ( to - from >= 0 )
		{
			len_dest = to - from + 1;
			step = 1;
		}
		else
		{
			len_dest = from - to + 1;
			step = -1;
		}

		if ( len_dest <= len_src )
		{
			dest = new char[len_dest + 1];
			memset (dest, 0, len_dest + 1);

			for ( i = 0; i < len_dest; ++i )
				*(dest + i) = *(src + from + i*step);

			*(dest + len_dest) = (char) NULL;
		}
	}

	return dest;
}

char *Utilities::MD5PasswordHash (const char *password)
{
	char *md5_password;
	char *md5_password_hex;
	char hexadecimal[3];
	int i, tamano;

	md5_password = new char [MD5_DIGEST_LENGTH + 1];
	memset (md5_password, 0, MD5_DIGEST_LENGTH + 1);
	md5_password_hex = new char [2 * MD5_DIGEST_LENGTH + 1];
	memset (md5_password_hex, 0, 2 * MD5_DIGEST_LENGTH + 1);

	tamano = strlen (password);
	MD5 ((unsigned char *) password, tamano, (unsigned char *) md5_password);

	for ( i = 0; i < MD5_DIGEST_LENGTH; ++i ) {
		snprintf (hexadecimal, 3, "%.2x", *(md5_password + i));
		strncat (md5_password_hex, hexadecimal, sizeof (hexadecimal));
	}

	delete[] md5_password;
	return md5_password_hex;
}

char *Utilities::EncodePassword (const char *password)
{
	char *encoded;
	int i;

	encoded = new char [strlen (password) + 1];
	memset (encoded, 0, strlen (password) + 1);

	for (i = 0; i <= (int) strlen (password) - 1; ++i)
		encoded[i] = password[i] ^ 0xAA;

	return encoded;
}

void Utilities::RandomString (char *str, int size)
{
	int i, rndval;

	for (i = 0; i <= size - 1; ++i)
	{
		rndval = (int) trunc (61.0 * rand () / (RAND_MAX + 1.0));
		
		if (rndval >= 0 && rndval <= 9)
			str[i] = (char) (rndval + 48);
		else if (rndval >= 10 && rndval <= 35)
			str[i] = (char) (rndval + 55);
		else if (rndval >= 36 && rndval <= 61)
			str[i] = (char) (rndval + 61);	
		else
			str[i] = '\0'; // ups!
	}
}

// definimos el objeto
Utilities Utils;
