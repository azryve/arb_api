#include <linux/module.h>
#include <linux/types.h>
#include <linux/printk.h>
#include <linux/genetlink.h>
#include <linux/netlink.h>
#include <linux/errno.h>
#include <linux/sched.h>
#include <linux/rcupdate.h>
#include "arb_genl_api.h"
#include "arb_api.h"

static const struct nla_policy arb_genl_api_policy[];
static const struct genl_ops arb_genl_api_ops[];


static struct genl_family arb_genl_api_family __read_mostly = {
	.id = GENL_ID_GENERATE,
	.name = ARB_GENL_FAMILY_NAME,
	.version = ARB_GENL_FAMILY_VERSION,
	.module = THIS_MODULE,
	.maxattr = ARB_GENL_ATTR_MAX,
};

static const struct genl_ops arb_genl_api_ops[] = {
	{ .cmd = ARB_GENL_CMD_SET_SERVICE,
	  .doit = arb_genl_api_set_service,
	  .policy = arb_genl_api_policy,
	  .flags = 0
	},
	{ .cmd = ARB_GENL_CMD_SET_MAP,
	  .doit = arb_genl_api_set_map,
	  .policy = arb_genl_api_policy,
	  .flags = GENL_ADMIN_PERM
	}
};

static const struct nla_policy arb_genl_api_policy[] = {
	[ARB_GENL_ATTR_MARK] = { .len = sizeof(int) },
	[ARB_GENL_ATTR_FD] = { .len = sizeof(int) },
	[ARB_GENL_ATTR_SERVICE_NAME] = { .type = NLA_NUL_STRING }
};
	
static int arb_genl_api_set_service(struct sk_buff *skb, struct genl_info *info)
{
	int *fd;
	char *service_name;
	pid_t pid = info->snd_portid;
	struct task_struct *tsk;

	rcu_read_lock();
	tsk = pid_task(find_vpid(pid), PIDTYPE_PID);
	if (tsk)
		printk("Called arb_genl_api_set_service from %u, %u\n", pid, tsk->tgid);
	else
		printk("Called arb_genl_api_set_service from %u\n", pid);
	rcu_read_unlock();

	if (info->attrs[ARB_GENL_ATTR_FD] && info->attrs[ARB_GENL_ATTR_SERVICE_NAME])
	{
		fd = nla_data(info->attrs[ARB_GENL_ATTR_FD]);
		service_name = nla_data(info->attrs[ARB_GENL_ATTR_SERVICE_NAME]);
		printk("Got fd %d, to set service %s\n", *fd, service_name);
	}
	else
	{
		if (!info->attrs[ARB_GENL_ATTR_FD])
			printk("Not passed fd\n");

		if (!info->attrs[ARB_GENL_ATTR_SERVICE_NAME])
			printk("Not passed service name\n");
	}

	return 0;
}

static int arb_genl_api_set_map(struct sk_buff *skb, struct genl_info *info)
{
	printk("Called arb_genl_api_set_map\n");
	return 0;
}

static int __init arb_genl_api_init(void)
{
	int err = -EINVAL;
	err = genl_register_family_with_ops(&arb_genl_api_family, arb_genl_api_ops);
	if (err) 
	{
		printk("Failed to register genl family: errcode 0x%x\n", -err);
		goto out;
	}
	printk("Registered genl family\n");
	printk("Loaded arb_genl_api\n");

out:
	return err;
}
module_init(arb_genl_api_init);

static void __exit arb_genl_api_exit(void)
{
	int err = -EINVAL;
	if ((err = genl_unregister_family(&arb_genl_api_family)) != 0)
		printk("Failed to unregister genl family: errcode 0x%x\n", -err);
		
	printk("Unloaded arb_genl_api\n");
}
module_exit(arb_genl_api_exit);

MODULE_DESCRIPTION("Generic netlink api for arbiter");
MODULE_AUTHOR("Fedor Zhukov <azryve@yandex-team.ru>");
MODULE_LICENSE("GPL");
