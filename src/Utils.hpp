//
// SAGS - Secure Administrator of Game Servers
// Copyright (C) 2004 Pablo Carmona Amigo
// 
// This file is part of SAGS Server.
//
// SAGS Server is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// SAGS Server is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Library General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//
// $Source: /home/pablo/Desarrollo/sags-cvs/clienttext/src/Utils.hpp,v $
// $Revision: 1.1 $
// $Date: 2005/03/21 15:33:27 $
//

#ifndef __UTILS_HPP__
#define __UTILS_HPP__

class Utilities
{
public:
	char **StrSplit (const char *str, char delim, int elem, int *count);
	char *SubString (const char *src, int from, int to);
	char *MD5PasswordHash (const char *password);
	char *EncodePassword (const char *password);
	void RandomString (char *str, int size);
};

extern Utilities Utils;

#endif // __UTILS_HPP__