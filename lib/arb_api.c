#include <stdio.h>
#include <err.h>
#include <netlink/netlink.h>
#include <netlink/list.h>
#include <netlink/genl/family.h>
#include <netlink/genl/genl.h>
#include <netlink/genl/ctrl.h>
#include <netlink/data.h>
#include <sys/socket.h>
#include <errno.h>
#include <fcntl.h>
#include "arb_api.h"

#define MAX_HOSTNAME 254

struct {
	struct nl_sock *sock;
	struct nl_cache *cache;
} link;

char sendbuf[64];


int init_link()
{
	if ((link.sock = nl_socket_alloc()) == 0) {
		warnx("Failed nl_socket_alloc");
		return -1;
	}
	if (genl_connect(link.sock)) {
		warnx("Failed to connect nl socket");
		return -1;
	}
	if ((genl_ctrl_alloc_cache(link.sock, &link.cache))) {
		warnx("Failed to alloc genl cache");
		return -1;
	}
	return 0;
}

int set_service(const int fd, const char *service_name)
{
	struct genl_family *family;
	struct nl_msg *msg;
	void *hdr;

	family = genl_ctrl_search_by_name(link.cache, ARB_GENL_FAMILY_NAME);
	if (family == NULL) {
		warnx("Failed to find family %s", ARB_GENL_FAMILY_NAME);
		return -1;
	}

	if ((msg = nlmsg_alloc()) == 0) {
		warnx("Falied at nlmsg_alloc");
		return -1;
	}

	hdr = genlmsg_put(msg, NL_AUTO_PORT, NL_AUTO_SEQ,
			genl_family_get_id(family),
			0, NLM_F_REQUEST | NLM_F_ACK,
			ARB_GENL_CMD_SET_SERVICE, 1);
	if (hdr == 0) {
		warnx("Failed at genlmsg_put");
		return -1;
	}


	if (nla_put(msg, ARB_GENL_ATTR_FD, sizeof(fd), &fd)) {
		warnx("Failed to put fd");
		return -1;
	}
	if (nla_put_string(msg, ARB_GENL_ATTR_SERVICE_NAME, service_name)) {
		warnx("Failed to put service name");
		return -1;
	}

	if (nl_send_auto(link.sock, msg) < 0) {
		warnx("Failed at nl_send_auto");
		return -1;
	}

	return 0;
}

int get_sock(char *host, char *port)
{
	struct addrinfo hints, *res;
	int sock;
	int err, ret = -1;

	memset(&hints, 0, sizeof(hints));
	hints.ai_flags = AI_ADDRCONFIG | AI_NUMERICSERV;
	hints.ai_protocol = IPPROTO_TCP;
	if ((err = getaddrinfo(host, port, &hints, &res)) != 0) {
		warnx("Failed getaddrinfo: %s", gai_strerror(err));
		return -1;
	}
	struct addrinfo *r;
	for (r = res; r; r = r->ai_next) {
		sock = socket(r->ai_family, r->ai_socktype, r->ai_protocol);
		if (sock == -1) {
			warnx("Failed to open socket");
			continue;
		}
		if (connect(sock, r->ai_addr, r->ai_addrlen) < 0) {
			warnx("Failed to connect: %s", strerror(errno));
			close(sock);
			continue;
		}
		break;
	}

out:
	freeaddrinfo(res);
	return sock;

}

int main(int argc, char *argv[])
{
	int sock;
	char service[] = "Svc1";
	if (init_link())
		return -1;

	if ((sock = get_sock("fol-a-arb.haze.yandex.net", "80")) == -1) {
		warnx("Failed to open a socket");
		return -1;
	}

	if (set_service(sock, service) != 0)
		return -1;

	int i;
	for (i = 10; i > 0; i--) {
		if (write(sock, sendbuf, sizeof(sendbuf)) == -1)
			warnx("Error writing");
	}
	close(sock);


	return 0;
}
