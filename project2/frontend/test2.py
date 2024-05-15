# Import necessary libraries
from fastapi import FastAPI
from pydantic import BaseModel
import uvicorn
import asyncio
from pywebio import start_server
from pywebio.output import put_html, clear, popup, put_buttons, put_row, put_column
from pywebio.input import input, select, textarea

# Define FastAPI application
app = FastAPI()

# Define data models
class Station(BaseModel):
    name: str
    status: str = "operational"  # Default status is operational

class Line(BaseModel):
    name: str

class PathQuery(BaseModel):
    start_station: str
    end_station: str

# Simulated data
stations = ["Station A", "Station B", "Station C"]
lines = ["Line 1", "Line 2", "Line 3"]

# Define API endpoints
@app.post("/add_station")
async def add_station(station: Station):
    # Simulate adding station to the database
    stations.append(station.name)
    return {"message": f"Station {station.name} with status {station.status} added successfully!"}

@app.post("/add_line")
async def add_line(line: Line):
    # Simulate adding line to the database
    lines.append(line.name)
    return {"message": f"Line {line.name} added successfully!"}

@app.post("/path_query")
async def path_query(query: PathQuery):
    # Simulate path query
    return {"message": f"Path found from {query.start_station} to {query.end_station}."}

# Function to run the FastAPI app
async def app_runner():
    config = uvicorn.Config(app=app, host="127.0.0.1", port=8000, log_level="info")
    server = uvicorn.Server(config)
    await server.serve()

# Start the event loop
loop = asyncio.get_event_loop()
task = loop.create_task(app_runner())

# Function to apply custom CSS
def set_custom_style():
    put_html("""
    <style>
        body { font-family: Arial, sans-serif; margin: 20px; }
        .pywebio-output-separator { display: none; }
        button { margin: 5px; padding: 10px 20px; border-radius: 5px; border: none; background-color: #4CAF50; color: white; cursor: pointer; }
        button:hover { background-color: #45a049; }
        h2 { color: #333; }
    </style>
    """)

# Admin Interface
def admin_interface():
    clear()
    put_html("<h2>Administrator Interface</h2>")
    put_row([
        put_column([
            put_buttons([("Add Station",)], onclick=[add_station_interface]),
            put_buttons([("Modify Station",)], onclick=[modify_station_interface]),
            put_buttons([("Delete Station",)], onclick=[delete_station_interface])
        ]),
        put_column([
            put_buttons([("Add Line",)], onclick=[add_line_interface]),
            put_buttons([("Modify Line",)], onclick=[modify_line_interface]),
            put_buttons([("Delete Line",)], onclick=[delete_line_interface])
        ]),
        put_column([
            put_buttons([("View Onboard Passengers",)], onclick=[view_onboard_passengers]),
            put_buttons([("Path Query",)], onclick=[path_query_interface]),
            put_buttons([("Update Station Status",)], onclick=[update_station_status_interface])
        ])
    ])

def add_station_interface():
    station_name = input("Enter station name:")
    status = select("Select station status", ["operational", "under construction", "closed"])
    # Simulate API call
    popup("Success", f"Station '{station_name}' with status '{status}' added.")

def modify_station_interface():
    station_name = select("Select station to modify", stations)
    new_name = input("Enter new station name:")
    # Simulate API call
    popup("Success", f"Station '{station_name}' renamed to '{new_name}'.")

def delete_station_interface():
    station_name = select("Select station to delete", stations)
    # Simulate API call
    stations.remove(station_name)
    popup("Success", f"Station '{station_name}' deleted.")

def add_line_interface():
    line_name = input("Enter line name:")
    # Simulate API call
    lines.append(line_name)
    popup("Success", f"Line '{line_name}' added.")

def modify_line_interface():
    line_name = select("Select line to modify", lines)
    new_name = input("Enter new line name:")
    # Simulate API call
    popup("Success", f"Line '{line_name}' renamed to '{new_name}'.")

def delete_line_interface():
    line_name = select("Select line to delete", lines)
    # Simulate API call
    lines.remove(line_name)
    popup("Success", f"Line '{line_name}' deleted.")

def view_onboard_passengers():
    # Simulate fetching and displaying onboard passengers
    popup("Onboard Passengers", "Passenger 1, Passenger 2")

def path_query_interface():
    start_station = input("Enter start station:")
    end_station = input("Enter end station:")
    # Simulate path query
    popup("Path Query Result", f"Path found from {start_station} to {end_station}.")

def update_station_status_interface():
    station_name = select("Select station to update status", stations)
    status = select("Select new status", ["operational", "under construction", "closed"])
    # Simulate status update
    popup("Status Updated", f"Station '{station_name}' status updated to '{status}'.")

# User Interface
def user_interface():
    clear()
    put_html("<h2>User Interface</h2>")
    put_buttons([
        ("Board Passenger", board_passenger_interface),
        ("Exit Passenger", exit_passenger_interface),
        ("Search Ride Records", search_ride_records_interface)
    ])

def board_passenger_interface():
    station_name = input("Enter boarding station:")
    # Simulate API call for boarding
    popup("Boarding Successful at", station_name)

def exit_passenger_interface():
    station_name = input("Enter exit station:")
    # Simulate API call for exiting
    popup("Exit Successful from", station_name)

def search_ride_records_interface():
    params = textarea("Enter search parameters (e.g., station name, passenger name, time period):")
    # Simulate search
    popup("Search Results", f"Results for: {params}")

# Main function to manage session and layout
def main():
    set_custom_style()
    put_buttons(['Admin Interface', 'User Interface'], onclick=[
        admin_interface,
        user_interface
    ])

# Start the PyWebIO server
start_server(main, port=8080, debug=True)
