# Project 1
Plase add a `bash` scipt named `setenv.sh` in `project1` directory that looks like following:
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
## Set Environment Variables
```bash
cd Cpp
```
## Deploy C++ Version
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

## Deploy Python Version
### Install Dependencies
```bash
source ../setenv.sh
conda create -n sql -y
conda activate sql
pip install -r requirements.txt
```

### Execute
TODO

## Deploy Java Version
TODO