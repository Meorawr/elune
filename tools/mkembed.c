#include <stdlib.h>
#include <stdio.h>

static const char *USAGE_TEXT = \
    "Usage: %s <symbol> <source> <output>\n\n"
    "Creates an 'output.c' file with the contents of 'source'.\n";


int main(int argc, char **argv) {
  int retcode = EXIT_FAILURE;

  if (argc < 3) {
    fprintf(stderr, USAGE_TEXT, argv[0]);
    goto exit_usage_error;
  }

  const char *symbol = argv[1];
  const char *sourcename = argv[2];
  const char *outputname = argv[3];

  FILE *sourcefile = fopen(sourcename, "r");

  if (!sourcefile) {
    perror(sourcename);
    goto exit_source_error;
  }

  FILE *outputfile = fopen(outputname, "w");

  if (!outputfile) {
    perror(outputname);
    goto exit_output_error;
  }

  fprintf(outputfile, "#include <stdlib.h>\n\n");
  fprintf(outputfile, "const char %s_data[] = {\n\t", symbol);

  {
    size_t bytesread = 0;
    size_t linebytes = 0;
    unsigned char buffer[256];

    do {
      bytesread = fread(buffer, 1, sizeof(buffer), sourcefile);

      for (size_t i = 0; i < bytesread; ++i) {
        fprintf(outputfile, "0x%02hhx, ", buffer[i]);

        if ((++linebytes) == 20) {
          fprintf(outputfile, "\n\t");
          linebytes = 0;
        }
      }
    } while (bytesread > 0);

    if (linebytes > 0) {
      fprintf(outputfile, "\n");
    }
  }

  fprintf(outputfile, "};\n\n");
  fprintf(outputfile, "const size_t %s_size = sizeof(%s_data);\n", symbol, symbol);

  retcode = EXIT_SUCCESS;

  fclose(outputfile);
exit_output_error:
  fclose(sourcefile);
exit_source_error:
  ;
exit_usage_error:
  ;

  return retcode;
}
