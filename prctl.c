/*
 * prctl - a tool to set process options
 *
 * Copyright (C) 2000 Hewlett-Packard Co
 * Copyright (C) 2000 Khalid Aziz <khalid_aziz@hp.com>
 * Copyright (C) 2014 Khalid Aziz <khalid@gonehiking.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License Version 2 as 
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <errno.h>
#include <pwd.h>
#include <sys/prctl.h>
#include <string.h>

/* Version */
#define VERSION	"1.7"

/* Shell to fall back on if no other shell can be found */
#define DEFAULT_SHELL	"/bin/bash"

#define PSR_UC		0x08
/* Command line options */

struct option longopts[] = {
	{"unaligned", 1, (int *)0, 'u'},
	{"fpemu", 1, (int *)0, 'f'},
	{"mcekill", 1, (int *)0, 'm'},
	{"version", 0, (int *)0, 'V'},
	{"help", 0, (int *)0, 'h'},
	{0, 0, (int *)0, 0}
};

/* Verbose mode */
int verbose=0;

void
print_version(char const *progname)
{
	printf("%s version %s\n", progname, VERSION);
}

void
usage(char const *progname)
{
	print_version(progname);
	printf("Usage: %s [-v] [-h|--help] [--version]\n", progname);
	printf("             <-q|<options>> [command]\n");
	printf("   Options:\n");
#ifdef __ia64__
	printf("       --unaligned=[silent|signal|always-signal|default]\n");
#else
	printf("       --unaligned=[silent|signal|default]\n");
#endif
	printf("       --fpemu=[silent|signal|default]\n");
	printf("       --mcekill=[early|late|default]\n");
}

int
set_unaligned(int prctl_val)
{
	int alignval, retval;
	int umask;

	/*
	 * Check if we need to display the value or set it.
	 */
	if (prctl_val == -1) {
		if ((retval = prctl(PR_GET_UNALIGN, &alignval)) != -1) {
			printf("%-13s= ", "unaligned");
			switch (alignval) {
			case 0:
				printf("default\n");
				break;

			case PR_UNALIGN_NOPRINT:
				printf("silent\n");
				break;

			case PR_UNALIGN_SIGBUS:
#ifdef __ia64__
				/* Get user mask */
				__asm__ __volatile__ ("mov %0=psr.um;;": "=r"(umask) :: "memory");
				if (umask & PSR_UC)
					printf("always-");
#endif
				printf("signal\n");
				break;
			}
		}
	} else {
		if ((retval = prctl(PR_SET_UNALIGN, prctl_val)) != -1) {
			if (verbose) {
				printf("Set \"unaligned\" to ");
				switch (prctl_val) {
				case 0:
					printf("\"default\"\n");
					break;

				case PR_UNALIGN_NOPRINT:
					printf("\"silent\"\n");
					break;

				case PR_UNALIGN_SIGBUS:
					printf("\"signal\"\n");
					break;
				}
			}
		}
	}
	if (retval == -1) {
		if (errno == EINVAL) {
			/*
			 * print an error message only if we were trying
			 * to set this value.
			 */
			if (prctl_val != -1)
				fprintf(stderr, "ERROR: This platform does not implement set_unaligned prctl feature.\n");
		} else {
			fprintf(stderr, "Failed to %s \"unalign\" value: %s\n",
				((prctl_val==-1)?"get":"set"),strerror(errno));
		}
	}
	return(retval);
}

int
set_fpemu(int prctl_val)
{
	int fpemuval, retval;

	/*
	 * Check if we need to display the value or set it.
	 */
	if (prctl_val == -1) {
		if ((retval = prctl(PR_GET_FPEMU, &fpemuval)) != -1) {
			printf("%-13s= ", "FP emulation");
			switch (fpemuval) {
			case 0:
				printf("default\n");
				break;

			case PR_FPEMU_NOPRINT:
				printf("silent\n");
				break;

			case PR_FPEMU_SIGFPE:
				printf("signal\n");
				break;
			}
		}
	} else {
		if ((retval = prctl(PR_SET_FPEMU, prctl_val)) != -1) {
			if (verbose) {
				printf("Set \"fpemu\" to ");
				switch (prctl_val) {
				case 0:
					printf("\"default\"\n");
					break;

				case PR_FPEMU_NOPRINT:
					printf("\"silent\"\n");
					break;

				case PR_FPEMU_SIGFPE:
					printf("\"signal\"\n");
					break;
				}
			}
		}
	}

	/*
	 * If this was a PR_GET_FPEMU operation and the error is EINVAL,
	 * this platform does not implement PR_GET_FPEMU (It may be
	 * implemented only on IA64 platforms). 
	 */
	if (retval == -1) {
		if (errno == EINVAL) {
			/*
			 * print an error message only if we were trying
			 * to set this value.
			 */
			if (prctl_val != -1)
				fprintf(stderr, "ERROR: This platform does not implement set_fpemu prctl feature.\n");
		} else {
			fprintf(stderr, "Failed to %s \"fpemu\" value: %s\n",
				((prctl_val==-1)?"get":"set"),strerror(errno));
		}
	}
	return(retval);
}

int
set_mcekill(int prctl_val)
{
	int killval, retval;
	int umask;

	/*
	 * Check if we need to display the value or set it.
	 */
	if (prctl_val == -1) {
		killval = prctl(PR_MCE_KILL_GET, 0, 0, 0, 0);
		printf("%-13s= ", "mcekill");
		switch (killval) {
		case PR_MCE_KILL_DEFAULT:
			printf("default\n");
			break;

		case PR_MCE_KILL_EARLY:
			printf("early\n");
			break;

		case PR_MCE_KILL_LATE:
			printf("late\n");
			break;
		}
	} else {
		if ((retval = prctl(PR_MCE_KILL, PR_MCE_KILL_SET,
						prctl_val, 0, 0)) != -1) {
			if (verbose) {
				printf("Set \"mcekill\" to ");
				switch (prctl_val) {
				case PR_MCE_KILL_DEFAULT:
					printf("\"default\"\n");
					break;

				case PR_MCE_KILL_EARLY:
					printf("\"early\"\n");
					break;

				case PR_MCE_KILL_LATE:
					printf("\"late\"\n");
					break;
				}
			}
		}
	}
	if (retval == -1) {
		if (errno == EINVAL) {
			/*
			 * print an error message only if we were trying
			 * to set this value.
			 */
			if (prctl_val != -1)
				fprintf(stderr, "ERROR: This platform does not implement set_mce_kill prctl feature.\n");
		} else {
			fprintf(stderr, "Failed to %s \"mcekill\" value: %s\n",
				((prctl_val==-1)?"get":"set"),strerror(errno));
		}
	}
	return(retval);
}

int
main(int argc, char **argv)
{
	int opt, cmd_start;
	char const *progname;
	char const *shellname;
	int unaligned_val = -99;
	int fpemu_val = -99;
	int mcekill_val = -99;
	int always_signal = 0;
	int display = 0;
	int display_all = 0;
	int umask;

	if ((progname = strrchr(argv[0], '/')) != NULL) {
		progname++;
	} else {
		progname = argv[0];
	}

	/*
	 * Parse command line options
	 */
	if (argc == 1) {
		usage(progname);
		exit(1);
	}
	opterr = 0;
	while ((opt = getopt_long(argc, argv, "+qhv", longopts,
					(int *) NULL)) != -1) {
		switch (opt) {
		case 'u':
			if (strcmp(optarg, "silent") == 0) {
				unaligned_val = PR_UNALIGN_NOPRINT;
			} else if (strcmp(optarg, "signal") == 0) {
				unaligned_val = PR_UNALIGN_SIGBUS;
#ifdef __ia64__
			} else if (strcmp(optarg, "always-signal") == 0) {
				unaligned_val = PR_UNALIGN_SIGBUS;
				always_signal = 1;
#endif
			} else if (strcmp(optarg, "default") == 0) {
				unaligned_val = 0;
			} else if (optarg[0] == 0) {
				unaligned_val = -1;
				display = 1;
			} else {
				usage(progname);
				exit(1);
			}
			break;

		case 'f':
			if (strcmp(optarg, "silent") == 0) {
				fpemu_val = PR_FPEMU_NOPRINT;
			} else if (strcmp(optarg, "signal") == 0) {
				fpemu_val = PR_FPEMU_SIGFPE;
			} else if (strcmp(optarg, "default") == 0) {
				fpemu_val = 0;
			} else if (optarg[0] == 0) {
				fpemu_val = -1;
				display = 1;
			} else {
				usage(progname);
				exit(1);
			}
			break;

		case 'm':
			if (strcmp(optarg, "early") == 0) {
				mcekill_val = PR_MCE_KILL_EARLY;
			} else if (strcmp(optarg, "late") == 0) {
				mcekill_val = PR_MCE_KILL_LATE;
			} else if (strcmp(optarg, "default") == 0) {
				mcekill_val = PR_MCE_KILL_DEFAULT;
			} else if (optarg[0] == 0) {
				mcekill_val = -1;
				display = 1;
			} else {
				usage(progname);
				exit(1);
			}
			break;

		case 'q':
			display_all = 1;
			display = 1;
			break;

		case 'h':
			usage(progname);
			exit(0);
			break;

		case 'v':
			verbose = 1;
			break;

		case 'V':
			print_version(progname);
			exit(0);
			break;

		case '?':
			fprintf(stderr, "%s: invalid option - %c\n", 
					progname, optopt);
			exit(1);
			break;
		}
	}

	/*
	 * Is there a command on the command line? If there is, there 
	 * there should be atleast one other option on the line and it
	 * should be something other than -v.
	 */
	cmd_start = optind;
	if ((cmd_start == 1) || ((cmd_start == 2) && (verbose))) {
		usage(progname);
		exit(1);
	}

	/*
	 * If -q option was given, we will ignore all other options
	 * and simply display current settings.
	 */
	if (display_all) {
		printf("Current settings for supported prctl operations:\n");
		set_unaligned(-1);
		set_fpemu(-1);
		set_mcekill(-1);
		printf("\n");
		exit(0);
	} else {
		/*
		 * Now set the correct prctl options if needed
		 */
		if (unaligned_val != -99) {
			if (set_unaligned(unaligned_val) == -1) {
				exit(1);
			}
#ifdef __ia64__
			if (always_signal) {
				/* Set alignment check in user mask */
				__asm__ __volatile__ ("sum psr.ac;;"::: "memory");
			}
#endif
		}
		if (fpemu_val != -99) {
			if (set_fpemu(fpemu_val) == -1) {
				exit(1);
			}
		}
		if (mcekill_val != -99) {
			if (set_mcekill(mcekill_val) == -1) {
				exit(1);
			}
		}
	}
	
	/*
	 * Check if we need to run a command or start a new shell.
	 *
	 * If the command line options asked to display any of the values
	 * and a command was not given on the command line, we will not
	 * start a shell.
	 *
	 */
	if (argv[cmd_start] == 0) {
		if (display) {
			exit(0);
		}

		printf("Starting a shell\n");
		shellname = getenv("SHELL");

		/*
		 * Make sure SHELL environment variable is not unset. If it
		 * is, start user login shell or bash.
		 */
		if (shellname == NULL) {
			struct passwd *pwd_entry;

			pwd_entry = getpwuid(getuid());
			if (pwd_entry != NULL && pwd_entry->pw_shell != NULL) {
				shellname = pwd_entry->pw_shell;
			} else {
				shellname = DEFAULT_SHELL;
			}
		}

		/*
		 * Now exec the shell
		 */
		if (execlp(shellname, shellname, (char *) 0) == -1) {
			fprintf(stderr, "Failed to exec the shell: %s\n",
				strerror(errno));
			exit(1);
		}
	} else {
		/*
		 * Now exec the user command 
		 */
		if (execvp(argv[cmd_start], argv+cmd_start) == -1) {
			fprintf(stderr, "Failed to exec command \"%s\": %s\n",
				argv[cmd_start], strerror(errno));
			exit(1);
		}
	}

	exit(0);
}
