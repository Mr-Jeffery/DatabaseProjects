# DatabaseProjects
Projects for CS307
## Set Your Own Server
[ssh](https://www.makeuseof.com/tag/beginners-guide-setting-ssh-linux-testing-setup/)\
[firewall](https://www.digitalocean.com/community/tutorials/how-to-set-up-a-firewall-with-ufw-on-ubuntu)\
port 5432 allowed for postgresql, port 3306 allowed for mysql.
## Set Up MySQL Service
### Install MySQL
Based on [official doc](https://ubuntu.com/server/docs/install-and-configure-a-mysql-server)
```bash
sudo apt install mysql-server
```
now check the status of `mysql` by 
```bash
sudo service mysql status
```
we should be able to see mysql service running:
```bash
● mysql.service - MySQL Community Server
     Loaded: loaded (/lib/systemd/system/mysql.service; enabled; vendor preset: enabled)
     Active: active (running) since Fri 2024-04-26 01:55:41 HKT; 19s ago
    Process: 17741 ExecStartPre=/usr/share/mysql/mysql-systemd-start pre (code=exited, status=0/SUCCESS)
   Main PID: 17749 (mysqld)
     Status: "Server is operational"
      Tasks: 38 (limit: 4461)
     Memory: 365.6M
        CPU: 2.184s
     CGroup: /system.slice/mysql.service
             └─17749 /usr/sbin/mysqld
```
Now we need to change the setting so that the server can listen to other IPs
```bash
$ sudo ss -tap | grep mysql

LISTEN 0      151        127.0.0.1:mysql           0.0.0.0:*     users:(("mysqld",pid=17749,fd=23))
LISTEN 0      70         127.0.0.1:33060           0.0.0.0:*     users:(("mysqld",pid=17749,fd=21))
```
We could see that the service only listening to localhost at the moment, we need to change that.
```bash
sudo nvim /etc/mysql/mysql.conf.d/mysqld.cnf
```
`vim` is also available in this case.\
In the `vim` or `nvim`, you can search for text with `/`:
```bash
/bind
```
Now press enter, the cursor should be around a line that looks like this:
```bash
bind-address           = 127.0.0.1
```
Press `i` to enter `INSERT` mode in editor, and change the line to:
```bash
bind-address           = 0.0.0.0
```
To save the change and exit `vim` or `nvim`, press `esc` to exit `INSERT` mode, then input
```bash
:wq
```
Now you should be back to the command line. Remember to restart the service with
```bash
sudo service mysql restart
```
### Create User in MySQL
Enter  `mysql -u root -p` to get into `mysql` service, \
if this does not work, try `sudo mysql`
```sql
ALTER USER 'root'@'localhost' IDENTIFIED WITH mysql_native_password BY 'new_password';
FLUSH PRIVILEGES;
exit;
```
And try again.

Next, we will create user for both local and remote clients:
```sql
create user 'client'@'%' IDENTIFIED BY 'new_password';
GRANT ALL PRIVILEGES ON *.* TO 'client'@'%' WITH GRANT OPTION;
GRANT FILE ON *.* TO 'client'@'%';
```
You can replace `client` with whatever username you like, `'%'` means from any ip, you can also replace it with a specific ip if you like. 
  More specific [here](https://tableplus.com/blog/2018/10/how-to-create-a-superuser-in-mysql.html)

You can only grant part of the privileges for a specific user like following:
```sql
GRANT SELECT ON project2.* TO 'guest'@'%';
REVOKE SELECT on project2.users FROM 'guest'@'%';
FLUSH PRIVILEGES;
```
The above code will allow `'guest'@'%'` to access database `project2` except table `users`.

From the client side, we should install `mysql-client` and header file:
```bash
sudo apt install mysql-client
sudo apt-get install libmysqlcppconn-dev
sudo apt-get install libmysqlclient-dev
```
then access the server with
```bash
mysql -u client -p -h 11.4.51.4
```
Replace `client` with your username, and replace the ip address with the actual ip of your server.

### Enable Local Infile
This has to be enabled on both server and client sides:
#### Server Side
Add the following lines to `/etc/mysql/my.cnf` or `/etc/mysql/my.ini`, then restart the service.
```
[mysqld]
local_infile=1
```
#### Client Side
For normal use, start `mysql` with `mysql --local-infile=1 -u myusername -p`, and the execute the following `sql` queries:
```
USE project1;
CREATE TABLE IF NOT EXISTS rides (rail_user VARCHAR(255), start_station VARCHAR(255), end_station VARCHAR(255), price DOUBLE PRECISION, start_time TIMESTAMP, end_time TIMESTAMP);
LOAD DATA LOCAL INFILE "/tmp/rides.csv" into table `rides`
FIELDS TERMINATED BY ',' 
LINES TERMINATED BY '\n'
IGNORE 1 ROWS;
```
this should upload `rides.csv` to the database.
## Set Up PostgreSQL Service
### Install PostgreSQL
Based on [official doc](https://ubuntu.com/server/docs/install-and-configure-postgresql)
```bash
sudo apt install postgresql
```
Similar process.
