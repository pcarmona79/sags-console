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
// $Source: /home/pablo/Desarrollo/sags-cvs/clienttext/src/Opt.hpp,v $
// $Revision: 1.1 $
// $Date: 2005/03/21 15:33:27 $
//

#ifndef __OPT_HPP__
#define __OPT_HPP__

enum { OPT_NULL = 0, OPT_BOOLEAN, OPT_STRING };

struct opt
{
	const char *lname;
	char sname;
	int type;
	void *var;
	int size;
};

class Options
{
private:
	struct opt *options;
	struct opt *last_option_added;

	char *SubStr (const char *src, int from, int to);
	int GetCharPosition (const char *str, char c);
#ifdef DEBUG
	void ListOptions (void);
#endif
	void InitOptions (struct opt *opts);
	int SetLongOption (const char *name, const char *value);
	int SetShortOption (char n, const char *value);
	int CheckTextArgument (const char *text);
	void SetInvalidOptionPosition (int *idx, int pos);

public:
	Options ();
	~Options ();

	void GetInvalidOptionPosition (int *idx, int *pos);
	int Parse (int argc, char** argv, struct opt *opts);
};

#endif // __OPT_HPP__
