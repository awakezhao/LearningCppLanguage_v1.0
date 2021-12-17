
#include <stdio.h>
#include <string.h>
#include <openssl/lhash.h>

#define NAME_LENGTH		32

typedef struct _Person {

    char name[NAME_LENGTH];
    int high;
    char otherInfo[NAME_LENGTH];

} Person;

static int person_cmp(const void *a, const void *b) {

    char *namea = ((Person*)a)->name;
	char *nameb = ((Person*)b)->name;

    return strcmp(namea, nameb);
}

void print_value(void *a) {

    Person *p = (Person*)a;

    printf("name: %s\n", p->name);
	printf("high: %d\n", p->high);
	printf("other info : %s\n", p->otherInfo);
}

int main() {

    _LHASH *h = lh_new(NULL, person_cmp);
    if (h == NULL) {
		printf("err.\n");
		return -1;
	}

    Person p1 = {"King", 170, "xxxx"};
	Person p2 = {"Darren", 175, "xxxx"};
	Person p3 = {"Mark", 170, "xxxx"};
	Person p4 = {"Milo", 170, "xxxx"};

    lh_insert(h, &p1);
	lh_insert(h, &p2);
	lh_insert(h, &p3);
	lh_insert(h, &p4);

    lh_doall(h, print_value);

    printf("\n\n\n------------------------------\n\n\n");
	
	void *data = lh_retrieve(h, (const char *)"King");
	if (data == NULL) {
		return -1;
	}

    print_value(data);

	lh_free(h);
    
    return 0;
}

