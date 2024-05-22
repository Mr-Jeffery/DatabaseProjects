import gradio as gr
from backend import crud
from backend import database
from backend import schemas

def add_station(name, location):
    db = database.SessionLocal()
    new_station = schemas.StationCreate(name=name, location=location)
    station = crud.create_station(db, new_station)
    db.close()
    return station

def add_line(name):
    db = database.SessionLocal()
    new_line = schemas.LineCreate(name=name)
    line = crud.create_line(db, new_line)
    db.close()
    return line

def place_station_on_line(line_id, station_id):
    db = database.SessionLocal()
    line_station = crud.add_station_to_line(db, line_id=line_id, station_id=station_id)
    db.close()
    return line_station

def search_stations(line_id, station_id, n_ahead, n_behind):
    db = database.SessionLocal()
    ahead = crud.get_nth_station_ahead(db, line_id=line_id, station_id=station_id, n=n_ahead)
    behind = crud.get_nth_station_behind(db, line_id=line_id, station_id=station_id, n=n_behind)
    db.close()
    return ahead, behind

def board_passenger(passenger_id, start_station_id, start_time):
    db = database.SessionLocal()
    boarding_info = schemas.BoardingCreate(passenger_id=passenger_id, start_station_id=start_station_id, start_time=start_time)
    boarding = crud.board_passenger(db, boarding_info)
    db.close()
    return boarding

def exit_passenger(passenger_id, end_station_id, end_time):
    db = database.SessionLocal()
    exit_info = schemas.ExitCreate(passenger_id=passenger_id, end_station_id=end_station_id, end_time=end_time)
    exit = crud.exit_passenger(db, exit_info)
    db.close()
    return exit

def view_all_boardings():
    db = database.SessionLocal()
    boardings = crud.get_all_boardings(db)
    db.close()
    return boardings

with gr.Blocks() as demo:
    with gr.Tab("Add Station"):
        name = gr.Textbox(label="Station Name")
        location = gr.Textbox(label="Station Location")
        add_station_btn = gr.Button("Add Station")
        add_station_btn.click(add_station, inputs=[name, location], outputs="text")
    
    with gr.Tab("Add Line"):
        line_name = gr.Textbox(label="Line Name")
        add_line_btn = gr.Button("Add Line")
        add_line_btn.click(add_line, inputs=line_name, outputs="text")
    
    with gr.Tab("Place Station on Line"):
        line_id = gr.Number(label="Line ID")
        station_id = gr.Number(label="Station ID")
        place_station_btn = gr.Button("Place Station")
        place_station_btn.click(place_station_on_line, inputs=[line_id, station_id], outputs="text")
    
    with gr.Tab("Search Stations"):
        line_id = gr.Number(label="Line ID")
        station_id = gr.Number(label="Station ID")
        n_ahead = gr.Number(label="N-th Station Ahead")
        n_behind = gr.Number(label="N-th Station Behind")
        search_btn = gr.Button("Search")
        search_btn.click(search_stations, inputs=[line_id, station_id, n_ahead, n_behind], outputs=["text", "text"])
    
    with gr.Tab("Board Passenger"):
        passenger_id = gr.Number(label="Passenger ID")
        start_station_id = gr.Number(label="Start Station ID")
        start_time = gr.Textbox(label="Start Time")
        board_btn = gr.Button("Board")
        board_btn.click(board_passenger, inputs=[passenger_id, start_station_id, start_time], outputs="text")
    
    with gr.Tab("Exit Passenger"):
        passenger_id = gr.Number(label="Passenger ID")
        end_station_id = gr.Number(label="End Station ID")
        end_time = gr.Textbox(label="End Time")
        exit_btn = gr.Button("Exit")
        exit_btn.click(exit_passenger, inputs=[passenger_id, end_station_id, end_time], outputs="text")
    
    with gr.Tab("View All Boardings"):
        view_btn = gr.Button("View All Boardings")
        view_btn.click(view_all_boardings, inputs=None, outputs="text")

demo.launch()
