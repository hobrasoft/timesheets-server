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
name     = &lt:username&gt;                         // database name
server   = &lt:database.hobrasoft.cz&gt;            // database server
port     = &lt:port number&gt;                      // default 5432
user     = &lt;user&gt;                             // database user name
password = &lt:password&gt;                         // database password

[httpd]
thread   = true                                     // Starts thread for each http request
address  = [2001:db8::100]:80 [192.168.1.100]:80    // Space separated list of listening addresses and ports
root     = /usr/lib/share/timesheets-server/htdocs  // Path to html content
```


## API Documentation

- http://potkan.hobrasoft.cz/doc/timesheets-server/group__apiurl.html

