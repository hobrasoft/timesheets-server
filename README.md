# Description


Server implements simple http API for timesheet information system.

The server uses PostgreSQL database but the plugin system
allows to connect the server to other database or
information system.

Timesheet server is the server part of Hobrasoft's timesheet suite:

- https://github.com/hobrasoft/timesheets-server
- https://github.com/hobrasoft/timesheets-kde
- https://github.com/hobrasoft/timesheets-android

## Build process
    git submodule init
    git submodule update
    qmake
    make

## Configuration
The configuration file can be located (in search order):
- ~/.config/hobrasoft.cz/timesheet-server.conf
- ~/.timesheet-server.conf
- /etc//timesheet-server.conf
- on the command line: -C &lt;path-to-configuration-file&gt;

```
[db]
name     = <username>                            // database name
server   = <database.hobrasoft.cz>               // database server
port     = <port number>                         // default 5432
user     = <user>                                // database user name
password = <password>                            // database password

[httpd]
thread   = true                                     // Starts thread for each http request
address  = [2001:db8::100]:80 [192.168.1.100]:80    // Space separated list of listening addresses and ports
root     = /usr/lib/share/timesheets-server/htdocs  // Path to html content
```

Database plugins

All database functions are built in a single plug-in so
it should be easy to adapt the server to various IT 
systems. Postgresql is a default plugin. To adapt to a new
system, create new database plugin derived from DatabasePlugin class.

With database plugin system it would be easy to connect the
timesheet server to any information system.

## API Documentation

- http://potkan.hobrasoft.cz/doc/timesheets-server/group__apiurl.html

