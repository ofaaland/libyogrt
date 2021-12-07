Notes on how best to integrate with flux.

branch b-support-flux-5:
	Uses KVS store to get resource.R and extract expiration from that.
	This turns out not to work for one of "flux mini {alloc,run,submit} fubar"
	although I don't remember which.  The problem is that for one of them,
	the KVS tree does not include resource.R, or the user doesn't have perms
	to get it.  I don't understand why.

	Too bad, because otherwise it would have been ready for review.

	Grondo says this is a corner case, and the usual stuff developers need,
	like querying resources, is done another way that is not affected by this.

branch b-support-flux-6:

My notes from slack are:
	Me> I need to re-read the conversation from my first query on this topic.  I see discussion of flux_job_list_id and talking to the parent...
	Grondo> Yes, that can get confusing. Also keep in mind that  processes under flux mini run will have FLUX_KVS_NAMESPACE set and will thus will by default use the job KVS namespace and not the "root" namespace
