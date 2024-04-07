install library:
```bash
pip install "psycopg[c]" 
```
import sql:
```python
cur = conn.cursor()
cur.execute(open("queries.sql", "r").read())
```
[Why use psycopg3](https://github.com/psycopg/psycopg/discussions/492)
[With `CPython`](https://pypi.org/project/psycopg-c/)
[basic operation](https://www.cnblogs.com/zszxz/p/12222201.html)
[Prepared Statement](https://www.psycopg.org/psycopg3/docs/advanced/prepare.html#prepared-statements)