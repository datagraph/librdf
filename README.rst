librdf++
========

.. image:: https://travis-ci.org/datagraph/librdf.png?branch=master
   :target: https://travis-ci.org/datagraph/librdf
   :align: right
   :alt: Travis CI build status

Build Prerequisites
-------------------

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
------------------

* libraptor2_ (>= 2.0.6)
* libxml2_ (>= 2.7.8), for the TriX format only
* Boost.Utility_ (>= 1.48)
* Boost.Test_ (>= 1.48), for ``make check`` unit tests only

.. _libraptor2:    http://librdf.org/raptor/
.. _libxml2:       http://www.xmlsoft.org/
.. _Boost.Utility: http://www.boost.org/libs/utility/
.. _Boost.Test:    http://www.boost.org/libs/test/

Installation on Unix
--------------------

::

   $ ./autogen.sh
   $ ./configure                        # on Linux
   $ ./configure --with-stdlib=libc++   # on FreeBSD / Mac OS X
   $ make
   $ sudo make install
   $ sudo ldconfig                      # on Linux

Elsewhere
---------

Find the project at: GitHub_, Bitbucket_, and `Travis CI`_.

.. _GitHub:      http://github.com/datagraph/librdf
.. _Bitbucket:   http://bitbucket.org/datagraph/librdf
.. _Travis CI:   http://travis-ci.org/datagraph/librdf
