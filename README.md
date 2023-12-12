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



## API Documentation

- https://potkan.hobrasoft.cz/doc/timesheets-server/group__apiurl.html

