/*
 * Verify the a CGC Executable File
 */

#include <err.h>
#include <fcntl.h>
#include <libcgcef.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int	 cgcef_verify_ident(CGCEf *);
int	 cgcef_verify_header(CGCEf *);
int	 cgcef_verify_program_hdrs(CGCEf *);
int	 cgcef_verify_section_hdrs(CGCEf *);

int
main(int argc, char **argv)
{
        CGCEf *cgcef;
	int fd, r = 0;

	if (argc != 2) {
		fprintf(stderr, "Usage: %s filename\n", argv[0]);
		exit(1);
	}
	if ((fd = open(argv[1], O_RDONLY)) < 0)
		err(1, "open(%s)", argv[1]);

        if (cgcef_version(EV_CURRENT) == EV_NONE)
		errx(1, "CGCEF library initialization failed: %s",
		    cgcef_errmsg(-1));


	if ((cgcef = cgcef_begin(fd, CGCEF_C_READ, NULL)) == NULL)
		errx(1, "cgcef_begin(): %s.", cgcef_errmsg(-1));


	r = cgcef_verify_ident(cgcef);
	if (cgcef_verify_header(cgcef))
		r = -1;
	if (cgcef_verify_section_hdrs(cgcef))
		r = -1;
	if (cgcef_verify_program_hdrs(cgcef))
		r = -1;

	cgcef_end(cgcef);
	close(fd);

	if (r) {
		printf("ERROR: not a DECREE executable\n");
		return (1);
	}
	// printf("Executable verified as CGC\n");
	return (0);
}


/*
 * Verify the identification header
 */
int
cgcef_verify_ident(CGCEf *cgcef)
{
	char *ident;
	size_t identlen;
	int r = 0;

	if ((ident = cgcef_getident(cgcef, &identlen)) == NULL) {
		warnx("cgcef_getident(): %s.", cgcef_errmsg(-1));
		return (-1);
	}
	if (identlen < EI_NIDENT) {
		warnx("short CGCEF identifier");
		return (-1);
	}

	if (ident[EI_MAG0] != CGCEFMAG0 ||
	    ident[EI_MAG1] != CGCEFMAG1 ||
	    ident[EI_MAG2] != CGCEFMAG2 ||
	    ident[EI_MAG3] != CGCEFMAG3) {
		warnx("did not identify as a DECREE binary (ident %c%c%c)",
		    ident[EI_MAG1], ident[EI_MAG2], ident[EI_MAG3]);
		return (-1);
	}
	if (ident[EI_CLASS] != CGCEFCLASS32) {
		warnx("did not identify as a 32bit binary");
		r = -1;
	}
	if (ident[EI_DATA] != CGCEFDATA2LSB) {
		warnx("did not identify as a little endian binary");
		r = -1;
	}
	if (ident[EI_VERSION] != EV_CURRENT) {
		warnx("did not identify as a version 1 binary");
		r = -1;
	}
	if (ident[EI_OSABI] != CGCEFOSABI_CGCOS) {
		warnx("did not identify as a DECREE ABI binary");
		r = -1;
	}
	if (ident[EI_ABIVERSION] != 1) {
		warnx("did not identify as a v1 DECREE ABI binary");
		r = -1;
	}

	return (r);
}


/*
 * Verify the program headers
 */
int
cgcef_verify_program_hdrs(CGCEf *cgcef)
{
	CGCEf32_Phdr *hdrs;
	CGCEf32_Ehdr *ehdr;
	int i, r = 0;


	if ((ehdr = cgcef32_getehdr(cgcef)) == NULL) {
		warnx("cgcef32_getehdr(): %s.", cgcef_errmsg(-1));
		return (-1);
	}

	if (ehdr->e_phnum == 0 ||
	    (hdrs = cgcef32_getphdr(cgcef)) == NULL) {
		warnx("No program headers");
		return (-1);
	}
	if (ehdr->e_phentsize != sizeof(CGCEf32_Phdr)) {
		warnx("Invalid program header size");
		r = -1;
	}


	for (i = 0; i < ehdr->e_shnum; i++) {
		switch (hdrs[i].p_type) {
		case PT_NULL:	/*FALLTHROUGH*/
		case PT_LOAD:	/*FALLTHROUGH*/
		case PT_PHDR:	/*FALLTHROUGH*/
		case PT_CGCPOV2:
			break;
		default:
#if 0
			warnx("Invalid program header #%d %xh.", i,
			    hdrs[i].p_type);
			r = -1;
			break;
#else
			warnx("Invalid program header #%d %xh. These will be considered invalid prior to CQE", i, hdrs[i].p_type);
			break;
		case 0x6474e551:
# ifdef notyet
			warnx("PT_GNU_STACK program header was detected. These will be considered invalid prior to CQE.");
# endif
			break;
#endif
		}
	}
	
	return (r);
}


/*
 * Verify the section headers
 */
int
cgcef_verify_section_hdrs(CGCEf *cgcef)
{
	CGCEf32_Ehdr *ehdr;

	if ((ehdr = cgcef32_getehdr(cgcef)) == NULL) {
		warnx("cgcef32_getehdr(): %s.", cgcef_errmsg(-1));
		return (-1);
	}
#ifdef notyet
	if (ehdr->e_shnum) {
		warnx("WARNING: DECREE Executable contained optional section headers");
		/* not a fatal error */
	}
#endif

	return (0);
}


/*
 * Verify the full CGC Executable File header
 */
int
cgcef_verify_header(CGCEf *cgcef)
{
	CGCEf32_Ehdr *ehdr;
	int r = 0;

	if ((ehdr = cgcef32_getehdr(cgcef)) == NULL) {
		warnx("cgcef32_getehdr(): %s.", cgcef_errmsg(-1));
		return (-1);
	}
	if (ehdr->e_ehsize != sizeof(*ehdr)) {
		warnx("invalid header size");
		return (-1);
	}
	if (ehdr->e_type != ET_EXEC) {
		warnx("did not identify as an executable");
		r = -1;
	}
	if (ehdr->e_machine != EM_386) {
		warnx("did not identify as i386");
		r = -1;
	}
	if (ehdr->e_version != EV_CURRENT) {
		warnx("did not identify as a version 1 binary");
		r = -1;
	}
	if (ehdr->e_flags != 0) {
		warnx("contained unsupported flag");
		r = -1;
	}

	return (r);
}
