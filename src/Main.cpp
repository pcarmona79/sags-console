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
// $Source: /home/pablo/Desarrollo/sags-cvs/clienttext/src/Main.cpp,v $
// $Revision: 1.1 $
// $Date: 2005/03/21 15:33:27 $
//

#include <cstdio>
#include <cstdlib>
#include <csignal>

#include "Main.hpp"
#include "Network.hpp"
#include "Log.hpp"

Main::Main () : SelectLoop ()
{
	debug = false;
}

Main::~Main ()
{
	
}

void Main::Init (bool debugmode)
{
	debug = debugmode;
	SelectLoop::Init ();
}

void Main::AddOptions (void)
{
	//usersfile = Config.Add (Conf::String, "Main", "UsersFile", "sags.users");
}

int Main::SignalEvent (int sig)
{
	switch (sig)
	{
	case SIGHUP:
		// releeemos los archivos de configuración y
		// el archivo de usuarios

		//Logs.Add (Log::Notice, "Reading configuration file");
		//Config.GetOptionsFromFile ();

		//Logs.Add (Log::Notice, "Reading users file");
		//Application.LoadUsers ();

		//Logs.Add (Log::Notice, "Checking network");
		//cout << "Notice: Checking network" << endl;
		//Net.CheckOptions ();

		break;

	case SIGTERM:
	case SIGINT:
		// Este bloque se llama cuando hay eventos que
		// quieren que el programa termine. Se debe salir
		// ordenadamente.

		// nos desconectamos
		//Net.Shutdown ();

		// una salida distinta de cero terminará el programa
		return sig;

	default:
		Logs.Add (Log::Warning, _("Unhandled event %d"), sig);
	}

	return 0;
}

void Main::DataEvent (int owner, int fd, bool writing)
{
	Logs.Add (Log::Debug,
		  _("DataEvent: owner=%04X fd=%d writing=%s"),
		  owner, fd, (writing) ? _("true"): _("false"));

	switch (owner & Owner::Mask)
	{
	case Owner::Input:
		Logs.Add (Log::Debug, _("Reading from user's input"));
		break;

	case Owner::Network:
		if ((owner & Owner::Send) && writing)
		{
			// socket listo para enviar
			Logs.Add (Log::Debug, _("Sending data to server"));
			//Net.SendData (fd);
		}
		else
		{
			Logs.Add (Log::Debug, _("Receiving data from server"));
			//Net.ReceiveData (fd);
		}
		break;

	default:
		Logs.Add (Log::Warning, _("Unknown owner %04X"), owner);
	}
}

void Main::TimeoutEvent (void)
{
	Logs.Add (Log::Debug, _("Timeout event"));

	// desconectar usuarios no válidos
	//Server.DropNotValidClients ();
}
#if 0
int Main::GenerateResponse (Client *Cl, Packet *Pkt)
{
	int dropit = -1;

	if (Cl != NULL && Pkt != NULL)
	{
		switch (Pkt->GetIndex ())
		{
		case Sync::Index:
			dropit = ProtoSync (Cl, Pkt);
			break;

		case Auth::Index:
			dropit = ProtoAuth (Cl, Pkt);
			break;

		case Error::Index:
			// clientes no envían mensajes de error
			return -1;

		default:
			// paquetes de sesión de un proceso
			if (Pkt->GetIndex () <= Session::MaxIndex)
				dropit = ProtoSession (Cl, Pkt);
			else
				return -1;
		}
	}

	return dropit;
}

int Main::ProtoSync (Client *Cl, Packet *Pkt)
{
	char hello_str[81];

	switch (Pkt->GetCommand ())
	{
	case Sync::Hello:

		snprintf (hello_str, 81, "Welcome %s to SAGS Service", Cl->ShowIP ());

		Cl->AddBuffer (Sync::Index, Sync::Hello, hello_str);
		Add (Owner::Client | Owner::Send, Cl->ShowSocket ());
		break;

	case Sync::Version:

		// chequeamos las versiones
		if (!strncmp (Pkt->GetData (), PCKT_VERSION, 1))
		{
			Cl->Add (new Packet (Sync::Index, Sync::Version, 1, 1, PCKT_VERSION));
			Add (Owner::Client | Owner::Send, Cl->ShowSocket ());
		}
		else
			Server.CloseConnection (Cl->ShowSocket (), Error::Index,
						Error::NotValidVersion);
		break;

	default:
		return -1;
	}

	return 0;
}

int Main::ProtoAuth (Client *Cl, Packet *Pkt)
{
	struct user *usr = NULL;

	switch (Pkt->GetCommand ())
	{
	case Auth::Username:

		if (Cl->GetStatus () == Usr::NeedUser)
		{
			Cl->SetUsername (Pkt->GetData ());
			Cl->SetStatus (Usr::NeedPass);

			// ahora le enviamos la cadena aleatoria
			usr = FindUser (Cl->GetUsername ());
			if (usr == NULL)
			{
				Logs.Add (Log::Warning,
					  "User \"%s\" don't exists",
					  Cl->GetUsername ());

				// enviamos una cadena aleatoria
				// ficticia para despistar :)
				char dumbstr[HASHLEN + 1];
				random_string (dumbstr, HASHLEN);
				Cl->AddBuffer (Auth::Index, Auth::RandomHash, dumbstr);
			}
			else
			{
				// actualizamos la cadena aleatoria
				usr->update ();
				Cl->AddBuffer (Auth::Index, Auth::RandomHash, usr->rndstr);
			}

			Logs.Add (Log::Info,
				  "Sending random string to user \"%s\"",
				  Cl->GetUsername ());
			Add (Owner::Client | Owner::Send, Cl->ShowSocket ());
		}
		break;

	case Auth::Password:

		if (Cl->GetStatus () == Usr::NeedPass)
		{
			usr = FindUser (Cl->GetUsername ());
			if (usr != NULL)
			{
				int retval = strncmp (usr->hash, Pkt->GetData (),
						      strlen (usr->hash));
				//Logs.Add (Log::Debug, "Checking \"%s\" <%d> \"%s\"",
				//	  usr->hash, retval, Pkt->GetData ());

				if (!retval)
				{
					// removemos clientes conectados con el mismo
					// nombre de usuario
					Server.DropDuplicatedClients (Cl);

					// usuario exitosamente autenticado
					Cl->SetStatus (Usr::Real);
					Cl->Add (new Packet (Auth::Index, Auth::Successful));
					Add (Owner::Client | Owner::Send, Cl->ShowSocket ());

					Logs.Add (Log::Notice,
						  "User \"%s\" has logged in from %s",
						  Cl->GetUsername (), Cl->ShowIP ());

					// sacamos el timeout
					Server.RemoveWatch (Cl);

					// agregamos al cliente los servidores autorizados
					// y su modo de mantenimiento
					AddAuthorizedProcesses (Cl, usr);

					// ingresamos al usuario al canal general
					GeneralChannel.UserJoin (Cl);
				}
				else
					Logs.Add (Log::Warning,
						  "User \"%s\" failed to get logged in",
						  Cl->GetUsername ());
			}
			else
				Logs.Add (Log::Warning,
					  "User \"%s\" don't exists",
					  Cl->GetUsername ());
		}
		if (Cl->GetStatus () != Usr::Real)
		{
			Server.CloseConnection (Cl->ShowSocket (), Error::Index,
						Error::LoginFailed);
			return 0;
		}
		break;

	default:
		return -1;
	}

	return 0;
}

int Main::ProtoSession (Client *Cl, Packet *Pkt)
{
	char serv_idx[3], *bufinfo = NULL;

	switch (Pkt->GetCommand ())
	{
	case Session::ConsoleNeedLogs:

		if (Cl->IsAuthorized (Pkt->GetIndex ()))
		{
			Server.SendProcessLogs (Cl, Pkt->GetIndex ());
			break;
		}
		return -1;

	case Session::ConsoleInput:

		if (Cl->IsAuthorized (Pkt->GetIndex ()))
		{
			if (ProcMaster.WriteToProcess (Pkt->GetIndex (), Pkt->GetData ()) > 0)
				Cl->Add (new Packet (Pkt->GetIndex (),
						     Session::ConsoleSuccess));
			else
			{
				snprintf (serv_idx, 3, "%02X", Pkt->GetIndex ());
				Cl->Add (new Packet (Error::Index, Error::CantWriteToProcess,
						     1, strlen (serv_idx), serv_idx));
			}

			Add (Owner::Client | Owner::Send, Cl->ShowSocket ());
			break;
		}
		return -1;

	case Session::ProcessGetInfo:

		// el cliente solicita la información del
		// proceso dado por el índice
		if (Cl->IsAuthorized (Pkt->GetIndex ()))
		{
			bufinfo = ProcMaster.GetProcessInfo (Pkt->GetIndex ());

			if (bufinfo != NULL)
			{
				Cl->AddBuffer (Pkt->GetIndex (), Session::ProcessInfo,
					       bufinfo);
				Add (Owner::Client | Owner::Send, Cl->ShowSocket ());

				// liberamos el espacio asignado
				delete[] bufinfo;
				bufinfo = NULL;
			}
			break;
		}
		return -1;

	case Session::ProcessKill:

		if (Cl->IsAuthorized (Pkt->GetIndex ()))
		{
			if (ProcMaster.KillProcess (Pkt->GetIndex ()))
			{
				snprintf (serv_idx, 3, "%02X", Pkt->GetIndex ());
				Cl->Add (new Packet (Error::Index, Error::ProcessNotKilled,
						     1, strlen (serv_idx), serv_idx));
				Add (Owner::Client | Owner::Send, Cl->ShowSocket ());
			}
			break;
		}
		return -1;

	case Session::ProcessLaunch:

		if (Cl->IsAuthorized (Pkt->GetIndex ()))
		{
			if (ProcMaster.LaunchProcess (Pkt->GetIndex ()))
			{
				snprintf (serv_idx, 3, "%02X", Pkt->GetIndex ());
				Cl->Add (new Packet (Error::Index, Error::ProcessNotLaunched,
						     1, strlen (serv_idx), serv_idx));
				Add (Owner::Client | Owner::Send, Cl->ShowSocket ());
			}
			break;
		}
		return -1;

	case Session::ProcessRestart:

		if (Cl->IsAuthorized (Pkt->GetIndex ()))
		{
			if (ProcMaster.RestartProcess (Pkt->GetIndex ()))
			{
				snprintf (serv_idx, 3, "%02X", Pkt->GetIndex ());
				Cl->Add (new Packet (Error::Index, Error::ProcessNotRestarted,
						     1, strlen (serv_idx), serv_idx));
				Add (Owner::Client | Owner::Send, Cl->ShowSocket ());
			}
			break;
		}
		return -1;

	case Session::Disconnect:

		// ahora basta que el cliente envíe uno
		// de estos paquetes para ser desconectado
		Logs.Add (Log::Notice,
			  "User \"%s\" has logged out",
			  Cl->GetUsername ());
		Cl->SetDrop (true);
		Server.CloseConnection (Cl->ShowSocket (), 0, 0);
		break;

	case Session::ChatTopicChange:

		GeneralChannel.ChangeTopic (Cl, Pkt);
		break;

	case Session::ChatMessage:
	case Session::ChatAction:
	case Session::ChatNotice:

		if (GeneralChannel.MessageChannel (Cl, Pkt))
			return -1;
		break;

	case Session::ChatPrivMessage:
	case Session::ChatPrivAction:
	case Session::ChatPrivNotice:

		if (GeneralChannel.MessagePrivate (Cl, Pkt))
			return -1;
		break;

	case Session::MaintenanceOn:

		// cambiar a modo de mantenimiento al proceso indicado
		// en DATA, sólo si es administrador
		if (Cl->IsAuthorized (0))
		{
			Logs.Add (Log::Notice,
				  "User \"%s\" is changing maintenance mode of process %d",
				  Cl->GetUsername (), Pkt->GetIndex ());
			if (ProcMaster.SetMaintenanceMode (Pkt->GetIndex (), true))
			{
				Cl->Add (new Packet (Error::Index, Error::MaintenanceDenied));
				Add (Owner::Client | Owner::Send, Cl->ShowSocket ());
			}
			else
			{
				// informamos a todos del cambio
				Server.SendToAllClients (Pkt->GetIndex (),
							 Session::MaintenanceOn);
			}
		}
		else
		{
			// enviamos error
			Cl->Add (new Packet (Error::Index, Error::MaintenanceDenied));
			Add (Owner::Client | Owner::Send, Cl->ShowSocket ());
		}
		break;

	case Session::MaintenanceOff:

		// salir del modo de mantenimiento del proceso indicado
		// en DATA, sólo si es administrador
		if (Cl->IsAuthorized (0))
		{
			Logs.Add (Log::Notice,
				  "User \"%s\" is changing maintenance mode of process %d",
				  Cl->GetUsername (), Pkt->GetIndex ());

			if (ProcMaster.SetMaintenanceMode (Pkt->GetIndex (), false))
			{
				Cl->Add (new Packet (Error::Index, Error::MaintenanceDenied));
				Add (Owner::Client | Owner::Send, Cl->ShowSocket ());
			}
			else
			{
				// informamos a todos del cambio
				Server.SendToAllClients (Pkt->GetIndex (),
							 Session::MaintenanceOff);
			}
		}
		else
		{
			// enviamos error
			Cl->Add (new Packet (Error::Index, Error::MaintenanceDenied));
			Add (Owner::Client | Owner::Send, Cl->ShowSocket ());
		}
		break;

	default:
		return -1;
	}

	return 0;
}
#endif
bool Main::IsDebugging (void)
{
	return debug;
}

void Main::PrintUsage (void)
{
	fprintf (stderr, _("Usage: %s [-D] [<config-file>]\n"), PACKAGE);
	fprintf (stderr, _("       -D: debug mode\n"));
	exit (EXIT_FAILURE);
}

// definimos el objeto
Main Application;
