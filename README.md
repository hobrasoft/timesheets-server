# Description

Server implements simple http API for timesheet information system.

The server uses PostgreSQL database but the plugin system
allows to connect the server to other database or
information system.

Timesheet server is the server part of Hobrasoft's timesheet suite:

- https://github.com/hobrasoft/timesheets-server
- https://github.com/hobrasoft/timesheets-kde
- https://github.com/hobrasoft/timesheets-android

## Database plugins

All database functions are built in a single plug-in so
it should be easy to adapt the server to various IT 
systems. Postgresql is a default plugin. To adapt to a new
system, create new database plugin derived from DatabasePlugin class.

With database plugin system it would be easy to connect the
timesheet server to any information system.

## API Documentation

- http://potkan.hobrasoft.cz/doc/timesheets-server/group__apiurl.html



# Build proces (Debian)

We use the Debian to run the timesheet server.

Before you can build the server from the source, you have to install 
some development packages:

    apt-get install qtbase5-dev g++ git make fakeroot

These packages are needed only to compile the server, there is no
need to have them installed in production environment.


Clone the repository and compile it:

    git submodule init
    git submodule update
    qmake
    make

Create deb package:

    ./makedeb

Now you should have the timesheet-server-(version)-(architecture).deb package in your working directory.

## Package installation

The package needs two special packages:

    libqt5network5
    libqt5sql5-psql

Install deb package:

    dpkg -i timesheet-server-(version)-(architecture).deb

If dependency packages are not installed, fix the installation with:

    apt-get install -f

## Database

The server uses postgresql database. The database server is not in package dependencies, because
it is common to run database server on different machine. Usualy you will want to run both the
timesheet-server and database server on the same machine.

In such case, you should install postgresql:

    apt-get install postgresql

Create database user and database:

    su - postgres
    createuser -P timesheet
    createdb -o timesheet timesheet
    exit

Usualy the local connections to database are allowed in Debian. If you use remote connection,
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
    address = [2001:db8::99]:8039
    root = /usr/lib/share/timesheets-server/htdocs

    [server]
    name = Timesheets
    description = Experimental timesheets server

    [log]
    all = true

The configuration file can be located in three different locations (in search order):
- ~/.config/hobrasoft.cz/timesheet-server.conf
- ~/.timesheet-server.conf
- /etc//timesheet-server.conf
- on the command line: -C &lt;path-to-configuration-file&gt;


## Running server

Start the server:

    systemctl start timesheet-server

At this moment, the server should connect to database and start listening on
http port 8039 (dependins on configuration).

On the localhost, the server should be available at:

> http://[::1]:8039/index.html


Or, remotely, on the listening addresss:

> http://[2001:db8::99]:8039/index.html


# Running behind a reverse proxy

Although the timesheet-server uses simple built-in http server, it is
practical to use Apache http server or other server as a reverse proxy:

- You can use SSL in a standard way.
- You can easily share IPv4 address with your other sites.
- The timesheet server can be run in its own virtual server.
- The timesheet server can be run in a controlled DMZ environment.

See:
https://www.hobrasoft.cz/en/blog/bravenec/reverse-proxy

## Apache configuration

We setup our proxy server to run only on https protocol. The http on port 80
is used only to obtain SSL certificates from Let's Encrypt authority.

/etc/apache2/sites-enabled/timesheets-yourdomain-cz.conf

    <VirtualHost *:80>
        ServerName timesheets.yourdomain.com
        RedirectMatch permanent ^/(?!(\.well-known)) https://timesheets.yourdomain.com/
        DocumentRoot /var/www/localhost/well-known
        <Directory /var/www/localhost/well-known>
            Order allow,deny
            Allow from all
            AllowOverride All
            Require all granted
        </Directory>
    </VirtualHost>

    <VirtualHost *:443>

        ServerName          timesheets.yourdomain.com
        ServerAdmin         info@yourdomain.com
        ProxyRequests       off
      # Use your own IP address of the timesheet-server
        ProxyPass           / http://[2001:db8::99]:8039/
        ProxyPassReverse    / http://[2001:db8::99]:8039/

        SSLEngine on
        SSLCipherSuite          HIGH:!aNULL:!MD5
        SSLCertificateFile      /home/certificates/.acme.sh/timesheets.yourdomain.com/timesheets.yourdomain.com.cer
        SSLCertificateKeyFile   /home/certificates/.acme.sh/timesheets.yourdomain.com/timesheets.yourdomain.com.key
        SSLCertificateChainFile /home/certificates/.acme.sh/timesheets.yourdomain.com/fullchain.cer

    </VirtualHost>

# Clients

You can connect to http but the http is not working properly, at the moment. 


