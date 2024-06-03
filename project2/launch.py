from dotenv import load_dotenv
import mysql.connector as mydb
import os
from sqlalchemy import create_engine
from sqlalchemy.ext.declarative import declarative_base
from sqlalchemy.orm import sessionmaker
from sqlalchemy.exc import SQLAlchemyError
import backend.models as models
import backend.schemas as schemas
import backend.crud as crud
import backend.database as database
import gradio as gr
import pandas as pd
from backend.database import SessionLocal
import json
import heapq
import pickle
from datetime import datetime

# ALTER DATABASE project2 CHARACTER SET = utf8mb4 COLLATE = utf8mb4_unicode_ci;
# ALTER TABLE project2.`lines` CONVERT TO CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci;
# ALTER TABLE project2.bus_lines CONVERT TO CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci;
# ALTER TABLE project2.bus_line_details CONVERT TO CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci;
# ALTER TABLE project2.bus_stations CONVERT TO CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci;
# ALTER TABLE project2.card_rides CONVERT TO CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci;
# ALTER TABLE project2.cards CONVERT TO CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci;
# ALTER TABLE project2.stations CONVERT TO CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci;
# ALTER TABLE project2.passengers CONVERT TO CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci;
# ALTER TABLE project2.line_details CONVERT TO CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci;
# ALTER TABLE project2.passenger_rides CONVERT TO CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci;
# ALTER TABLE project2.prices CONVERT TO CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci;

def calculate_shortest_path(start_station_name, end_station_name):
    try:
        db = SessionLocal()
        # Load the graph
        with open('graph.pkl', 'rb') as f:
            graph = pickle.load(f)

        # Get the stations
        start_station = crud.get_station_by_name(db, (start_station_name))
        end_station = crud.get_station_by_name(db, (end_station_name))

        # Dijkstra's algorithm
        def shortest_path(start, end):
            queue = [(0, start, [])]
            seen = set()
            while queue:
                (cost, node, path) = heapq.heappop(queue)
                if node not in seen:
                    seen.add(node)
                    path = path + [node]
                    if node == end:
                        return cost, path
                    for next_node, next_cost in graph[node]:
                        if next_node not in seen:
                            heapq.heappush(queue, (cost + next_cost, next_node, path))
            return float('inf'), []
        # Calculate the shortest path
        cost, path = shortest_path(start_station.id, end_station.id)
        path_name=path
        realcost=crud.calculate_price(db,start_station.id, end_station.id)
        path_name=[crud.get_station_by_id(db,station_id).chinese_name for station_id in path]
        return f"The shortest path from {start_station_name} to {end_station_name} is {path_name} with a cost of {realcost} CNY."
    except Exception as e:
        db.rollback()
        return str(e)
    finally:
        db.close()

with gr.Blocks() as demo:        
    with gr.Tab("Station Management"):
    
        with gr.Group():
            gr.Markdown("### View All Stations")
            view_stations_btn = gr.Button("View All Stations")
            view_stations_output = gr.Dataframe(pd.DataFrame([], columns=["ID", "Chinese Name", "Name", "District", "Status", "Intro"]))
            def get_all_stations():
                db = database.SessionLocal()
                try:
                    stations = crud.get_stations(db,limit=500)
                    station_info = []
                    for station in stations:
                        station_info.append([
                            station.id,
                            station.chinese_name,
                            station.name,
                            station.district,
                            station.status,
                            station.intro
                        ])
                    df = pd.DataFrame(station_info, columns=["ID", "Chinese Name", "Name", "District", "Status", "Intro"])
                    return df
                except Exception as e:
                    db.rollback()
                    return str(e)
                finally:
                    db.close()

            view_stations_btn.click(get_all_stations, inputs=None, outputs=view_stations_output)

        with gr.Group():
            gr.Markdown("### Search Station")
            station_name = gr.Textbox(label="Station Name")
            search_station_btn = gr.Button("Search Station")
            view_stations_output = gr.Dataframe(pd.DataFrame([], columns=["ID", "Chinese Name", "Name", "District", "Status", "Intro"]))
            def search_station(name):
                db = database.SessionLocal()
                try:
                    stations = crud.get_stations_by_name(db, name)
                    station_info = []
                    for station in stations:
                        station_info.append([
                            station.id,
                            station.chinese_name,
                            station.name,
                            station.district,
                            station.status,
                            station.intro
                        ])
                    df = pd.DataFrame(station_info, columns=["ID", "Chinese Name", "Name", "District", "Status", "Intro"])
                    return df
                except Exception as e:
                    db.rollback()
                    return str(e)
                finally:
                    db.close()
            search_station_btn.click(search_station, inputs=[station_name], outputs=view_stations_output)

        with gr.Group():
            gr.Markdown("### Add Station")
            name = gr.Textbox(label="Name")
            district = gr.Textbox(label="District")
            intro = gr.Textbox(label="Intro")
            chinese_name = gr.Textbox(label="Chinese Name")
            status = gr.Radio(["operational", "under construction", "closed"], label="Station Status")
            add_station_btn = gr.Button("Add Station")
            
            add_station_output = gr.Textbox()

            def add_station(name, district, intro, chinese_name,status):
                db = SessionLocal()
                try:
                    new_station = schemas.StationCreate(
                        name=(name),
                        district=(district),
                        intro=(intro),
                        chinese_name=(chinese_name),
                        status=(status)
                    )
                    station = crud.create_station(db, new_station)
                    return f"Station '{station.name}' added with ID {station.id}"
                except Exception as e:
                    db.rollback()
                    return str(e)
                finally:
                    db.close()

            add_station_btn.click(add_station, inputs=[name, district, intro, chinese_name,status], outputs=add_station_output)

        with gr.Group():
            gr.Markdown("### Update Station")
            station_id = gr.Number(label="Station ID")
            name = gr.Textbox(label="Name")
            district = gr.Textbox(label="District")
            intro = gr.Textbox(label="Intro")
            chinese_name = gr.Textbox(label="Chinese Name")
            status = gr.Radio(["operational", "under construction", "closed"], label="Station Status")
            update_station_btn = gr.Button("Update Station")
            update_station_output = gr.Textbox()

            def update_station(station_id, name, district, intro, chinese_name,status):
                db = SessionLocal()
                try:
                    station_update = schemas.StationUpdate(
                        name=(name),
                        district=(district),
                        intro=(intro),
                        chinese_name=(chinese_name),
                        status=(status)
                    )
                    station = crud.update_station(db, station_id, station_update)
                    return f"Station ID {station_id} updated"
                except Exception as e:
                    db.rollback()
                    return str(e)
                finally:
                    db.close()

            update_station_btn.click(update_station, inputs=[station_id, name, district, intro, chinese_name, status], outputs=update_station_output)

        with gr.Group():
            gr.Markdown("### Delete Station")
            station_id = gr.Number(label="Station ID")
            delete_station_btn = gr.Button("Delete Station")
            delete_station_output = gr.Textbox()

            def delete_station(station_id):
                db = SessionLocal()
                try:
                    #stationname=crud.get_station_by_id(db,station_id)
                    station = crud.delete_station(db, station_id)
                    
                    return f"Station ID {station_id} named {station.name} is deleted"
                except Exception as e:
                    db.rollback()
                    return str(e)
                finally:
                    db.close()

            delete_station_btn.click(delete_station, inputs=[station_id], outputs=delete_station_output)


    with gr.Tab("Line Management"):
        with gr.Group():
            gr.Markdown("### View All Lines")
            view_lines_btn = gr.Button("View All Lines")
            view_lines_output = gr.Dataframe(pd.DataFrame([], columns=["ID", "Name", "Color", "Start Time", "End Time"]))

            def get_all_lines():
                db = SessionLocal()
                try:
                    lines = crud.get_lines(db)
                    line_info = []
                    for line in lines:
                        # line_info += f"ID: {line.id}, \nName: {line.name}, \nColor: {line.color}, \nStart Time: {line.start_time}, \nEnd Time: {line.end_time}\n"
                        line_info.append([
                            line.id,
                            line.name,
                            line.color,
                            line.start_time,
                            line.end_time
                        ])
                    df = pd.DataFrame(line_info, columns=["ID", "Name", "Color", "Start Time", "End Time"])
                    return df
                except Exception as e:
                    db.rollback()
                    return str(e)
                finally:
                    db.close()

            view_lines_btn.click(get_all_lines, inputs=None, outputs=view_lines_output)

        with gr.Group():
            gr.Markdown("### Search Line")
            line_name = gr.Textbox(label="Line Name")
            search_line_btn = gr.Button("Search Line")
            search_line_output = gr.Dataframe(pd.DataFrame([], columns=["ID", "Name", "Color", "Start Time", "End Time"]))
            # search_line_output = gr.Textbox()
            def search_lines(line_name):
                db = SessionLocal()
                try:
                    lines= crud.get_lines_by_name(db, line_name)
                    line_info = []
                    for line in lines:
                        # line_info += f"ID: {line.id}, \nName: {line.name}, \nColor: {line.color}, \nStart Time: {line.start_time}, \nEnd Time: {line.end_time}\n"
                        line_info.append([
                            line.id,
                            line.name,
                            line.color,
                            line.start_time,
                            line.end_time
                        ])
                    df = pd.DataFrame(line_info, columns=["ID", "Name", "Color", "Start Time", "End Time"])
                    return df
                except Exception as e:
                    db.rollback()
                    return str(e)
                finally:
                    db.close()

            search_line_btn.click(search_lines, inputs=[line_name], outputs=search_line_output)

        with gr.Group():
            gr.Markdown("### Add Line")
            name = gr.Textbox(label="Name")
            color = gr.Textbox(label="Color")
            start_time = gr.Textbox(label="Start Time (HH:MM)")
            end_time = gr.Textbox(label="End Time (HH:MM)")
            add_line_btn = gr.Button("Add Line")
            add_line_output = gr.Textbox()

            def add_line(name, color, start_time, end_time):
                # start_time1 = datetime.strptime(start_time, "%H:%M").time()
                # end_time1 = datetime.strptime(end_time, "%H:%M").time()
                start_time1 = datetime.strptime(start_time, "%H:%M")
                end_time1 = datetime.strptime(end_time, "%H:%M")
                db = SessionLocal()
                try:
                    new_line = schemas.LineCreate(
                        name=(name),
                        color=(color),
                        start_time=start_time1, #
                        end_time=end_time1 #
                        
                    )
                    line = crud.create_line(db, new_line)
                    return f"Line '{line.name}' added with ID {line.id}"
                except Exception as e:
                    db.rollback()
                    print(e)
                    return str(e)
                finally:
                    db.close()

            add_line_btn.click(add_line, inputs=[name, color, start_time, end_time], outputs=add_line_output)

        with gr.Group():
            gr.Markdown("### Update Line")
            line_id = gr.Number(label="Line ID")
            name = gr.Textbox(label="Name")
            color = gr.Textbox(label="Color")
            start_time = gr.Textbox(label="Start Time (HH:MM)")

            end_time = gr.Textbox(label="End Time (HH:MM)")

            update_line_btn = gr.Button("Update Line")
            update_line_output = gr.Textbox()

            def update_line(line_id, name, color, start_time, end_time):
                start_time1 = datetime.strptime(start_time, "%H:%M")
                end_time1 = datetime.strptime(end_time, "%H:%M")
                db = SessionLocal()
                try:
                    line_update = schemas.LineUpdate(
                        name=(name),
                        color=(color),
                        start_time=start_time1,
                        end_time=end_time1
                    )
                    line = crud.update_line(db, line_id, line_update)
                    return f"Line ID {line_id} updated"
                except Exception as e:
                    db.rollback()
                    return str(e)
                finally:
                    db.close()

            update_line_btn.click(update_line, inputs=[line_id, name, color, start_time, end_time], outputs=update_line_output)

        with gr.Group():
            gr.Markdown("### Delete Line")
            line_id = gr.Number(label="Line ID")
            delete_line_btn = gr.Button("Delete Line")
            delete_line_output = gr.Textbox()

            def delete_line(line_id):
                db = SessionLocal()
                try:
                    line = crud.delete_line(db, line_id)
                    return f"Line ID {line_id} deleted"
                except Exception as e:
                    db.rollback()
                    return str(e)
                finally:
                    db.close()

            delete_line_btn.click(delete_line, inputs=[line_id], outputs=delete_line_output)


    with gr.Tab("Station-Line Management"):
        with gr.Group():
            gr.Markdown("### View Stations on line")
            line_id = gr.Number(label="Line ID")
            view_line_stations_btn = gr.Button("View Station on line")
            # view_line_stations_output = gr.Textbox()
            view_line_stations_output = gr.Dataframe(pd.DataFrame([], columns=["Station ID", "Station Name", "Index"]))

            def view_all_line_stations(line_id):
                db = SessionLocal()
                try:
                    line_stations = crud.get_all_line_stations(db, line_id=line_id)
                    line_station_info = []
                    index : int = 1
                    for line_station in line_stations:
                        station_id = line_station[0]  # Extract the station_id from the tuple
                        station = crud.get_station_by_id(db, station_id)
                        # line_station_info += f"Station ID: {station_id}, named {station.name}\n"
                        line_station_info.append([
                            station.id,
                            station.chinese_name,
                            index
                        ])
                        index += 1
                    df = pd.DataFrame(line_station_info, columns=["Station ID", "Station Name", "Index"])
                    return df
                except Exception as e:
                    db.rollback()
                    return str(e)
                finally:
                    db.close()

            view_line_stations_btn.click(view_all_line_stations, inputs=[line_id], outputs=view_line_stations_output)

        with gr.Group():
            gr.Markdown("### Place Station on Line")
            line_id = gr.Number(label="Line ID")
            station_id = gr.Number(label="Station ID")
            order=gr.Number(label="Order")
            place_station_btn = gr.Button("Place Station on Line")
            place_station_output = gr.Textbox()

            def place_station_on_line(line_id, station_id, order):
                order=order+1# as compensation for the order
                db = SessionLocal()
                try:
                    result = crud.insert_station_to_line(db, line_id=line_id, station_id=station_id,order=order)
                    return f"Station ID {station_id} placed on Line ID {line_id} at order {order}"
                except Exception as e:
                    db.rollback()
                    return str(e)
                finally:
                    db.close()

            place_station_btn.click(place_station_on_line, inputs=[line_id, station_id,order], outputs=place_station_output)

        with gr.Group():
            gr.Markdown("### Remove Station from Line")
            line_id = gr.Number(label="Line ID")
            station_id = gr.Number(label="Station ID")
            remove_station_btn = gr.Button("Remove Station from Line")
            remove_station_output = gr.Textbox()

            def remove_station_from_line(line_id, station_id):
                db = SessionLocal()
                try:
                    result = crud.remove_station_from_line(db, line_id=line_id, station_id=station_id)
                    return f"Station ID {station_id} removed from Line ID {line_id}"
                except Exception as e:
                    db.rollback()
                    return str(e)
                finally:
                    db.close()

            remove_station_btn.click(remove_station_from_line, inputs=[line_id, station_id], outputs=remove_station_output)

        with gr.Group():
            gr.Markdown("### Search N-th Station Ahead")
            line_id = gr.Number(label="Line ID")
            station_id = gr.Number(label="Station ID")
            n = gr.Number(label="N-th Station Ahead")
            search_ahead_btn = gr.Button("Search")
            search_ahead_output = gr.Textbox()

            def search_nth_station_ahead(line_id, station_id, n):
                db = SessionLocal()
                try:
                    ahead = crud.get_nth_station_ahead(db, line_id=line_id, station_id=station_id, n=n)
                    if ahead:
                        return f"{n}-th station ahead: {ahead.name}"
                    else:
                        return "No such station found."
                except Exception as e:
                    db.rollback()
                    return str(e)
                finally:
                    db.close()

            search_ahead_btn.click(search_nth_station_ahead, inputs=[line_id, station_id, n], outputs=search_ahead_output)

        with gr.Group():
            gr.Markdown("### Search N-th Station Behind")
            line_id = gr.Number(label="Line ID")
            station_id = gr.Number(label="Station ID")
            n = gr.Number(label="N-th Station Behind")
            search_behind_btn = gr.Button("Search")
            search_behind_output = gr.Textbox()

            def search_nth_station_behind(line_id, station_id, n):
                db = SessionLocal()
                try:
                    behind = crud.get_nth_station_behind(db, line_id=line_id, station_id=station_id, n=n)
                    if behind:
                        return f"{n}-th station behind: {behind.name}"
                    else:
                        return "No such station found."
                except Exception as e:
                    db.rollback()
                    return str(e)
                finally:
                    db.close()
            search_behind_btn.click(search_nth_station_behind, inputs=[line_id, station_id, n], outputs=search_behind_output)
  
    with gr.Tab("Ride"):
            with gr.Group():
                gr.Markdown("### View Current Boardings")
                view_boardings_btn = gr.Button("View Current Boardings")
                # view_boardings_output = gr.Textbox()
                view_boardings_output = gr.DataFrame(pd.DataFrame([], columns=["ID", "Start Station", "Start Time"]))

                def get_boardings():
                    db = SessionLocal()
                    try:
                        boardings = crud.get_current_boardings(db)
                        boarding_info = []
                        for passenger in boardings["passengers"]:
                            # boarding_info += f"Passenger ID: {passenger.id}, Start Station: {passenger.start_station_id}, named: {crud.get_station_by_id(db, passenger.start_station_id).name} at {passenger.start_time}\n"
                            boarding_info.append([
                                passenger.id,
                                passenger.start_station_id,
                                crud.get_station_by_id(db, passenger.start_station_id).name,
                                passenger.start_time
                            ])
                        for card in boardings["cards"]:
                            # boarding_info += f"Card ID: {card.code}, Start Station: {card.start_station_id}, named: {crud.get_station_by_id(db, card.start_station_id).name} at {card.start_time}\n"
                            boarding_info.append([
                                card.code,
                                card.start_station_id,
                                crud.get_station_by_id(db, card.start_station_id).name,
                                card.start_time
                            ])
                        df = pd.DataFrame(boarding_info, columns=["ID", "Start Station", "Start Time"])
                        return df
                        # return boarding_info
                    except Exception as e:
                        db.rollback()
                        return str(e)
                    finally:
                        db.close()

                view_boardings_btn.click(get_boardings, inputs=None, outputs=view_boardings_output)

            with gr.Group():
                gr.Markdown("### Search Rides")
                start_station_input = gr.Textbox(label="Start Station")
                end_station_input = gr.Textbox(label="End Station")
                start_time_input = gr.Textbox(label="Start Time (YYYY-MM-DD HH:MM)")
                end_time_input = gr.Textbox(label="End Time (YYYY-MM-DD HH:MM)")
                passenger_id_input = gr.Number(label="Passenger ID")
                card_code_input = gr.Number(label="Card Code")
                search_rides_btn = gr.Button("Search Rides")
                # search_rides_output = gr.Textbox()
                search_rides_output = gr.DataFrame(pd.DataFrame([], columns=["ID", "Start Station", "End Station", "Start Time", "End Time"]))

                def search_rides(start_station, end_station, start_time, end_time, passenger_id, card_code):
                    db = SessionLocal()
                    try:
                        if passenger_id==0:
                            passenger_id=None
                        if card_code==0:
                            card_code=None
                        start_station_id =crud.get_station_by_name(db,start_station).id if start_station else None
                        end_station_id =crud.get_station_by_name(db,end_station).id if end_station else None
                        start_time = datetime.strptime(start_time, "%Y-%m-%d %H:%M") if start_time else None
                        end_time = datetime.strptime(end_time, "%Y-%m-%d %H:%M") if end_time else None
                        passenger_results, card_results = crud.get_rides_by_parameters(db, start_station_id, end_station_id, start_time, end_time, passenger_id, card_code)
                        ride_info = []
                        for ride in passenger_results:
                            # ride_info += f"Passenger Ride ID: {ride.id}, Start Station: {ride.start_station_id}, named {crud.get_station_by_id(db,ride.start_station_id).name}, End Station: {ride.end_station_id}, named {crud.get_station_by_id(db,ride.end_station_id).name}, Start Time: {ride.start_time}, End Time: {ride.end_time}\n"
                            ride_info.append([
                                ride.id,
                                # ride.start_station_id,
                                crud.get_station_by_id(db,ride.start_station_id).chinese_name,
                                # ride.end_station_id,
                                crud.get_station_by_id(db,ride.end_station_id).chinese_name,
                                ride.start_time,
                                ride.end_time
                            ])
                        for ride2 in card_results:
                            # ride_info += f"Card Ride Code: {ride2.code}, Start Station: {ride2.start_station_id}, named {crud.get_station_by_id(db,ride2.start_station_id).name} End Station: {ride2.end_station_id}, named {crud.get_station_by_id(db,ride2.end_station_id).name}, Start Time: {ride2.start_time}, End Time: {ride2.end_time}\n"
                            ride_info.append([
                                ride2.code,
                                # ride2.start_station_id,
                                crud.get_station_by_id(db,ride2.start_station_id).chinese_name,
                                # ride2.end_station_id,
                                crud.get_station_by_id(db,ride2.end_station_id).chinese_name,
                                ride2.start_time,
                                ride2.end_time
                            ])           
                        # return ride_info
                        df = pd.DataFrame(ride_info, columns=["ID", "Start Station", "End Station", "Start Time", "End Time"])
                        return df
                    except Exception as e:
                        db.rollback()
                        return str(e)
                    finally:
                        db.close()

                search_rides_btn.click(search_rides, inputs=[start_station_input, end_station_input, start_time_input, end_time_input, passenger_id_input, card_code_input], outputs=search_rides_output)
            
            with gr.Row():
                with gr.Column():
                    with gr.Group():
                        gr.Markdown("### Board Passenger")
                        passenger_id = gr.Textbox(label="Passenger ID")
                        start_station = gr.Textbox(label="Start Station")
                        start_time = gr.Textbox(label="Start Time (YYYY-MM-DD HH:MM)")
                        board_passenger_btn = gr.Button("Board Passenger")
                        board_passenger_output = gr.Textbox()

                        def board_passenger(passenger_id, start_station, start_time):
                            start_time1 = datetime.strptime(start_time, "%Y-%m-%d %H:%M")
                            db = SessionLocal()
                            try:
                                boarding = schemas.Boarding(
                                    start_time=start_time1,
                                    passengerOrCardsId=passenger_id,
                                    start_station_id=crud.get_station_by_name(db, start_station).id
                                )
                                ride = crud.board_passenger(db, boarding)
                                return f"Passenger '{ride.id}' boarded at {ride.start_station_id}, named {start_station} at {ride.start_time}"
                            except Exception as e:
                                db.rollback()
                                return str(e)
                            finally:
                                db.close()

                        board_passenger_btn.click(board_passenger, inputs=[passenger_id, start_station, start_time], outputs=board_passenger_output)

                    with gr.Group():
                        gr.Markdown("### Exit Passenger")
                        passenger_id = gr.Textbox(label="Passenger ID")
                        end_station = gr.Textbox(label="End Station")
                        end_time = gr.Textbox(label="End Time (YYYY-MM-DD HH:MM)")
                        exit_passenger_btn = gr.Button("Exit Passenger")
                        exit_passenger_output = gr.Textbox()
                        carriage = gr.Radio(['Bussiness Carriage', 'Closed'], label="Carriage selection")
                        def exit_passenger(passenger_id, end_station, end_time):
                            end_time1 = datetime.strptime(end_time, "%Y-%m-%d %H:%M")
                            db = SessionLocal()
                            try:
                                exit_info = schemas.ExitInfo(
                                    end_station_id=crud.get_station_by_name(db, end_station).id,
                                    end_time=end_time1
                                )
                                ride = crud.exit_passenger(db, passenger_id, exit_info)
                                return f"Passenger '{ride.id}' exited at {ride.end_station_id} named: {end_station} with fare: {ride.price}"
                            except Exception as e:
                                db.rollback()
                                return str(e)
                            finally:
                                db.close()
                        exit_passenger_btn.click(exit_passenger, inputs=[passenger_id, end_station, end_time], outputs=exit_passenger_output)

                with gr.Column():
                    with gr.Group():
                        gr.Markdown("### Board Card")
                        card_code = gr.Textbox(label="Card code")
                        start_station = gr.Textbox(label="Start Station")
                        start_time = gr.Textbox(label="Start Time (YYYY-MM-DD HH:MM)")
                        board_card_btn = gr.Button("Board Card")
                        board_card_output = gr.Textbox()

                        def board_card(card_code, start_station, start_time):
                            start_time1 = datetime.strptime(start_time, "%Y-%m-%d %H:%M")
                            db = SessionLocal()
                            try:
                                boarding = schemas.Boarding(
                                    start_time=start_time1,
                                    passengerOrCardsId=card_code,
                                    start_station_id=crud.get_station_by_name(db, start_station).id
                                )
                                ride = crud.board_card(db, boarding)
                                return f"Card '{ride.code}' boarded at {ride.start_station_id}, named {start_station} at {ride.start_time}"
                            except Exception as e:
                                db.rollback()
                                return str(e)
                            finally:
                                db.close()

                        board_card_btn.click(board_card, inputs=[card_code, start_station, start_time], outputs=board_card_output)

                    with gr.Group():
                        gr.Markdown("### Exit Card")
                        card_code = gr.Textbox(label="Card Code")
                        end_station = gr.Textbox(label="End Station")
                        end_time = gr.Textbox(label="End Time (YYYY-MM-DD HH:MM)")
                        exit_card_btn = gr.Button("Exit Card")
                        exit_card_output = gr.Textbox()

                        def exit_card(card_code, end_station, end_time):
                            end_time1 = datetime.strptime(end_time, "%Y-%m-%d %H:%M")
                            db = SessionLocal()
                            try:
                                exit_info = schemas.ExitInfo(
                                    end_station_id=crud.get_station_by_name(db, end_station).id,
                                    end_time=end_time1
                                )
                                ride = crud.exit_card(db, card_code, exit_info)
                                balance = crud.get_card_by_code(db, card_code).balance
                                return f"Card '{ride.code}' exited at {ride.end_station_id} named: {end_station} with fare: {ride.price} and balance: {balance}"
                            except Exception as e:
                                db.rollback()
                                return str(e)
                            finally:
                                db.close()

                        exit_card_btn.click(exit_card, inputs=[card_code, end_station, end_time], outputs=exit_card_output)


    with gr.Tab("User Management"):
            with gr.Row():
                with gr.Column():
                    gr.Markdown("### Passenger Management")

                    with gr.Group():
                        gr.Markdown("### Add Passenger")
                        name = gr.Textbox(label="Name")
                        id = gr.Textbox(label="ID")
                        phone_number = gr.Textbox(label="Phone Number")
                        gender = gr.Textbox(label="Gender")
                        district = gr.Textbox(label="District")
                        add_passenger_btn = gr.Button("Add Passenger")
                        add_passenger_output = gr.Textbox()

                        def add_passenger(name, id, phone_number, gender, district):
                            db = SessionLocal()
                            try:
                                new_passenger = schemas.PassengerCreate(
                                    name=name,
                                    id=id,
                                    phone_number=phone_number,
                                    gender=gender,
                                    district=district
                                )
                                passenger = crud.create_passenger(db, new_passenger)
                                return f"Passenger '{passenger.name}' added with ID {passenger.id}"
                            except Exception as e:
                                db.rollback()
                                return str(e)
                            finally:
                                db.close()

                        add_passenger_btn.click(add_passenger, inputs=[name, id, phone_number, gender, district], outputs=add_passenger_output)

                    with gr.Group():
                        gr.Markdown("### Update Passenger")
                        passenger_id = gr.Number(label="Passenger ID")
                        name = gr.Textbox(label="Name")
                        id = gr.Textbox(label="ID")
                        phone_number = gr.Textbox(label="Phone Number")
                        gender = gr.Textbox(label="Gender")
                        district = gr.Textbox(label="District")
                        update_passenger_btn = gr.Button("Update Passenger")
                        update_passenger_output = gr.Textbox()

                        def modify_passenger(passenger_id, name, id, phone_number, gender, district):
                            db = SessionLocal()
                            try:
                                passenger_update = schemas.PassengerUpdate(
                                    name=name,
                                    id=id,
                                    phone_number=phone_number,
                                    gender=gender,
                                    district=district
                                )
                                passenger = crud.update_passenger(db, passenger_id, passenger_update)
                                return f"Passenger ID {passenger_id} updated"
                            except Exception as e:
                                db.rollback()
                                return str(e)
                            finally:
                                db.close()

                        update_passenger_btn.click(modify_passenger, inputs=[passenger_id, name, id, phone_number, gender, district], outputs=update_passenger_output)

                    with gr.Group():
                        gr.Markdown("### Delete Passenger")
                        passenger_id = gr.Number(label="Passenger ID")
                        delete_passenger_btn = gr.Button("Delete Passenger")
                        delete_passenger_output = gr.Textbox()

                        def remove_passenger(passenger_id):
                            db = SessionLocal()
                            try:
                                passenger = crud.delete_passenger(db, passenger_id)
                                return f"Passenger ID {passenger_id} deleted"
                            except Exception as e:
                                db.rollback()
                                return str(e)
                            finally:
                                db.close()

                        delete_passenger_btn.click(remove_passenger, inputs=[passenger_id], outputs=delete_passenger_output)

                    with gr.Group():
                        gr.Markdown("### View All Passengers")
                        view_passengers_btn = gr.Button("View All Passengers")
                        # view_passengers_output = gr.Textbox()
                        view_passengers_output = gr.DataFrame(pd.DataFrame([], columns=["ID", "Name", "Phone Number", "Gender", "District"]))

                        def get_all_passengers():
                            db = SessionLocal()
                            try:
                                passengers = crud.get_all_passengers(db)
                                passenger_info = ""
                                for passenger in passengers:
                                    # passenger_info += f"ID: {passenger.id}, Name: {passenger.name}, Phone Number: {passenger.phone_number}, Gender: {passenger.gender}, District: {passenger.district}\n"
                                    passenger_info.append([
                                        passenger.id,
                                        passenger.name,
                                        passenger.phone_number,
                                        passenger.gender,
                                        passenger.district
                                    ])
                                df = pd.DataFrame(passenger_info, columns=["ID", "Name", "Phone Number", "Gender", "District"])
                                return df
                            except Exception as e:
                                db.rollback()
                                return str(e)
                            finally:
                                db.close()

                        view_passengers_btn.click(get_all_passengers, inputs=None, outputs=view_passengers_output)

                with gr.Column():
                    gr.Markdown("### Card Management")

                    with gr.Group():
                        gr.Markdown("### Add Card")
                        card_code = gr.Textbox(label="Card Code")
                        start_time = gr.Textbox(label="Create Time")
                        balance = gr.Number(label="Balance")
                        add_card_btn = gr.Button("Add Card")
                        add_card_output = gr.Textbox()

                        def add_card(card_code, balance,start_time):
                            start_time1 = datetime.strptime(start_time, "%Y-%m-%d %H:%M")
                            db = SessionLocal()
                            try:
                                new_card = schemas.CardCreate(
                                    code=card_code,
                                    balance=balance,
                                    create_time=start_time1
                                )
                                card = crud.create_card(db, new_card)
                                return f"Card '{card.code}' added with balance {card.balance}"
                            except Exception as e:
                                db.rollback()
                                return str(e)
                            finally:
                                db.close()

                        add_card_btn.click(add_card, inputs=[card_code, balance,start_time], outputs=add_card_output)

                    with gr.Group():
                        gr.Markdown("### Update Card")
                        card_code = gr.Textbox(label="Card Code")
                        balance = gr.Number(label="Balance")
                        start_time = gr.Textbox(label="Create Time")
                        update_card_btn = gr.Button("Update Card")
                        update_card_output = gr.Textbox()

                        def modify_card(card_code, balance,start_time):
                            start_time1 = datetime.strptime(start_time, "%Y-%m-%d %H:%M")
                            db = SessionLocal()
                            try:
                                card_update = schemas.CardCreate(
                                    code=card_code,
                                    balance=balance,
                                    create_time=start_time1
                                )
                                card = crud.update_card(db, card_code, card_update)
                                return f"Card ID {card_code} updated"
                            except Exception as e:
                                db.rollback()
                                return str(e)
                            finally:
                                db.close()

                        update_card_btn.click(modify_card, inputs=[card_code, balance,start_time], outputs=update_card_output)

                    with gr.Group():
                        gr.Markdown("### Delete Card")
                        card_id = gr.Number(label="Card Code")
                        delete_card_btn = gr.Button("Delete Card")
                        delete_card_output = gr.Textbox()

                        def remove_card(card_id):
                            db = SessionLocal()
                            try:
                                card = crud.delete_card(db, card_id)
                                return f"Card ID {card_id} deleted"
                            except Exception as e:
                                db.rollback()
                                return str(e)
                            finally:
                                db.close()

                        delete_card_btn.click(remove_card, inputs=[card_id], outputs=delete_card_output)

                    with gr.Group():
                        gr.Markdown("### View All Cards")
                        view_cards_btn = gr.Button("View All Cards")
                        view_cards_output = gr.DataFrame(pd.DataFrame([], columns=["Code", "Balance", "Create Time"]))

                        def get_all_cards():
                            db = SessionLocal()
                            try:
                                cards = crud.get_all_cards(db)
                                card_info = []
                                for card in cards:
                                    # card_info += f"Code: {card.code}, Balance: {card.balance}, create at: {card.create_time}\n"
                                    card_info.append([
                                        card.code,
                                        card.balance,
                                        card.create_time
                                    ])
                                df = pd.DataFrame(card_info, columns=["Code", "Balance", "Create Time"])
                                return df
                            except Exception as e:
                                db.rollback()
                                return str(e)
                            finally:
                                db.close()

                        view_cards_btn.click(get_all_cards, inputs=None, outputs=view_cards_output)

    with gr.Tab("Path Queries"):
        with gr.Group():
            gr.Markdown("### Calculate Shortest Path")
            db = SessionLocal()
            stations = [station.chinese_name for station in crud.get_stations(db,limit=500)]
            # start_station_name = gr.Textbox(label="Start Station Name")
            # end_station_name = gr.Textbox(label="End Station Name")
            start_station_name = gr.Dropdown(stations, label="Start Station Name")
            end_station_name = gr.Dropdown(stations, label="End Station Name" )
            calculate_path_btn = gr.Button("Calculate Shortest Path")
            calculate_path_output = gr.Textbox()
            calculate_path_btn.click(calculate_shortest_path, inputs=[start_station_name, end_station_name], outputs=calculate_path_output)
            
def auth_fn(username, password):
    global SessionLocal
    db_name = os.getenv('DB_NAME')
    db_host = os.getenv('DB_HOST')
    db_port = os.getenv('DB_PORT')
    db_user=username
    db_password=password
    SQLALCHEMY_DATABASE_URL = f"mysql+mysqlconnector://{db_user}:{db_password}@{db_host}:{db_port}/{db_name}?charset=utf8mb4"
    # print(SQLALCHEMY_DATABASE_URL)
    try:
        engine = create_engine(
            SQLALCHEMY_DATABASE_URL
        )
        SessionLocal = sessionmaker(autocommit=False, autoflush=False, bind=engine)
        db = SessionLocal()
        if crud.log_in(db, username, password):
            return True
        else:
            return False
    except SQLAlchemyError:
        return False
    
# demo.launch(server_name="10.16.214.232",server_port=54723,share=True,auth=auth_fn,auth_message="Please log in to use the system.") 
demo.launch(server_name="10.16.214.232",server_port=54723) 
