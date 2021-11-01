/***************************************************************************
 *  Copyright (C) 2017, Lawrence Livermore National Security, LLC.
 *  Produced at the Lawrence Livermore National Laboratory (cf, DISCLAIMER).
 *  Written by Olaf Faaland <faaland1@llnl.gov>
 *  UCRL-CODE-235649. All rights reserved.
 *
 *  This file is part of libyogrt.
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library.  If not, see
 *  <http://www.gnu.org/licenses/>.
 ***************************************************************************/


/*
 * Based on src/cmd/flux-kvs.c
 */

#include <stdio.h>
#include <jansson.h>
#include <time.h>

#include <flux/core.h>

#include "internal_yogrt.h"

int verbosity = 0;
int jobid_valid = 0;
LS_LONG_INT jobid;

int internal_init(int verb)
{
	verbosity = verb;

        /* Don't know how to determine if job is running under flux.
           Just assume always valid. */
        return 1;
}

char *internal_backend_name(void)
{
	return "FLUX";
}

int internal_get_rem_time(time_t now, time_t last_update, int cached)
{
        int secs_left = 0;

	/* only do this lookup with a valid jobid */
	if (! jobid_valid) {
		error("FLUX: No valid jobid to lookup!\n");
		return -1;
	}

	debug2("FLUX reports remaining time of %d sec.\n", secs_left);
	return secs_left;
}

int internal_get_rank(void)
{
	return 0;
}

int internal_fudge(void)
{
	return 0;
}
