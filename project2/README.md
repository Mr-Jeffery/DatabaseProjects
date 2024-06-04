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