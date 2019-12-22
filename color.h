void change_color(int); //function to change color of text

/*
 * 0  -> regular color (reset)
 * 1  -> red
 * 2  -> bold red
 * 3  -> green
 * 4  -> bold green
 * 5  -> yellow
 * 6  -> bold yellow
 * 7  -> blue
 * 8  -> bold blue
 * 9  -> magenta
 * 10 -> bold magenta
 * 11 -> cyan
 * 12 -> bold cyan
 * 13 -> bg black
 * 14 -> bg red
 * 15 -> bg green
 * 16 -> bg yellow
 * 17 -> bg blue
 * 18 -> bg magenta
 * 19 -> bg cyan
 * 20 -> bg light gray
*/

void change_color(int option){
	if(option == 0) printf("\033[0m");
	else if(option < 13) printf("\033[%d;3%dm", (option % 2 == 0 ? 1 : 0), (option+1)/2);
	else printf("\033[4%dm", option-13);

	return;
}
