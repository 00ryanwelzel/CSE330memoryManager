#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/sched/signal.h>

#include <linux/mm_types.h>
#include <linux/highmem.h>
#include <linux/mm.h>

#include <linux/swap.h>
#include <linux/swapops.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("RYAN WELZEL");

static unsigned int pid = 0;
module_param(pid, int, 0);
static unsigned long long addr = 0;
module_param(addr, ullong, 0);


static int __init memman_init(void){
	struct task_struct *task;
	struct mm_struct *mm;

	pgd_t *pgd;
	p4d_t *p4d;
	pud_t *pud;
	pmd_t *pmd;
	pte_t *pte;

	unsigned long vaddr = (unsigned long)addr;
	unsigned long paddr;
	unsigned long pageoffset;
	unsigned long pagebase;

	swp_entry_t entry;
	unsigned long swap_id;

	// get mm for input pid
	rcu_read_lock();
	for_each_process(task){
		if(task->pid == pid){
			mm = task->mm;
			break;
		}
	}
	rcu_read_unlock();

	if(!task || task->pid != pid){
		pr_err("mm dne");
		return 0;
	}

	// traverse
	pgd = pgd_offset(mm, vaddr);
	if(pgd_none(*pgd) || pgd_bad(*pgd)){
		pr_info("[CSE330-Memory-Manager] PID [%d]: virtual address [%llx] physical address [NA] swap identifier [NA]\n", pid, addr);
		return 0;
	}

	p4d = p4d_offset(pgd, vaddr);
	if(p4d_none(*p4d) || p4d_bad(*p4d)){
		pr_info("[CSE330-Memory-Manager] PID [%d]: virtual address [%llx] physical address [NA] swap identifier [NA]\n", pid, addr);
		return 0;
	}

	pud = pud_offset(p4d, vaddr);
	if(pud_none(*pud) || pud_bad(*pud)){
		pr_info("[CSE330-Memory-Manager] PID [%d]: virtual address [%llx] physical address [NA] swap identifier [NA]\n", pid, addr);
		return 0;
	}

	pmd = pmd_offset(pud, vaddr);
	if(pmd_none(*pmd) || pmd_bad(*pmd)){
		pr_info("[CSE330-Memory-Manager] PID [%d]: virtual address [%llx] physical address [NA] swap identifier [NA]\n", pid, addr);
		return 0;
	}

	pte = pte_offset_kernel(pmd, vaddr);
	// no page, page gone
	if(!pte){
		pr_info("[CSE330-Memory-Manager] PID [%d]: virtual address [%llx] physical address [NA] swap identifier [NA]\n", pid, addr);
		return 0;
	}

	// address translation
	// page present
	if(pte_present(*pte)){
		pagebase = (pte_pfn(*pte) << PAGE_SHIFT);
		pageoffset = vaddr & ~PAGE_MASK;
		paddr = pagebase | pageoffset;
		pr_info("[CSE330-Memory-Manager] PID [%d]: virtual address [%llx] physical address [%lx] swap identifier [NA]\n", task->pid, addr, paddr);
	} else if (!pte_none(*pte)){
		// page in swap
		entry = pte_to_swp_entry(*pte);
		swap_id = entry.val;
		pr_info("[CSE330-Memory-Manager] PID [%d]: virtual address [%llx] physical address [NA] swap identifier [%lx]\n", task->pid, addr, swap_id);

	}
	
	return 0;
}

static void __exit memman_exit(void){

}

module_init(memman_init);
module_exit(memman_exit);
