//
// OPT - Command line parser
// Copyright (C) 2005 Pablo Carmona Amigo
//
// This file is part of OPT.
//
// OPT is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// OPT is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Library General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//
// $Source: /home/pablo/Desarrollo/sags-cvs/clienttext/src/Opt.cpp,v $
// $Revision: 1.1 $
// $Date: 2005/03/21 15:33:27 $
//

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "Opt.hpp"

Options::Options ()
{
	options = NULL;
	last_option_added = NULL;
}

Options::~Options ()
{
	
}

char *Options::SubStr (const char *src, int from, int to)
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

int Options::GetCharPosition (const char *str, char c)
{
	int i;

	if (str == NULL)
		return -1;

	for (i = 0; i <= (int) strlen (str) - 1; ++i)
		if (str[i] == c)
			return i;

	return -1;
}

#ifdef DEBUG
void Options::ListOptions (void)
{
	int i;

	for (i = 0; options[i].type != OPT_NULL; ++i)
	{
		printf ("%2d -> {\"%s\", '%c', ", i, options[i].lname, options[i].sname);
		switch (options[i].type)
		{
		case OPT_BOOLEAN:
			printf ("OPT_BOOLEAN, %d, ", *((int*) options[i].var));
			break;
		case OPT_STRING:
			printf ("OPT_STRING, \"%s\", ", (char *) options[i].var);
			break;
		}
		printf ("%d}\n", options[i].size);
	}
}
#endif

void Options::InitOptions (struct opt *opts)
{
	int i;

	options = opts;

	for (i = 0; options[i].type != OPT_NULL; ++i)
		memset (options[i].var, 0, options[i].size);

#ifdef DEBUG
	ListOptions ();
#endif
}

int Options::SetLongOption (const char *name, const char *value)
{
	int i;

#ifdef DEBUG
	printf ("OPTION LONG \"%s\" VALUE \"%s\"\n", name, value);
#endif

	for (i = 0; options[i].type != OPT_NULL; ++i)
	{
		if (!strncasecmp (options[i].lname, name, strlen (name)))
		{
			switch (options[i].type)
			{
			case OPT_BOOLEAN:
				if (!strncasecmp (value, "true", 4))
					*((int *) options[i].var) = 1;
				else
					*((int *) options[i].var) = 0;
				break;
			case OPT_STRING:
				strncpy ((char *) options[i].var,
					 value,
					 options[i].size - 1);
			}
			last_option_added = &options[i];
			return 0;
		}
	}
	return -1;
}

int Options::SetShortOption (char n, const char *value)
{
	int i;

#ifdef DEBUG
	printf ("OPTION SHORT '%c' VALUE \"%s\"\n", n, value);
#endif

	for (i = 0; options[i].type != OPT_NULL; ++i)
	{
		if (options[i].sname == n)
		{
			switch (options[i].type)
			{
			case OPT_BOOLEAN:
				if (!strncasecmp (value, "true", 4))
					*((int *) options[i].var) = 1;
				else
					*((int *) options[i].var) = 0;
				break;
			case OPT_STRING:
				strncpy ((char *) options[i].var,
					 value,
					 options[i].size - 1);
			}
			last_option_added = &options[i];
			return 0;
		}
	}
	return -1;
}

int Options::CheckTextArgument (const char *text)
{
#ifdef DEBUG
	printf ("TEXT \"%s\"\n", text);
#endif
	/* si la ultima opcion agregada era OPT_STRING,
	   pero fue agregada como boolean (contiene "true"),
	   entonces agregar este texto a esa opcion */

	if (last_option_added != NULL)
		if (last_option_added->type == OPT_STRING)
			if (!strncasecmp ((char *) last_option_added->var, "true", 4))
			{
				strncpy ((char *) last_option_added->var,
					 text,
					 last_option_added->size - 1);
				return 0;
			}

	return -1;
}

void Options::SetInvalidOptionPosition (int *idx, int pos)
{
	*idx = -((pos << 24) | *idx);
}

void Options::GetInvalidOptionPosition (int *idx, int *pos)
{
	*idx = -*idx;
	*pos = *idx >> 24;
	*idx = *idx << 24;
	*idx = *idx >> 24;
}

int Options::Parse (int argc, char** argv, struct opt *opts)
{
	int i, j, n, ne = 0;
	char *optarg, *name, *value;

	InitOptions (opts);

	for (i = 1; i <= argc - 1; ++i)
	{
		optarg = SubStr (argv[i], 0, -1);

		if (optarg[0] == '-')
		{
			if (optarg[1] == '-')
			{
				if (strlen (optarg) == 2)
				{
					/* -- indica que despues no vienen
					   opciones, por lo que salimos */
					++i;
					free (optarg);
					break;
				}

				n = GetCharPosition (optarg, '=');
				if (n > 2)
				{
					/* --name=value */
					name = SubStr (optarg, 2, n - 1);
					value = SubStr (optarg, n + 1, -1);
					ne = SetLongOption (name, value);
				}
				else
				{
					/* --name */
					name = SubStr (optarg, 2, -1);
					value = SubStr ("true", 0, -1);
					ne = SetLongOption (name, value);
				}
				free (name);
				free (value);

				if (ne)
				{
					/* opcion no existe */
					i = -i;
					free (optarg);
					break;
				}
			}
			else
			{
				/* -a o -abcd */
				for (j = 1; j <= (int) strlen (optarg) - 1; ++j)
				{
					ne = SetShortOption (optarg[j], "true");
					if (ne)
						break;
				}
				if (ne)
				{
					/* opcion no existe, enmascaramos la opcion
					   que no existe */
					SetInvalidOptionPosition (&i, j);
					free (optarg);
					break;
				}
			}
		}
		else
		{
			/* probamos si este argumento pertenece a alguna
			   opcion OPT_STRING */
			if (CheckTextArgument (optarg))
			{
				/* argumento es solo texto por lo que salimos del bucle */
				free (optarg);
				break;
			}
		}

		free (optarg);
	}

#ifdef DEBUG
	ListOptions ();
#endif

	/* retornamos el indice de la ultima argv revisada */
	return i;
}
