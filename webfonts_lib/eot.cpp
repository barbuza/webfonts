/* Trivial utility to create EOT files on Linux */

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <limits.h>
#include <string.h>

#include <vector>

#ifndef _MSC_VER
# include <stdint.h>
#else
typedef unsigned char uint8_t;
#endif

#if defined(_MSC_VER) || defined(__MINGW32__)
# include <io.h>
# include <fcntl.h> 
# ifndef _O_BINARY
#  define _O_BINARY 0x8000
# endif
# ifndef _O_TEXT
#  define _O_TEXT 0x4000
# endif
#endif

#include "eot/OpenTypeUtilities.h"

#ifndef SIZE_MAX
# define SIZE_MAX UINT_MAX
#endif

#include <Python.h>

using std::vector;

static void die(const char* msg) {
  fprintf(stderr, "# fatal error: %s\n", msg);
  exit(2);
}

static PyObject* convert(PyObject* self, PyObject* args) {
  const char* sourceFilename;
  const char* outFilename;
  if (!PyArg_ParseTuple(args, "ss", &sourceFilename, &outFilename)) {
    return NULL;
  }
  
  const size_t kFontInitSize = 8192;
  vector<uint8_t> eotHeader(512);
  size_t overlayDst = 0;
  size_t overlaySrc = 0;
  size_t overlayLength = 0;
  size_t fontSize = 0;
  size_t fontOff = 0;
  FILE *input;
  unsigned char *fontData;

#if defined(_MSC_VER) || defined(__MINGW32__)
  setmode(_fileno(stdin), _O_BINARY);
  setmode(_fileno(stdout), _O_BINARY);
  setmode(_fileno(stderr), _O_TEXT);
#endif
  
  input = fopen(sourceFilename, "rb");
  if (input == NULL) {
    die("could not open input file");
  }
  
  if ((fontData = (unsigned char *) malloc(fontSize = kFontInitSize)) == NULL) {
    die("allocation failure");
  }
  
  do {
    size_t ret = fread(fontData + fontOff, 1, fontSize - fontOff, input);
    if (ret && fontSize <= SIZE_MAX / 2) {
      fontOff += ret;
      if ((fontData = (unsigned char *) realloc(fontData, fontSize *= 2)) == NULL) {
        die("allocation failure");
      }
    } else if (ret) {
      die("too much data");
    } else {
      fontData = (unsigned char *) realloc(fontData, fontSize = fontOff);
      break;
    }
  } while (true);
  
  if (getEOTHeader(fontData, fontSize, eotHeader, overlayDst, overlaySrc, overlayLength)) {
    FILE* outFile = fopen(outFilename, "wb");
    if (!outFile) {
      die("unable to open output file");
    }
    fwrite(&eotHeader[0], eotHeader.size(), 1, outFile);
    fwrite(fontData, fontSize, 1, outFile);
    fclose(outFile);
    return Py_BuildValue("i", 1);
  } else {
    die("unknown error parsing input font");
  }
}

static PyMethodDef eotMethods[] = {
    {"convert", convert, METH_VARARGS, "convert ttf / otf to eot"},
    {NULL, NULL, 0, NULL}
};

PyMODINIT_FUNC initeot(void) {
    (void) Py_InitModule("eot", eotMethods);
}

