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

#include "internal_yogrt.h"

#define BOGUS_TIME -1

int jobid_valid = 0;
int verbosity = 0;

static flux_jobid_t jobid = 0;

int internal_init(int verb)
{
    char *jobid_str;

	verbosity = verb;
    jobid_valid = 0;

    if ((jobid_str = getenv("FLUX_JOB_ID")) == NULL) {
        error("ERROR: FLUX_JOB_ID is not set."
              " Remaining time will be a bogus value.\n");
        return jobid_valid;
    }

	if (flux_job_id_parse(jobid_str, &jobid) < 0) {
        error("ERROR: Unable to parse FLUX_JOB_ID %s."
              " Remaining time will be a bogus value.\n", jobid_str);
        return jobid_valid;
    }

    jobid_valid = 1;

    return jobid_valid;
}

char *internal_backend_name(void)
{
    return "FLUX";
}

static int get_job_expiration(flux_jobid_t id, long int *expiration)
{
    flux_t *h = NULL;
    flux_t *child_handle = NULL;
    flux_future_t *f;
    json_t *job;
    json_t *value;
    json_t *ovalue;
    double exp;
    const char *uri = NULL;
    int rc = -1;
    int thisrc;
    const flux_msg_t *msg;
    long long int job_jobid;

    if (!(h = flux_open(NULL, 0))) {
        error("ERROR: flux_open() failed\n");
        goto out;
    }

    /*
     * Determine whether to ask our parent or not
     * See https://github.com/flux-framework/flux-core/issues/3817
     */

	if (!getenv("FLUX_KVS_NAMESPACE")) {
        uri = flux_attr_get(h, "parent-uri");
        if (!uri) {
		    error("ERROR: no FLUX_KVS_NAMESPACE and flux_attr_get failed\n");
            goto out;
        }

        child_handle = h;
        h = flux_open(uri, 0);
        if (!h) {
		    printf("flux_open with parent-uri %s failed\n", uri);
            goto out;
        }
    }

    if (!(f = flux_job_list_id(h, jobid, "[\"expiration\"]"))) {
        error("ERROR: flux_job_list failed.\n");
        goto out;
    }

    if (flux_rpc_get_unpack(f, "{s:o}", "job", &job) < 0) {
        error("ERROR: flux_rpc_get_unpack failed with errno %d.\n", errno);
        goto out;
    }

    if (!(ovalue = json_object_get(job, "expiration"))) {
        error("ERROR: flux_object_get for id failed.\n");
        goto out;
    }

    if ((exp = json_real_value(ovalue)) == 0.0) {
        error("ERROR: json_real_value failed.\n");
        goto out;
    }

    *expiration = (long int) exp;
    rc = 0;

out:

    if (f)
        flux_future_destroy(f);
    if (h)
        flux_close(h);
    if (child_handle)
        flux_close(child_handle);

    return rc;
}

int internal_get_rem_time(time_t now, time_t last_update, int cached)
{
    long int expiration;
    int remaining_sec = BOGUS_TIME;

    /* only do this lookup with a valid jobid */
    if (! jobid_valid) {
        error("FLUX: No valid jobid to lookup!\n");
        return BOGUS_TIME;
    }

	if (get_job_expiration(jobid, &expiration)) {
        error("get_job_expiration failed\n");
        goto out;
    }

    remaining_sec = (int) (expiration - time(NULL));
    debug("flux remaining seconds is %ld\n", remaining_sec);

out:
    return remaining_sec;
}

int internal_get_rank(void)
{
    char *rank_str;

    if ((rank_str = getenv("FLUX_TASK_RANK")) == NULL) {
        error("ERROR: FLUX_TASK_RANK is not set.\n"
              " All ranks will maintain remaining time.\n");
        return 0;
    }

    return atoi(rank_str);
}

int internal_fudge(void)
{
    return 0;
}

/*
 * vim: tabstop=4 shiftwidth=4 expandtab smartindent:
 */
