/* ' '(blank) and 'w' or 'W' control the bird 
 * 'z' and 'Z' stop 
 * gcc -o xxx xxx.c -lcurses
 * */

#include<curses.h>
#include<stdlib.h>
#include<signal.h>
#include<sys/time.h>
#include<unistd.h>

#define CHAR_BIRD '0'
#define CHAR_STONE '*'
#define CHAR_BLANK ' '

typedef struct node {

	int		x, y;		//height and wide

	struct	node *next;
}node, *Node;

Node head, tail;

int bird_x, bird_y;		//bird move

int ticker;				//D DA D DA D DA 

/* function */

void init();			//init all

void init_bird();		//init little bird

void init_draw();		//draw wall

void init_head();

void init_wall();

void drop(int sig);

int	 set_ticker(int n);

/* set time */

int set_ticker(int n_msec)
{
	struct itimerval timeset;

	long   n_sec, n_usec;

	
	n_sec	= n_msec /  1000;
	n_usec	= (n_msec % 1000) * 1000L;

	timeset.it_interval.tv_sec  = n_sec;
	timeset.it_interval.tv_usec = n_usec;

	timeset.it_value.tv_sec  =n_sec;
	timeset.it_value.tv_usec =n_usec;

	return setitimer(ITIMER_REAL, &timeset, NULL);

}

/* received signal */

void drop(int sig)
{
	int		j;
	Node	tmp, p;

	// draw bird
	
	move	(bird_y, bird_x);
	addch	(CHAR_BLANK);
	refresh	();
	bird_y++;
	move	(bird_y, bird_x);
	addch	(CHAR_BIRD);

	if ((char)inch () == CHAR_STONE) {

		set_ticker (0);

		sleep (1);
		endwin ();
		exit (0);
	}

	//first wall out of screen?
	
	p = head->next;

	if (p->x < 0) {
		head->next = p->next;
		free (p);
		tmp = malloc (sizeof (node));

		tmp->x = 99;
		
		do {
			tmp->y = rand () % 16;
		} while (tmp->y < 5);
		tail->next = tmp;
		tmp->next  = NULL;
		tail = tmp;
		ticker -= 10;		//speed up!

		set_ticker (ticker);
	}

	//draw new walls
	
	for (p = head->next; p->next != NULL; p->x--, p = p->next) {

		for (j = 0; j < p->y; j++) {

			move (j, p->x);
			addch (CHAR_BLANK);
			refresh ();
		}
		for (j = p->y + 5; j <= 23; j++) {

			move (j, p->x);
			addch (CHAR_BLANK);
			refresh ();
		}

		if (p->x - 10 >= 0 && p->x < 80) {

			for (j = 0; j < p->y; j++) {
				 
				move (j, p->x - 10);
				addch (CHAR_STONE);
				refresh ();
			}
			for (j = p->y + 5; j <= 23; j++) {

				move (j, p->x - 10);
				addch (CHAR_STONE);
				refresh ();
			}
		}
	}
	tail->x--;
}

int main ()
{
	char ch;

	init ();
	while (1) {

		ch = getch ();
		if (ch == ' ' || ch == 'w' || ch == 'W') {
			move (bird_y, bird_x);		//move x to y
			addch (CHAR_BLANK);			//x --> ' '
			refresh ();
			bird_y--;
			move (bird_y, bird_x);
			addch (CHAR_BIRD);
			refresh();
			if ((char)inch () == CHAR_STONE) {

				set_ticker (0);

				sleep (1);
				endwin ();
				exit (0);
			}
		} 
		else if (ch == 'z' || ch == 'Z') {

			set_ticker (0);

			do {
				ch = getch();
			} while (ch != 'z' && ch != 'Z');
			set_ticker (ticker);
		} 
		else if (ch == 'q' || ch == 'Q') {

			sleep (1);
			endwin ();
			exit (0);
		}
	}

	return 0;
}

/* init all */

void init ()
{
	initscr ();
	cbreak ();
	noecho ();

	curs_set (0);

	srand (time(0));

	signal (SIGALRM, drop);

	init_bird ();
	init_head ();
	init_wall ();
	init_draw ();

	sleep (1);

	ticker = 500;

	set_ticker (ticker);

}

/* little bird */

void init_bird ()
{
	bird_x = 5;

	bird_y = 15;

	move (bird_y, bird_x);
	addch (CHAR_BIRD);
	refresh ();
	
	sleep (1);

}

/* init list of wall */

void init_head ()	// with header
{
	Node tmp;

	tmp = malloc (sizeof(node));

	tmp->next = NULL;

	head = tmp;
	tail = head;
}

void init_wall()
{
	int i;
	Node tmp, p;

	p = head;
	for (i = 19; i <= 99; i += 20) {

		tmp = malloc (sizeof(node));
		tmp->x = i;
		do {
			tmp->y = rand() % 16;
		} while (tmp->y < 5);
		
		p->next = tmp;
		tmp->next = NULL;
		p = tmp;
	}
	tail = p;
}

void init_draw ()
{
	Node p;
	int i, j;

	for (p = head->next; p->next != NULL; p = p->next) {

		for (i = p->x; i > p->x - 10; i--) {

			for (j = 0; j < p->y; j++) {

				move (j, i);
				addch (CHAR_STONE);
				refresh ();
			}

			for (j = p->y + 5; j <= 23; j++) {

				move (j, i);
				addch (CHAR_STONE);
				refresh ();
			}
		}
		sleep (1);
	}
}
