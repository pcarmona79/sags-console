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
// $Source: /home/pablo/Desarrollo/sags-cvs/clienttext/src/Network.hpp,v $
// $Revision: 1.1 $
// $Date: 2005/03/21 15:33:27 $
//

#ifndef __NETWORK_HPP__
#define __NETWORK_HPP__

#include "Protocol.hpp"
#include "Packet.hpp"
#include "List.hpp"

class Network : private Protocol
{
private:
	// lista de paquetes
	List<Packet> n_outgoing;
	List<Packet> n_incoming;

	// datos
	char *n_address;
	char *n_port;
	char *n_username;
	char *n_password;

	// estado
	bool n_connected;
	bool n_exiting;

	// lista de índices de procesos
	List<unsigned int> n_procs_received;

public:
	Network (const char *address,
		 const char *port,
		 const char *username,
		 const char *password);
	~Network ();

	void AddBuffer (List<Packet> &PktList, unsigned int idx,
			unsigned int com, const char *data);
	void AddBufferOut (unsigned int idx, unsigned int com, const char *data);
	void AddOut (unsigned int idx, unsigned int com);

	int Connect (void);
	int Disconnect (bool exiting = false);
	void Drop (bool shutdown_ssl = true);

	int Send (void);
	int Receive (void);

	bool IsConnected (void);
	bool IsExiting (void);
	const char *GetAddress (void);
	const char *GetPort (void);
	const char *GetUsername (void);
	const char *GetPassword (void);
	Packet *Get (void);
};

#endif // __NETWORK_HPP__
