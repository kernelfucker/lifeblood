/* See LICENSE file for license details */
/* lifeblood - attempts passwds on luks-encrypted volumes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

#define version "0.1"

int try_passwd(const char *volume, const char *passwd){
	int p[2];
	int n;
	if(pipe(p) == -1) return -1;
	pid_t pid = fork();
	if(pid == 0){
		close(p[1]);
		dup2(p[0], STDIN_FILENO);
		n = open("/dev/null", O_WRONLY);
		dup2(n, STDERR_FILENO);
		close(n);
		execlp("cryptsetup", "cryptsetup", "open", "--test-passphrase", volume, "--key-file=-", NULL);
		exit(1);
	} else if(pid > 0){
		close(p[0]);
		write(p[1], passwd, strlen(passwd));
		close(p[1]);
		int status;
		waitpid(pid, &status, 0);
		return WEXITSTATUS(status);
	}

	return -1;
}

void show_version(){
	printf("lifeblood-%s\n", version);
}

void help(){
	printf("usage: lifeblood [options]..\n");
	printf("options:\n");
	printf("  -f	passwd file\n");
	printf("  -l	luks volume\n");
	printf("  -o	save true passwd to file\n");
	printf("  -v	show version information\n");
	printf("  -h	display this\n");
}

int main(int argc, char *argv[]){
	char *passwd_file = NULL;
	char *volume = NULL;
	char *output = NULL;
	int opt;
	while((opt = getopt(argc, argv, "hvf:l:o:")) != -1){
		switch(opt){
			case 'h': help(); return 0;
			case 'v': show_version(); return 0;
			case 'f': passwd_file = optarg; break;
			case 'l': volume = optarg; break;
			case 'o': output = optarg; break;
			default: fprintf(stderr, "invalid option\n");

			return 1;
		}
	}

	if(!passwd_file || !volume){
		fprintf(stderr, "lifeblood: -f and -l are required\n");
		help();
		return 1;
	}

	FILE *f = fopen(passwd_file, "r");
	if(!f){
		perror("error opening passwd file");
		return 1;
	}

	char passwd[256];
	while(fgets(passwd, sizeof(passwd), f)){
		passwd[strcspn(passwd, "\n")] = '\0';
		int r = try_passwd(volume, passwd);
		if(r == 0){
			printf("true: %s\n", passwd);
			if(output){
				FILE *o = fopen(output, "w");
				if(o) fprintf(o, "%s\n", passwd);
				fclose(o);
			}

			fclose(f);
			return 0;
		} else {
			printf("false: %s\n", passwd);
		}
	}

	fclose(f);
	return 1;
}
