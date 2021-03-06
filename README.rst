==========================================
FogLAMP email notification delivery plugin
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
		"subject": "Foglamp alert notification",
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
To build FogLAMP "email notification" C++ plugin:

.. code-block:: console

  $ ./requirements.sh
  $ mkdir build
  $ cd build
  $ cmake ..
  $ make

- By default the FogLAMP develop package header files and libraries
  are expected to be located in /usr/include/foglamp and /usr/lib/foglamp
- If **FOGLAMP_ROOT** env var is set and no -D options are set,
  the header files and libraries paths are pulled from the ones under the
  FOGLAMP_ROOT directory.
  Please note that you must first run 'make' in the FOGLAMP_ROOT directory.

You may also pass one or more of the following options to cmake to override 
this default behaviour:

- **FOGLAMP_SRC** sets the path of a FogLAMP source tree
- **FOGLAMP_INCLUDE** sets the path to FogLAMP header files
- **FOGLAMP_LIB sets** the path to FogLAMP libraries
- **FOGLAMP_INSTALL** sets the installation path of Random plugin

NOTE:
 - The **FOGLAMP_INCLUDE** option should point to a location where all the FogLAMP 
   header files have been installed in a single directory.
 - The **FOGLAMP_LIB** option should point to a location where all the FogLAMP
   libraries have been installed in a single directory.
 - 'make install' target is defined only when **FOGLAMP_INSTALL** is set

Examples:

- no options

  $ cmake ..

- no options and FOGLAMP_ROOT set

  $ export FOGLAMP_ROOT=/some_foglamp_setup

  $ cmake ..

- set FOGLAMP_SRC

  $ cmake -DFOGLAMP_SRC=/home/source/develop/FogLAMP  ..

- set FOGLAMP_INCLUDE

  $ cmake -DFOGLAMP_INCLUDE=/dev-package/include ..
- set FOGLAMP_LIB

  $ cmake -DFOGLAMP_LIB=/home/dev/package/lib ..
- set FOGLAMP_INSTALL

  $ cmake -DFOGLAMP_INSTALL=/home/source/develop/FogLAMP ..

  $ cmake -DFOGLAMP_INSTALL=/usr/local/foglamp ..
