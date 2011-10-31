#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <Python.h>
#include "woff/woff.h"

static void die(const char* msg) {
  fprintf(stderr, "# fatal error: %s\n", msg);
  exit(2);
}

const uint8_t* readFile(const char* name, uint32_t* len) {
  FILE* inFile = fopen(name, "rb");
  if (!inFile) {
    char buf[200];
    sprintf(buf, "unable to open file %s", name);
    die(buf);
  }
  if (fseek(inFile, 0, SEEK_END) != 0) {
    die("seek failure");
  }
  *len = ftell(inFile);
  if (fseek(inFile, 0, SEEK_SET) != 0) {
    die("seek failure");
  }
  uint8_t* data = (uint8_t*) malloc(*len);
  if (!data) {
    die("malloc failure");
  }
  if (fread(data, 1, *len, inFile) != *len) {
    die("file read failure");
  }
  fclose(inFile);
  return data;
}

static void reportErr(uint32_t status) {
  woffPrintStatus(stderr, status, "### ");
  exit(2);
}

static PyObject* convert(PyObject* self, PyObject* args) {
  
  const char* sourceFilename;
  const char* outFilename;
  if (!PyArg_ParseTuple(args, "ss", &sourceFilename, &outFilename)) {
    return NULL;
  }
  
  unsigned int maj = 0, min = 0;
  uint32_t status = eWOFF_ok;
  
  uint32_t sfntLen;
  const uint8_t* sfntData = readFile(sourceFilename, &sfntLen);
  
  uint32_t woffLen;
  const uint8_t* woffData = woffEncode(sfntData, sfntLen, maj, min, &woffLen, &status);
  free((void*)sfntData);
  if (WOFF_FAILURE(status)) {
    reportErr(status);
  }
  
  if (WOFF_WARNING(status)) {
    woffPrintStatus(stderr, status, "### ");
  }
  
  if (woffData) {
    FILE* outFile = fopen(outFilename, "wb");
    if (!outFile) {
      die("unable to open output file");
    }
    if (fwrite(woffData, 1, woffLen, outFile) != woffLen) {
      die("file write failure");
    }
    fclose(outFile);
    free((void*) woffData);
  } else {
    die("unable to create WOFF data");
  }

  return Py_BuildValue("i", 1);
}

static PyMethodDef woffMethods[] = {
    {"convert", convert, METH_VARARGS, "convert ttf / otf to woff"},
    {NULL, NULL, 0, NULL}
};

PyMODINIT_FUNC initwoff(void) {
    (void) Py_InitModule("woff", woffMethods);
}
