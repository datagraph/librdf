********
librdf++
********

.. image:: https://api.travis-ci.org/datagraph/librdf.svg?branch=master
   :target: https://travis-ci.org/datagraph/librdf
   :alt: Travis CI build status

.. image:: https://scan.coverity.com/projects/3233/badge.svg
   :target: https://scan.coverity.com/projects/3233
   :alt: Coverity Scan build status

Build Prerequisites
===================

* Clang_ (>= 3.0) or GCC_ (>= 4.7)
* Autoconf_ (>= 2.68)
* Automake_ (>= 1.11)
* Libtool_ (>= 2.2)
* pkg-config_ (>= 0.24)

.. _Clang:      http://clang.llvm.org/
.. _GCC:        http://gcc.gnu.org/
.. _Autoconf:   http://www.gnu.org/software/autoconf/
.. _Automake:   http://www.gnu.org/software/automake/
.. _Libtool:    http://www.gnu.org/software/libtool/
.. _pkg-config: http://pkg-config.freedesktop.org/

Build Dependencies
==================

* libraptor2_ (>= 2.0.6)
* libxml2_ (>= 2.7.8), for the TriX format only
* XQilla_ (>= 2.3), for the XSLT pipeline only

.. _libraptor2: http://librdf.org/raptor/
.. _libxml2:    http://www.xmlsoft.org/
.. _XQilla:     http://xqilla.sourceforge.net/

Installation on Unix
====================

::

   $ ./autogen.sh
   $ ./configure                        # on Linux
   $ ./configure --with-stdlib=libc++   # on FreeBSD / Mac OS X
   $ make
   $ sudo make install
   $ sudo ldconfig                      # on Linux

Elsewhere
=========

Find the project at: GitHub_, Bitbucket_, `Travis CI`_, and `Coverity
Scan`_.

.. _GitHub:        http://github.com/datagraph/librdf
.. _Bitbucket:     http://bitbucket.org/datagraph/librdf
.. _Travis CI:     http://travis-ci.org/datagraph/librdf
.. _Coverity Scan: http://scan.coverity.com/projects/3233
