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
// $Source: /home/pablo/Desarrollo/sags-cvs/clienttext/src/Network.cpp,v $
// $Revision: 1.1 $
// $Date: 2005/03/21 15:33:27 $
//

#include <cstdio>
#include <cstring>
#include <cmath>
#include <openssl/md5.h>

#include "Config.hpp"
#include "Network.hpp"
#include "Utils.hpp"

Network::Network (const char *address, const char *port,
		  const char *username, const char *password)
{
	n_address = new char [strlen (address + 1)];
	strncpy (n_address, address, strlen (address));

	n_port = new char [strlen (port + 1)];
	strncpy (n_port, port, strlen (port));

	n_username = new char [strlen (username + 1)];
	strncpy (n_username, username, strlen (username));

	n_password = new char [strlen (password + 1)];
	strncpy (n_password, password, strlen (password));

	n_connected = false;
	n_exiting = false;
}

Network::~Network ()
{
	delete[] n_address;
	delete[] n_port;
	delete[] n_username;
	delete[] n_password;
}

void Network::AddBuffer (List<Packet> &PktList, unsigned int idx,
			 unsigned int com, const char *data)
{
	const char *p = data;
	int s;

	// data NO DEBE ser nulo!!!

	// calculamos cuantos paquetes necesitaremos
	// que corresponde a la parte entera más uno de
	// TamañoTotal / PCKT_MAXDATA
	s = (int) trunc (strlen (data) / PCKT_MAXDATA) + 1;

	while (strlen (p) >= PCKT_MAXDATA)
	{
		PktList << new Packet (idx, com, s--, strlen (p), p);
		p += PCKT_MAXDATA;
	}

	if (strlen (p) > 0 && strlen (p) < PCKT_MAXDATA)
		PktList << new Packet (idx, com, s--, strlen (p), p);
}

void Network::AddBufferOut (unsigned int idx, unsigned int com, const char *data)
{
	AddBuffer (n_outgoing, idx, com, data);
}

void Network::AddOut (unsigned int idx, unsigned int com)
{
	n_outgoing << new Packet (idx, com);
}

int Network::Connect (void)
{
	int retval = Protocol::Connect (n_address, n_port);

	if (retval)
		n_connected = false;
	else
		n_connected = true;

	return retval;
}

int Network::Disconnect (bool exiting)
{
	n_exiting = (exiting) ? true : false;

	// enviar un paquete de desconexión
	// FIXME: esto bloquea la UI!
	n_outgoing << new Packet (Session::MainIndex, Session::Disconnect);
	Send ();
	//if (SendPacket (new Packet (Session::MainIndex, Session::Disconnect)) < 0)
	//	return -1;

	return 0;
}

void Network::Drop (bool shutdown_ssl)
{
	// cerramos la conexión SSL
	Protocol::Disconnect (shutdown_ssl);
	n_connected = false;
}

int Network::Send (void)
{
	Packet *Sending = NULL;
	int bytes = 0, total = 0;

	while (n_outgoing.GetCount ())
	{
		Sending = n_outgoing.ExtractFirst ();
		bytes = SendPacket (Sending);
		delete Sending;

		if (bytes < 0)
		{
			// paquete no se envió
			perror ("SendPacket");
			return -1; 
		}

		total += bytes;
	}

	return bytes;
}

int Network::Receive (void)
{
	Packet *Pkt = RecvPacket ();

	if (Pkt == NULL)
		return -1;

	n_incoming << Pkt;

	if (Pkt->GetIndex () == Error::Index)
	{
		switch (Pkt->GetCommand ())
		{
			case Error::ServerFull:
				return 2;
			case Error::NotValidVersion:
			case Error::LoginFailed:
			case Error::AuthTimeout:
			case Error::ServerQuit:
			case Error::LoggedFromOtherPlace:
				return 1;
		}
	}
	else if (Pkt->GetIndex () >= Session::MainIndex &&
		 Pkt->GetIndex () <= Session::MaxIndex)
	{
		switch (Pkt->GetCommand ())
		{
			case Session::Disconnect:
				return 1;
		}
	}

	return 0;
}

bool Network::IsConnected (void)
{
	return n_connected;
}

bool Network::IsExiting (void)
{
	return n_exiting;
}

const char *Network::GetAddress (void)
{
	return n_address;
}

const char *Network::GetPort (void)
{
	return n_port;
}

const char *Network::GetUsername (void)
{
	return n_username;
}

const char *Network::GetPassword (void)
{
	return n_password;
}

Packet *Network::Get (void)
{
	Packet *Item = n_incoming.ExtractFirst ();

	if (Item == NULL)
	{
		printf ("Incoming NULL\n");
		return NULL;
	}

	return Item;
}

#if 0
void *Network::Entry (void)
{
	int val, idx;
	bool send_now = FALSE;
	wxString hello_msg, pwdhash;
	Packet *Pkt = NULL;

	if (!Connected)
		Connect ();

	if (!Connected)
	{
		// hay que enviar un evento de NetEvt::FailConnect
		wxSocketEvent ConnectFailed (NetEvt::FailConnect);
		EvtParent->AddPendingEvent ((wxEvent&) ConnectFailed);
		return (ExitCode) -1;
	}

	// enviar un evento de NetEvt::Connect
	wxSocketEvent ConnectSuccessful (NetEvt::Connect);
	EvtParent->AddPendingEvent ((wxEvent&) ConnectSuccessful);

	// La autenticación comieza enviando un Sync::Hello
	hello_msg.Printf ("SAGS Client %s", VERSION);
	Lock (OutgoingMutex);
	AddBuffer (Outgoing, Sync::Index, Sync::Hello, hello_msg.c_str ());
	Unlock (OutgoingMutex);
	Send ();

	// Si se leen datos se envía el evento NetEvt::Read
	// Si falla algo al leer, enviamos NetEvt::FailRead y salimos
	while (1)
	{
		// nos bloqueamos esperando un paquete
		val = Receive ();
		if (val < 0)
		{
			// error al leer, cerrar la conexión
			Drop ();

			// si estamos saliendo... salimos :)
			if (Exiting)
				break;

			// enviar evento
			wxSocketEvent ReadFailed (NetEvt::FailRead);
			EvtParent->AddPendingEvent ((wxEvent&) ReadFailed);
			break;
		}
		else
		{
			// manejamos la autenticación
			Lock (OutgoingMutex);
			Lock (IncomingMutex);

			idx = Incoming.GetCount () - 1;
			if (idx < 0)
				continue;
			Pkt = Incoming[idx];

			if (Pkt->IsSync ())
			{
				switch (Pkt->GetCommand ())
				{
				case Sync::Hello:
					Outgoing << new Packet (Sync::Index, Sync::Version,
								1, 1, PCKT_VERSION);
					send_now = TRUE;
					break;

				case Sync::Version:
					AddBuffer (Outgoing, Auth::Index, Auth::Username,
						   Username.c_str ());
					send_now = TRUE;
					break;
				}
			}
			else if (Pkt->IsAuth ())
			{
				switch (Pkt->GetCommand ())
				{
				case Auth::RandomHash:
					pwdhash = Pkt->GetData ();
					pwdhash += Password;
					AddBuffer (Outgoing, Auth::Index, Auth::Password,
						   Utils.MD5PasswordHash (pwdhash));
					send_now = TRUE;
					break;

					// ahora hay que esperar que lleguen
					// los paquetes Session::Authorized
				}
			}
			else if (Pkt->IsSession ())
			{
				switch (Pkt->GetCommand ())
				{
				case Session::Authorized:

					ProcsReceived << Pkt->GetIndex ();
					break;

				case Session::ProcessStart:
				case Session::MaintenanceOn:
				case Session::MaintenanceOff:

					Outgoing << new Packet (Pkt->GetIndex (),
								Session::ProcessGetInfo);
					send_now = TRUE;

					break;
				}
			}
			Unlock (OutgoingMutex);
			Unlock (IncomingMutex);

			if (send_now)
			{
				Send ();
				send_now = FALSE;
			}

			// recibimos un paquete, enviar evento
			// la aplicación debiera leerlo con Net->Get ()
			wxSocketEvent ReadSuccessful (NetEvt::Read);
			EvtParent->AddPendingEvent ((wxEvent&) ReadSuccessful);

			// si es mayor que 1 entonces recibimos SessionDisconnect
			// o algún error por lo que debemos salir
			if (val > 0)
			{
				if (val == 2)
					Drop (FALSE);
				else
					Drop ();
				break;
			}
		}
	}

	return (ExitCode) 0;
}
#endif
