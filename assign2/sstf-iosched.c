/*
 * elevator sstf
 */
#include <linux/blkdev.h>
#include <linux/elevator.h>
#include <linux/bio.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/init.h>

#define FORWARD 1
#define BACKWARD 0

struct sstf_data {
	struct list_head queue;
	//SSTF Additions. Scan direction and head location.
	sector_t head_loc;
	int dir;
};

static void sstf_merged_requests(struct request_queue *q, struct request *rq,
				 struct request *next)
{
	list_del_init(&next->queuelist);
}

static int sstf_dispatch(struct request_queue *q, int force)
{
	struct sstf_data *nd = q->elevator->elevator_data;
	//printk("SSTF Begin dispatch");
	if (!list_empty(&nd->queue)) {
		struct request *rq, *next_rq, *prev_rq;

		next_rq = list_entry(nd->queue.next, struct request, queuelist);
		prev_rq = list_entry(nd->queue.next, struct request, queuelist);
		//Check list to see if theres more than one req
		if (next_rq == prev_rq) {
			//One list entry case
			rq = next_rq;
		} else {
			//Multiple entires case. Seeking must be handled.
			if (nd->dir == FORWARD){
				//Forward Search
				if(nd->head_loc < blk_rq_pos(next_rq)) {
					rq = next_rq;
				}else {
					nd->dir = BACKWARD;
					rq = prev_rq;
				}
			}else{
				//Backward Search
				if(nd->head_loc > blk_rq_pos(prev_rq)) {
					rq = prev_rq;
				} else {
					nd->dir = FORWARD;
					rq = next_rq;
				}
			}
		}

		//rq = list_entry(nd->queue.next, struct request, queuelist);
		list_del_init(&rq->queuelist);
		//Update read head sector posistion. Current sector + bytes remaining in the request.
		nd->head_loc = blk_rq_pos(rq) + blk_rq_sectors(rq);
		elv_dispatch_add_tail(q, rq);
		printk("SSTF DISPATCHING %llu\n", nd->head_loc);

		//elv_dispatch_sort(q, rq);
		return 1;
	}
	return 0;
}

static void sstf_add_request(struct request_queue *q, struct request *rq)
{
	struct sstf_data *nd = q->elevator->elevator_data;
	//Maintains sorted order by adding to the tail.
	//printk("SSTF ADDING REQ %llu\n", (unsigned long long) blk_rq_pos(rq));
	list_add_tail(&rq->queuelist, &nd->queue);
}

static struct request *
sstf_former_request(struct request_queue *q, struct request *rq)
{
	struct sstf_data *nd = q->elevator->elevator_data;

	if (rq->queuelist.prev == &nd->queue)
		return NULL;
	return list_entry(rq->queuelist.prev, struct request, queuelist);
}

static struct request *
sstf_latter_request(struct request_queue *q, struct request *rq)
{
	struct sstf_data *nd = q->elevator->elevator_data;

	if (rq->queuelist.next == &nd->queue)
		return NULL;
	return list_entry(rq->queuelist.next, struct request, queuelist);
}

static int sstf_init_queue(struct request_queue *q, struct elevator_type *e)
{
	struct sstf_data *nd;
	struct elevator_queue *eq;

	eq = elevator_alloc(q, e);
	if (!eq)
		return -ENOMEM;

	nd = kmalloc_node(sizeof(*nd), GFP_KERNEL, q->node);
	if (!nd) {
		kobject_put(&eq->kobj);
		return -ENOMEM;
	}
	nd->dir = 1;
	nd->head_loc = 0;
	eq->elevator_data = nd;

	INIT_LIST_HEAD(&nd->queue);

	spin_lock_irq(q->queue_lock);
	q->elevator = eq;
	spin_unlock_irq(q->queue_lock);
	return 0;
}

static void sstf_exit_queue(struct elevator_queue *e)
{
	struct sstf_data *nd = e->elevator_data;

	BUG_ON(!list_empty(&nd->queue));
	kfree(nd);
}

static struct elevator_type elevator_sstf = {
	.ops = {
		.elevator_merge_req_fn		= sstf_merged_requests,
		.elevator_dispatch_fn		= sstf_dispatch,
		.elevator_add_req_fn		= sstf_add_request,
		.elevator_former_req_fn		= sstf_former_request,
		.elevator_latter_req_fn		= sstf_latter_request,
		.elevator_init_fn		= sstf_init_queue,
		.elevator_exit_fn		= sstf_exit_queue,
	},
	.elevator_name = "sstf",
	.elevator_owner = THIS_MODULE,
};

static int __init sstf_init(void)
{
	return elv_register(&elevator_sstf);
}

static void __exit sstf_exit(void)
{
	elv_unregister(&elevator_sstf);
}

module_init(sstf_init);
module_exit(sstf_exit);


MODULE_AUTHOR("George Crary");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("SSTF IO scheduler");
