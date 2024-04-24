-- Step 1: Drop the existing primary key constraint
-- ALTER TABLE stations DROP CONSTRAINT stations_pkey CASCADE;
-- ALTER TABLE stations ADD COLUMN station_id BIGINT GENERATED ALWAYS AS IDENTITY PRIMARY KEY;

-- UPDATE stations
-- SET id = new_id
-- FROM (SELECT row_number() OVER (ORDER BY some_column) AS new_id, primary_key_or_unique_column
--       FROM your_table_name) AS rows
-- WHERE your_table_name.primary_key_or_unique_column = rows.primary_key_or_unique_column;


--Q1: The numbrer of stations, in each district, on each line or in total.
--Q1.1: The number of stations in each district.
select district, count(name) as number_of_stations
from stations
group by district;
--Q1.2: The number of stations on each line.
select line_name, count(line_details.station_name) as number_of_stations
from line_details
group by line_name;
--Q1.3: The number of stations in total.
select count(name) as number_of_stations
from stations;

--Q2: Number of female passengers and male passengers respectively.
select gender, count(gender)
from passengers
group by gender;

--Q3: List the number of passengers from Mainland China, Hong Kong, Macau, and Taiwan
select district, count(district)
from passengers
group by district;

--Q4: List the buses, buildings, or landmarks near a specific station exit.
--Q4.1: buses
select bus_line_name
from bus_line_details
         join(select *
              from stations
                       join (select *
                             from exits
                                      natural join exit_details
                             where exits.station_name = 'Changlingpi'
                               and name = 'A1出入口') as exit_total
                            on stations.name = exit_total.station_name) as station_exit_total
             on bus_line_details.bus_station_name = station_exit_total.bus_station_name
group by bus_line_name;
--Q4.2: buildings or landmarks
select textt
from bus_line_details
         join(select *
              from stations
                       join (select *
                             from exits
                                      natural join exit_details
                             where exits.station_name = 'Changlingpi'
                               and name = 'A1出入口') as exit_total
                            on stations.name = exit_total.station_name) as station_exit_total
             on bus_line_details.bus_station_name = station_exit_total.bus_station_name
group by textt;

--Q5: List all information about a specific passenger's journey, including passenger name, entry station, exit station, date, and time.
select name, start_station, end_station, start_time, end_time
from rides
         join passengers on rides.rail_user = passengers.id_number
where name = '臧宜君';

--Q6: List all journey records for a specific travel card, including card number, entry station, exit station, date, and time.
select code, start_station, end_station, start_time, end_time
from rides
         join cards on cards.code = rides.rail_user
where cards.code = '883706296';

--Q7: Query information about a specific subway station, including Chinese name, English name, number of exits, the district it is located in, and the subway line it belongs to.
(select chinese_name, station_name, count(name), district, line_name
 from exit_details
          natural join
      (select station_name, district, chinese_name, line_name
       from stations
                join line_details on stations.name = line_details.station_name) as station_line
 where exit_details.station_name = 'Changlingpi'
 group by chinese_name, station_name, district, line_name);

--Q8: Query information about a specific subway line, including start time, end time, first opening time, number of stations, and an introduction
select name, start_time, end_time, first_opening, count(line_details.station_name), intro
from lines
         join line_details on lines.name = line_details.line_name
where name = '1号线'
group by name, start_time, end_time, first_opening, intro;
