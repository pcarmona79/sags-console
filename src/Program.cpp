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
// $Source: /home/pablo/Desarrollo/sags-cvs/clienttext/src/Program.cpp,v $
// $Revision: 1.1 $
// $Date: 2005/03/21 15:33:27 $
//

#include <sys/types.h>
#include <unistd.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cerrno>
#include <sys/stat.h>

#include "Log.hpp"
#include "Network.hpp"
#include "Main.hpp"
#include "Opt.hpp"

int main (int argc, char **argv)
{
	char config_file[101];
	int retval = 0, debug_mode, n, pos;
	Options Opt;
	struct opt options[] = {
		{"debug", 'D', OPT_BOOLEAN, (void *) &debug_mode, sizeof (int)},
		{"conf", 'c', OPT_STRING, (void *) config_file, 101},
		{0, 0, 0, 0, 0}
	};

	// revisamos la línea de comandos
	n = Opt.Parse (argc, argv, options);	

	if (n < 0)
	{
		Opt.GetInvalidOptionPosition (&n, &pos);
		if (pos > 0)
			fprintf (stderr, _("There is an invalid option: '%c'\n"), argv[n][pos]);
		else
			fprintf (stderr, _("There is an invalid option: \"%s\"\n"), argv[n]);

		exit (EXIT_FAILURE);
	}

	// si no se especificó un archivo de configuración
	// usamos $HOME/.sagscl-text
	if (strlen (config_file) == 0)
		snprintf (config_file, 101, "%s/.%s", getenv("HOME"), PACKAGE);

	// el programa principal comienza aquí
	Application.Init (debug_mode);

	//Server.AddOptions ();
	//Logs.AddOptions ();
	//Application.AddOptions ();

	//Logs.Start ();

/*	if (absolute_config_file != NULL)
		Config.GetOptionsFromFile (&configuration, absolute_config_file);
	else
		Config.GetOptionsFromFile (&configuration, config_file);

	Logs.Add (Log::Info, "SAGS Server version %s", VERSION);
*/
	//Server.Start ();

	//retval = Application.Run ();

	// si estamos aquí es que estamos saliendo
	Logs.Add (Log::Notice, _("Exiting with %d"), retval);
	getchar ();

	return retval;
}
