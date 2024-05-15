from sqlalchemy.orm import Session
from . import models, schemas

def get_line(db: Session, line_id: int):
    return db.query(models.Line).filter(models.Line.id == line_id).first()

def get_lines_by_name(db: Session, line_name: str):
    # if contains line_name in the line_name column
    return db.query(models.Line).filter(line_name in models.Line.name).all()



