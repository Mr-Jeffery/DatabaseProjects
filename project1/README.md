# Project 1

## Set Environment Variables

Plase add a `bash` scipt named `setenv.sh` in current directory that looks like following:
```bash
export DB_NAME=project1 # Your database name
export DB_USER=postgres # Your user Name
export DB_PASSWORD=123456 # Your passoword
export DB_HOST=localhost # Your host address
export DB_PORT=5432 # Port of the database
```

## Deploy C++ Version
```bash
cd Cpp
chmod +x ./load.sh
./load.sh
```
