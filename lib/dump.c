#include <stdio.h>
#include <err.h>
#include <netlink/netlink.h>
#include <netlink/list.h>
#include <netlink/genl/family.h>
#include <netlink/genl/genl.h>

#define ERRX(msg) errx(1, msg)

int main(int argc, char *argv[])
{
	struct nl_sock *sock;
	struct nl_cache *cache;
	struct nl_dump_params params = {
		.dp_type = NL_DUMP_LINE,
		.dp_fd = stdout,
	};

	if ((sock = nl_socket_alloc()) == 0)
		ERRX("Failed nl_socket_alloc");
	if (genl_connect(sock))
		ERRX("Failed genl_connect");
	
	if (genl_ctrl_alloc_cache(sock, &cache))
		ERRX("Failed genl_ctrl_alloc_cache");

	nl_cache_dump(cache, &params);


	return 0;
}
