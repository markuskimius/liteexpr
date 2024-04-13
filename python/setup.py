#!/usr/bin/env python3

import os
import sys
from setuptools import setup

SCRIPTDIR = os.path.abspath(os.path.dirname(__file__))
sys.path.insert(0, os.path.abspath(os.path.join(SCRIPTDIR, "src")))

with open(os.path.join(SCRIPTDIR, "README.md"), encoding="utf-8") as f:
    long_description = f.read()

setup(
                             name = "liteexpr",
                      description = "A light, expression language.",
                          version = "0.0.10",
                          license = "Apache 2.0",
                           author = "Mark Kim",
                     author_email = "markuskimius+py@gmail.com",
                              url = "https://github.com/markuskimius/liteexpr",
                         keywords = [ "expression", "language", "antlr4" ],
                 long_description = long_description,
    long_description_content_type = "text/markdown",
                         packages = [ "liteexpr" ],
                      package_dir = { "" : "src" },
                 install_requires = [ "antlr4-python3-runtime==4.13.1" ],
                   extras_require = {
                        "compile" : [
                            "antlr4-tools>=0.2",
                        ]
                   },
             include_package_data = True,
)

