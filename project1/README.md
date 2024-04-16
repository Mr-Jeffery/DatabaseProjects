# Project 1
## Project Info
[【腾讯文档】Project1 Introduction](https://docs.qq.com/doc/DVmJ4Q2dNSGJnSXNv)
## Project Report Link
[Latex report](https://sharelatex.cra.ac.cn/project/66193f1fafc1ee76074e378e)--In Progress
##  E-R Diagram
[ER diagram](https://lucid.app/lucidchart/381208ad-beeb-4a6d-9209-00c39bd6f42d/edit?view_items=LPrsYmKL4.7G&invitationId=inv_45428aec-4b47-4305-88bb-a0c90220279f)--In progress

## Data Corruption Found
in `stations.json`:
```json
"busName": "1、尚景欣园:802、低碳城假日专线、M276、M386、M556；2、盛平地铁站"
```
```json
"busName": "万科城①,万科城②"
```
```json
"Universiade",
"Universiade Center",
"  Longcheng Park",
"Huanggekeng  station",
"Yuyuan  station",
"Huilongpu  station\n",
```
in `rides.json`:
```json
"start_station": "Huilongpu  station\n",
```
```json
"end_station": "Dongjiang Column Memorial \nHall Station",
```
## Relational Database Design
TODO

## Data Import
<!-- psql -U postgres -d project1 -p 5432 -h localhost -->
### Set Environment Variables
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

### Deploy Python Version
#### Install Dependencies
```bash
conda create -n sql -y
conda activate sql
pip install -r requirements.txt
```

#### Execute
TODO

### Deploy C++ Version
For windows version, replace `.sh` with `.ps1`
```bash
cd Cpp
```

#### Compile
```bash
chmod +x ./setup.sh
./setup.sh
```

#### Execute
```bash
chmod +x ./load.sh
./load.sh
```

### Deploy Java Version
TODO