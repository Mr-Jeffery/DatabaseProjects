-- Q1: Station Queries
-- Q1.1: Number of Stations in Each District
SELECT district, COUNT(station_id) AS number_of_stations
FROM stations
GROUP BY district;
--Q1.2: Number of Stations on Each Line
SELECT l.name AS line_name, COUNT(ld.station_id) AS number_of_stations
FROM line_details ld
JOIN lines l ON l.line_ID = ld.line_id
GROUP BY l.name;
--Q1.3: Total Number of Stations
SELECT COUNT(station_id) AS number_of_stations
FROM stations;

--Q2: Passenger Gender Counts
SELECT gender, COUNT(gender) AS count
FROM passengers
GROUP BY gender;

--Q3: Passenger Origin Counts
SELECT district, COUNT(district) AS count
FROM passengers
GROUP BY district;

--Q4: Nearby Buses, Buildings, or Landmarks for Specific Station Exit
--Q4.1: Buses
SELECT DISTINCT bus_lines.bus_line_name
FROM bus_line_details bld
JOIN exit_details ed ON bld.bus_station_ID = ed.bus_station_ID
JOIN exits e ON e.station_id = ed.station_id AND e.name = ed.name
JOIN stations s ON s.station_id = e.station_id
join bus_lines on bld.bus_line_id=bus_lines.bus_lines_id
WHERE s.station_id = (SELECT station_id FROM stations WHERE name = 'Changlingpi') AND e.name = 'A1出入口';
--Q4.2: Buildings or Landmarks
SELECT DISTINCT e.textt
FROM bus_line_details bld
JOIN exit_details ed ON bld.bus_station_ID = ed.bus_station_ID
JOIN exits e ON e.station_id = ed.station_id AND e.name = ed.name
JOIN stations s ON s.station_id = e.station_id
WHERE s.station_id = (SELECT station_id FROM stations WHERE name = 'Changlingpi') AND e.name = 'A1出入口'
GROUP BY e.textt;


--Q5: Specific Passenger Journey Information
SELECT p.name, s1.name AS entry_station, s2.name AS exit_station, r.start_time, r.end_time
FROM rides r
JOIN passengers p ON r.rail_user = p.id_number
JOIN stations s1 ON r.start_station_id = s1.station_id
JOIN stations s2 ON r.end_station_id = s2.station_id
WHERE p.name = '臧宜君';

--Q6: Specific Travel Card Journey Records
SELECT c.code, s1.name AS entry_station, s2.name AS exit_station, r.start_time, r.end_time
FROM rides r
JOIN cards c ON r.rail_user = c.code
JOIN stations s1 ON r.start_station_id = s1.station_id
JOIN stations s2 ON r.end_station_id = s2.station_id
WHERE c.code = '883706296';

--Q7: Specific Subway Station Information
SELECT s.chinese_name, s.name AS station_name, COUNT(e.name) AS number_of_exits, s.district, lines.name AS line_name
FROM stations s
JOIN exits e ON s.station_id = e.station_id
JOIN line_details ld ON s.station_id = ld.station_id
JOIN lines ON ld.line_id = lines.line_id
WHERE s.name = 'Changlingpi'
GROUP BY s.chinese_name, s.name, s.district, lines.name;

--Q8: Specific Subway Line Information
SELECT l.name, l.start_time, l.end_time, l.first_opening, COUNT(ld.station_id) AS number_of_stations, l.intro
FROM lines l
JOIN line_details ld ON l.line_ID = ld.line_id
WHERE l.name = '1号线'
GROUP BY l.name, l.start_time, l.end_time, l.first_opening, l.intro;
