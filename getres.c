#include <stdio.h>

#include <flux/core.h>

struct lookup_ctx {
	char *resource;
};

void lookup_continuation (flux_future_t *f, void *arg)
{
    struct lookup_ctx *ctx = arg;
    const char *key = flux_kvs_lookup_get_key (f);
    const char *value;

    if (flux_kvs_lookup_get (f, &value) < 0) {
        perror("flux_kvs_lookup_get failed");
	return;
    }

    ctx->resource = strdup(value);

    flux_future_destroy (f);
}

char * fetch_resource_string()
{
	flux_t *h;
	flux_future_t *f;
	flux_reactor_t *r;
	char *ns = NULL;
	struct lookup_ctx ctx = {0};
	const char *key = "resource.R";

	if (!(h = flux_open(NULL, 0))) {
		perror("flux_open failed");
		exit(1);
	}

	if (!(f = flux_kvs_lookup(h, ns, 0, key))) {
		perror("flux_kvs_lookup failed");
		exit(2);
	}

	if (flux_future_then (f, -1., lookup_continuation, &ctx) < 0) {
		perror("flux_future_then failed");
		exit(3);
	}

	if (!(r = flux_get_reactor(h))) {
		perror ("flux_get_reactor failed");
		exit(4);
	}

	if (flux_reactor_run(r, 0) < 0) {
		perror ("flux_reactor_run failed");
		exit(5);
	}

	flux_close(h);

	return ctx.resource;

}

int main(int argc, char **argv)
{
	char *res;

	res = fetch_resource_string();
	printf("resource is %s\n", res);
	free(res);

	return(0);
}
