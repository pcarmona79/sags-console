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
// $Revision: 1.2 $
// $Date: 2005/03/23 22:20:50 $
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
#include "UI.hpp"
#include "Config.hpp"

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

#ifdef ENABLE_NLS
	textdomain (GETTEXT_PACKAGE);
	bindtextdomain (GETTEXT_PACKAGE, PACKAGE_LOCALE_DIR);
#endif

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

	// el programa principal comienza aquí
	Application.Init (debug_mode);

	// iniciamos la interfaz de usuario
	NUI = new UserInterface ();

	//Server.AddOptions ();
	Logs.AddOptions ();
	//Application.AddOptions ();

	Logs.Start ();

	Config.GetOptionsFromFile (config_file);

	Logs.Add (Log::Info, _("SAGS Text Client version %s"), VERSION);

	//Server.Start ();

	//retval = Application.Run ();

	// si estamos aquí es que estamos saliendo
	Logs.Add (Log::Notice, _("Exiting with %d"), retval);
	getchar ();

	// terminamos la interfaz de usuario
	delete NUI;

	return retval;
}
