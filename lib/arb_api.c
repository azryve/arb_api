#include <stdio.h>
#include <err.h>
#include <netlink/netlink.h>
#include <netlink/list.h>
#include <netlink/genl/family.h>
#include <netlink/genl/genl.h>
#include <netlink/genl/ctrl.h>
#include <netlink/data.h>
#include <sys/socket.h>
#include <fcntl.h>
#include "arb_api.h"

struct {
	struct nl_sock *sock;
	struct nl_cache *cache;
} link;


void init_link()
{
	if ((link.sock = nl_socket_alloc()) == 0)
		errx(1, "Failed nl_socket_alloc");
	if (genl_connect(link.sock))
		errx(1, "Failed to connect nl socket");
	if ((genl_ctrl_alloc_cache(link.sock, &link.cache)))
		errx(1, "Failed to alloc genl cache");
}

int main(int argc, char *argv[])
{
	struct genl_family *family;
	struct nl_msg *msg;
	void *hdr;
	int fd = socket(AF_INET, SOCK_STREAM, 0);
	int mark = 999;
	char service[] = "Svc1";

	init_link();
	family = genl_ctrl_search_by_name(link.cache, ARB_GENL_FAMILY_NAME);
	if (family == NULL)
		errx(1, "Failed to find family %s", ARB_GENL_FAMILY_NAME);

	if ((msg = nlmsg_alloc()) == 0)
		errx(1, "Falied at nlmsg_alloc");

	hdr = genlmsg_put(msg, NL_AUTO_PORT, NL_AUTO_SEQ,
			genl_family_get_id(family),
			0, NLM_F_REQUEST | NLM_F_ACK,
			ARB_GENL_CMD_SET_SERVICE, 1);
	if (hdr == 0)
		errx(1, "Failed at genlmsg_put");


	if (nla_put(msg, ARB_GENL_ATTR_FD, sizeof(fd), &fd))
		errx(1, "Failed to put fd");
	if (nla_put(msg, ARB_GENL_ATTR_MARK, sizeof(mark), &mark))
		errx(1, "Failed to put mark");
	if (nla_put_string(msg, ARB_GENL_ATTR_SERVICE_NAME, service))
		errx(1, "Failed to put service name");

	if (nl_send_auto(link.sock, msg) < 0)
		errx(1, "Failed at nl_send_auto");

	sleep(2);
	
	return 0;
}
