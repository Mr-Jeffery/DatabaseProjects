# Project 1



## Deploy C++ Version
### Set Environment Variables
```bash
cd Cpp
```
Plase add a `bash` scipt named `setenv.sh` in `Cpp` directory that looks like following:
```bash
export DB_NAME= # Your database name
export DB_USER= # Your user Name
export DB_PASSWORD= # Your passoword
export DB_HOST=localhost # Your host address
export DB_PORT=5432 # Port of the database
```
then run the bash with
```bash
source ./setenv.sh
```

### Compile
```bash
chmod +x ./setup.sh
./setup.sh
```

### Execute
```bash
chmod +x ./load.sh
./load.sh
```
