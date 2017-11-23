#include"definitions.h"
#include"vm.h"
#include<sys/shm.h>
#include<sys/stat.h>
#include"queue.h"

#define SIZE 65536

int seg[4];
PageTableElement *table[4];
Queue requests;

PageTableElement* createPageTable(int pnumber){

    int i;
    seg[pnumber] = shmget(IPC_PRIVATE, SIZE*sizeof(PageTableElement), IPC_CREAT | IPC_EXCL | S_IRWXU);	// Page Table
    table[pnumber] = (PageTableElement *) shmat(seg[pnumber], 0, 0);
        
    if(requests == NULL){
    	requests = queue_create();
    }

    // Initialize table
    for(i = 0; i < SIZE; i++){
        table[pnumber][i].frame = NULL;
        table[pnumber][i].page.index = i;
        table[pnumber][i].page.proc_number = pnumber;
        table[pnumber][i].page.offset = NULL;
        table[pnumber][i].page.type = NULL;
    }

    return table[pnumber];
}

PageTableElement* getPageTable(int pnumber){
    return table[pnumber];
}

void trans(int pnumber, unsigned int i, unsigned int o, char rw){

    if(table[pnumber][i].frame != NULL){
        printf("P%d, %x%x, %c", pnumber, table[pnumber][i].frame.index, o, rw);
    }
    else{
        Page *pg = (Page *) malloc(sizeof(Page));
        
	    pg->index = i;
	    pg->proc_number = pnumber;
	    pg->offset = o;
	    pg->type = rw;
	    
    	queue_push(requests, pg);	
        kill(getppid(), SIGUSR1);   // Ask MM for Page Frame
        pause();                    // Waits until MM sets a Page Frame to current page
        printf("P%d, %x%x, %c", pnumber, table[pnumber][i].frame.index, o, rw);
    }
}

Page * getCurrentRequest(){
	return queue_pop(results);	
}




