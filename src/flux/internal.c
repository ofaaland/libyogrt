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


#include <stdio.h>
#include <jansson.h>
#include <time.h>
#include <limits.h>
#include <flux/core.h>

struct lookup_ctx {
    char *resource;
};

#include "internal_yogrt.h"

#define BOGUS_TIME -1

int verbosity = 0;
int jobid_valid = 0;

int internal_init(int verb)
{
	verbosity = verb;

        if (getenv("FLUX_JOBID") != NULL) {
		jobid_valid = 1;
        } else {
                error("ERROR: FLUX_JOBID is not set."
                      " Remaining time will be a bogus value.\n");
		jobid_valid = 0;
        }

        return jobid_valid;
}

char *internal_backend_name(void)
{
	return "FLUX";
}

void lookup_continuation (flux_future_t *f, void *arg)
{
    struct lookup_ctx *ctx = arg;
    const char *key = flux_kvs_lookup_get_key (f);
    const char *value;

    if (flux_kvs_lookup_get (f, &value) < 0) {
        error("flux_kvs_lookup_get failed");
        return;
    }

    ctx->resource = strdup(value);

    flux_future_destroy (f);
}

int fetch_resource_string(char **s)
{
    flux_t *h = NULL;
    flux_future_t *f = NULL;
    flux_reactor_t *r = NULL;
    char *ns = NULL;
    struct lookup_ctx ctx = {0};
    const char *key = "resource.R";
    int rc = 0;

    if (!(h = flux_open(NULL, 0))) {
        debug("flux_open failed");
        rc = BOGUS_TIME;
        goto out;
    }

    if (!(f = flux_kvs_lookup(h, ns, 0, key))) {
        error("flux_kvs_lookup failed");
        rc = BOGUS_TIME;
        goto out;
    }

    if (flux_future_then (f, -1., lookup_continuation, &ctx) < 0) {
        error("flux_future_then failed");
        rc = BOGUS_TIME;
        goto out;
    }

    if (!(r = flux_get_reactor(h))) {
        error ("flux_get_reactor failed");
        rc = BOGUS_TIME;
        goto out;
    }

    if (flux_reactor_run(r, 0) < 0) {
        error ("flux_reactor_run failed");
        rc = BOGUS_TIME;
        goto out;
    }

out:

    if (h)
        flux_close(h);

    *s = ctx.resource;
    return rc;
}

int extract_expiration(char *resource)
{
    json_t *root;
    json_t *execution;
    json_t *startjson;
    json_t *expirjson;
    size_t flags = 0;
    json_error_t error = {0};
    double starttime, expiration;

    root = json_loads(resource, flags, &error);
    if (root == NULL) {
        error("failed to load json string\n");
        return BOGUS_TIME;
    }

    if (json_unpack(root, "{s:{s?F}}", "execution", "expiration", &expiration) == -1) {
        error("json_unpack failed");
        return BOGUS_TIME;
    }

    return (int) expiration;
}

int internal_get_rem_time(time_t now, time_t last_update, int cached)
{
	char *res = NULL;
	long int expiration;
	int remaining_sec = BOGUS_TIME;

	/* only do this lookup with a valid jobid */
	if (! jobid_valid) {
		error("FLUX: No valid jobid to lookup!\n");
		return BOGUS_TIME;
	}

	if (fetch_resource_string(&res) != 0) {
	    error("fetch_resourcestring failed");
        goto out;
    }

	expiration = extract_expiration(res);
    if (expiration == BOGUS_TIME) {
	    error("extract_expiration failed");
        goto out;
    }

	remaining_sec = (int) (expiration - time(NULL));
	debug2("flux remaining seconds is %ld\n", remaining_sec);

out:
	if (res)
		free(res);

	return remaining_sec;
}

int internal_get_rank(void)
{
	return 0;
}

int internal_fudge(void)
{
	return 0;
}
