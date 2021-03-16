#ifndef PAGE_H
	#define PAGE_H
	struct ppage {
		struct ppage *next;
		struct ppage *prev;
		void *physical_addr;
	};

	typedef struct ppage ppage;

	void ppageAdd(ppage **head, ppage *newElement);
	void ppageRemove(ppage *b);
	void init_pfa_list(void);
	void free_physical_pages(ppage *ppage_list);
	ppage *allocate_physical_pages(unsigned int npages);
	
#endif