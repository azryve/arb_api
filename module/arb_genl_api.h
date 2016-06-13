#ifndef __ARB_GENL_API_H__
#define __ARB_GENL_API_H__


#include <net/netlink.h>
#include <net/genetlink.h>


static int arb_genl_api_set_service(struct sk_buff *skb, struct genl_info *info);
static int arb_genl_api_set_map(struct sk_buff *skb, struct genl_info *info);

#endif  // __ARB_GENL_API_H__
