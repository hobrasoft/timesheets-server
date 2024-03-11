## Apache

The timesheet-server uses simple built-in http server. The server has no SSL support.
It is better to use Apache http server or other server as a reverse proxy:

- You can easily share IPv4 address with your other sites
- The timesheet server can be run in its own virtual server
- The timesheet server can be run in a controlled DMZ environment

See:
https://www.hobrasoft.cz/en/blog/bravenec/reverse-proxy

## Apache configuration

We setup our proxy server to run only on https protocol. The http on port 80
is used only to obtain SSL certificates from Let's Encrypt authority.

/etc/apache2/sites-enabled/timesheets-hobrasoft-cz.conf

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
        RewriteEngine       On
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



