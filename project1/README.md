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
"busName": "中粮创芯公园北（原"
"busName": "角塘西:M198、M309、M521、M568、M597、高峰专线221B出入口城市山海云汇"
"busName": "万科城①,万科城②"
"busName": "A1、A2出入口丨鹏基工业区"
"busName": "福田公交枢纽站公交信息"
```
```json
{
"busInfo": "M165需删除、M361、M195需删除、M219、M276、E6、E7、E23、E33需删除、E34、M277、M304、M305、M359、818、M230、M136需删除、M266、高峰专线17（新增）",
"busName": "双龙地铁站②③"
}
```
```json
{
"busOutInfo": [],
"chukou": "此站暂无数据"
}
```

in `lines.json`:
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
    psql --command="CREATE TABLE tmp_table(name VARCHAR(255) PRIMARY KEY, district VARCHAR(255));" postgresql://$DB_USER:$DB_PASSWORD@$DB_HOST:$DB_PORT/$DB_NAME
    psql --command="\COPY tmp_table FROM '$CSV_PATH/bus_stations.csv'; \
                    INSERT INTO bus_stations (station_id, station_name, line_id) \
                    SELECT station_id, station_name, line_id FROM tmp_table \
                    ON CONFLICT DO NOTHING;" postgresql://$DB_USER:$DB_PASSWORD@$DB_HOST:$DB_PORT/$DB_NAME