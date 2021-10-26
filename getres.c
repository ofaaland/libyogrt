
/*
 * Based on src/cmd/flux-kvs.c
 */

#include <stdio.h>
#include <jansson.h>
#include <time.h>

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

long int extract_expiration(char *resource)
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
        printf("failed to load json string\n");
        return -1.;
    }

    execution = json_object_get(root, "execution");
    if (execution == NULL) {
        printf("failed to get object execution\n");
        return -1.;
    }

    expirjson = json_object_get(execution, "expiration");
    if (expirjson == NULL) {
        printf("failed to get object expirjson\n");
        return -1.;
    }

    expiration = json_number_value(expirjson);

    return (long int) expiration;
}

int main(int argc, char **argv)
{
    char *res;
    long int expiration;
    long int remaining_sec;

    res = fetch_resource_string();
    printf("resource is %s\n", res);

    expiration = extract_expiration(res);
    printf("expiration is %ld\n", expiration);

    remaining_sec = expiration - time(NULL);
    printf("remaining seconds is %ld\n", remaining_sec);
    printf("remaining minutes is %ld\n", remaining_sec / 60);
    printf("remaining hours   is %ld\n", remaining_sec / (60*60) );
    printf("remaining days    is %ld\n", remaining_sec / (24*60*60) );

    free(res);

    return(0);
}

/*
 * vi:tabstop=4 shiftwidth=4 expandtab
 */
