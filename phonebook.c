#include <termios.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>


//=====================================defines=====================================
#define LCHILD(x) 2 * x + 1	//This is a defined function to calculate the index of the left child of given layer
#define RCHILD(x) 2 * x + 2	//This is a defined function to calculate the index of the right child of given layer
#define MAX_FNAME_LNAME_SIZE 31	//This is a defined value for the maximum chars possible for first and last name to have
#define MAX_TELEP_CELLP_SIZE 15	//This is a defined value for the maximum chars possible for telephone and cellphone to have 

//=====================================structs=====================================
//A struct that defines all the information of a contact
struct contact{
	char fname[MAX_FNAME_LNAME_SIZE];
	char lname[MAX_FNAME_LNAME_SIZE];
	char telep[MAX_TELEP_CELLP_SIZE];
	char cellp[MAX_TELEP_CELLP_SIZE];
};

//A struct that represents a maximum heap 
struct max_heap{
	int size;
	struct contact *elem;
};

//A struct that includes pointers to all existing maximum heaps
struct heap_pointers{
	struct max_heap* unsorted_heap;
	struct max_heap* sorted_by_fname;
	struct max_heap* sorted_by_lname;
	struct max_heap* sorted_by_telep;
	struct max_heap* sorted_by_cellp;
};


//===========================================functions===================================================
void free_allocations(struct heap_pointers*);
struct max_heap init_heap(int);
void build_heaps(struct heap_pointers*, int);
size_t get_input(char* field_array, const size_t max_size);
void heapify_by_fname(struct max_heap*, int, int);
void heapify_by_lname(struct max_heap*, int, int);
void heapify_by_telep(struct max_heap*, int, int);
void heapify_by_cellp(struct max_heap*, int, int);
void print_sorted_heap(struct heap_pointers*);
void sort_by_fname(struct max_heap*);
void sort_by_lname(struct max_heap*);
void sort_by_telep(struct max_heap*);
void sort_by_cellp(struct max_heap*);
void swap(struct contact*, struct contact*);
void delete_contact(struct heap_pointers* hp);
void delete_in_each_heap(struct max_heap*, char[], char[]);
void edit_contact(struct heap_pointers*);
void update_each_heap(struct max_heap*, char[], char[], char[], char[], char[], char[]);
void search(struct max_heap*);
void search_by_fname(struct max_heap* hp);
void search_by_lname(struct max_heap* hp);
void search_by_telephone(struct max_heap* hp);
void search_by_cellphone(struct max_heap* hp);
void list(struct max_heap*);
void print(struct max_heap*, int, int);
void help(void);
void clrscr();
void gotoxy(int x, int y);

//==============================================main===================================================
int main()
{
	//initializations
	struct heap_pointers* heaps_pointers =  (struct heap_pointers*) malloc (sizeof(struct heap_pointers));
	struct max_heap unsorted_heap = init_heap(0);
	struct max_heap sorted_by_fname = init_heap(0);
	struct max_heap sorted_by_lname = init_heap(0);
	struct max_heap sorted_by_telep = init_heap(0);
	struct max_heap sorted_by_cell = init_heap(0);

	heaps_pointers->unsorted_heap = &unsorted_heap;
	heaps_pointers->sorted_by_fname = &sorted_by_fname;
	heaps_pointers->sorted_by_lname = &sorted_by_lname;
	heaps_pointers->sorted_by_telep = &sorted_by_telep;
	heaps_pointers->sorted_by_cellp = &sorted_by_cell;
	char user_input;

	while(1) {
		clrscr();						
		printf("\n|Phone Book12<::>Home|\n");
		printf("--------------------------------------------------------------------------------\n");
		printf("Welcome to PHONE BOOK2022!\nYou have inserted ( %d ) contacts.\n\n",heaps_pointers->unsorted_heap->size);
		printf("\t[1] |--> Insert\n");
		printf("\t[2] |--> Delete\n");
		printf("\t[3] |--> Edit\n");
		printf("\t[4] |--> Search\n");
		printf("\t[5] |--> Show All\n");
		printf("\t[6] |--> Sort\n");
		printf("\t[7] |--> Help\n");
		printf("\t[8] |--> Exit\n");
		printf("\n\tPlease Enter Your Choice (1-8): ");
		user_input = getc(stdin);
		getc(stdin);
		switch(user_input) {
			  case '1':
			  build_heaps(heaps_pointers, heaps_pointers->unsorted_heap->size + 1);
			  break;
			  case '2':
			  delete_contact(heaps_pointers);
			  break;
			  case '3':
			  edit_contact(heaps_pointers);
			  break;
			  case '4':
			  search(heaps_pointers->unsorted_heap);
			  break;
			  case '5':
			  list(heaps_pointers->unsorted_heap);
			  break;
			  case '6':
			  print_sorted_heap(heaps_pointers);
			  break;
			  case '7':
			  help();
			  break;
			  case '8':
			  free_allocations(heaps_pointers);
			  exit(0);
			  default:
			  printf("\nThere is no item with symbol \"%c\".Please enter a number between 1-8!\nPress any key to continue...", user_input);
			  getc(stdin);
		      getc(stdin);
			  break;
		}//End of swicth
	}
	
	free_allocations(heaps_pointers);
	return 0;
}

//This function frees all allocated memory
void free_allocations(struct heap_pointers* heaps_pointers)
{
	free(heaps_pointers->sorted_by_fname->elem);
	free(heaps_pointers->sorted_by_lname->elem);
	free(heaps_pointers->sorted_by_telep->elem);
	free(heaps_pointers->sorted_by_cellp->elem);
	free(heaps_pointers->unsorted_heap->elem);
	free(heaps_pointers);
}

//This function inits the heap and sets it's size to be 0. It returns an initialized heap
struct max_heap init_heap(int size)
{
	struct max_heap hp;
	hp.size = 0;
	return hp;
}

//This function builds all the heaps that the heap_pointers struct contains
void build_heaps(struct heap_pointers* heaps_pointers, int size)
{
	
	struct contact new_contact;	//This node will save the information of the new contact we will add to the heaps
	char ans='y';			//Saves the answer of the user
    	printf("\nPhone Book12<::>Insert Contacts");
    	printf("\n--------------------------------------------------------------------------------");
	
	while(ans=='y'){	//The user wants to insert a contact
		int i;
		if(heaps_pointers->unsorted_heap->size){	//The heaps aren't empty
			heaps_pointers->unsorted_heap->elem = realloc(heaps_pointers->unsorted_heap->elem, (heaps_pointers->unsorted_heap->size + 1) * sizeof(struct contact));
			heaps_pointers->sorted_by_fname->elem = realloc(heaps_pointers->sorted_by_fname->elem, (heaps_pointers->sorted_by_fname->size + 1) * sizeof(struct contact));
			heaps_pointers->sorted_by_lname->elem = realloc(heaps_pointers->sorted_by_lname->elem, (heaps_pointers->sorted_by_lname->size + 1) * sizeof(struct contact));
			heaps_pointers->sorted_by_telep->elem = realloc(heaps_pointers->sorted_by_telep->elem, (heaps_pointers->sorted_by_telep->size + 1) * sizeof(struct contact));
			heaps_pointers->sorted_by_cellp->elem = realloc(heaps_pointers->sorted_by_cellp->elem, (heaps_pointers->sorted_by_cellp->size + 1) * sizeof(struct contact));
		}

		else{		//The heaps are empty	
			heaps_pointers->unsorted_heap->elem = malloc(sizeof(struct contact)); 			
			heaps_pointers->sorted_by_fname->elem = malloc(sizeof(struct contact)); 			
			heaps_pointers->sorted_by_lname->elem = malloc(sizeof(struct contact)); 
			heaps_pointers->sorted_by_telep->elem = malloc(sizeof(struct contact)); 
			heaps_pointers->sorted_by_cellp->elem = malloc(sizeof(struct contact)); 
		}

		clrscr();
		
		//Inserting the data to the temporary node
		printf("\n\nData of Contact %2.2d{\n",heaps_pointers->unsorted_heap->size + 1);
		printf("\n\t  1-F.Name: ");
		get_input(new_contact.fname, MAX_FNAME_LNAME_SIZE);
		
		printf("\t  2-L.Name: ");
		get_input(new_contact.lname, MAX_FNAME_LNAME_SIZE);
			
		printf("\t  3-Tele.P: ");
		get_input(new_contact.telep, MAX_TELEP_CELLP_SIZE);
			
		printf("\t  4-Cell.P: ");
		get_input(new_contact.cellp, MAX_TELEP_CELLP_SIZE);
		
		//Add the new node to each heap in the last index
		heaps_pointers->unsorted_heap->elem[(heaps_pointers->unsorted_heap->size)++] = new_contact;
		heaps_pointers->sorted_by_fname->elem[(heaps_pointers->sorted_by_fname->size)++] = new_contact;
		heaps_pointers->sorted_by_lname->elem[(heaps_pointers->sorted_by_lname->size)++] = new_contact;
		heaps_pointers->sorted_by_telep->elem[(heaps_pointers->sorted_by_telep->size)++] = new_contact;
		heaps_pointers->sorted_by_cellp->elem[(heaps_pointers->sorted_by_cellp->size)++] = new_contact;

		//Sort each heap using the correct sorting function
		sort_by_fname(heaps_pointers->sorted_by_fname);
		sort_by_lname(heaps_pointers->sorted_by_lname);
		sort_by_telep(heaps_pointers->sorted_by_telep);
		sort_by_cellp(heaps_pointers->sorted_by_cellp);

		printf("\n|-->Data Recorded!}");
		printf("\n\t\t\tNext Contact?(y/n) Answer:");
		ans = getc(stdin);
		getc(stdin);
	}
	printf("\n\nYou have inserted ( %d ) contact!\nPress a key to return main page & continue program|-->",heaps_pointers->unsorted_heap->size);
	getc(stdin);

}

//This function reads the data for each contact and makes sure the size of the input is not too long
size_t get_input(char* field_array, const size_t max_size)
{
	char* string = NULL;
	size_t len=0;
	size_t len_size=0;

	len_size = getline(&string,&len,stdin);
	if(len_size < max_size - 1)
	{	
		strncpy(field_array, string, len_size);
		field_array[len_size] = '\0';	
	}

	else
	{
		strncpy(field_array, string, max_size);
		field_array[max_size] = '\0';
		len_size = max_size;
	}

	free(string);
	return len_size;
}

//This function does heapify by first name of the contact
void heapify_by_fname(struct max_heap* hp, int sub_tree, int i) 
{
	//Compare dad and left child and find the index where the first name that appears later in alphabetical order
	int largest = (LCHILD(i) < sub_tree && strcmp(hp->elem[LCHILD(i)].fname, hp->elem[i].fname) > 0) ? LCHILD(i) : i;
	//Check if right child is smaller
	if(RCHILD(i) < sub_tree && strcmp(hp->elem[RCHILD(i)].fname, hp->elem[largest].fname) > 0)	
		largest = RCHILD(i);
	
	//If the current root is not the smallest 
	if(largest != i){
		swap(&(hp->elem[i]), &(hp->elem[largest]));	
		heapify_by_fname(hp, sub_tree, largest);
	}
	
}

//This function does heapify by last name of the contact
void heapify_by_lname(struct max_heap* hp, int sub_tree, int i)
{
	//Compare dad and left child and find the index where last name that appears later in alphabetical order
	int largest = (LCHILD(i) < sub_tree && strcmp(hp->elem[LCHILD(i)].lname, hp->elem[i].lname) > 0) ? LCHILD(i) : i;
	//Check if right child is smaller
	if(RCHILD(i) < sub_tree && strcmp(hp->elem[RCHILD(i)].lname, hp->elem[largest].lname) > 0)	
		largest = RCHILD(i);
	
	//If the current root is not the smallest 
	if(largest != i){
		swap(&(hp->elem[i]), &(hp->elem[largest]));	
		heapify_by_lname(hp, sub_tree, largest);
	}
}

//This function does heapify by  telephone of the contact
void heapify_by_telep(struct max_heap* hp, int sub_tree, int i)
{
	//Compare dad and left child and find the index where telephone that is the largest compared to rest telephone numbers
	int largest = (LCHILD(i) < sub_tree && strcmp(hp->elem[LCHILD(i)].telep, hp->elem[i].telep) > 0) ? LCHILD(i) : i;
	//Check if right child is smaller
	if(RCHILD(i) < sub_tree && strcmp(hp->elem[RCHILD(i)].telep, hp->elem[largest].telep) > 0)	
		largest = RCHILD(i);
	
	//If the current root is not the smallest 
	if(largest != i){
		swap(&(hp->elem[i]), &(hp->elem[largest]));	
		heapify_by_telep(hp, sub_tree, largest);
	}
}

//This function does heapify by cellphone of the contact
void heapify_by_cellp(struct max_heap* hp, int sub_tree, int i)
{
	//Compare dad and left child and find the index where the cellphone that is the largest compared to rest cellphone numbers
	int largest = (LCHILD(i) < sub_tree && strcmp(hp->elem[LCHILD(i)].cellp, hp->elem[i].cellp) > 0) ? LCHILD(i) : i;
	//Check if right child is smaller
	if(RCHILD(i) < sub_tree && strcmp(hp->elem[RCHILD(i)].cellp, hp->elem[largest].cellp) > 0)	
		largest = RCHILD(i);
	
	//If the current root is not the smallest 
	if(largest != i){
		swap(&(hp->elem[i]), &(hp->elem[largest]));	
		heapify_by_cellp(hp, sub_tree, largest);
	}
}

//This function prints the sorted heap that the user wants
void print_sorted_heap(struct heap_pointers* heaps_pointers)
{
	while(1){
		char ch;
		clrscr();		
	    	printf("\nPhone Book12<::>$earch Contacts");
	    	printf("\n--------------------------------------------------------------------------------");
		printf("\nChoose sort type,please:\n\n");
		printf("\t[1] |--> Sort by first name\n");
		printf("\t[2] |--> Sort by last name\n");
		printf("\t[3] |--> Sort by phone number\n");
		printf("\t[4] |--> Sort by cellphone number\n");
		printf("\t[5] |--> Main Menu\n");	
		printf("\n\t::Enter a number (1-5): ");
		ch = getc(stdin);
		getc(stdin);
		printf("\n--------------------------------------------------------------------------------");
	    	switch(ch){
			case '1':
			list(heaps_pointers->sorted_by_fname);
			break;
			case '2':
			list(heaps_pointers->sorted_by_lname);
			break;
			case '3':
			list(heaps_pointers->sorted_by_telep);
			break;
			case '4':
			list(heaps_pointers->sorted_by_cellp);
			break;
			default:
			return ;
		}
	}
}

//This function sorts the heap by first name
void sort_by_fname(struct max_heap* hp)
{
	int i;
	//Starting with the dad who has the last child/children (the two/ one nodes/ node are his children/ child)
	for(i = hp->size / 2 - 1; i >= 0; i--)
		heapify_by_fname(hp, hp->size, i);

	for(i = hp->size - 1; i >= 0; i--){	//For each node try swapping with root and heapify the sub tree
		swap(&(hp->elem[0]), &(hp->elem[i]));
		heapify_by_fname(hp, i, 0); 
	}
}

//This function sorts the heap by last name
void sort_by_lname(struct max_heap* hp)
{
	int i;
	//Starting with the dad who has the last child/children (the two/ one nodes/ node are his children/ child)
	for(i = hp->size / 2 - 1; i >= 0; i--)
		heapify_by_lname(hp, hp->size, i);

	for(i = hp->size - 1; i >= 0; i--){	//For each node try swapping with root and heapify the sub tree
		swap(&(hp->elem[0]), &(hp->elem[i]));
		heapify_by_lname(hp, i, 0); 
	}
}

//This function sorts the heap by telephone
void sort_by_telep(struct max_heap* hp)
{
	int i;
	//Starting with the dad who has the last child/children (the two/ one nodes/ node are his children/ child)
	for(i = hp->size / 2 - 1; i >= 0; i--)
		heapify_by_telep(hp, hp->size, i);

	for(i = hp->size - 1; i >= 0; i--){	//For each node try swapping with root and heapify the sub tree
		swap(&(hp->elem[0]), &(hp->elem[i]));
		heapify_by_telep(hp, i, 0); 
	}
}

//This function sorts the heap by cellphone
void sort_by_cellp(struct max_heap* hp)
{
	int i;
	//Starting with the dad who has the last child/children (the two/ one nodes/ node are his children/ child)
	for(i = hp->size / 2 - 1; i >= 0; i--)
		heapify_by_cellp(hp, hp->size, i);

	for(i = hp->size - 1; i >= 0; i--){	//For each node try swapping with root and heapify the sub tree
		swap(&(hp->elem[0]), &(hp->elem[i]));
		heapify_by_cellp(hp, i, 0); 
	}
}

//This function deletes a contact from all heaps
void delete_contact(struct heap_pointers* heap_pointers) {
	char dfname_string[MAX_FNAME_LNAME_SIZE],dlname_string[MAX_FNAME_LNAME_SIZE];
	size_t len=0;
	size_t len_size=0;
	int i, 
		found = 0; //A flag that tells us if the contact was successfully deleted and needs to be deleted from rest heaps
	char ch;
	clrscr();
    	printf("\nPhone Book12<::>Delete Contacts");
    	printf("\n--------------------------------------------------------------------------------");
	printf ("\n::Enter data of the contact that you want delete it,please:");
	printf("\n\n  ::Enter first name: ");
	get_input(dfname_string, MAX_FNAME_LNAME_SIZE);
		
	printf("\n  ::Enter last name: ");
	get_input(dlname_string, MAX_FNAME_LNAME_SIZE);
	
	for(i = 0; i < heap_pointers->unsorted_heap->size; i++){	//Go over the unsorted heap and try to find the contact
		if(strcmp(heap_pointers->unsorted_heap->elem[i].fname, dfname_string) == 0 && strcmp(heap_pointers->unsorted_heap->elem[i].lname, dlname_string) == 0){
			printf("\nContact was found! Details:");
			printf("\n\nCantact %2.2d{",i+1);
			printf("\n\t   F.Name:%s\n\t   L.name:%s\n\t   Tele.P:%s\n\t   Cell.P:%s\n\t   }",heap_pointers->unsorted_heap->elem[i].fname, heap_pointers->unsorted_heap->elem[i].lname, heap_pointers->unsorted_heap->elem[i].telep,heap_pointers->unsorted_heap->elem[i].cellp);
			printf("\n\nAre you sure you want to delete this contact?(y/n) ");
			ch = getc(stdin);
			getc(stdin);
			if(ch == 'y'){	//Found the contact and the user wants to delete
				heap_pointers->unsorted_heap->elem[i] = heap_pointers->unsorted_heap->elem[--(heap_pointers->unsorted_heap->size)];
				heap_pointers->unsorted_heap->elem = realloc(heap_pointers->unsorted_heap->elem, heap_pointers->unsorted_heap->size * sizeof(struct contact));

				found = 1;	//The flag tells us the contact exists and now we need to remove it from all other heaps
			}
			break;	
		}
	}

	if(!found)
		printf("\t\t\n<<This contact does not exist in this list or program can not delete it.>>");

	else{	//The contact was deleted from the unordered heap and now should be deleted from the rest

		//Delete from the heap that is sorted by first name and heapify it
		delete_in_each_heap(heap_pointers->sorted_by_fname, dfname_string, dlname_string);	
		
		//Delete from the heap that is sorted by last name and heapify it
		delete_in_each_heap(heap_pointers->sorted_by_lname, dfname_string, dlname_string);	
	
		//Delete from the heap that is sorted by telephone and heapify it
		delete_in_each_heap(heap_pointers->sorted_by_telep, dfname_string, dlname_string);	

		//Delete from the heap that is sorted by cellphone and heapify it
		delete_in_each_heap(heap_pointers->sorted_by_cellp, dfname_string, dlname_string);	
	
		//Sort each heap by it's sorting function
		sort_by_fname(heap_pointers->sorted_by_fname);
		sort_by_lname(heap_pointers->sorted_by_lname);
		sort_by_telep(heap_pointers->sorted_by_telep);
		sort_by_cellp(heap_pointers->sorted_by_cellp);

		
		printf("\t\t\n<<Target contact was successfully deleted from list!>>");
	}
	printf("\n\n\nPress a key to return main page & continue program|-->");
	getc(stdin);
}

//This function deletes a contact which's first name is fname and it's last name is lname from a given heap
void delete_in_each_heap(struct max_heap* hp, char fname[MAX_FNAME_LNAME_SIZE], char lname[MAX_FNAME_LNAME_SIZE])
{
	int i;
	for(i = 0; i < hp->size; i++){
		if(strcmp(hp->elem[i].fname, fname) == 0){
			hp->elem[i] = hp->elem[--(hp->size)];
			hp->elem = realloc(hp->elem, hp->size * sizeof(struct contact));		
		}	
	}	
}

//This function edits a contact
void edit_contact(struct heap_pointers* heap_pointers)
{
	char efname[MAX_FNAME_LNAME_SIZE] ,elname[MAX_FNAME_LNAME_SIZE];
	size_t len=0;
	size_t len_size=0;
	int i, 
		found = 0; //A flag that tells us if a contact was edited 
	char ch;
	clrscr();	
    	printf("\nPhone Book12<::>Edit Contacts");
    	printf("\n--------------------------------------------------------------------------------");
	printf ("\n::Enter data of the contact that you want edit it,please:");
	
	printf("\n\n  ::Enter first name: ");
	get_input(efname, MAX_FNAME_LNAME_SIZE);
	printf("\n  ::Enter last name: ");
	get_input(elname, MAX_FNAME_LNAME_SIZE);

	for(i = 0; i < heap_pointers->unsorted_heap->size; i++){	//Goes over the unsorted heap
		if(strcmp(heap_pointers->unsorted_heap->elem[i].fname, efname) == 0 && strcmp(heap_pointers->unsorted_heap->elem[i].lname, elname) == 0){
			printf("\nContact was found! Details:");
			printf("\n\nCantact %2.2d{",i+1);
			printf("\n\t   F.Name:%s\n\t   L.name:%s\n\t   Tele.P:%s\n\t   Cell.P:%s\n\t   }",heap_pointers->unsorted_heap->elem[i].fname,heap_pointers->unsorted_heap->elem[i].lname, heap_pointers->unsorted_heap->elem[i].telep,heap_pointers->unsorted_heap->elem[i].cellp);
			printf("\n\nDo you want edit it?(y/n) ");
			ch = getc(stdin);
			getc(stdin);
			if(ch == 'y'){	//Contact to edit was found and the user wants to edit it
				printf("\n::Enter NEW data for this contact...");
				printf("\n >|Enter new first name: ");	
				get_input(heap_pointers->unsorted_heap->elem[i].fname, MAX_FNAME_LNAME_SIZE);		

				printf(" >|Enter new last name: ");
				get_input(heap_pointers->unsorted_heap->elem[i].lname, MAX_FNAME_LNAME_SIZE);
			
				printf(" >|Enter new telephone number: ");
				get_input(heap_pointers->unsorted_heap->elem[i].telep, MAX_TELEP_CELLP_SIZE);

				printf(" >|Enter new cellphone number: ");
				get_input(heap_pointers->unsorted_heap->elem[i].cellp, MAX_TELEP_CELLP_SIZE);
				found = 1;
			}
			break;	
		}
	}
	if(found){	//The contact was successfully edited and now should be edited in all heaps
		
		//Edit the contact in the heap sorted by first name
		update_each_heap(heap_pointers->sorted_by_fname, heap_pointers->unsorted_heap->elem[i].fname, heap_pointers->unsorted_heap->elem[i].lname, heap_pointers->unsorted_heap->elem[i].telep, heap_pointers->unsorted_heap->elem[i].cellp, efname, elname);

		//Edit the contact in the heap sorted by last name
		update_each_heap(heap_pointers->sorted_by_lname, heap_pointers->unsorted_heap->elem[i].fname, heap_pointers->unsorted_heap->elem[i].lname, heap_pointers->unsorted_heap->elem[i].telep, heap_pointers->unsorted_heap->elem[i].cellp, efname, elname);

		//Edit the contact in the heap sorted by telephone
		update_each_heap(heap_pointers->sorted_by_telep, heap_pointers->unsorted_heap->elem[i].fname, heap_pointers->unsorted_heap->elem[i].lname, heap_pointers->unsorted_heap->elem[i].telep, heap_pointers->unsorted_heap->elem[i].cellp, efname, elname);

		////Edit the contact in the heap sorted by cellphone
		update_each_heap(heap_pointers->sorted_by_cellp, heap_pointers->unsorted_heap->elem[i].fname, heap_pointers->unsorted_heap->elem[i].lname, heap_pointers->unsorted_heap->elem[i].telep, heap_pointers->unsorted_heap->elem[i].cellp, efname, elname);
		
		//Sort all heaps again
		sort_by_fname(heap_pointers->sorted_by_fname);
		sort_by_lname(heap_pointers->sorted_by_lname);
		sort_by_telep(heap_pointers->sorted_by_telep);
		sort_by_cellp(heap_pointers->sorted_by_cellp);


		printf("\t\t\n<<Target contact was successfully updated!>>");
	}
	else
		printf("\t\t\n<<This contact does not exist or you chose not to Edit it.>>");
	printf("\n\n\nPress a key to return main page & continue program|-->");
	getc(stdin);
	
	

}

//This function edits information of a contact with given information in a given heap
void update_each_heap(struct max_heap* hp, char fname[MAX_FNAME_LNAME_SIZE], char lname[MAX_FNAME_LNAME_SIZE], char telep[MAX_TELEP_CELLP_SIZE], char cellp[MAX_TELEP_CELLP_SIZE], char curr_fname[MAX_FNAME_LNAME_SIZE], char curr_lname[MAX_FNAME_LNAME_SIZE])
{
	int i;
	for(i = 0; i < hp->size; i++){
		if(strcmp(hp->elem[i].fname, curr_fname) == 0 && strcmp(hp->elem[i].lname, curr_lname) == 0){

			strncpy(hp->elem[i].fname, fname, MAX_FNAME_LNAME_SIZE);	
			strncpy(hp->elem[i].lname, lname, MAX_FNAME_LNAME_SIZE);
			strncpy(hp->elem[i].telep, telep, MAX_TELEP_CELLP_SIZE);
			strncpy(hp->elem[i].cellp, cellp, MAX_TELEP_CELLP_SIZE);
		
			break;	
		}
	}
}

//This funcion searches for a contact 
void search(struct max_heap* hp)
{
	char ch;
	clrscr();
    	printf("\nPhone Book12<::>$earch Contacts");
    	printf("\n--------------------------------------------------------------------------------");
	printf("\nChoose search type,please:\n\n");
	printf("\t[1] |--> Search by first name\n");
	printf("\t[2] |--> Search by last name\n");
	printf("\t[3] |--> Search by phone number\n");
	printf("\t[4] |--> Search by cellphone number\n");
	printf("\t[5] |--> Main Menu\n");
	printf("\n\t::Enter a number (1-5): ");
	ch = getc(stdin);
	getc(stdin);
	printf("\n--------------------------------------------------------------------------------");
	switch(ch) {
		case '1':
		search_by_fname(hp);
	    	break;
		case '2':
		search_by_lname(hp);
		break;
		case '3':
		search_by_telephone(hp);
		break;
		case '4':
		search_by_cellphone(hp);
		break;
		case '5':
		default:
		return;
    }
}

//This function searches a contact by a first name
void search_by_fname(struct max_heap* hp)
{
	char fname[MAX_FNAME_LNAME_SIZE];
	register int i,find=0;
	size_t len=0;
	size_t len_size=0;
	printf("Enter a first name to search:");
	get_input(fname, MAX_FNAME_LNAME_SIZE);

    	for(i = 0;i < hp->size; i++)
		if(strcmp(fname,hp->elem[i].fname) == 0) {
			if (!find)	//First contact that has the same first name as wanted (for printing)
				printf("\t\n<<Target contact found! Details:>>");
			printf("\n\nCantact %2.2d{",i+1);
			printf("\n\t   F.Name:%s\n\t   L.name:%s\n\t   Tele.P:%s\n\t   Cell.P:%s\n\t   }",hp->elem[i].fname,hp->elem[i].lname, 					hp->elem[i].telep, hp->elem[i].cellp);
			find=1;
		}
	if(!find)
		printf("\t\n<<Not Find!There is no contact with this name in phone book.>>");	
	
	printf("\nPress a key to search another contact.");
	getc(stdin);
	search(hp);
}

//This function searches a contact by last name
void search_by_lname(struct max_heap* hp)
{
	char lname[MAX_FNAME_LNAME_SIZE];
	register int i,find=0;
	size_t len=0;
	size_t len_size=0;
	printf("Enter a last name to search:");
	get_input(lname, MAX_FNAME_LNAME_SIZE);

    	for(i = 0;i < hp->size; i++)
		if(strcmp(lname,hp->elem[i].lname) == 0) {
			if (!find)	//First contact that has the same last name as wanted (for printing)
				printf("\t\n<<Target contact found! Details:>>");
			printf("\n\nCantact %2.2d{",i+1);
			printf("\n\t   F.Name:%s\n\t   L.name:%s\n\t   Tele.P:%s\n\t   Cell.P:%s\n\t   }",hp->elem[i].fname,hp->elem[i].lname, 					hp->elem[i].telep, hp->elem[i].cellp);
			find=1;
		}
	if(!find)
		printf("\t\n<<Not Find!There is no contact with this name in phone book.>>");	
	
	printf("\nPress a key to search another contact.");
	getc(stdin);
	search(hp);
}

//This function searches a contact by telephone
void search_by_telephone(struct max_heap* hp)
{
	char telephone[MAX_TELEP_CELLP_SIZE];
	register int i,find=0;
	size_t len=0;
	size_t len_size=0;
	printf("Enter a first name to search:");
	get_input(telephone, MAX_TELEP_CELLP_SIZE);

    	for(i = 0;i < hp->size; i++)
		if(strcmp(telephone, hp->elem[i].telep) == 0) {
			if (!find)	//First contact that has the same telephone as wanted (for printing)
				printf("\t\n<<Target contact found! Details:>>");
			printf("\n\nCantact %2.2d{",i+1);
			printf("\n\t   F.Name:%s\n\t   L.name:%s\n\t   Tele.P:%s\n\t   Cell.P:%s\n\t   }",hp->elem[i].fname,hp->elem[i].lname, 					hp->elem[i].telep, hp->elem[i].cellp);
			find=1;
		}
	if(!find)
		printf("\t\n<<Not Find!There is no contact with this name in phone book.>>");	
	
	printf("\nPress a key to search another contact.");
	getc(stdin);
	search(hp);
}

//This function searches a contact by cellphone 
void search_by_cellphone(struct max_heap* hp)
{
	char cellphone[MAX_TELEP_CELLP_SIZE];
	register int i,find=0;
	size_t len=0;
	size_t len_size=0;
	printf("Enter a first name to search:");
	get_input(cellphone, MAX_TELEP_CELLP_SIZE);
    	for(i = 0;i < hp->size; i++)
		if(strcmp(cellphone, hp->elem[i].cellp) == 0) {
			if (!find)	//First contact that has the same cellphone as wanted (for printing)
				printf("\t\n<<Target contact found! Details:>>");
			printf("\n\nCantact %2.2d{",i+1);
			printf("\n\t   F.Name:%s\n\t   L.name:%s\n\t   Tele.P:%s\n\t   Cell.P:%s\n\t   }",hp->elem[i].fname,hp->elem[i].lname, 					hp->elem[i].telep, hp->elem[i].cellp);
			find=1;
		}
	if(!find)
		printf("\t\n<<Not Find!There is no contact with this name in phone book.>>");	
	
	printf("\nPress a key to search another contact.");
	getc(stdin);
	search(hp);
}

//This function swaps two contact nodes
void swap(struct contact* n1, struct contact* n2)
{
	struct contact temp = *n1;
	*n1 = *n2;
	*n2 = temp;
}

//This function prints the wanted phone book (the wanted heap)
void list(struct max_heap* hp)
{
	int layers = 1;
	int current_layer = 1;
	register int i = 0;
	clrscr();
    	printf("\nPhone Book12<::>All Contacts List");
    	printf("\n--------------------------------------------------------------------------------");
	gotoxy(1,4);
	printf("Row");
	gotoxy(9,4);
	printf("First Name");
	gotoxy(27,4);
	printf("Last Name");
	gotoxy(44,4);
	printf("Telephone");
	gotoxy(60,4);
	printf("Cellphone");
	printf("\n--------------------------------------------------------------------------------");
	
	for(;i < hp->size; i++){
		gotoxy(1,i+6);						 
		printf("%3.3d",i+1);
		gotoxy(9,i+6);
		printf(" %s ", hp->elem[i].fname);
		gotoxy(28,i+6);
		printf("%s",hp->elem[i].lname);
		gotoxy(44,i+6);
		printf("%s",hp->elem[i].telep);
		gotoxy(60,i+6);
		printf("%s",hp->elem[i].cellp);
	}
	
	printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
	printf("\n\t\t    **********End Of list!**********");	
	printf("\n\n\n   ::Press a key to return main page & continue program.|--> %d", hp->size);
	getc(stdin);	
	
}

void help(void)
{
	 clrscr();
	 gotoxy(26,1);
	 printf("--------------------------");
	 gotoxy(26,2);
	 printf("|Welcome To Phone Book12!|");
	 gotoxy(26,3);
	 printf("--------------------------");
	 gotoxy(1,5);
	 printf("PhoneBook12>>[Help & Instructions] \n");
	 printf("--------------------------------------------------------------------------------\n");
	 printf("\nAbout Phonebook12\n *Version: 2.5\n *Valence: 50 Contacts(MAX)\n  *By: Morteza Zakeri (2011-12)");
	 printf("\n\nThis program allows you to have a list of your contacts.\nIt also allows you to have your listing added or deleted from your list.\nOther features of this program include:\nSearch,edit,sort & list.\n\n   ::Press a key to return main page & continue program.|-->");
	 getc(stdin);
}

void clrscr()
{
	system("clear");
}
 
 
void gotoxy(int x, int y)
{
	printf("%c[%d;%df", 0x1B, y, x);
}

