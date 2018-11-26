// File author is Ítalo Lima Marconato Matias
//
// Created on November 19 of 2018, at 12:17 BRT
// Last edited on November 25 of 2018, at 14:40 BRT

#include <ir.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

void *read_file(char *fname) {
	FILE *file = fopen(fname, "rb");																	// Try to open the file
	
	if (file == NULL) {
		return NULL;																					// Failed...
	}
	
	fseek(file, 0, SEEK_END);																			// Go to the end of the file (to get the length)
	
	long length = ftell(file);																			// Get the current position!
	void *buf = malloc(length);																			// Try to alloc our buffer
	
	if (buf == NULL) {
		fclose(file);
		return NULL;
	}
	
	rewind(file);																						// Rewind it back to the beginning
	
	if (!fread(buf, length, 1, file)) {																	// Try to read it!
		free(buf);																						// Failed...
		fclose(file);
		return NULL;
	}
	
	fclose(file);																						// Close the file
	
	return buf;																							// Return the buffer!
}

int main(int argc, char **argv) {
	char *input = NULL;
	
	if (argc < 2) {																						// Check if we have any arguments
		printf("Usage: %s [options] file\r\n", argv[0]);												// We don't have any, just print the usage
		return 1;
	}
	
	for (int i = 1; i < argc; i++) {																	// Let's parse the arguments!
		if ((!strcmp(argv[i], "-h")) || (!strcmp(argv[i], "--help"))) {									// Help
			printf("Usage: %s [options] file...\r\n", argv[0]);
			printf("Options:\n");
			printf("    -h or --help          Show this help dialog\r\n");
			printf("    -v or --version       Show the version of this program\r\n");
			return 0;
		} else if ((!strcmp(argv[i], "-v")) || (!strcmp(argv[i], "--version"))) {						// Version
			printf("CHicago Operating System Project\r\n");
			printf("CHicago Intermediate Language Compiler Version 1.0\r\n");
			return 0;
		} else {
			if (input == NULL) {																		// It's the input?
				input = argv[i];																		// Yes!
			} else {
				printf("Error: unrecognized option: '%s'\r\n", argv[i]);								// No, so it's a unrecognized option
				return 1;
			}
		}
	}
	
	if (input == NULL) {																				// We have any input file?
		printf("Error: expected input file\r\n");														// No...
		return 1;
	}
	
	char *code = read_file(input);																		// Try to read the source code
	
	if (code == NULL) {
		printf("Error: couldn't open '%s'\r\n", input);													// Failed to read it...
		return 1;
	}
	
	ir_module_t *module = ir_module_from_text(code);													// Generate the module from the source code
	
	free(code);																							// Free the source code, we don't need it anymore
	
	if (module == NULL) {
		printf("Error: couldn't parse '%s'\r\n", input);												// Failed to parse...
		return 1;
	}
	
	ir_free_module(module);																				// Free the module
	
	return 0;
}
