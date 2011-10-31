# -*- coding: utf-8 -*-

import os
import subprocess

def convert(source, dest):
    basedir = os.path.dirname(os.path.abspath(__file__))
    jar = os.path.join(basedir, "batik-ttf2svg.jar")
    proc = subprocess.Popen(["java", "-jar", jar, source,
                             "-o", dest, "-id", "webfont"])
    proc.wait()
    return 1
