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
// $Source: /home/pablo/Desarrollo/sags-cvs/clienttext/src/Config.cpp,v $
// $Revision: 1.2 $
// $Date: 2005/03/23 22:20:48 $
//

#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdlib>
#include <cerrno>

#include "Config.hpp"
#include "Log.hpp"
#include "Utils.hpp"

using namespace std;

Configuration::Configuration ()
{
	ConfigFile = NULL;
	memset (FileName, 0, CONF_MAX_FILENAME + 1);
}

Configuration::~Configuration ()
{
	
}

void Configuration::GetOptionsFromFile (const char *filename)
{
	char line[CONF_MAX_LINE + 1];
	char *group = NULL, *name = NULL, *data = NULL;
	int i, value;
	unsigned int n;
	bool using_default_config_file = false;

	if (filename != NULL)
	{
		// copiamos sólo si no está vacío
		if (strlen (filename))
			strncpy (FileName, filename, strlen (filename));
	}

	// si no se especificó un archivo de configuración
	// usamos $HOME/.sagscl-text
	if (strlen (FileName) == 0)
	{
		snprintf (FileName, CONF_MAX_FILENAME + 1, "%s/.%s", getenv("HOME"), PACKAGE);
		using_default_config_file = true;
	}

	// reutilizar el archivo
	if (ConfigFile != NULL)
		ConfigFile->open (FileName);
	else
		// abrimos para lectura
		ConfigFile = new fstream (FileName, ios::in);

	// archivo reabierto queda en EOF
	ConfigFile->clear ();

	// si el archivo no existe, entonces terminamos el programa,
	// a menos que el archivo que no se pudo abrir fuera $HOME/.sagscl-text
	// y en ese caso lo creariamos y vaciaríamos en él los valores por defecto
	if (!ConfigFile->is_open ())
	{
		if (using_default_config_file)
		{
			Logs.Add (Log::Config | Log::Info,
				  _("Using default file \"%s\""), FileName);

			// escribir opciones por defecto
			WriteOptions ();

			// reabrimos el archivo y lo reseteamos
			ConfigFile->open (FileName, ios::in);
			ConfigFile->clear ();
		}
		else
		{
			Logs.Add (Log::Config | Log::Critical,
				  _("%s: %s"), FileName, strerror (errno));
			errno = 0;
		}
	}

	for (i = 1; !ConfigFile->eof(); ++i)
	{
		ConfigFile->getline (line, CONF_MAX_LINE);
		//Logs.Add (Log::Config | Log::Debug, "%2d %s", i, line);

		// comprobamos que la línea
		// sea de configuración

		if (line[0] == '#')
		{
			// un comentario descartado
			//Logs.Add (Log::Config | Log::Debug,
			//	  "Comment at line %d discarted", i);
			continue;
		}

		if (line[0] == '\0')
		{
			// una línea en blanco descartada
			//Logs.Add (Log::Config | Log::Debug,
			//	  "Blank line %d discarted", i);
			continue;
		}

		// buscamos un grupo
		if (line[0] == '[' && line[strlen(line) - 1] == ']')
		{
			if (group != NULL)
			{
				delete[] group;
				group = NULL;
			}

			group = Utils.SubString (line, 1, strlen (line) - 2);

			if (group == NULL)
			{
				// no es una línea de configuración
				Logs.Add (Log::Config | Log::Notice,
					  _("No group! Line %d isn't a valid configure line"), i);
			}
			//else
			//	Logs.Add (Log::Config | Log::Debug,
			//		  "New group [%s]", group);

			continue;
		}

		for (n = 0; n <= strlen (line) - 1; ++n)
		{
			if (line[n] == '=')
				break;
		}

		if (line[n] == '\0')
		{
			// no es una línea de configuración
			Logs.Add (Log::Config | Log::Notice,
				  _("Line %d isn't a valid configure line"), i);
			continue;
		}

		name = Utils.SubString (line, 0, n - 1);
		data = Utils.SubString (line, n + 1, strlen (line) - 1);

		if (name == NULL || data == NULL)
		{
			// no es una línea de configuración
			Logs.Add (Log::Config | Log::Notice,
				  _("Line %d isn't a valid configure line"), i);
			continue;
		}

		//Logs.Add (Log::Config | Log::Debug, "Option [%s]%s=%s",
		//	  group, name, data);

		// agregamos a la lista luego de
		// comprobar el tipo de dato

		if (data[0] == '\"')
		{
			// si es una cadena de texto sacamos las comillas
			++data;
			if (data[strlen (data) - 1] == '\"')
				data[strlen (data) - 1] = '\0';
			Set (Conf::String, group, name, data);
			--data;
		}
		else if (!strcasecmp (data, "yes") ||
			 !strcasecmp (data, "true") ||
			 !strcasecmp (data, "on"))
		{
			// variable booleana con valor verdadero
			Set (Conf::Boolean, group, name, 1);
		}
		else if (!strcasecmp (data, "no") ||
			 !strcasecmp (data, "false") ||
			 !strcasecmp (data, "off"))
		{
			// variable booleana con valor falso
			Set (Conf::Boolean, group, name, 0);
		}
		else
		{
			// es un valor numérico
			value = (int) strtol (data, (char **)NULL, 10);
			if (errno == EINVAL && value == 0)
			{
				// número no válido, se descarta la línea
				Logs.Add (Log::Config | Log::Notice,
					  _("Number not valid at line %d"), i);
				continue;
			}
			Set (Conf::Numeric, group, name, value);
		}

		// liberamos el espacio asignado
		if (name != NULL)
			delete[] name;
		if (data != NULL)
			delete[] data;
	}

	// liberamos el espacio asignado al grupo
	if (group != NULL)
		delete[] group;

	// cerramos el archivo de configuración
	ConfigFile->close ();

	// por último reiniciamos Logs para que
	// tome los nuevos valores
	Logs.Start ();

	Logs.Add (Log::Config | Log::Info, _("Processed file \"%s\""), FileName);
}

struct option *Configuration::Add (Conf::OpType type, const char *group,
				   const char *name, int val)
{
	struct option *opt = new struct option (group, name, type);

	//cout << "Adding ('" << opt->group << "', '" << opt->name << "') = " << val << endl;

	// val debe ser convertido según type
	switch (opt->type)
	{
	case Conf::Boolean:
		opt->value = (val) ? 1 : 0;
		opt->string[0] = '\0';
		list << opt;
		break;

	case Conf::Numeric:
		opt->value = val;
		opt->string[0] = '\0';
		list << opt;
		break;

	default:
		delete opt;
		return NULL;
	}

	return opt;
}

struct option *Configuration::Add (Conf::OpType type, const char *group,
				   const char *name, const char *val)
{
	struct option *opt = new struct option (group, name, type);

	//cout << "Adding ('" << opt->group << "', '" << opt->name << "') = " << val << endl;

	// val es char* y ya opt es Conf::String, pero comprobamos igual
	switch (opt->type)
	{
	case Conf::String:
		strncpy (opt->string, val, CONF_MAX_STRING);
		opt->value = 0;
		list << opt;
		break;
	default:
		delete opt;
		return NULL;
	}

	return opt;
}

struct option *Configuration::Get (Conf::OpType type, const char *group,
				   const char *name, const char *val)
{
	struct option *opt, searched (group, name);

	opt = list.Find (searched);

	if (opt != NULL)
	{
		// retornamos objeto encontrado
		return opt;
	}

	// si llegamos aquí es que es una opción desconocida
	return Add (type, group, name, val);
}

struct option *Configuration::Get (Conf::OpType type, const char *group,
				   const char *name, int val)
{
	struct option *opt, searched (group, name);

	opt = list.Find (searched);

	if (opt != NULL)
	{
		// retornamos objeto encontrado
		return opt;
	}

	// si llegamos aquí es que es una opción desconocida
	return Add (type, group, name, val);
}

void Configuration::Set (Conf::OpType type, const char *group,
			 const char *name, int val)
{
	struct option *opt, searched (group, name);

	opt = list.Find (searched);

	if (opt != NULL)
	{
		// val debe ser convertido según type
		switch (type)
		{
		case Conf::Boolean:
			opt->value = (val) ? 1 : 0;
			opt->string[0] = '\0';
			//Logs.Add (Log::Config | Log::Debug,
			//	  "Changing Boolean %s.%s to %d", group, name, val);
			break;

		case Conf::Numeric:
			opt->value = val;
			opt->string[0] = '\0';
			//Logs.Add (Log::Config | Log::Debug,
			//	  "Changing Numeric %s.%s to %d", group, name, val);
			break;

		default:
			;
		}
	}
	else
		// si no existe la opción, la agregamos
		Add (type, group, name, val);
}

void Configuration::Set (Conf::OpType type, const char *group,
			 const char *name, const char *val)
{
	struct option *opt, searched (group, name);

	opt = list.Find (searched);

	if (opt != NULL)
	{
		// opt ya es de tipo Conf::String, pero por si acaso comprobaremos
		switch (opt->type)
		{
		case Conf::String:
			strncpy (opt->string, val, CONF_MAX_STRING);
			opt->value = 0;
			//Logs.Add (Log::Config | Log::Info,
			//	  "Changing String %s.%s to %s", group, name, val);
			break;
		default:
			;
		}
	}
	else
		// si no existe la opción, la agregamos
		Add (type, group, name, val);
}

bool Configuration::Check (const char *group, const char *name)
{
	struct option *opt, searched (group, name);

	opt = list.Find (searched);

	return (opt != NULL);
}

void Configuration::WriteOptions (void)
{
	// crear archivo por defecto
	ConfigFile->open (FileName, ios::in | ios::out | ios::trunc);

	// TODO: recorrer list sacando las opciones
	//       y escribiéndolas al archivo

	ConfigFile->close ();
}

// definimos el objeto
Configuration Config;
