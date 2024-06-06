# ORM Backend
## Backend Structure
```
.
└── backend
    ├── __init__.py
    ├── crud.py # undone
    ├── database.py
    ├── main.py # undone
    ├── models.py
    └── schemas.py # undone
```
[relational database](https://fastapi.tiangolo.com/tutorial/sql-databases/#__tabbed_2_1)

```sql
GRANT SELECT ON project2.* TO 'guest'@'%';
REVOKE SELECT on project2.users FROM 'guest'@'%';
FLUSH PRIVILEGES;
```
## How to Run
- Install Environment
```bash
conda create -n sql
conda activate sql
pip install -r requirements.txt
```
- Import Data
You need to have a `.env` and `setenv.sh` under `project2` folder, similar to the one in project 1, except you need to add root user in `.env`:
```
DB_NAME=project2
DB_USER=client
DB_PASSWORD=123456
DB_HOST=localhost
DB_PORT=3306

ROOT_USER=root
ROOT_PASSWORD=123456
```
Then you can load the data into MySQL database:
```bash
cd data
/load.sh
```
- Run App
```bash
python launch.py
```
