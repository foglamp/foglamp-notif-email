==========================================
Fledge email notification delivery plugin
==========================================

Email notification delivery plugin

Example value for plugin 'emailCfg' param:

        {
        "value": {
		"email_from": "dianomic.alerts@gmail.com",
		"email_from_name": "Notification alerts",
		"email_to": "alert.subscriber@dianomic.com",
		"email_to_name": "Notification alert subscriber",
		"server": "smtp.gmail.com",
		"port": 587,
		"subject": "Fledge alert notification",
		"use_ssl_tls": true,
		"username": "dianomic.alerts@gmail.com",
		"password": "password"
                }
        }

Above example is when one wants to use Gmail SMTP server to send out alert emails. Also for Gmail, one needs to enable IMAP in Gmail account Settings->"Forwarding and POP/IMAP"->"Enable IMAP". Also in google account settings at https://myaccount.google.com/lesssecureapps, enable "Allow less secure apps".

Note:

This plugin requires the installation of libcurl-dev apt package and that
is a virtual package provided by 'libcurl4-openssl-dev' among other options.

.. code-block:: console

  $ sudo apt-get install libcurl4-openssl-dev

Build
-----
To build Fledge "email notification" C++ plugin:

.. code-block:: console

  $ ./requirements.sh
  $ mkdir build
  $ cd build
  $ cmake ..
  $ make

- By default the Fledge develop package header files and libraries
  are expected to be located in /usr/include/fledge and /usr/lib/fledge
- If **FLEDGE_ROOT** env var is set and no -D options are set,
  the header files and libraries paths are pulled from the ones under the
  FLEDGE_ROOT directory.
  Please note that you must first run 'make' in the FLEDGE_ROOT directory.

You may also pass one or more of the following options to cmake to override 
this default behaviour:

- **FLEDGE_SRC** sets the path of a Fledge source tree
- **FLEDGE_INCLUDE** sets the path to Fledge header files
- **FLEDGE_LIB sets** the path to Fledge libraries
- **FLEDGE_INSTALL** sets the installation path of Random plugin

NOTE:
 - The **FLEDGE_INCLUDE** option should point to a location where all the Fledge 
   header files have been installed in a single directory.
 - The **FLEDGE_LIB** option should point to a location where all the Fledge
   libraries have been installed in a single directory.
 - 'make install' target is defined only when **FLEDGE_INSTALL** is set

Examples:

- no options

  $ cmake ..

- no options and FLEDGE_ROOT set

  $ export FLEDGE_ROOT=/some_fledge_setup

  $ cmake ..

- set FLEDGE_SRC

  $ cmake -DFLEDGE_SRC=/home/source/develop/Fledge  ..

- set FLEDGE_INCLUDE

  $ cmake -DFLEDGE_INCLUDE=/dev-package/include ..
- set FLEDGE_LIB

  $ cmake -DFLEDGE_LIB=/home/dev/package/lib ..
- set FLEDGE_INSTALL

  $ cmake -DFLEDGE_INSTALL=/home/source/develop/Fledge ..

  $ cmake -DFLEDGE_INSTALL=/usr/local/fledge ..
