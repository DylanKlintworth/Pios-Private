#ifndef PAGE_H
	#define PAGE_H
	struct ppage {
		struct ppage *next;
		struct ppage *prev;
		void *physical_addr;
	};

	typedef struct ppage ppage;
#endif