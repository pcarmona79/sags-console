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
// $Source: /home/pablo/Desarrollo/sags-cvs/clienttext/src/Main.hpp,v $
// $Revision: 1.1 $
// $Date: 2005/03/21 15:33:27 $
//

#ifndef __MAIN_HPP__
#define __MAIN_HPP__

#include "Loop.hpp"
#include "Packet.hpp"
#include "Config.hpp"
#include "List.hpp"
#include "Utils.hpp"

class Main : public SelectLoop
{
private:
	bool debug;

public:
	Main ();
	~Main ();

	void Init (bool debugmode = false);
	void AddOptions (void);
	int SignalEvent (int sig = 0);
	void DataEvent (int owner, int fd, bool writing);
	void TimeoutEvent (void);
#if 0
	int GenerateResponse (Client *Cl, Packet *Pkt);
	int ProtoSync (Client *Cl, Packet *Pkt);
	int ProtoAuth (Client *Cl, Packet *Pkt);
	int ProtoSession (Client *Cl, Packet *Pkt);
#endif
	bool IsDebugging (void);
	void PrintUsage (void);
};

extern Main Application;

#endif // __MAIN_HPP__
