#include "lib/dma_pool.h"

altix_dma_pool* altix_dma_pool_init(struct pci_dev* dev)
{
	uint i;
	altix_dma_pool* pool = kmalloc(sizeof(altix_dma_pool), GFP_KERNEL);
	altix_dma_unit * node = kmalloc(sizeof(altix_dma_unit), GFP_KERNEL);
	pool->size = ALTIX_DMA_POOL_INITIAL_ALLOC;
	pool->root = node;
	for(i = 0; i < ALTIX_DMA_POOL_INITIAL_ALLOC - 1; i++)
	{
		node->next = kmalloc(sizeof(altix_dma_unit), GFP_KERNEL);
		node = node->next;
	}
	node->next = NULL;
	node = pool->root;
	for(i = 0; i < ALTIX_DMA_POOL_INITIAL_ALLOC; i++)
	{
		node->dma_vbase = pci_alloc_consistent(dev, ALTIX_DMA_UNIT_SIZE, &(node->dma_handle));
		memset(node->dma_vbase, 0x00, ALTIX_DMA_UNIT_SIZE);
		if(node->next != NULL)
			node = node->next;
		else
			break;
	}
	node = pool->root;
	for(i = 0; i< ALTIX_DMA_POOL_INITIAL_ALLOC; i++)
	{
		node->dma_vbase[0] = node->dma_handle + ALTIX_DMA_UNIT_OFFSET;
		node->dma_vbase[1] = ALTIX_DMA_UNIT_USEFULL_SIZE;
		if(node->next != NULL)
		{
			node->dma_vbase[2] = node->next->dma_handle;
			node = node->next;
		}
		else
		{
			node->dma_vbase[2] = 0xFFFFFFFC;
			break;
		}
	}
	printk(KERN_INFO "Allocated dma pool of size %d\n", i);
	return pool;
}

void altix_dma_pool_free(altix_dma_pool* pool, struct pci_dev* dev)
{
	uint i;
	altix_dma_unit * node;
	altix_dma_unit * next_node;
	node = pool->root;
	for(i = 0; i< pool->size; i++)
	{
		pci_free_consistent(dev, ALTIX_DMA_UNIT_SIZE, node->dma_vbase, node->dma_handle);
		next_node = node->next;
		kfree(node);
		node = next_node;
	}
	kfree(pool);
	return;
}

dma_addr_t altix_dma_pool_alloc(struct pci_dev* dev, int size, altix_dma_pool* pool)
{
	uint i;
	int slabs;
	int new_slabs;
	altix_dma_unit * node;
	altix_dma_unit * last_node;
	if(size == 0)
		return pool->root->dma_handle;
	slabs = ((size -1)/ALTIX_DMA_UNIT_USEFULL_SIZE) +1;
	if(pool->size >= slabs)
		return pool->root->dma_handle;
	new_slabs = slabs - pool->size;
	printk(KERN_INFO "Adding %d pages to the dma pool\n", new_slabs);
	node = pool->root;
	for(i = 0; i< pool->size; i++)
	{
		if(node->next == NULL) break;
		node = node->next;
	}
	last_node = node;
	for(i = 0; i < new_slabs; i++)
	{
		node->next = kmalloc(sizeof(altix_dma_unit), GFP_KERNEL);
		node = node->next;
	}
	node->next = NULL; //

	node = last_node->next;
	for(i = 0; i < new_slabs; i++)
	{
		node->dma_vbase = pci_alloc_consistent(dev, ALTIX_DMA_UNIT_SIZE, &(node->dma_handle));
		if(node->next != NULL)
			node = node->next;
		else
			break;
	}
	node = last_node->next;
	for(i = 0; i< new_slabs; i++)
	{
		node->dma_vbase[0] = node->dma_handle + ALTIX_DMA_UNIT_OFFSET;
		node->dma_vbase[1] = ALTIX_DMA_UNIT_USEFULL_SIZE;
		if(node->next != NULL)
		{
			node->dma_vbase[2] = node->next->dma_handle;
			node = node->next;
		}
		else
		{
			node->dma_vbase[2] = 0xFFFFFFFC;
			break;
		}
	}
	last_node->dma_vbase[2] = last_node->next->dma_handle;
	pool->size = slabs;
	printk(KERN_INFO "New pool size is %d\n", pool->size);
	return pool->root->dma_handle;
}

uint altix_dma_pool_read(altix_dma_pool* pool, int size, void __user * buff)
{
	int i;
	int slabs;
	int ret_size;
	altix_dma_unit * node;
	if(size <= 0)
		return 0;
	slabs = ((size -1)/ALTIX_DMA_UNIT_USEFULL_SIZE) +1;
	//printk(KERN_INFO "Slabs: %d, Required slabs %d, usefull size:%d\n",pool->size, slabs, ALTIX_DMA_UNIT_USEFULL_SIZE);
	if(!access_ok(VERIFY_WRITE, buff, size))
	{
		return 0;
	}
	if(slabs > pool->size)
	{
		size = ALTIX_DMA_UNIT_USEFULL_SIZE * pool->size;
	}
	node = pool->root;
	ret_size = size;
	for(i = 0; i<slabs; i++)
	{
		if(size > ALTIX_DMA_UNIT_USEFULL_SIZE)
		{
			if(copy_to_user(buff + ALTIX_DMA_UNIT_USEFULL_SIZE*i, ((char*)node->dma_vbase) + ALTIX_DMA_UNIT_OFFSET, ALTIX_DMA_UNIT_USEFULL_SIZE) != 0)
			{
				printk(KERN_INFO "altix_dma_pool: dma pool copy to user failed");
				return -1;
			}
			if(node->next == NULL)
			{
				break;
			}
			node = node->next;
			size = size -ALTIX_DMA_UNIT_USEFULL_SIZE;
		}
		else
		{
			if(copy_to_user(buff + ALTIX_DMA_UNIT_USEFULL_SIZE*i, ((char*)node->dma_vbase) + ALTIX_DMA_UNIT_OFFSET, size) != 0)
			{
				printk(KERN_INFO "altix_dma_pool: dma pool copy to user failed");
				return -1;
			}
			size = 0;
			break;
		}
	}
	return ret_size - size;
}
uint altix_dma_pool_write(altix_dma_pool* pool, int size, void __user * buff)
{
	int i;
	int slabs;
	int ret_size;
	altix_dma_unit * node;
	if(size <= 0)
		return 0;
	slabs = ((size -1)/ALTIX_DMA_UNIT_USEFULL_SIZE) +1;
	if(!access_ok(VERIFY_READ, buff, size))
	{
		return 0;
	}
	if(slabs > pool->size)
	{
		size = ALTIX_DMA_UNIT_USEFULL_SIZE * pool->size;
	}
	node = pool->root;
	ret_size = size;
	for(i = 0; i<slabs; i++)
	{
		if(size > ALTIX_DMA_UNIT_USEFULL_SIZE)
		{
			if(copy_from_user(((char*)node->dma_vbase) + ALTIX_DMA_UNIT_OFFSET, buff + ALTIX_DMA_UNIT_USEFULL_SIZE*i,  ALTIX_DMA_UNIT_USEFULL_SIZE) != 0)
			{
				printk(KERN_INFO "altix_dma_pool: dma pool copy to user failed");
				return -1;
			}
			if(node->next == NULL)
			{
				break;
			}
			node = node->next;
			size = size -ALTIX_DMA_UNIT_USEFULL_SIZE;
		}
		else
		{
			if(copy_from_user(((char*)node->dma_vbase) + ALTIX_DMA_UNIT_OFFSET,buff + ALTIX_DMA_UNIT_USEFULL_SIZE*i, size) != 0)
			{
				printk(KERN_INFO "altix_dma_pool: dma pool copy to user failed");
				return -1;
			}
			size = 0;
			break;
		}
	}
	return ret_size - size;
}
