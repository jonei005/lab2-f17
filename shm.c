#include "param.h"
#include "types.h"
#include "defs.h"
#include "x86.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "spinlock.h"

struct {
  struct spinlock lock;
  struct shm_page {
    uint id;
    char *frame;
    int refcnt;
  } shm_pages[64];
} shm_table;

void shminit() {
  int i;
  initlock(&(shm_table.lock), "SHM lock");
  acquire(&(shm_table.lock));
  for (i = 0; i< 64; i++) {
    shm_table.shm_pages[i].id =0;
    shm_table.shm_pages[i].frame =0;
    shm_table.shm_pages[i].refcnt =0;
  }
  release(&(shm_table.lock));
}

int shm_open(int id, char **pointer) {
	
	struct proc* p = myproc();
	pde_t* pgdir = p->pgdir;
	int permissions = PTE_U | PTE_W;
	
	// look through shm_table to see if this id already exists
	int i;
	int idFound = 0;
	//acquire(&(shm_table.lock));
	for (i = 0; i < 64; i++) {
		if (shm_table.shm_pages[i].id == id) {
			// if it does, use mappages to add the mapping between the virtual 
			// address and the physical address
			cprintf("ID found in shm_table: %d\n", id);
			idFound = 1;
				
			if (mappages(pgdir, (char*)p->sz, PGSIZE, V2P(shm_table.shm_pages[i].frame), permissions) < 0) {
				cprintf("Error in shm_open using mappages\n");
				return -1;
			}
			
			// increment refcnt for that id
			acquire(&(shm_table.lock));
			shm_table.shm_pages[i].refcnt++;
			release(&(shm_table.lock));
			
			// return pointer to the virtual address
			*pointer = (char*)p->sz;
			
			// increment process sz, as the address space expanded
			p->sz += PGSIZE;
			
			return 0;
		}
	}
	//release(&(shm_table.lock));
	
	if (!idFound) {
		cprintf("ID not found in shm_table: %d\n", id);
		// if it doesn't, then allocate a page and map it, then store this information 
		// in the shm_table
		
		char* mem;
		if ((mem = kalloc()) == 0) {
			cprintf("Error allocating new page with kalloc for shm_open\n");
			return -1;
		}
		memset(mem, 0, PGSIZE);
		
		// find empty entry in shm_table
		int i;
		int entry = -1;
		for (i = 0; i < 64; i++) {
			if (shm_table.shm_pages[i].frame == 0) {
				entry = i;
				break;
			}
		}
		if (entry == -1) {
			cprintf("No empty entries in shm_table\n");
			return -1;
		}
		
		// initialize entry: set refcnt to 1, set proper frame and id
		acquire(&(shm_table.lock));
		shm_table.shm_pages[entry].refcnt = 1;
		shm_table.shm_pages[entry].frame = mem;
		shm_table.shm_pages[entry].id = id;
		release(&(shm_table.lock));
		
		if (mappages(pgdir, (char*)p->sz, PGSIZE, V2P(shm_table.shm_pages[entry].frame), permissions) < 0) {
			cprintf("Error in shm_open using mappages (2)\n");
			release(&(shm_table.lock));
			return -1;
		}
		
		// return pointer to the virtual address
		*pointer = (char*)p->sz;
		
		// increment sz, as the address space expanded
		p->sz += PGSIZE;
		
		
		return 0;
	}
	
	cprintf("Something went wrong in shm_open\n");
	return -1; 
}


int shm_close(int id) {
	// look for shared memory segment in shm_table
	
	int i;
	for (i = 0; i < 64; i++) {
		if (shm_table.shm_pages[i].id == id) {
			// decrement refcnt
			acquire(&(shm_table.lock));
			if (--shm_table.shm_pages[i].refcnt == 0) {
				// if refcnt reaches 0, clear the entry
				shm_table.shm_pages[i].id = 0;
				shm_table.shm_pages[i].frame = 0;
				cprintf("shm_close: refcnt is 0 for id: %d\n", id);
				// no need to free up the page
				
				release(&(shm_table.lock));
				return 0;
			}
			release(&(shm_table.lock));
		}
	}

	return 0; 
}
