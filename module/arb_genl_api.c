#include <linux/module.h>
#include <linux/types.h>
#include <linux/printk.h>
#include <linux/genetlink.h>
#include <linux/netlink.h>
#include <linux/errno.h>
#include <linux/sched.h>
#include <linux/rcupdate.h>
#include <linux/fdtable.h>
#include <linux/net.h>
#include <net/sock.h>
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


static int sockfd_set_mark(const int fd, const pid_t pid, const u32 mark)
{
	struct file *file = NULL;
	struct task_struct *task = NULL;
	struct socket *socket = NULL;
	int err = 0;

	task = get_pid_task(find_vpid(pid), PIDTYPE_PID);
	if (!task) {
		err = -EINVAL;
		goto out;
	}

	task_lock(task);
	if (!task->files) {
		err = -EINVAL;
		goto task_out;
	}

	rcu_read_lock(); // for fcheck_files
	file = fcheck_files(task->files, fd);
	if (file) {
		spin_lock(&file->f_lock);
		socket = sock_from_file(file, &err); // -ENOTSOCK if not socket
		if (socket)
			socket->sk->sk_mark = mark;
		spin_unlock(&file->f_lock);
	}
	else
		err = -ENOENT; // fd not found
	rcu_read_unlock();
task_out:
	task_unlock(task);
	put_task_struct(task);
out:
	return err;
}

static int arb_genl_api_set_service(struct sk_buff *skb, struct genl_info *info)
{
	int *fd;
	char *service_name;
	pid_t pid = info->snd_portid;
	u32 mark = 555;
	int err;

	printk("Entered arb_genl_api_set_service\n");

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

		return -1;
	}
	if ((err = sockfd_set_mark(*fd, pid, mark)) != 0) {
		switch (err) {
		case -EINVAL:
			printk("Not found pid %u\n", pid);
			break;
		case -ENOENT:
			printk("Not found fd %d\n", *fd);
			break;
		case -ENOTSOCK:
			printk("Not socket fd passed %d\n", *fd);
			break;
		default:
			printk("Unknown error from sockfd_set_mark\n");
		}
	}
	else
		printk("Set pid's: %u fd: %d, mark %u\n", pid, *fd, mark);


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
