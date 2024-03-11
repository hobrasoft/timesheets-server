
## Building from source

Install some development packages:

> apt-get install qtbase5-dev g++ git make fakeroot


Clone the repository and compile it:

> git clone git@github.com:hobrasoft/timesheets-server.git    
> cd timesheet-server    
> git submodule init    
> git submodule update    
> qmake    
> make

Create deb package:

> ./makedeb

Now you should have the timesheet-server-(version)-(architecture).deb package in your working directory.

## Package installation

The package needs two special packages:
> libqt5network5    
> libqt5sql5-psql

Install deb package:

> dpkg -i timesheet-server-(version)-(architecture).deb

If dependency packages are not installed, fix the installation with:

> apt-get install -f

## Database

The server uses postgresql database. The database server is not in package dependencies, because
it is common to run database server on different machine. Usualy you will want to run both the
timesheet-server and database server on the same machine. 

In such case, you should install postgresql:

> apt-get install postgresql

Create database user and database:

> su - postgres    
> createuser -P timesheet    
> createdb -o timesheet timesheet    
> exit

Usualy the local connections to database are allowed in debian. If you use remote connection,
fix your */etc/postgresql/15/main/pg_hba.conf* appropriately.


## Configuration file

Check the configuration file:

/etc/timesheet-server.conf

    [db]
    plugin   = postgres
    name     = timesheet
    server   = localhost
    port     = 5432
    user     = timesheet
    password = ABCD123      # user timesheet's password

    [httpd]
    threads = true
    maxRequestSize = 99999999
    address = [2a0a:1c01:0:1405::100]:8039
    root = /home/tycho/SRC/timesheets-server/htdocs

    [server]
    name = Timesheets
    description = Experimental timesheets server

    [log]
    all = true


## Running server

Start the server:

> systemctl start timesheet-server

At this moment, the server should connect to database and start listening on
http port 8039 (dependins on configuration). 

We recommend to setup Apache reverse proxy to make the timesheet server available
on internet, see: 

- https://www.hobrasoft.cz/en/blog/bravenec/reverse-proxy 
- or INSTALLATION-apache.md




